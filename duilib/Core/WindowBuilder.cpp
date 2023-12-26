#include "StdAfx.h"

namespace ui 
{

WindowBuilder::WindowBuilder(CreateControlCallback pCallback)
{
	m_createControlCallback = pCallback;
}

Box* WindowBuilder::Create(STRINGorID xml, CreateControlCallback pCallback, 
	Window* pManager, Box* pParent, Box* pUserDefinedBox)
{
	if (!LoadBuilder(xml))
		return nullptr;
	return Create(pCallback, pManager, pParent, pUserDefinedBox);
}

Box* WindowBuilder::Create(CreateControlCallback pCallback, Window* pManager, Box* pParent, Box* pUserDefinedBox)
{
	m_createControlCallback = pCallback;
	CMarkupNode root = m_xml.GetRoot();
	if( !root.IsValid() ) 
		return NULL;
	ui::string strRoot = root.GetName();
	if (strRoot == _T("Global"))
	{
		BuilderGlobal();
		return pParent;
	}
	if( pManager ) 
	{
		if(strRoot == _T("Window") )
		{
			BuilderWindow(pManager);
		}
		else
		{
			assert(false);
			return pParent;
		}
	}
	auto pControl = BuilderControl(pManager, pParent, pUserDefinedBox);
	return pControl;
}

bool WindowBuilder::LoadBuilder(STRINGorID xml)
{
	//资源ID为0-65535，两个字节；字符串指针为4个字节
	//字符串以<开头认为是XML字符串，否则认为是XML文件
	//如果使用了 zip 压缩包，则从内存中读取
	if (HIWORD(xml.m_lpstr) != NULL)
	{
		if (*(xml.m_lpstr) == _T('<'))
		{
			return m_xml.Load(xml.m_lpstr);
		}
		else if (GlobalManager::IsUseZip())
		{
			ui::string sFile = GlobalManager::GetResourcePath();
			sFile += xml.m_lpstr;
			HGLOBAL hGlobal = GlobalManager::GetData(sFile);
			if (hGlobal)
			{
				std::string src((LPSTR)GlobalLock(hGlobal), GlobalSize(hGlobal));
				ui::string string_resourse;
#ifdef _UNICODE	
				StringHelper::MBCSToUnicode(src.c_str(), string_resourse, CP_UTF8);
#else
				string_resourse = src;
#endif
				GlobalFree(hGlobal);
				return m_xml.Load(string_resourse.c_str());
			}
			else
			{
				return m_xml.LoadFromFile(xml.m_lpstr);
			}
		}
		else 
		{
			return m_xml.LoadFromFile(xml.m_lpstr);
		}
	}
	else
	{
		assert(false);
		return false;
	}
	return true;
}

bool WindowBuilder::BuilderGlobal()
{
	CMarkupNode root = m_xml.GetRoot();
	if (!root.IsValid())
		return false;
	ui::string strClass;
	ui::string strName;
	ui::string strValue;
	LPTSTR pstr = NULL;
	strClass = root.GetName();
	if (strClass != _T("Global"))
		return true;
	int nGlobalAttributes = root.GetAttributeCount();
	for (int i = 0; i < nGlobalAttributes; i++) {
		strName = root.GetAttributeName(i);
		strValue = root.GetAttributeValue(i);
		if (strName == _T("disabledfontcolor")) {
			GlobalManager::SetDefaultDisabledTextColor(strValue);
		}
		else if (strName == _T("defaultfontcolor")) {
			GlobalManager::SetDefaultTextColor(strValue);
		}
		else if (strName == _T("linkfontcolor")) {
			DWORD clrColor = GlobalManager::GetTextColor(strValue);
			GlobalManager::SetDefaultLinkFontColor(clrColor);
		}
		else if (strName == _T("linkhoverfontcolor")) {
			DWORD clrColor = GlobalManager::GetTextColor(strValue);
			GlobalManager::SetDefaultLinkHoverFontColor(clrColor);
		}
		else if (strName == _T("selectedcolor")) {
			DWORD clrColor = GlobalManager::GetTextColor(strValue);
			GlobalManager::SetDefaultSelectedBkColor(clrColor);
		}
	}
	//
	for (CMarkupNode node = root.GetChild(); node.IsValid(); node = node.GetSibling()) {
		strClass = node.GetName();
		if (strClass == _T("Image")) {
			ASSERT(FALSE);	//废弃
		}
		else if (strClass == DUI_STYLE_FONT) {
			int nAttributes = node.GetAttributeCount();
			ui::string strFontId;
			ui::string strFontName;
			int size = 12;
			bool bold = false;
			bool underline = false;
			bool italic = false;
			bool isDefault = false;
			for (int i = 0; i < nAttributes; i++) {
				strName = node.GetAttributeName(i);
				strValue = node.GetAttributeValue(i);
				if (strName == _T("id"))
				{
					strFontId = strValue;
				}
				else if (strName == _T("name")) {
					strFontName = strValue;
				}
				else if (strName == _T("size")) {
					size = _tcstol(strValue.c_str(), &pstr, 10);
				}
				else if (strName == _T("bold")) {
					bold = (strValue == _T("true"));
				}
				else if (strName == _T("underline")) {
					underline = (strValue == _T("true"));
				}
				else if (strName == _T("italic")) {
					italic = (strValue == _T("true"));
				}
				else if (strName == _T("default")) {
					isDefault = (strValue == _T("true"));
				}
			}
			if (!strFontName.empty()) {
				GlobalManager::AddFont(strFontId, strFontName, size, bold, underline, italic, isDefault);
			}
		}
		else if (strClass == DUI_STYLE_CLASS) {
			int nAttributes = node.GetAttributeCount();
			ui::string strClassName;
			ui::string strAttribute;
			for (int i = 0; i < nAttributes; i++) {
				strName = node.GetAttributeName(i);
				strValue = node.GetAttributeValue(i);
				if (strName == _T("name")) {
					strClassName = strValue;
				}
				else if (strName == _T("value")) {
					strAttribute.append(strValue);
				}
				else if (strName == _T("_value")) {
					strAttribute.append(StringHelper::Printf(_T(" value=\"%s\""), strValue.c_str()));
				}
				else if (strName == _T("class")) {
					strAttribute.append(GlobalManager::GetClassAttributes(strValue));
				}
				else {
					strAttribute.append(StringHelper::Printf(_T(" %s=\"%s\""),
						strName.c_str(), strValue.c_str()));
				}
			}
			if (!strClassName.empty()) {
				StringHelper::TrimLeft(strAttribute);
				GlobalManager::AddClass(strClassName, strAttribute);
			}
		}
		else if (strClass == DUI_STYLE_TEXTCOLOR) {
			int nAttributes = node.GetAttributeCount();
			ui::string strColorName;
			ui::string strColor;
			for (int i = 0; i < nAttributes; i++) {
				strName = node.GetAttributeName(i);
				strValue = node.GetAttributeValue(i);
				if (strName == _T("name")) {
					strColorName = strValue;
				}
				else if (strName == _T("value")) {
					strColor = strValue;
				}
			}
			if (!strColorName.empty()) {
				GlobalManager::AddTextColor(strColorName, strColor);
			}
		}
		else if (strClass == DUI_STYLE_SKIN)
		{
			// 需要注意Skin的声明位置,应排在内部样式元素声明之后
			SkinBox* pSkinBox = new SkinBox;
			int nAttributes = node.GetAttributeCount();
			for (int i = 0; i < nAttributes; i++) {
				pSkinBox->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
			}
			_Parse(&node, pSkinBox, nullptr);
			GlobalManager::AddSkin(pSkinBox);
		}
		else if (strClass == DUI_STYLE_WINDOW)
		{
			int nAttributes = node.GetAttributeCount();
			ui::string sWndName;
			for (int i = 0; i < nAttributes; i++)
			{
				strName = node.GetAttributeName(i);
				if (strName == _T("name"))
				{
					sWndName = node.GetAttributeValue(i);
					break;
				}
			}
			ui::string sNode;
			if (!sWndName.empty() && node.ToString(sNode))
			{
				GlobalManager::AddWindowDefine(sWndName, sNode);
			}
		}
		else {
			::MessageBox(nullptr, ui::string(_T("无效的全局节点:") + strClass).c_str(), _T("错误"), MB_ICONERROR);
		}
	}
	return true;
}

Box* WindowBuilder::BuilderControl(Window* pWindow /*= nullptr*/, Box* pParent /*= nullptr*/, Box* pUserDefinedBox /*= nullptr*/)
{
	CMarkupNode root = m_xml.GetRoot();
	if (!root.IsValid())
		return NULL;
	ui::string strRoot = root.GetName();
	for (CMarkupNode node = root.GetChild(); node.IsValid(); node = node.GetSibling()) 
	{
		ui::string strClass = node.GetName();
		if (strClass == _T("Image") || 
			strClass == DUI_STYLE_FONT ||
			strClass == DUI_STYLE_CLASS || 
			strClass == DUI_STYLE_TEXTCOLOR || 
			strClass == DUI_STYLE_WINDOW ||
			strClass == DUI_STYLE_SKIN) 
		{
			continue;
		}
		else 
		{
			if (!pUserDefinedBox) 
			{
				return (Box*)_Parse(&root, pParent, pWindow);
			}
			else 
			{
				int nAttributes = node.GetAttributeCount();
				for (int i = 0; i < nAttributes; i++) {
					ASSERT(i == 0 || _tcscmp(node.GetAttributeName(i), ATTR_CTR_class) != 0);	//class必须是第一个属性
					pUserDefinedBox->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
				}
				_Parse(&node, pUserDefinedBox, pWindow);
				return pUserDefinedBox;
			}
		}
	}
	assert(false);
	return nullptr;
}

bool WindowBuilder::BuilderWindow(Window* pWindow)
{
	if (pWindow == nullptr || pWindow->GetHWND() == NULL)
		return true;
	CMarkupNode root = m_xml.GetRoot();
	if (!root.IsValid())
		return false;
	ui::string strClass;
	ui::string strName;
	ui::string strValue;
	LPTSTR pstr = NULL;
	strClass = root.GetName();
	if (strClass != _T("Window"))
		return true;
	int nWindowAttributes = root.GetAttributeCount();
	for (int i = 0; i < nWindowAttributes; i++)
	{
		strName = root.GetAttributeName(i);
		strValue = root.GetAttributeValue(i);
		if (strName == ATTR_WINDOW_size) {
			pstr = NULL;
			int cx = _tcstol(strValue.c_str(), &pstr, 10);	ASSERT(pstr);
			int cy = _tcstol(pstr + 1, &pstr, 10);	ASSERT(pstr);
			pWindow->SetInitSize(cx, cy);
		}
		else if (strName == ATTR_WINDOW_heightpercent) {
			double lfHeightPercent = _ttof(strValue.c_str());
			pWindow->SetHeightPercent(lfHeightPercent);

			MONITORINFO oMonitor = {};
			oMonitor.cbSize = sizeof(oMonitor);
			::GetMonitorInfo(::MonitorFromWindow(pWindow->GetHWND(), MONITOR_DEFAULTTOPRIMARY), &oMonitor);
			int nWindowHeight = int((oMonitor.rcWork.bottom - oMonitor.rcWork.top) * lfHeightPercent);
			int nMinHeight = pWindow->GetMinInfo().cy;
			int nMaxHeight = pWindow->GetMaxInfo().cy;
			if (nMinHeight != 0 && nWindowHeight < nMinHeight) {
				nWindowHeight = nMinHeight;
			}
			if (nMaxHeight != 0 && nWindowHeight > nMaxHeight) {
				nWindowHeight = nMaxHeight;
			}

			CSize xy = pWindow->GetInitSize();
			pWindow->SetInitSize(xy.cx, nWindowHeight, false, false);
		}
		else if (strName == ATTR_WINDOW_sizebox) {
			UiRect rcSizeBox;
			pstr = NULL;
			rcSizeBox.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
			rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			pWindow->SetSizeBox(rcSizeBox);
		}
		else if (strName == ATTR_WINDOW_caption) {
			UiRect rcCaption;
			pstr = NULL;
			rcCaption.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
			rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			pWindow->SetCaptionRect(rcCaption);
		}
		else if (strName == ATTR_WINDOW_textid) {
			pWindow->SetTextId(strValue);
		}
		else if (strName == ATTR_WINDOW_roundcorner) {
			pstr = NULL;
			int cx = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
			int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			pWindow->SetRoundCorner(cx, cy);
		}
		else if (strName == ATTR_WINDOW_mininfo) {
			pstr = NULL;
			int cx = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
			int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			pWindow->SetMinInfo(cx, cy);
		}
		else if (strName == ATTR_WINDOW_maxinfo) {
			pstr = NULL;
			int cx = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
			int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			pWindow->SetMaxInfo(cx, cy);
		}
		else if (strName == ATTR_WINDOW_shadowattached) {
			pWindow->SetShadowAttached(strValue == _T("true"));
		}
		else if (strName == ATTR_WINDOW_shadowimage) {
			pWindow->SetShadowImage(strValue);
		}
		else if (strName == ATTR_WINDOW_shadowcorner) {
			UiRect rc;
			pstr = NULL;
			rc.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
			rc.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rc.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rc.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			pWindow->SetShadowCorner(rc);
		}
		else if (strName == ATTR_WINDOW_alphafixcorner || strName == ATTR_WINDOW_custom_shadow) {
			UiRect rc;
			pstr = NULL;
			rc.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
			rc.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rc.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rc.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			pWindow->SetAlphaFixCorner(rc);
		}
	}
	// 内部样式
	for (CMarkupNode node = root.GetChild(); node.IsValid(); node = node.GetSibling()) 
	{
		strClass = node.GetName();
		if (strClass == DUI_STYLE_CLASS) {
			int nAttributes = node.GetAttributeCount();
			ui::string strClassName;
			ui::string strAttribute;
			for (int i = 0; i < nAttributes; i++) {
				strName = node.GetAttributeName(i);
				strValue = node.GetAttributeValue(i);
				if (strName == _T("name")) {
					strClassName = strValue;
				}
				else if (strName == _T("value")) {
					strAttribute.append(strValue);
				}
				else if (strName == _T("_value")) {
					strAttribute.append(StringHelper::Printf(_T(" value=\"%s\""), strValue.c_str()));
				}
				else {
					strAttribute.append(StringHelper::Printf(_T(" %s=\"%s\""),
						strName.c_str(), strValue.c_str()));
				}
			}
			if (!strClassName.empty()) {
				ASSERT(GlobalManager::GetClassAttributes(strClassName).empty());	//窗口中的Class不能与全局的重名
				StringHelper::TrimLeft(strAttribute);
				pWindow->AddClass(strClassName, strAttribute);
			}
		}
	}
	return true;
}

CMarkup* WindowBuilder::GetMarkup()
{
    return &m_xml;
}

void WindowBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
}

void WindowBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorLocation(pstrSource, cchMax);
}

Control* WindowBuilder::_Parse(CMarkupNode* pRoot, Control* pParent, Window* pManager)
{
    Control* pReturn = NULL;
    for( CMarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() ) {
		ui::string strClass = node.GetName();
		if( strClass == _T("Image") || strClass == DUI_STYLE_FONT
			|| strClass == DUI_STYLE_CLASS || strClass == DUI_STYLE_TEXTCOLOR) {
				continue;
		}

        Control* pControl = NULL;
        if( strClass == _T("Include") ) { // 待废弃节点,已支持行内引用
            if( !node.HasAttributes() ) 
				continue;
            int nCount = 1;
            LPTSTR pstr = NULL;
            TCHAR szValue[500] = { 0 };
            SIZE_T cchLen = lengthof(szValue) - 1;
            if ( node.GetAttributeValue(_T("count"), szValue, cchLen) )
                nCount = _tcstol(szValue, &pstr, 10);
            cchLen = lengthof(szValue) - 1;
            if ( !node.GetAttributeValue(_T("source"), szValue, cchLen) ) continue;
            for ( int i = 0; i < nCount; i++ ) {
                WindowBuilder builder;
                pControl = builder.Create((LPCTSTR)szValue, m_createControlCallback, pManager, (Box*)pParent);
            }
            continue;
        }
        else {
			pControl = CreateControlByClass(strClass);
			if (pControl == nullptr) {
				if (strClass == _T("Event") || strClass == _T("BubbledEvent")) {
					bool bBubbled = (strClass == _T("BubbledEvent"));
					AttachXmlEvent(bBubbled, node, pParent);
					continue;
				}
			}

            // User-supplied control factory
            if( pControl == NULL ) {
				pControl = GlobalManager::CreateControl(strClass);
            }

            if( pControl == NULL && m_createControlCallback ) {
                pControl = m_createControlCallback(strClass);
            }
        }

		if( pControl == NULL ) {
			ASSERT(FALSE);
			continue;
		}

		pControl->SetWindow(pManager);
		// Process attributes
		if( node.HasAttributes() ) {
			// Set ordinary attributes
			int nAttributes = node.GetAttributeCount();
			for( int i = 0; i < nAttributes; i++ ) {
				ASSERT(i == 0 || _tcscmp(node.GetAttributeName(i), ATTR_CTR_class) != 0);	//class必须是第一个属性
				pControl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
			}
		}

        // Add children
        if( node.HasChildren() ) {
            _Parse(&node, (Box*)pControl, pManager);
        }

		// Attach to parent
        // 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
		if( pParent != NULL ) {
			Box* pContainer = dynamic_cast<Box*>(pParent);
			ASSERT(pContainer);
			if( pContainer == NULL ) 
				return NULL;
			if( !pContainer->Add(pControl) ) {
				ASSERT(FALSE);
				delete pControl;
				continue;
			}
		}
        
        // Return first item
        if( pReturn == NULL ) 
			pReturn = pControl;
    }
    return pReturn;
}

Control* WindowBuilder::CreateControlByClass(const ui::string& strControlClass)
{
	Control* pControl = nullptr;
	static std::map<ui::string, std::function<Control* (void)>> _funcCreate =
	{
		{ DUI_CTR_BOX,	[]() { return new Box; } },
		{ DUI_CTR_HBOX, []() { return new HBox; } },
		{ DUI_CTR_VBOX, []() { return new VBox; } },
		{ DUI_CTR_COMBO, []() { return new Combo; } },
		{ DUI_CTR_LABEL, []() { return new Label; } },
		{ DUI_CTR_BUTTON, []() { return new Button; } },
		{ DUI_CTR_OPTION, []() { return new Option; } },
		{ DUI_CTR_SLIDER, []() { return new Slider; } },
		{ DUI_CTR_TABBOX, []() { return new TabBox; } },
		{ DUI_CTR_CONTROL, []() { return new Control; } },
		{ DUI_CTR_TILEBOX, []() { return new TileBox; } },
		{ DUI_CTR_LISTBOX, []() { return new ListBox(new Layout); } },
		{ DUI_CTR_SKINBOX,	[]() { return new SkinBox; } },
		{ DUI_CTR_PROGRESS, []() { return new Progress; } },
		{ DUI_CTR_RICHEDIT, []() { return new RichEdit; } },
		{ DUI_CTR_CHECKBOX, []() { return new CheckBox; } },
		{ DUI_CTR_TREEVIEW, []() { return new TreeView; } },
		{ DUI_CTR_TREENODE, []() { return new TreeNode; } },
		{ DUI_CTR_HLISTBOX, []() { return new ListBox(new HLayout); } },
		{ DUI_CTR_VLISTBOX, []() { return new ListBox(new VLayout); } },
		{ DUI_CTR_CHILDBOX, []() { return new ChildBox; } },
		{ DUI_CTR_LABELBOX, []() { return new LabelBox; } },
		{ DUI_CTR_SCROLLBAR, []() { return new ScrollBar; } },
		{ DUI_CTR_BUTTONBOX, []() { return new ButtonBox; } },
		{ DUI_CTR_OPTIONBOX, []() { return new OptionBox; } },
		{ DUI_CTR_TILELISTBOX, []() { return new ListBox(new TileLayout); } },
		{ DUI_CTR_CHECKBOXBOX, []() { return new CheckBoxBox; } },
		{ DUI_CTR_VIRTUALLISTBOX, []() { return new VirtualListBox; } },
		{ DUI_CTR_CIRCLEPROGRESS, []() { return new CircleProgress; } },
		{ DUI_CTR_LISTCONTAINERELEMENT, []() { return new ListContainerElement; } },
	};
	const auto& itFun = _funcCreate.find(strControlClass);
	if (itFun != _funcCreate.end())
	{
		pControl = itFun->second();
		pControl->m_strControlClass = strControlClass;
	}
		
	return pControl;
}

void WindowBuilder::AttachXmlEvent(bool bBubbled, CMarkupNode& node, Control* pParent)
{
	auto nAttributes = node.GetAttributeCount();
	ui::string strType;
	ui::string strReceiver;
	ui::string strApplyAttribute;
	ui::string strName;
	ui::string strValue;
	for( int i = 0; i < nAttributes; i++ ) {
		strName = node.GetAttributeName(i);
		strValue = node.GetAttributeValue(i);
		ASSERT(i != 0 || strName == _T("type"));
		ASSERT(i != 1 || strName == _T("receiver"));
		ASSERT(i != 2 || strName == _T("applyattribute"));
		if( strName == _T("type") ) {
			strType = strValue;
		}
		else if( strName == _T("receiver") ) {
			strReceiver = strValue;
		}
		else if( strName == _T("applyattribute") ) {
			strApplyAttribute = strValue;
		}
	}

	auto typeList = StringHelper::Split(strType, _T(" "));
	auto receiverList = StringHelper::Split(strReceiver, _T(" "));
	for (auto itType = typeList.begin(); itType != typeList.end(); itType++) {
		for (auto itReceiver = receiverList.begin(); itReceiver != receiverList.end(); itReceiver++) {
			EventType eventType = StringToEnum(*itType);
			auto callback = nbase::Bind(&Control::OnApplyAttributeList, pParent, *itReceiver, strApplyAttribute, std::placeholders::_1);
			if (bBubbled == false) {
				pParent->AttachXmlEvent(eventType, callback);
			}
			else {
				if (Box* tmpParent = dynamic_cast<Box*>(pParent)) {
					tmpParent->AttachXmlBubbledEvent(eventType, callback);
				}
				else {
					ASSERT(FALSE);
				}

			}
		}
	}
}

} // namespace ui

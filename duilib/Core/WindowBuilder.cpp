#include "StdAfx.h"

namespace ui 
{

WindowBuilder::WindowBuilder()
{

}

Box* WindowBuilder::Create(STRINGorID xml, CreateControlCallback pCallback, 
	Window* pManager, Box* pParent, Box* pUserDefinedBox)
{
	//资源ID为0-65535，两个字节；字符串指针为4个字节
	//字符串以<开头认为是XML字符串，否则认为是XML文件
	//如果使用了 zip 压缩包，则从内存中读取
	if (HIWORD(xml.m_lpstr) != NULL) {
		if (*(xml.m_lpstr) == _T('<')) {
			if (!m_xml.Load(xml.m_lpstr)) return NULL;
		}
		else if (GlobalManager::IsUseZip()) {
			std::wstring sFile = GlobalManager::GetResourcePath();
			sFile += xml.m_lpstr;
			HGLOBAL hGlobal = GlobalManager::GetData(sFile);
			if (hGlobal)
			{
				std::string src((LPSTR)GlobalLock(hGlobal), GlobalSize(hGlobal));
				std::wstring string_resourse;
				StringHelper::MBCSToUnicode(src.c_str(), string_resourse, CP_UTF8);
				GlobalFree(hGlobal);
				if (!m_xml.Load(string_resourse.c_str())) return NULL;
			}
			else
			{
				if (!m_xml.LoadFromFile(xml.m_lpstr)) return NULL;
			}
		}
		else {
			if (!m_xml.LoadFromFile(xml.m_lpstr)) return NULL;
		}
	}
	else {
		ASSERT(FALSE);
	}

	return Create(pCallback, pManager, pParent, pUserDefinedBox);
}

Box* WindowBuilder::Create(CreateControlCallback pCallback, Window* pManager, Box* pParent, Box* pUserDefinedBox)
{
	m_createControlCallback = pCallback;
	CMarkupNode root = m_xml.GetRoot();
	if( !root.IsValid() ) return NULL;

	if( pManager ) {
		std::wstring strClass;
		int nAttributes = 0;
		std::wstring strName;
		std::wstring strValue;
		LPTSTR pstr = NULL;
		strClass = root.GetName();

		if( strClass == _T("Global") )
		{
			int nAttributes = root.GetAttributeCount();
			for( int i = 0; i < nAttributes; i++ ) {
				strName = root.GetAttributeName(i);
				strValue = root.GetAttributeValue(i);
				if( strName == _T("disabledfontcolor") ) {
					GlobalManager::SetDefaultDisabledTextColor(strValue);
				} 
				else if( strName == _T("defaultfontcolor") ) {	
					GlobalManager::SetDefaultTextColor(strValue);
				}
				else if( strName == _T("linkfontcolor") ) {
					DWORD clrColor = GlobalManager::GetTextColor(strValue);
					GlobalManager::SetDefaultLinkFontColor(clrColor);
				} 
				else if( strName == _T("linkhoverfontcolor") ) {
					DWORD clrColor = GlobalManager::GetTextColor(strValue);
					GlobalManager::SetDefaultLinkHoverFontColor(clrColor);
				} 
				else if( strName == _T("selectedcolor") ) {
					DWORD clrColor = GlobalManager::GetTextColor(strValue);
					GlobalManager::SetDefaultSelectedBkColor(clrColor);
				}
			}
		}
		else if( strClass == _T("Window") ) {
			if( pManager->GetHWND() ) {
				int nAttributes = root.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) {
					strName = root.GetAttributeName(i);
					strValue = root.GetAttributeValue(i);
					if( strName == ATTR_WINDOW_size) {
						LPTSTR pstr = NULL;
						int cx = _tcstol(strValue.c_str(), &pstr, 10);	ASSERT(pstr);    
						int cy = _tcstol(pstr + 1, &pstr, 10);	ASSERT(pstr); 
						pManager->SetInitSize(cx, cy);
					} 
					else if( strName == ATTR_WINDOW_heightpercent) {
						double lfHeightPercent = _ttof(strValue.c_str());
						pManager->SetHeightPercent(lfHeightPercent);

						MONITORINFO oMonitor = {}; 
						oMonitor.cbSize = sizeof(oMonitor);
						::GetMonitorInfo(::MonitorFromWindow(pManager->GetHWND(), MONITOR_DEFAULTTOPRIMARY), &oMonitor);
						int nWindowHeight = int((oMonitor.rcWork.bottom - oMonitor.rcWork.top) * lfHeightPercent);
						int nMinHeight = pManager->GetMinInfo().cy;
						int nMaxHeight = pManager->GetMaxInfo().cy;
						if (nMinHeight != 0 && nWindowHeight < nMinHeight) {
							nWindowHeight = nMinHeight;
						}
						if (nMaxHeight != 0 && nWindowHeight > nMaxHeight) {
							nWindowHeight = nMaxHeight;
						}

						CSize xy = pManager->GetInitSize();
						pManager->SetInitSize(xy.cx, nWindowHeight, false, false);
					}
					else if( strName == ATTR_WINDOW_sizebox) {
						UiRect rcSizeBox;
						LPTSTR pstr = NULL;
						rcSizeBox.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);    
						rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
						rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
						rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
						pManager->SetSizeBox(rcSizeBox);
					}
					else if( strName == ATTR_WINDOW_caption) {
						UiRect rcCaption;
						LPTSTR pstr = NULL;
						rcCaption.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);    
						rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
						rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
						rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
						pManager->SetCaptionRect(rcCaption);
					}
					else if( strName == ATTR_WINDOW_textid) {
						pManager->SetTextId(strValue);
					}
					else if( strName == ATTR_WINDOW_roundcorner) {
						LPTSTR pstr = NULL;
						int cx = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);    
						int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
						pManager->SetRoundCorner(cx, cy);
					} 
					else if( strName == ATTR_WINDOW_mininfo) {
						LPTSTR pstr = NULL;
						int cx = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);    
						int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
						pManager->SetMinInfo(cx, cy);
					}
					else if( strName == ATTR_WINDOW_maxinfo) {
						LPTSTR pstr = NULL;
						int cx = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);    
						int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
						pManager->SetMaxInfo(cx, cy);
					}
					else if( strName == ATTR_WINDOW_shadowattached) {
						pManager->SetShadowAttached(strValue == _T("true"));
					}
					else if (strName == ATTR_WINDOW_shadowimage) {
						pManager->SetShadowImage(strValue);
					}
					else if (strName == ATTR_WINDOW_shadowcorner) {
						UiRect rc;
						LPTSTR pstr = NULL;
						rc.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
						rc.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
						rc.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
						rc.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
						pManager->SetShadowCorner(rc);
					}
					else if (strName == ATTR_WINDOW_alphafixcorner || strName == ATTR_WINDOW_custom_shadow) {
						UiRect rc;
						LPTSTR pstr = NULL;
						rc.left = _tcstol(strValue.c_str(), &pstr, 10);  ASSERT(pstr);
						rc.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
						rc.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
						rc.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
						pManager->SetAlphaFixCorner(rc);
					}
				}
			}
		}

		if( strClass == _T("Global") ) {
			for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
				strClass = node.GetName();
				if( strClass == _T("Image") ) {
					ASSERT(FALSE);	//废弃
				}
				else if( strClass == _T("Font") ) {
					nAttributes = node.GetAttributeCount();
					std::wstring strFontId;
					std::wstring strFontName;
					int size = 12;
					bool bold = false;
					bool underline = false;
					bool italic = false;
					bool isDefault = false;
					for( int i = 0; i < nAttributes; i++ ) {
						strName = node.GetAttributeName(i);
						strValue = node.GetAttributeValue(i);
						if (strName == _T("id"))
						{
							strFontId = strValue;
						}
						else if( strName == _T("name") ) {
							strFontName = strValue;
						}
						else if( strName == _T("size") ) {
							size = _tcstol(strValue.c_str(), &pstr, 10);
						}
						else if( strName == _T("bold") ) {
							bold = (strValue == _T("true"));
						}
						else if( strName == _T("underline") ) {
							underline = (strValue == _T("true"));
						}
						else if( strName == _T("italic") ) {
							italic = (strValue == _T("true"));
						}
						else if( strName == _T("default") ) {
							isDefault = (strValue == _T("true"));
						}
					}
					if( !strFontName.empty() ) {
						GlobalManager::AddFont(strFontId, strFontName, size, bold, underline, italic, isDefault);
					}
				}
				else if( strClass == _T("Class") ) {
					nAttributes = node.GetAttributeCount();
					std::wstring strClassName;
					std::wstring strAttribute;
					for( int i = 0; i < nAttributes; i++ ) {
						strName = node.GetAttributeName(i);
						strValue = node.GetAttributeValue(i);
						if( strName == _T("name") ) {
							strClassName = strValue;
						}
						else if( strName == _T("value") ) {
							strAttribute.append(strValue);
						}
						else if (strName == _T("_value")) {
							strAttribute.append(StringHelper::Printf(L" value=\"%s\"",strValue.c_str()));
						}
						else {
							strAttribute.append(StringHelper::Printf(L" %s=\"%s\"",
								strName.c_str(), strValue.c_str()));
						}
					}
					if( !strClassName.empty() ) {
						StringHelper::TrimLeft(strAttribute);
						GlobalManager::AddClass(strClassName, strAttribute);
					}
				}
				else if( strClass == _T("TextColor") ) {
					nAttributes = node.GetAttributeCount();
					std::wstring strColorName;
					std::wstring strColor;
					for( int i = 0; i < nAttributes; i++ ) {
						strName = node.GetAttributeName(i);
						strValue = node.GetAttributeValue(i);
						if( strName == _T("name") ) {
							strColorName = strValue;
						}
						else if( strName == _T("value") ) {
							strColor = strValue;
						}
					}
					if( !strColorName.empty()) {
						GlobalManager::AddTextColor(strColorName, strColor);
					}
				}
			}
		}
		else if ( strClass == _T("Window") )
		{
			for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
				strClass = node.GetName();
				if( strClass == _T("Class") ) {
					nAttributes = node.GetAttributeCount();
					std::wstring strClassName;
					std::wstring strAttribute;
					for( int i = 0; i < nAttributes; i++ ) {
						strName = node.GetAttributeName(i);
						strValue = node.GetAttributeValue(i);
						if( strName == _T("name") ) {
							strClassName = strValue;
						}
						else if( strName == _T("value") ) {
							strAttribute.append(strValue);
						}
						else if (strName == _T("_value")) {
							strAttribute.append(StringHelper::Printf(L" value=\"%s\"", strValue.c_str()));
						}
						else {
							strAttribute.append(StringHelper::Printf(L" %s=\"%s\"",
								strName.c_str(), strValue.c_str()));
						}
					}
					if( !strClassName.empty() ) {
						ASSERT( GlobalManager::GetClassAttributes(strClassName).empty() );	//窗口中的Class不能与全局的重名
						StringHelper::TrimLeft(strAttribute);
						pManager->AddClass(strClassName, strAttribute);
					}
				}
			}
		}
	}

	for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
		std::wstring strClass = node.GetName();
		if( strClass == _T("Image") || strClass == _T("Font")
			|| strClass == _T("Class") || strClass == _T("TextColor") ) {

		}
		else {
			if (!pUserDefinedBox) {
				return (Box*)_Parse(&root, pParent, pManager);
			}
			else {
				int nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) {
					ASSERT(i == 0 || _tcscmp(node.GetAttributeName(i), _T("class")) != 0);	//class必须是第一个属性
					pUserDefinedBox->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
				}
				
				_Parse(&node, pUserDefinedBox, pManager);
				return pUserDefinedBox;
			}
		}
	}

	return nullptr;
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
        std::wstring strClass = node.GetName();
		if( strClass == _T("Image") || strClass == _T("Font")
			|| strClass == _T("Class") || strClass == _T("TextColor") ) {
				continue;
		}

        Control* pControl = NULL;
        if( strClass == _T("Include") ) {
            if( !node.HasAttributes() ) continue;
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
				if (strClass == L"Event" || strClass == L"BubbledEvent") {
					bool bBubbled = (strClass == L"BubbledEvent");
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
				ASSERT(i == 0 || _tcscmp(node.GetAttributeName(i), _T("class")) != 0);	//class必须是第一个属性
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
			if( pContainer == NULL ) return NULL;
			if( !pContainer->Add(pControl) ) {
				ASSERT(FALSE);
				delete pControl;
				continue;
			}
		}
        
        // Return first item
        if( pReturn == NULL ) pReturn = pControl;
    }
    return pReturn;
}

Control* WindowBuilder::CreateControlByClass(const std::wstring& strControlClass)
{
	Control* pControl = nullptr;
	static std::map<std::wstring, std::function<Control* (void)>> _funcCreate = 
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
		pControl = itFun->second();
	return pControl;
}

void WindowBuilder::AttachXmlEvent(bool bBubbled, CMarkupNode& node, Control* pParent)
{
	auto nAttributes = node.GetAttributeCount();
	std::wstring strType;
	std::wstring strReceiver;
	std::wstring strApplyAttribute;
	std::wstring strName;
	std::wstring strValue;
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

	auto typeList = StringHelper::Split(strType, L" ");
	auto receiverList = StringHelper::Split(strReceiver, L" ");
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

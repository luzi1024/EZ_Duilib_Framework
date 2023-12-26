#include "StdAfx.h"

namespace ui
{

class WindowComboList : public ui::WindowImplBase
{
public:
	WindowComboList(Combo* pOwn) : m_pOwner(pOwn), m_pList(nullptr) {};
	virtual ~WindowComboList() {};
	virtual ui::string GetWindowClassName() const override { return _T("WindowComboList"); };
	virtual ui::string GetSkinFolder() override { return _T(""); };
	virtual ui::string GetSkinFile() override { return _T(""); };
	virtual Box* BuilderContent() override
	{
		WindowBuilder builder;
		std::string sSkinData;
		ui::GlobalManager::GetWindowDefine(m_pOwner->m_sControlNameListwnd, sSkinData);
		if (!builder.LoadBuilder(sSkinData.c_str()))
			return nullptr;
		builder.BuilderWindow(this);
		return m_pOwner->m_pListBox.get();
	}
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override
	{
		bHandled = false;
		m_pOwner->FindButtonControl()->SetMouseEnabled(true);
		m_pOwner->FindButtonControl()->SetState(kControlStateNormal);
		m_pOwner->m_pWndList = NULL;
		m_pOwner->Invalidate();
		return 0;
	}
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override
	{
		bHandled = false;
		if (m_hWnd != (HWND)wParam) 
		{
			auto pRes = DetachDialog();
			assert(pRes == nullptr || pRes == m_pOwner->m_pListBox.get());
			Close();
		}
		return 0;
	}
	virtual void InitWindow() override { };

	void show()
	{
		CSize szDrop = m_pOwner->GetDropBoxSize();
		UiRect rcOwner = m_pOwner->GetPosWithScrollOffset();
		UiRect rc = rcOwner;
		rc.top = rc.bottom + 1;		// 父窗口left、bottom位置作为弹出窗口起点
		rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
		if (szDrop.cx > 0) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

		CSize szAvailable(rc.right - rc.left, rc.bottom - rc.top);
		int cyFixed = 0;
		for (int it = 0; it < m_pOwner->m_pListBox->GetCount(); it++) {
			Control* pControl = m_pOwner->m_pListBox->GetItemAt(it);
			if (!pControl->IsVisible()) 
				continue;
			CSize sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;
		}
		cyFixed += 2; // VBox 默认的Padding 调整
		rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

		::MapWindowRect(m_pOwner->GetWindow()->GetHWND(), HWND_DESKTOP, &rc);

		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(GetHWND(), MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		UiRect rcWork = oMonitor.rcWork;
		if (rc.bottom > rcWork.bottom || m_pOwner->IsPopupTop()) {
			rc.left = rcOwner.left;
			rc.right = rcOwner.right;
			if (szDrop.cx > 0) rc.right = rc.left + szDrop.cx;
			rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
			rc.bottom = rcOwner.top;
			::MapWindowRect(m_pOwner->GetWindow()->GetHWND(), HWND_DESKTOP, &rc);
		}

		HWND hWnd = Create(m_pOwner->GetWindow()->GetHWND(), _T(""), WS_POPUP, WS_EX_TOOLWINDOW, true, rc);
		if (hWnd == NULL)
			return;
	
		ShowWindow();
		// HACK: Don't deselect the parent's caption
		HWND hWndParent = m_hWnd;
		while (::GetParent(hWndParent) != NULL)
			hWndParent = ::GetParent(hWndParent);
		::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
	}
private:
	Combo* m_pOwner;
	ListBox* m_pList;
};

////////////////////////////////////////////////////////

Combo::Combo() :
	m_pWndList(nullptr),
	m_iCurSel(-1),
	m_szDropBox(0, 150),
	m_bPopupTop(false),
	m_sControlNameLabel(_T("")),
	m_sControlNameButton(_T("")),
	m_sControlNameListwnd(_T(""))
{
}

Combo::~Combo()
{
	m_pListBox.reset();
	m_pItemTemp.reset();
}

bool Combo::Add(Control* pControl)
{
	// 注册消息处理函数
	auto pButton = ((Box*)pControl)->FindSubControl(m_sControlNameButton);
	if (pButton)
	{
		pButton->AttachEvent(kEventClick, nbase::Bind(&Combo::OnClickControl, this, std::placeholders::_1));
	}
	return __super::Add(pControl);
}

bool Combo::AddItem(ListContainerElement* pItem)
{
	if (!m_pListBox)
		return false;
	m_pListBox->Add(pItem);
	pItem->SetReceivePointerMsg(true);
	m_iCurSel = m_pListBox->GetCurSel();
	return true;
}

bool Combo::AddString(const ui::string& str)
{
	if (!m_pItemTemp)
		return false; // ！未定义下拉项模板
	assert(m_pItemTemp->GetControlClass() == DUI_CTR_LISTCONTAINERELEMENT);
	ListContainerElement* element = new ListContainerElement(*m_pItemTemp);
	element->SetText(str);
	if (AddItem(element))
		return true;
	delete element;
	return false;
}

bool Combo::RemoveItem(ListContainerElement* pItem)
{
	if (!m_pListBox)
		return false;
	bool ret = m_pListBox->Remove(pItem);
	m_iCurSel = m_pListBox->GetCurSel();
	return ret;
}

bool Combo::RemoveItemAt(int iIndex)
{
	if (iIndex < 0)
		iIndex = GetItemCount() - iIndex;
	bool ret = m_pListBox->RemoveAt(iIndex);
	m_iCurSel = m_pListBox->GetCurSel();
	return ret;
}

void Combo::RemoveItemAll()
{
	m_pListBox->RemoveAll();
	m_iCurSel = -1;
}

void Combo::SetAttribute(const ui::string& strName, const ui::string& strValue)
{
	if (strName == ATTR_COMBO_vscrollbar) {}
	else if (strName == ATTR_COMBO_dropboxsize)
	{
		CSize szDropBoxSize;
		LPTSTR pstr = NULL;
		szDropBoxSize.cx = _tcstol(strValue.c_str(), &pstr, 10); ASSERT(pstr);
		szDropBoxSize.cy = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
		SetDropBoxSize(szDropBoxSize);
	}
	else if (strName == ATTR_COMBO_namelistwnd)
	{
		m_sControlNameListwnd = strValue;
		// 容器内部固定结构,可省略控件名称
		// -Window
		//	-VListBox
		//	 -ListContainerElement
		Box* pBox = GlobalManager::CreateWindowControl(m_sControlNameListwnd);
		ListBox* pListBox = dynamic_cast<ListBox*>(pBox);
		m_pListBox.reset(pListBox);
		if (m_pListBox)
		{
			int nItem = m_pListBox->GetCount();
			for (int i = 0; i < nItem; i++)
			{
				auto pItem = m_pListBox->GetItemAt(i);
				if (pItem->GetControlClass() == DUI_CTR_LISTCONTAINERELEMENT)
				{
					m_pListBox->SetAutoDestroy(false);
					m_pListBox->RemoveAt(0);
					m_pListBox->SetAutoDestroy(true);
					ListContainerElement* pCe = static_cast<ListContainerElement*>(pItem);
					m_pItemTemp.reset(pCe);
					break;
				}
			}
			m_pListBox->AttachSelect(nbase::Bind(&Combo::OnSelectItem, this, std::placeholders::_1));
		}
		else
			::MessageBox(GetWindow()->GetHWND(), StringHelper::Printf(_T("无效的'namelistwnd'属性:%s"), strValue.c_str()).c_str(), _T("错误"), MB_ICONERROR);
	}
	else if (strName == ATTR_COMBO_popuptop) SetPopupTop(strValue == _T("true"));
	else if (strName == ATTR_COMBO_namelabel) m_sControlNameLabel = strValue;
	else if (strName == ATTR_COMBO_namebutton) m_sControlNameButton = strValue;
	else Box::SetAttribute(strName, strValue);
}

ui::string Combo::GetText() const
{
	auto pControl = m_pListBox->GetItemAt(m_iCurSel);
	return pControl ? pControl->GetText() : _T("");
}

CSize Combo::GetDropBoxSize() const
{
    return m_szDropBox;
}

void Combo::SetDropBoxSize(CSize szDropBox)
{
	DpiManager::GetInstance()->ScaleSize(szDropBox);
    m_szDropBox = szDropBox;
}

bool Combo::SelectItem(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_pListBox->GetCount())
		return false;
	if (m_iCurSel == iIndex)
		return true;
	m_iCurSel = iIndex;
	m_pListBox->SelectItem(m_iCurSel, false, false);
	auto pLable = FindLabelControl();
	if (pLable)
		pLable->SetText(GetText());
	return true;
}

bool Combo::OnSelectItem(EventArgs* args)
{
	m_pWndList->PostMessage(WM_KILLFOCUS);
	m_iCurSel = m_pListBox->GetCurSel();
	auto pControl = m_pListBox->GetItemAt(m_iCurSel);
	if (pControl != NULL) 
		pControl->SetState(kControlStateNormal);
	if (m_pWndList != NULL)
		m_pWndList->SendNotify(this, kEventSelect, m_iCurSel, -1);
	auto pLable = FindLabelControl();
	if (pLable && pControl)
		pLable->SetText(pControl->GetText());
	return true;
}

bool Combo::OnClickControl(EventArgs* args)
{
	auto name = args->pSender->GetName();
	if (name == m_sControlNameButton)
	{
		if (!IsActivatable())
			return true;
		if (m_pListBox)
		{
			FindLabelControl()->SetFocus();
			FindButtonControl()->SetMouseEnabled(false);
			assert(m_pWndList == nullptr);
			m_pWndList = new WindowComboList(this);
			m_pWndList->show();
		}
		else
		{
			::MessageBox(GetWindow()->GetHWND(), _T("未设置'namelistwnd'属性."), _T("错误"), MB_ICONERROR);
		}
	}
	return true;
}

} // namespace ui

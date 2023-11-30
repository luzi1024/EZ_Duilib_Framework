#include "stdafx.h"
#include "main_form.h"

const std::wstring MainForm::kClassName = L"svgviewer";

MainForm::MainForm()
{
}


MainForm::~MainForm()
{
}

std::wstring MainForm::GetSkinFolder()
{
	return L"svgviewer";
}

std::wstring MainForm::GetSkinFile()
{
	return L"svgviewer.xml";
}

std::wstring MainForm::GetWindowClassName() const
{
	return kClassName;
}

void MainForm::InitWindow()
{
	//FindControl(_T("btn_ok"))->AttachEvent(ui::kEventClick, nbase::Bind(&MainForm::OnClickBtn, this, std::placeholders::_1));
	//ui::Label* pLab = new ui::Label;
	FindControl(_T("btn_ok"))->SetVisible(false);
	//pLab->SetText(_T("11111111222222233333"));
	//pLab->SetBkColor(_T("gray"));

	//ui::Box* pBox = dynamic_cast<ui::Box*>(m_pRoot->GetItemAt(0));
	//pBox->Add(pLab);

}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

bool MainForm::OnClickBtn(ui::EventArgs*)
{
#if 0
	int nRes = ui::MessageBox(GetHWND(), _T("测试可以123sd."), MB_OKCANCEL | MB_ICONWARNING);
	if (nRes == IDOK)
	{
		ui::MessageBoxFake(GetHWND(), _T("点击确定"));
	}
#endif
	ui::WindowJit* pWnd = new ui::WindowJit;
	pWnd->Create(GetHWND(), _T(""), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
	pWnd->CenterWindow();
	pWnd->ShowModalFake();
	return true;
}

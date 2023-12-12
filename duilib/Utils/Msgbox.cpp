#include "stdafx.h"
#include "msgbox.h"

namespace ui {	
	class MsgBox : public ui::WindowImplBase
	{
	public:
		MsgBox(const ui::string& st, UINT ntp = MB_OK) : sText(st), nType(ntp) {};
		virtual ~MsgBox() {};
		virtual UINT GetClassStyle() const override { return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS); };
		virtual ui::string GetWindowClassName() const override { return _T("MsgBox"); };
		virtual ui::string GetSkinFolder() override { return _T("msgbox"); };
		virtual ui::string GetSkinFile() override { return _T("msgbox.xml"); };
		virtual ui::string GetZIPFileName() const { return _T("msgbox.zip"); };
		virtual void InitWindow() override 
		{
			// Control
			auto pIcon		= FindControl(_T("icon"));
			auto pTitle		= FindControl(_T("title"));
			auto pContent	= FindControl(_T("content"));
			auto pBtnOK		= FindControl(_T("btn_ok"));
			auto pBtnCancel = FindControl(_T("btn_cancel"));
			ASSERT(pIcon && pTitle && pContent && pBtnOK && pBtnCancel);
			int nIcon = nType >> 4;
			int nFlag = nType & 0x000f;
			// Icon
			if (nIcon == MB_ICONWARNING >> 4)
			{
				pTitle->SetText(_T("警告"));
				pIcon->SetBkImage(_T("warning.png"));
			}
			else if (nIcon == MB_ICONQUESTION >> 4)
			{
				pTitle->SetText(_T("确认"));
				pIcon->SetBkImage(_T("question.png"));
			}
			else if (nIcon == MB_ICONINFORMATION >> 4)
			{
				pTitle->SetText(_T("消息"));
				pIcon->SetBkImage(_T("infor.png"));
			}
			else if (nIcon == MB_ICONERROR >> 4)
			{
				pTitle->SetText(_T("错误"));
				pIcon->SetBkImage(_T("error.png"));
			}
			else
			{
				pTitle->SetText(_T("警告"));
				pIcon->SetBkImage(_T("warning.png"));
			}
			// Btn
			if (nFlag == MB_OK)
			{
				pBtnOK->SetText(_T("确定"));
				pBtnOK->AttachEvent(ui::kEventClick, [this](ui::EventArgs*) { this->Close(IDOK); return true; });
				if (pBtnCancel->RemoveSelf() == false)
					ASSERT(FALSE);
			}
			else if (nFlag == MB_OKCANCEL)
			{
				pBtnOK->SetText(_T("确定"));
				pBtnCancel->SetText(_T("取消"));
				pBtnOK->AttachEvent(ui::kEventClick, [this](ui::EventArgs*) { this->Close(IDOK); return true; });
				pBtnCancel->AttachEvent(ui::kEventClick, [this](ui::EventArgs*) { this->Close(IDCANCEL); return true; });
			}
			else if (nFlag == MB_YESNO)
			{
				pBtnOK->SetText(_T("是"));
				pBtnCancel->SetText(_T("否"));
				pBtnOK->AttachEvent(ui::kEventClick, [this](ui::EventArgs*) { this->Close(IDYES); return true; });
				pBtnCancel->AttachEvent(ui::kEventClick, [this](ui::EventArgs*) { this->Close(IDNO); return true; });
			}
			else
				ASSERT(FALSE);
			// Text
			pContent->SetText(sText);
		};
	private:
		UINT nType;
		ui::string sText;
	};

	UINT MessageBox(HWND hwndParent, const ui::string& s, UINT ntype /*= MB_OK*/)
	{
		ASSERT(::IsWindow(hwndParent));
		MsgBox* msgbox = new MsgBox(s, ntype);
		HWND hWnd = msgbox->Create(hwndParent, _T(""), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
		if (hWnd == NULL)
			return 0;
		msgbox->CenterWindow();
		return msgbox->ShowModal();
	}

	void MessageBoxFake(HWND hwndParent, const ui::string& s, UINT ntype/* = MB_OK*/, std::function<void(Window* self, UINT ret)> cb /*= nullptr*/)
	{
		ASSERT(::IsWindow(hwndParent));
		MsgBox* msgbox = new MsgBox(s, ntype);
		HWND hWnd = msgbox->Create(hwndParent, _T(""), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
		if (hWnd == NULL)
			return;
		msgbox->CenterWindow();
		msgbox->ShowModalFake(cb);
	}
}

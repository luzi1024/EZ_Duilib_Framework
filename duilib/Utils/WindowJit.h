#pragma once
#include "duilib/UIlib.h"

namespace ui
{
	class UILIB_API WindowJit : public WindowImplBase
	{
	public:
		WindowJit() {
			_skin = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
				_T("<Window size=\"540,320\" caption=\"0,0,0,34\">")
				_T("<VBox bkcolor=\"bk_wnd_darkcolor\">")
				_T("<HBox class=\"caption\">")
				_T("<Label class=\"font_title\" name=\"title\" width=\"stretch\" valign=\"center\" normaltextcolor=\"darkcolor\" />")
				_T("<Button class=\"btn_wnd_close\" name=\"closebtn\" margin = \"10,6,0,0\" />")
				_T("</HBox>")
				_T("</VBox>")
				_T("</Window>");
		};
		~WindowJit() {};

		virtual bool IsSkinJit() override { return true; };
		virtual ui::string GetWindowClassName() const override { return _T("WindowJit"); };
		virtual ui::string GetSkinFolder() override { return _T("public"); };
		virtual ui::string GetSkinFile() override { return _skin; };
		virtual void InitWindow() override
		{
			// Ê¾Àý
			FindControl(_T("title"))->SetText(_T("Jit´°¿Ú"));

			ui::Label* pLab = new ui::Label;
			pLab->SetText(_T("Hello!"));
			pLab->SetMargin(ui::UiRect(5, 5, 5, 5));

			ui::RichEdit* pEdit = new ui::RichEdit;
			pEdit->SetClass(_T("simple input"));
			pEdit->SetText(_T("World!"));
			pEdit->SetMaxHeight(30);
			pEdit->SetMaxWidth(200);
			pEdit->SetMargin(ui::UiRect(5, 5, 5, 5));
			pEdit->GetLayout()->SetPadding(ui::UiRect(6, 6, 6, 0));

			ui::Button* pBtn = new ui::Button;
			pBtn->SetClass(_T("btn_global_white_80x30"));
			pBtn->SetText(_T("OK"));
			pBtn->SetMargin(ui::UiRect(5,5,5,5));
			pBtn->AttachEvent(ui::kEventClick, [this](auto s) { this->Close(); return true; });

			ui::Box* pBox = dynamic_cast<ui::Box*>(m_pRoot->GetItemAt(0));
			pBox->Add(pLab);
			pBox->Add(pEdit);
			pBox->Add(pBtn);
		}
	private:
		ui::string _skin;
	};

};
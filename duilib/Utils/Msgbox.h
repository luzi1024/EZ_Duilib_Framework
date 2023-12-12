#pragma once
#include "duilib/UIlib.h"

namespace ui{

	/**
	 * \brief 以模态方式弹出消息框.
	 * \ep.
	 *		int nRes = ui::MessageBox(GetHWND(), _T("DoModal box."), MB_OKCANCEL | MB_ICONWARNING);
	 *		if (nRes == IDOK) { ... }
	 * 
	 *		ui::MessageBoxFake(GetHWND(), _T("Fake DoModal box."), MB_OKCANCEL | MB_ICONWARNING, [](Window* self, UINT ret){
	 *			if (ret == IDOK) { ... }
	 *		});
	 *		
	 * @return 返回对应的按钮类型ID值. 若点击标题栏关闭按钮则返回0
	 * @param hwndParent[in] 父窗口句柄,必须传入有效的句柄.
	 * @param s[in] 消息内容
	 * @param ntype[in] 图标及按钮类型
	 * 支持的按钮类型 :
	 *		MB_OK
	 *		MB_OKCANCEL
	 *		MB_YESNO
	 * 支持的图标类型 :
	 *		MB_ICONWARNING
	 *		MB_ICONQUESTION
	 *		MB_ICONINFORMATION
	 *		MB_ICONERROR
	 */
	UINT MessageBox(HWND hwndParent, const ui::string& s, UINT ntype = MB_OK);

	/**
	 * \brief 以异步方式模拟模态对话框.
	 * \details
	 *
	 * @return 无.
	 * @param hwndParent[in] 同上.
	 * @param s[in] 同上
	 * @param ntype[in] 同上
	 * @param cb[in] 对话框返回的回调,参数为自身指针及点击按钮类型ID值.
	 */
	void MessageBoxFake(HWND hwndParent, const ui::string& s, UINT ntype = MB_OK, std::function<void(Window* self, UINT ret)> cb = nullptr);
}

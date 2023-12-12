#pragma once
#include "duilib/UIlib.h"

namespace ui{

	/**
	 * \brief ��ģ̬��ʽ������Ϣ��.
	 * \ep.
	 *		int nRes = ui::MessageBox(GetHWND(), _T("DoModal box."), MB_OKCANCEL | MB_ICONWARNING);
	 *		if (nRes == IDOK) { ... }
	 * 
	 *		ui::MessageBoxFake(GetHWND(), _T("Fake DoModal box."), MB_OKCANCEL | MB_ICONWARNING, [](Window* self, UINT ret){
	 *			if (ret == IDOK) { ... }
	 *		});
	 *		
	 * @return ���ض�Ӧ�İ�ť����IDֵ. ������������رհ�ť�򷵻�0
	 * @param hwndParent[in] �����ھ��,���봫����Ч�ľ��.
	 * @param s[in] ��Ϣ����
	 * @param ntype[in] ͼ�꼰��ť����
	 * ֧�ֵİ�ť���� :
	 *		MB_OK
	 *		MB_OKCANCEL
	 *		MB_YESNO
	 * ֧�ֵ�ͼ������ :
	 *		MB_ICONWARNING
	 *		MB_ICONQUESTION
	 *		MB_ICONINFORMATION
	 *		MB_ICONERROR
	 */
	UINT MessageBox(HWND hwndParent, const ui::string& s, UINT ntype = MB_OK);

	/**
	 * \brief ���첽��ʽģ��ģ̬�Ի���.
	 * \details
	 *
	 * @return ��.
	 * @param hwndParent[in] ͬ��.
	 * @param s[in] ͬ��
	 * @param ntype[in] ͬ��
	 * @param cb[in] �Ի��򷵻صĻص�,����Ϊ����ָ�뼰�����ť����IDֵ.
	 */
	void MessageBoxFake(HWND hwndParent, const ui::string& s, UINT ntype = MB_OK, std::function<void(Window* self, UINT ret)> cb = nullptr);
}

#pragma once

namespace ui 
{

class WindowComboList;
class UILIB_API Combo : public Box
{
	friend class WindowComboList;
public:
	Combo();
	Combo(const Combo& r) = delete;
	Combo& operator=(const Combo& r) = delete;
	virtual ~Combo();
	/**
	 * @brief ��ȡ��ǰѡ�����ı�
	 * @return ���ص�ǰѡ�����ı�
	 */
	virtual ui::string GetText() const override;

	/**
	 * @brief ���������
	 * @return ��
	 */
	bool AddItem(ListContainerElement* pItem);

	/**
	 * @brief ����������ַ���
	 * @return ��
	 */
	bool AddString(const ui::string& str);

	/**
	 * @brief �Ƴ�ָ��������
	 * @return �Ƴ��ɹ�True
	 */
	bool RemoveItem(ListContainerElement* pControl);

	/**
	 * @brief ͨ�������Ƴ�������,���Ϊ������ʾ��������
	 * @return �Ƴ��ɹ�True
	 */
	bool RemoveItemAt(int iIndex);

	/**
	 * @brief �Ƴ�����������
	 * @return ��
	 */
	void RemoveItemAll();

	/**
	 * @brief ��ȡ��ǰ������ List ����
	 * @return ���������� List ����ָ��
	 */
	ListBox* GetListBox() { return m_pListBox.get(); }

	/**
	 * @brief ��ȡ������������С
	 * @return ����������С
	 */
    CSize GetDropBoxSize() const;

	/**
	 * @brief ����������������С
	 * @param[in] szDropBox Ҫ���õĴ�С��Ϣ
	 * @return ��
	 */
    void SetDropBoxSize(CSize szDropBox);
	
	/**
	 * @brief ���� Combobox �Ƿ����ϵ���
	 * @param[in] top Ϊ true �����ϵ�����false ΪĬ�����µ���
	 * @return ��
	 */
	void SetPopupTop(bool top) { m_bPopupTop = top; };
	
	/**
	 * @brief �ж� Combobox ����ģʽ�Ƿ������ϵ���
	 * @return ���� true ��ʾ���ϵ���������Ϊ false
	 */
	bool IsPopupTop() const { return m_bPopupTop; };

	/**
	 * @brief ѡ��һ������
	 * @param[in] iIndex Ҫѡ�����������
	 * @return ���� true ��ʾ�ɹ�������Ϊ false
	 */
	bool SelectItem(int iIndex);

	/**
	 * @brief ��ȡ��ǰѡ��������
	 * @return ���ص�ǰѡ��������
	 */
	int GetCurSel() const { return m_iCurSel; }

	/**
	 * @brief ��ȡ������������
	 * @return ����������������
	 */
	int GetItemCount() const { return m_pListBox->GetCount(); }
    
protected:
	// ���ظ�����������
	virtual bool Add(Control* pControl) override;
	virtual void SetAttribute(const ui::string& strName, const ui::string& strValue) override;
	Control* FindLabelControl() { return FindSubControl(m_sControlNameLabel); };
	Control* FindButtonControl() { return FindSubControl(m_sControlNameButton); };
private:
	/**
	 * @brief Ĭ�ϵ����ѡ������
	 * @param[in] args �����б�
	 * @return ʼ�շ��� true
	 */
	bool OnSelectItem(EventArgs* args);

	/**
	 * @brief Ĭ�ϵ��ӿؼ����������
	 * @param[in] args �����б�
	 * @return ʼ�շ��� true
	 */
	bool OnClickControl(EventArgs* args);
protected:
	WindowComboList* m_pWndList;
	std::unique_ptr<ListBox> m_pListBox;
	std::unique_ptr<ListContainerElement> m_pItemTemp;
    int m_iCurSel;  
	CSize m_szDropBox;
	bool m_bPopupTop;
	ui::string m_sControlNameLabel;
	ui::string m_sControlNameButton;
	ui::string m_sControlNameListwnd;
};

} // namespace ui


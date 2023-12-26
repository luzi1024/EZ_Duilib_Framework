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
	 * @brief 获取当前选择项文本
	 * @return 返回当前选择项文本
	 */
	virtual ui::string GetText() const override;

	/**
	 * @brief 添加下拉项
	 * @return 无
	 */
	bool AddItem(ListContainerElement* pItem);

	/**
	 * @brief 添加下拉项字符串
	 * @return 无
	 */
	bool AddString(const ui::string& str);

	/**
	 * @brief 移除指定下拉项
	 * @return 移除成功True
	 */
	bool RemoveItem(ListContainerElement* pControl);

	/**
	 * @brief 通过索引移除下拉项,如果为负数表示反向索引
	 * @return 移除成功True
	 */
	bool RemoveItemAt(int iIndex);

	/**
	 * @brief 移除所有下拉项
	 * @return 无
	 */
	void RemoveItemAll();

	/**
	 * @brief 获取当前所属的 List 对象
	 * @return 返回所属的 List 对象指针
	 */
	ListBox* GetListBox() { return m_pListBox.get(); }

	/**
	 * @brief 获取下拉框容器大小
	 * @return 返回容器大小
	 */
    CSize GetDropBoxSize() const;

	/**
	 * @brief 设置下拉框容器大小
	 * @param[in] szDropBox 要设置的大小信息
	 * @return 无
	 */
    void SetDropBoxSize(CSize szDropBox);
	
	/**
	 * @brief 设置 Combobox 是否向上弹出
	 * @param[in] top 为 true 则向上弹出，false 为默认向下弹出
	 * @return 无
	 */
	void SetPopupTop(bool top) { m_bPopupTop = top; };
	
	/**
	 * @brief 判断 Combobox 弹出模式是否是向上弹出
	 * @return 返回 true 表示向上弹出，否则为 false
	 */
	bool IsPopupTop() const { return m_bPopupTop; };

	/**
	 * @brief 选择一个子项
	 * @param[in] iIndex 要选择的子项索引
	 * @return 返回 true 表示成功，否则为 false
	 */
	bool SelectItem(int iIndex);

	/**
	 * @brief 获取当前选择项索引
	 * @return 返回当前选择项索引
	 */
	int GetCurSel() const { return m_iCurSel; }

	/**
	 * @brief 获取所有子项数量
	 * @return 返回所有子项数量
	 */
	int GetItemCount() const { return m_pListBox->GetCount(); }
    
protected:
	// 重载父类容器方法
	virtual bool Add(Control* pControl) override;
	virtual void SetAttribute(const ui::string& strName, const ui::string& strValue) override;
	Control* FindLabelControl() { return FindSubControl(m_sControlNameLabel); };
	Control* FindButtonControl() { return FindSubControl(m_sControlNameButton); };
private:
	/**
	 * @brief 默认的子项被选择处理函数
	 * @param[in] args 参数列表
	 * @return 始终返回 true
	 */
	bool OnSelectItem(EventArgs* args);

	/**
	 * @brief 默认的子控件被点击函数
	 * @param[in] args 参数列表
	 * @return 始终返回 true
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


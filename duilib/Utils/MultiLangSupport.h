#ifndef UI_UTILS_MULTILANGSUPPORT_H_
#define UI_UTILS_MULTILANGSUPPORT_H_

#pragma once

namespace ui 
{

class UILIB_API MutiLanSupport
{
public:
	static MutiLanSupport* GetInstance();

	/**
	 * @brief 根据ID获取指定语言的字符串
	 * @param[in] id 指定字符串 ID
	 * @return 返回 ID 对应的语言字符串
	 */
	ui::string GetStringViaID(const ui::string& id);

	/**
	 * @brief 加载所有语言映射表
	 * @param[in] strFilePath 语言文件路径
	 * @return 返回 true 表示成功，返回 false 表示失败
	 */
	bool LoadStringTable(const ui::string& strFilePath);

	/**
	 * @brief 从内存中加载所有语言映射表
	 * @param[in] hGlobal 要加载的语言映射表内存首地址
	 * @return 返回 true 表示成功，返回 false 表示失败
	 */
	bool LoadStringTable(const HGLOBAL& hGlobal);

private:
	MutiLanSupport() {};
	~MutiLanSupport() {};
	MutiLanSupport(const MutiLanSupport&) = delete;
	MutiLanSupport& operator = (const MutiLanSupport&) = delete;

	/**
	 * @brief 清理所有
	 * @return 无
	 */
	void ClearAll();

	/**
	 * @brief 分析语言映射表内容
	 * @param[in] list 读取出来的映射表内容列表
	 * @return 成功返回 true，否则返回 false
	 */
	bool AnalyzeStringTable(const std::vector<ui::string>& list);

private: 
	std::map<ui::string, ui::string>  m_stringTable;
};

}
#endif //UI_UTILS_MULTILANGSUPPORT_H_

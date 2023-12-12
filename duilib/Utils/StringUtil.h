#ifndef UI_UTILS_STRINGUTIL_H_
#define UI_UTILS_STRINGUTIL_H_

namespace ui
{

class UILIB_API StringHelper
{
public:
	static ui::string ReparsePath(const ui::string& strFilePath);

	// format a string
	static ui::string Printf(const TCHAR* format, ...);

	// replace all 'find' with 'replace' in the string
	static size_t ReplaceAll(const ui::string& find, const ui::string& replace, ui::string& output);

	static ui::string MakeLowerString(const ui::string&str);
	static ui::string MakeUpperString(const ui::string&str);

	static bool MBCSToUnicode(const char *input, std::wstring& output, int code_page = CP_ACP);
	static bool MBCSToUnicode(const std::string &input, std::wstring& output, int code_page = CP_ACP);
	static bool UnicodeToMBCS(const wchar_t *input, std::string &output, int code_page = CP_ACP);
	static bool UnicodeToMBCS(const std::wstring& input, std::string &output, int code_page = CP_ACP);

	// UTF8 to GB2312  
	static std::string U2G(const char* utf8);
	// GB2312 to UTF8   
	static std::string G2U(const char* gb2312);

	// trimming, removing extra spaces
	static std::string TrimLeft(const char *input);
	static std::string TrimRight(const char *input);
	static std::string Trim(const char *input); /* both left and right */
	static std::string& TrimLeft(std::string &input);
	static std::string& TrimRight(std::string &input);
	static std::string& Trim(std::string &input); /* both left and right */
	static std::wstring TrimLeft(const wchar_t *input);
	static std::wstring TrimRight(const wchar_t *input);
	static std::wstring Trim(const wchar_t *input); /* both left and right */
	static std::wstring& TrimLeft(std::wstring &input);
	static std::wstring& TrimRight(std::wstring &input);
	static std::wstring& Trim(std::wstring &input); /* both left and right */

	// find all tokens splitted by one of the characters in 'delimitor'
	static std::list<ui::string> Split(const ui::string& input, const ui::string& delimitor);

	//
	static bool StartWith(const ui::string& str, const ui::string& head);
	static bool EndWith(const ui::string& str, const ui::string& tail);
};

}

#endif // UI_UTILS_STRINGUTIL_H_

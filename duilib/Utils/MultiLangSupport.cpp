#include "StdAfx.h"

namespace ui 
{

MutiLanSupport* MutiLanSupport::GetInstance()
{
	static MutiLanSupport mutiLanSupport;
	return &mutiLanSupport;
}

bool MutiLanSupport::LoadStringTable(const ui::string &strFilePath)
{
	ClearAll();

	FILE *hFile;
	_tfopen_s(&hFile, strFilePath.c_str(), _T("r"));
	if (hFile == NULL) {
		return false;
	}

	TCHAR strRead[4096];
	std::vector<ui::string> stringList;
	while (_fgetts(strRead, 4096, hFile) != NULL)
	{
		ui::string strResource = strRead;
		strResource = StringHelper::TrimLeft(strResource);
		strResource = StringHelper::TrimRight(strResource);
		if (!strResource.empty()) {
			stringList.emplace_back(strResource);
		}
	}
	fclose(hFile);

	AnalyzeStringTable(stringList);
	return true;
}

bool MutiLanSupport::LoadStringTable(const HGLOBAL& hGlobal)
{
	std::vector<ui::string> string_list;
	ui::string fragment((LPTSTR)GlobalLock(hGlobal), GlobalSize(hGlobal));
	fragment.append(_T("\n"));
	ui::string src;
	for (auto& it : fragment)
	{
		if (it == _T('\0') || it == _T('\n'))
		{
			ui::string string_resourse = src;
			string_resourse = StringHelper::TrimLeft(string_resourse);
			string_resourse = StringHelper::TrimRight(string_resourse);
			if (!string_resourse.empty())
			{
				string_list.emplace_back(string_resourse);
			}
			src.clear();
			continue;
		}
		src.push_back(it);
	}

	GlobalUnlock(hGlobal);
	AnalyzeStringTable(string_list);

	return true;
}

ui::string MutiLanSupport::GetStringViaID(const ui::string& id)
{
	ui::string text;
	if(id.empty())
		return text;

	auto it = m_stringTable.find(id);
	if (it == m_stringTable.end()) {
		ASSERT(FALSE);
		return text;
	}
	else {
		text = it->second;
		StringHelper::ReplaceAll(_T("\\r"), _T("\r"), text);
		StringHelper::ReplaceAll(_T("\\n"), _T("\n"), text);
	}

	return text;
}

bool MutiLanSupport::AnalyzeStringTable(const std::vector<ui::string> &list)
{
	int	nCount = (int)list.size();
	if (nCount <= 0)
		return false;
	
	for(int i = 0; i < nCount; i++)
	{
		ui::string strSrc = list[i];
		std::list<ui::string> idAndString = StringHelper::Split(strSrc, _T("="));
		if (idAndString.size() != 2) {
			continue;
		}

		ui::string id = *(idAndString.begin());
		id = StringHelper::TrimLeft(id);
		id = StringHelper::TrimRight(id);

		idAndString.pop_front();

		ui::string strResource = *(idAndString.begin());
		strResource = StringHelper::TrimLeft(strResource);
		strResource = StringHelper::TrimRight(strResource);

		if (id.find(_T(";")) == -1) {
			m_stringTable[id] = strResource;
		}
	}

	return true;
}

void MutiLanSupport::ClearAll()
{
	m_stringTable.clear();
}

}
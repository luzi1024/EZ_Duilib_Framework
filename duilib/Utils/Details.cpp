#include "StdAfx.h"

namespace ui
{
	bool Details::HasFile(const std::wstring& sf)
	{
		int r = _taccess(sf.c_str(), 0);
		if (-1 == r)
			return false;
		return true;
	}

} // namespace nbase

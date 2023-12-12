#include "StdAfx.h"
#include <io.h>

namespace ui
{
	bool Details::HasFile(const ui::string& sf)
	{
		int r = _taccess(sf.c_str(), 0);
		if (-1 == r)
			return false;
		return true;
	}

} // namespace nbase

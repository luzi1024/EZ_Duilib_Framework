﻿#ifndef BASE_FILE_FILE_PATH_H_
#define BASE_FILE_FILE_PATH_H_

#include <string>

namespace nbase
{

#if defined(OS_POSIX)
typedef std::string PathString;
#elif defined(OS_WIN)
#ifdef _UNICODE	
	typedef std::wstring PathString;
#else
	typedef std::string PathString;
#endif
#endif

typedef PathString::value_type PathChar;

} // namespace nbase

#endif // BASE_FILE_FILE_PATH_H_

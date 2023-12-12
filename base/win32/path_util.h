// utilities for path operation

#ifndef BASE_WIN32_PATH_UTIL_H_
#define BASE_WIN32_PATH_UTIL_H_

#include "base/base_config.h"

#if defined(OS_WIN)

#include "base/base_export.h"
#include <string>

namespace nbase
{
namespace win32
{

BASE_EXPORT UIString GetModulePathName(HMODULE module_handle);
BASE_EXPORT UIString GetModuleDirectory(HMODULE module_handle);
BASE_EXPORT UIString GetModuleName(HMODULE module_handle);
BASE_EXPORT UIString GetCurrentModuleName();
BASE_EXPORT UIString GetCurrentModulePathName();
BASE_EXPORT UIString GetCurrentModuleDirectory();
BASE_EXPORT UIString GetWindowsDir();
BASE_EXPORT UIString GetSystemDir();
BASE_EXPORT UIString GetTempDir();
BASE_EXPORT UIString GetLocalAppDataDir(HANDLE token = NULL);

BASE_EXPORT bool CreateDirectoryRecursively(const char *full_dir);
BASE_EXPORT bool DeleteDirectoryRecursively(const char *full_dir);

}
}

#endif // OS_WIN
#endif // BASE_WIN32_PATH_UTIL_H_
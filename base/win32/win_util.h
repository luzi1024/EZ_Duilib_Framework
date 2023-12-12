#ifndef BASE_WIN32_WIN_UTIL_H_
#define BASE_WIN32_WIN_UTIL_H_

#include "base/base_config.h"

#if defined(OS_WIN)

#include "base/base_export.h"
#include <string>

namespace nbase
{
namespace win32
{

BASE_EXPORT bool IsAddressInCurrentModule(void* address);
BASE_EXPORT bool IsModuleHandleValid(HMODULE module_handle);
BASE_EXPORT HMODULE GetModuleHandleFromAddress(void* address);
BASE_EXPORT HMODULE GetCurrentModuleHandle();

BASE_EXPORT UIString GetHostName();

BASE_EXPORT bool RunApp(const TCHAR *application, HANDLE *process = NULL);
BASE_EXPORT bool RunAppWithCommand(const TCHAR* application, const TCHAR *command, HANDLE *process = NULL);
BASE_EXPORT bool RunAppWithRedirection(const TCHAR *application,
									const TCHAR *command,
									HANDLE input,
									HANDLE output,
									HANDLE error,
									HANDLE *process = NULL);

BASE_EXPORT bool MinimizeProcessWorkingSize();
BASE_EXPORT bool SingletonRun(const TCHAR *application);

BASE_EXPORT bool IsRunningOnVistaOrHigher();

BASE_EXPORT bool OpenResource(const TCHAR *resource,
						   const TCHAR *type,
						   void *&data,
						   unsigned long &size,
						   HMODULE module = NULL);

} // namespace win32
} // namespace nbase

#endif // OS_WIN
#endif // BASE_WIN32_WIN_UTIL_H_

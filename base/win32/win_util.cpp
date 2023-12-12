/*
 *	Author		wrt(guangguang)
 *	Date		2011-06-14
 *	Copyright	Hangzhou, Netease Inc.
 *	Brief		Utilities for Windows applications
 */

#include "base/win32/win_util.h"
#if defined(OS_WIN)
#include <assert.h>
#include <stdlib.h>

namespace nbase
{
namespace win32
{

HMODULE GetModuleHandleFromAddress(void* address)
{
	MEMORY_BASIC_INFORMATION mbi = {0};
	DWORD result = ::VirtualQuery(address, &mbi, sizeof(mbi));
	assert(result == sizeof(mbi));
	return static_cast<HMODULE>(mbi.AllocationBase);
}

HMODULE GetCurrentModuleHandle()
{
	return GetModuleHandleFromAddress(GetCurrentModuleHandle);
}

bool IsAddressInCurrentModule(void* address)
{
	return GetCurrentModuleHandle() == GetModuleHandleFromAddress(address);
}

UIString GetHostName()
{
	UIString host_name;
	DWORD name_len = MAX_COMPUTERNAME_LENGTH + 1;
	host_name.resize(name_len);
	bool result = !!::GetComputerName(&host_name[0], &name_len);
	assert(result);
	host_name.resize(name_len);
	return host_name;
}

bool IsModuleHandleValid(HMODULE module_handle)
{
	if (!module_handle)
		return true;
	return module_handle == GetModuleHandleFromAddress(module_handle);
}

bool RunApp(const TCHAR* application, HANDLE *process)
{
	return RunAppWithRedirection(application, NULL, NULL, NULL, NULL, process);
}

bool RunAppWithCommand(const TCHAR* application, const TCHAR* command, HANDLE *process)
{
	return RunAppWithRedirection(application, command, NULL, NULL, NULL, process);
}

bool RunAppWithRedirection(const TCHAR* application,
						   const TCHAR* command,
						   HANDLE input,
						   HANDLE output,
						   HANDLE error,
						   HANDLE *process)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	memset(&si, 0, sizeof(si));

	if (!!input || !!output || !!error)
		si.dwFlags = STARTF_USESTDHANDLES;

	si.cb			= sizeof(si);
	si.hStdInput	= input ? input : ::GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput	= output ? output : ::GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError	= error ? error : ::GetStdHandle(STD_ERROR_HANDLE);

	TCHAR* command_dup = _tcsdup(command);

	if (::CreateProcess(application,
						 command_dup,
						 NULL,
						 NULL,
						 (si.dwFlags & STARTF_USESTDHANDLES) ? TRUE : FALSE,
						 CREATE_NO_WINDOW|CREATE_SUSPENDED,
						 NULL,
						 NULL,
						 &si,
						 &pi))
	{
		::CloseHandle(pi.hThread);
		if (process == NULL)
			::CloseHandle(pi.hProcess);
		else
			*process = pi.hProcess;
		free(command_dup);
		return true;
	}

	free(command_dup);
	return false;
}

bool MinimizeProcessWorkingSize()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	::GetVersionEx(&osvi);
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		::SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
		return true;
	}

	return false;
}

bool SingletonRun(const TCHAR* application)
{
	assert(application);
	if (application == NULL)
		return false;

	HANDLE hMutex = ::CreateMutex(NULL, TRUE, application);

	if (hMutex == NULL)
		return false;

	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		::CloseHandle(hMutex);
		return false;
	}

	/* 从此这个互斥信号量就被不受控地打开了，直到进程退出 */
	return true;
}

bool IsRunningOnVistaOrHigher()
{
	OSVERSIONINFO os_version = {0};
	os_version.dwOSVersionInfoSize = sizeof(os_version);
	GetVersionEx(&os_version);
	return os_version.dwMajorVersion >= 6;
}

bool OpenResource(const TCHAR* resource,
				  const TCHAR* type,
				  void *&data,
				  unsigned long &size,
				  HMODULE module)
{
	HRSRC resource_handle = ::FindResource(module,
											resource,
											type);
	if (resource_handle == NULL)
		return false;
	HGLOBAL data_handle = ::LoadResource(module, resource_handle);
	if (data_handle == NULL)
		return false;
	size = ::SizeofResource(module, resource_handle);
	if (size == 0)
		return false;
	data = ::LockResource(data_handle);
	if (data == NULL)
		return false;
	return true;
}

} // namespace win32
} // namespace nbase

#endif // OS_WIN

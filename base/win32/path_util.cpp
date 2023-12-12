// Copyright (c) 2012, NetEase Inc. All rights reserved.
//
// wrt(guangguang)
// 2012/5/31
//
// utilities for path operation

#include "base/win32/path_util.h"
#if defined(OS_WIN)
#include <shlobj.h> // SHCreateDirectory
#include <shellapi.h> // SHFileOperation
#include "base/file/file_util.h"
//#include "base/log/log.h"
#include "base/win32/win_util.h"

namespace nbase
{
namespace win32
{

	UIString GetModulePathName(HMODULE module_handle)
{
	//DCHECK(IsModuleHandleValid(module_handle));
		UIString mod_path;
	mod_path.resize(MAX_PATH);
	mod_path.resize(::GetModuleFileName(module_handle, &mod_path[0], (DWORD)mod_path.size()));
	return mod_path;
}

	UIString GetModuleDirectory(HMODULE module_handle)
{
	//DCHECK(IsModuleHandleValid(module_handle));
		UIString module_directory;
	if (FilePathApartDirectory(GetModulePathName(module_handle), module_directory))
		return module_directory;
	return _T("");
}

	UIString GetModuleName(HMODULE module_handle)
{
	//DCHECK(IsModuleHandleValid(module_handle));
		UIString module_name;
	if (FilePathApartFileName(GetModulePathName(module_handle), module_name))
		return module_name;
	return _T("");
}

	UIString GetCurrentModulePathName()
{
	return GetModulePathName(GetCurrentModuleHandle());
}

	UIString GetCurrentModuleDirectory()
{
	return GetModuleDirectory(GetCurrentModuleHandle());
}

	UIString GetCurrentModuleName()
{
	return GetModuleName(GetCurrentModuleHandle());
}

	UIString GetWindowsDir()
{
		UIString windows_path;
	windows_path.resize(MAX_PATH);
	DWORD result = ::GetWindowsDirectory(&windows_path[0], MAX_PATH);
	windows_path.resize(result);
	return windows_path;
}

	UIString GetSystemDir()
{
		UIString system_path;
	system_path.resize(MAX_PATH);
	DWORD result = ::GetSystemDirectory(&system_path[0], MAX_PATH);
	system_path.resize(result);
	return system_path;
}

	UIString GetTempDir()
{
		UIString temp_path;
	temp_path.resize(MAX_PATH);
	DWORD result = ::GetTempPath(MAX_PATH, &temp_path[0]);
	temp_path.resize(result);
	return temp_path;
}

	UIString GetLocalAppDataDir(HANDLE token /* = NULL */)
{
#if (NTDDI_VERSION < NTDDI_VISTA)
#ifndef KF_FLAG_CREATE
#define KF_FLAG_CREATE 0x00008000
#endif
#endif

		UIString temp_path;
	if (IsRunningOnVistaOrHigher())
	{
		typedef HRESULT (WINAPI *__SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PCTSTR*);
		HMODULE moudle_handle = ::LoadLibrary(_T("shell32.dll"));
		if (moudle_handle != NULL)
		{
			__SHGetKnownFolderPath _SHGetKnownFolderPath =
				reinterpret_cast<__SHGetKnownFolderPath>(::GetProcAddress(moudle_handle, "SHGetKnownFolderPath"));
			if (_SHGetKnownFolderPath != NULL)
			{
				PCTSTR result = NULL;
				if (S_OK == _SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, token, &result))
				{
					temp_path = result;
					::CoTaskMemFree((void*)result);
				}
			}
			::FreeLibrary(moudle_handle);
		}
	}
	else
	{
		// On Windows XP, CSIDL_LOCAL_APPDATA represents "{user}\Local Settings\Application Data"
		// while CSIDL_APPDATA represents "{user}\Application Data"
		TCHAR buffer[MAX_PATH];
		if (S_OK == ::SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE, token, SHGFP_TYPE_CURRENT, buffer))
			temp_path = buffer;
	}
	if (!temp_path.empty())
		if (temp_path.back() != _T('\\'))
			temp_path.push_back(_T('\\'));
	return temp_path;
}

bool CreateDirectoryRecursively(const TCHAR* full_dir)
{
	 HRESULT result = ::SHCreateDirectoryEx(NULL, full_dir, nullptr);
	 if (result == ERROR_SUCCESS || result == ERROR_FILE_EXISTS || result == ERROR_ALREADY_EXISTS)
		 return true;
	 return false;
}

bool DeleteDirectoryRecursively(const TCHAR* full_dir)
{
	DWORD attributes = ::GetFileAttributes(full_dir);
	if (attributes == INVALID_FILE_ATTRIBUTES) // not exists
		return true;
	if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) // not a file
		return false;
	SHFILEOPSTRUCT file_op = {NULL,
							   FO_DELETE,
							   full_dir,
							   _T(""),
							   FOF_NOCONFIRMATION|FOF_NOERRORUI|FOF_SILENT,
							   FALSE,
							   0,
							   _T("")};
	if (::SHFileOperation(&file_op) && !file_op.fAnyOperationsAborted)
		return true;
	return false;
}

} // namespace win32
} // namespace nbase

#endif // OS_WIN
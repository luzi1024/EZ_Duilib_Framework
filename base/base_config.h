﻿// Copyright (c) 2012, NetEase Inc. All rights reserved.
//
// WRT
// 2012/5/4
//
// base config

#ifndef BASE_BASE_CONFIG_H_
#define BASE_BASE_CONFIG_H_

#include "port.h"

#if defined(OS_WIN)
#include "build/winsdk_config.h"
#include <windows.h>
#include <tchar.h>
#include <string>
#ifdef _UNICODE	
typedef std::wstring UIString;
#else
typedef std::string UIString;
#endif
#endif

#endif // BASE_BASE_CONFIG_H_

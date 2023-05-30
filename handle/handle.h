#pragma once
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <exception>
#include <psapi.h>

LONG_PTR GetPID(const TCHAR* processName);
LONG_PTR GetModuleBase(const TCHAR* moduleName , HANDLE handle);

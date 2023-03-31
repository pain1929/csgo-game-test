#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <string_view>
#include <Psapi.h>
HMODULE getModuleBase(const TCHAR* name, DWORD PID) {
    MODULEENTRY32 moduleEntry;
    HANDLE handle = NULL;
    handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
    ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    if (!Module32First(handle, &moduleEntry)) {
        CloseHandle(handle);
        return NULL;
    }
    do {
        if (_tcscmp(moduleEntry.szModule, name) == 0) { return moduleEntry.hModule; }
    } while (Module32Next(handle, &moduleEntry));
    return 0;
}

DWORD  getPID(const std::string_view processName) {
    DWORD PID = 0;
    ::PROCESSENTRY32 entry = { };
    entry.dwSize = sizeof(::PROCESSENTRY32);

    const auto snapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    while (::Process32Next(snapShot, &entry))
    {
        if (!processName.compare(entry.szExeFile))
        {
            PID = entry.th32ProcessID;
            break;
        }
    }

    // Free handle
    if (snapShot)
        ::CloseHandle(snapShot);
    return PID;
}

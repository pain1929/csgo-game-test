#include <Windows.h>
BOOL InjectDll(HANDLE hProcess, LPCSTR dllPath){
    LPVOID lpRemoteMem = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (lpRemoteMem == NULL){
        // 分配内存失败
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, lpRemoteMem, dllPath, strlen(dllPath) + 1, NULL)){
        // 写入内存失败
        VirtualFreeEx(hProcess, lpRemoteMem, 0, MEM_RELEASE);
        return FALSE;
    }

    HMODULE hKernel32 = GetModuleHandle("Kernel32");
    if (hKernel32 == NULL){
        // 获取Kernel32模块句柄失败
        VirtualFreeEx(hProcess, lpRemoteMem, 0, MEM_RELEASE);
        return FALSE;
    }

    LPVOID lpLoadLibraryA = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");
    if (lpLoadLibraryA == NULL){
        // 获取LoadLibraryA函数地址失败
        VirtualFreeEx(hProcess, lpRemoteMem, 0, MEM_RELEASE);
        return FALSE;
    }
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadLibraryA, lpRemoteMem, 0, NULL);
    if (hThread == NULL){
        // 创建远程线程失败
        VirtualFreeEx(hProcess, lpRemoteMem, 0, MEM_RELEASE);
        return FALSE;
    }
    //等待线程结束
    WaitForSingleObject(hThread, INFINITE);

    //释放内存
    VirtualFreeEx(hProcess, lpRemoteMem, 0, MEM_RELEASE);
    CloseHandle(hThread);
    return TRUE;
}

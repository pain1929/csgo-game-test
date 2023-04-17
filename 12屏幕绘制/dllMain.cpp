// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"


#include <d3d9.h>
#include <d3dx9.h>
#include <detours.h>

#include "drawFunc.hpp"

#pragma comment(lib , "d3d9.lib")
#pragma comment(lib , "d3dx9.lib")
#pragma comment(lib , "detours.lib")

typedef HRESULT(__stdcall* endScene)(IDirect3DDevice9*);

endScene oldFunc;

ID3DXLine* line = 0; //绘制线段
ID3DXFont* font = 0; //绘制文字

HRESULT __stdcall hook_func(IDirect3DDevice9* device) {
    //绘制
    if (line == 0 && font == 0) {
        D3DXCreateLine(device, &line);
        line->SetWidth(1.5);
        font = CreateFont_(device, 19);
    }
    DrawLine(0, 0, 100, 100, line, RED);
    Text(font, "hello world", {200 , 200 , 0 , 0} , GREEN);

    return oldFunc(device);
}

//hook endScene


void run() {
    //endScene 函数地址 : d3d9.dll + 0x63130
    int funcaddr = (int)GetModuleHandle("d3d9.dll") + 0x63130;


    oldFunc = (endScene)DetourFunction((PBYTE)funcaddr, (PBYTE)(hook_func));
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        run();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


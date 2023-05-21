
#include "getFunc.h"
IDirect3DDevice9* pDevice = nullptr;

void* GetFunc(int index)
{
    if (pDevice == nullptr)
    {
        IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
        D3DPRESENT_PARAMETERS d3dparams = { 0 };
        d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dparams.hDeviceWindow = GetForegroundWindow();
        d3dparams.Windowed = true;
        HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetForegroundWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dparams, &pDevice);
    }

    void** table = *((void***)pDevice);
    return table[index];
}

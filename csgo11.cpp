const int m_fFlags = 0x104;
const int dwLocalPlayer = 0xDEA964;
const int dwForceJump = 0x52BBD50;

//client.dll
int CLIENT;
// engine.dll+80CD98
int CMD;
bool bhop = false;
DWORD CALLBACK thread_func(LPVOID data) {
    CLIENT = (int)GetModuleHandle("client.dll");
    CMD = (int)GetModuleHandle("engine.dll") + 0x80CD98;

    while (1) {

        if (strcmp((char*)CMD, "bhop on") == 0) {
            bhop = true;
        }
        if (strcmp((char*)CMD, "bhop off") == 0) {
            bhop = false;
        }


        if (!bhop) {
            continue;
        }
        void* local_player = *(void**)(CLIENT + dwLocalPlayer);
        int flag = 0;
        flag = *(int*)((int)local_player + m_fFlags);
        int buffer = 0;
        if (flag & 1) {
            buffer = 5;
        }
        else {
            buffer = 4;
        }
        // 4 5

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            *(int*)(CLIENT + dwForceJump) = buffer;
        }
    }
    
   

}


void run() {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(thread_func), NULL, 0, NULL);
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

#include <iostream>
using namespace std;
#define LOCAL_PLAYER 0xDEB964
#define HEALTH 0x100
#define CLIENT 0x274B0000
#include <Windows.h>
//client.dll
int main(){
	DWORD PID;
	HWND hwnd = FindWindowA(0, "Counter-strike: Global Offensive - Direct3D 9");
	GetWindowThreadProcessId(hwnd, &PID);
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS , FALSE , PID);
	int local_player = 0;
	ReadProcessMemory(handle, (void*)(LOCAL_PLAYER + CLIENT), &local_player, 4, 0);
	int health = 0;
	while (1) {
		ReadProcessMemory(handle, (void*)(local_player + HEALTH), &health, 4, 0);
		printf("%d \n", health);
	}
	
}

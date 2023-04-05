#include <iostream>
#include "handle.hpp"

#define CLIENTSTATE 0x59F19C
#define VIEWANGLE 0x4D90

//强制玩家的视角
int main(){
	DWORD PID = getPID("csgo.exe");
	//engine.dll
	int ENGINE  = (int)getModuleBase(TEXT("engine.dll"), PID);
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	int state = 0;
	ReadProcessMemory(handle, (void*)(ENGINE + CLIENTSTATE), &state, 4, 0);
  
  //这里将 玩家的视角改为 0，0
	float x = 0;
	float y = 0;

	while (1) {
		WriteProcessMemory(handle, (void*)(state + VIEWANGLE), &x, 4, 0);
		WriteProcessMemory(handle, (void*)(state + VIEWANGLE + 0x4), &y, 4, 0);
	}
	
	
} 

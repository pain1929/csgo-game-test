#include <iostream>
#include "handle.hpp"
#define LIST 0x4dfffc4
//实体列表
//第6期视频的代码

int getPlayer(int index, HANDLE handle , int CLIENT) {
	int player = 0;
	ReadProcessMemory(handle, (void*)(CLIENT + LIST + (index * 0x10)), &player, 4, 0);
	return player;
}
int main(){
	DWORD PID = getPID("csgo.exe");
	//client.dll
	int CLIENT = (int)getModuleBase(TEXT("client.dll"), PID);
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	int player = 0;
	while (1) {
		for (int i = 0; i < 10; i++) {
			player = getPlayer(i, handle, CLIENT);
			int health = 0;
			ReadProcessMemory(handle, (void*)(player + 0x100), &health, 4, 0);
			printf("%d \n", health);
		}
		system("cls");
	}
	
//如果你发现代码不起作用 一定是因为偏移量发生了变化 请找到最新的偏移量
}

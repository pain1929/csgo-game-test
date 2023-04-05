
#include <iostream>
#include "handle.hpp"
#define LOCAL_PLAYER 0xDEA964

#define CLIENTSTATE 0x59F19C
#define VIEWANGLE 0x4D90

#define LIST 0x4DFFFC4

#define BoneMatrix 0x26A8
#define boneMatrix_head 0x180

struct Vec3 {
	float x;
	float y;
	float z;
};

//获取本地玩家地址
int getLocalPlayer(HANDLE handle , int CLIENT) {
	int local_player = 0;
	ReadProcessMemory(handle, (void*)(CLIENT + LOCAL_PLAYER), &local_player, 4, 0);
	return local_player;
}

//获取血量
int getHealth(int player, HANDLE handle) {
	int health = 0;
	ReadProcessMemory(handle, (void*)(player + 0x100), &health, 4, 0);
	return health;
}
//根据玩家地址获取玩家头坐标
Vec3 getHead(int player , HANDLE handle) {
	int martix = 0;
	ReadProcessMemory(handle, (void*)(player + BoneMatrix), &martix, 4, 0);
	float x = 0;
	float y = 0;
	float z = 0;
	ReadProcessMemory(handle, (void*)(martix + boneMatrix_head + 12), &x, 4 ,0);
	ReadProcessMemory(handle, (void*)(martix + boneMatrix_head + 28), &y, 4, 0);
	ReadProcessMemory(handle, (void*)(martix + boneMatrix_head + 44), &z, 4, 0);
	return { x ,y,z };
}

//判断这个玩家是不是本地玩家
bool isLocalPlayer(int player , HANDLE handle , int CLIENT) {
	
	return player == getLocalPlayer(handle , CLIENT);
}

//根据索引获取玩家地址
int getPlayer(int index, HANDLE handle, int CLIENT) {
	int player = 0;
	ReadProcessMemory(handle, (void*)(CLIENT + LIST + (index * 0x10)), &player, 4, 0);
	return player;
}

//修改本地玩家的视角
void setViewAngle(Vec3 vec , HANDLE handle , int ENGINE) {
	int state = 0;
	float x = vec.x;
	float y = vec.y;
	ReadProcessMemory(handle, (void*)(ENGINE + CLIENTSTATE), &state, 4, 0);
	WriteProcessMemory(handle, (void*)(state + VIEWANGLE), &x, 4, 0);
	WriteProcessMemory(handle, (void*)(state + VIEWANGLE + 0x4), &y, 4, 0);
}

//v1:自己的头坐标
//v2:目标玩家的头坐标
//返回值：一个本地玩家视角值 (将此值写入玩家视角内存 即可完成将瞄准十字指向目标玩家头)
Vec3 getAngle(Vec3 v1, Vec3 v2) {
	Vec3 rotate;
	float a = v2.x - v1.x;
	float b = v2.y - v1.y;
	float c = sqrt(a * a + b * b);
	float angles = atan(b / a);
	if (a > 0) {
		rotate.y = angles * (180 / 3.14159265359);
	}
	else {
		rotate.y = angles * (180 / 3.14159265359) + 180;
	}

	b = v2.z - v1.z;
	angles = atan(b / c);
	rotate.x = -angles * (180 / 3.14159265359);
	if (rotate.y > 180) {
		rotate.y = rotate.y - 360;
	}
	return rotate;
}

int main(){
	DWORD PID = getPID("csgo.exe");
	int CLIENT = (int)getModuleBase(TEXT("client.dll"), PID);
	int ENGINE = (int)getModuleBase(TEXT("engine.dll"), PID);
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	
	int local_player = getLocalPlayer(handle, CLIENT);
	while (1) {
		for (int i = 0; i < 10; i++) {
			int player = 0;
			player = getPlayer(i, handle, CLIENT);
			if (!isLocalPlayer(player, handle, CLIENT) && getHealth(player, handle) > 0) {
				Vec3 back = getAngle(getHead(local_player, handle), getHead(player, handle));
				setViewAngle(back, handle, ENGINE);
				break;
			}
			
		}
	}
	
}

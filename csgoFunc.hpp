#include "csgo.hpp"
#include <Windows.h>

struct POS {
	float x, y, z;
};


//矩阵结构体
struct ViewMatrix {
	float floats[16];
};

void* GetLocalPlayer();
void* GetPlayer(int index);
bool IsLocalPlayer(void* player_addr);
POS GetHead(void* player_addr);
POS GetPos(void* player_addr);
int GetHealth(void* player_addr);
int GetTeam(void* player_addr);
ViewMatrix GetViewMatrix();
void WorldToScreen(POS player_pos, ViewMatrix viewMatrix, POS RES, POS* buffer);
POS WindowSize(HWND hwnd);
BOOL CALLBACK enumFunc(HWND hwnd, LPARAM data);
HWND GetWindowHandle(DWORD PID);



//模块地址
const void* CLIENT = GetModuleHandle("client.dll");
const void* ENGINE = GetModuleHandle("engine.dll");


//窗口大小
const POS RES = WindowSize(GetWindowHandle(GetCurrentProcessId()));




//获取本地玩家地址
void* GetLocalPlayer() {
	return *(void**)((int)CLIENT + hazedumper::signatures::dwLocalPlayer);
}


//根据索引 获取玩家地址 0-63
void* GetPlayer(int index) {
	return *(void**)((int)CLIENT + hazedumper::signatures::dwEntityList + (index * 0x10));
}


//判断是否是本地玩家
bool IsLocalPlayer(void* player_addr) {
	return (int)player_addr == (int)GetLocalPlayer();
}

//获取玩家地址获取玩家头坐标
POS GetHead(void* player_addr) {
	POS pos;
	void* bone = *(void**)((int)player_addr + hazedumper::netvars::m_dwBoneMatrix);
	pos.x = *(float*)((int)bone + 0x180 + 12);
	pos.y = *(float*)((int)bone + 0x180 + 28);
	pos.z = *(float*)((int)bone + 0x180 + 44);
	return pos;
}

//根据玩家地址获取玩家脚底坐标
POS GetPos(void* player_addr) {
	POS pos = { 0 };
	pos.x = *(float*)((int)player_addr + 0x138);
	pos.y = *(float*)((int)player_addr + 0x138 + 4);
	pos.z = *(float*)((int)player_addr + 0x138 + 8);
	return pos;
}


//获取玩家血量
int GetHealth(void* player_addr) {
	return *(int*)((int)player_addr + hazedumper::netvars::m_iHealth);
}

//获取玩家阵营
int GetTeam(void* player_addr) {
	return *(int*)((int)player_addr + hazedumper::netvars::m_iTeamNum);
}

//获得矩阵
ViewMatrix GetViewMatrix() {
	return *(ViewMatrix*)((int)CLIENT + hazedumper::signatures::dwViewMatrix);
}




//将人物在游戏中的 x y z 三维坐标 转换为你屏幕上的平面坐标 x y
//player_pos : 敌人坐标（头或者脚）
//viewMatrix : 视角矩阵 （通过本文件中的GetViewMatrix函数获得）
//RES        ：游戏窗口的尺寸
//buffer     :返回的数据
void WorldToScreen(POS player_pos, ViewMatrix viewMatrix, POS RES, POS* buffer) {
	POS out;
	float _x = viewMatrix.floats[0] * player_pos.x + viewMatrix.floats[1] * player_pos.y + viewMatrix.floats[2] * player_pos.z + viewMatrix.floats[3];
	float _y = viewMatrix.floats[4] * player_pos.x + viewMatrix.floats[5] * player_pos.y + viewMatrix.floats[6] * player_pos.z + viewMatrix.floats[7];
	out.z = viewMatrix.floats[12] * player_pos.x + viewMatrix.floats[13] * player_pos.y + viewMatrix.floats[14] * player_pos.z + viewMatrix.floats[15];
	if (out.z <= 0) {
		buffer->x = 0;
		buffer->y = 0;
		buffer->z = 0;
		return;
	}
	_x *= 1.f / out.z;
	_y *= 1.f / out.z;

	out.x = RES.x * .5f;
	out.y = RES.y * .5f;

	out.x += 0.5f * _x * RES.x + 0.5f;
	out.y -= 0.5f * _y * RES.y + 0.5f;

	buffer->x = out.x;
	buffer->y = out.y;
	buffer->z = out.z;
}


//根据窗口句柄获取窗口尺寸
POS WindowSize(HWND hwnd) {
	RECT rect;
	GetWindowRect(hwnd, &rect);
	return { (float)(rect.right - rect.left) , (float)(rect.bottom - rect.top) , 0 };
}

//回调函数
BOOL CALLBACK enumFunc(HWND hwnd, LPARAM data) {
	DWORD TARGET_PID = *(DWORD*)(data);
	DWORD PID = 0;
	GetWindowThreadProcessId(hwnd, &PID);
	if (TARGET_PID == PID) {
		*(HWND*)(data) = hwnd;
		return false;
	}
	else {
		return true;
	}
}

//获取窗口句柄
HWND GetWindowHandle(DWORD PID) {
	EnumWindows(enumFunc, (LPARAM)&PID);
	return *(HWND*)&PID;
}

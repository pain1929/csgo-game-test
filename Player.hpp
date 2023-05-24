#include "csgo.hpp"
#include <Windows.h>



// pain1929 2023-05-01  五一快乐


enum BONE {
	HEAD,
	ROOT
};

namespace CSGO_INFO {
	struct POS {
		float x, y, z;
	};
	CSGO_INFO::POS WindowSize(HWND hwnd);
	BOOL CALLBACK enumFunc(HWND hwnd, LPARAM data);
	HWND GetWindowHandle(DWORD PID);

	struct ViewMatrix {
		float floats[16];
	};

	void* CLIENT = GetModuleHandle("client.dll");
	void* ENGINE = GetModuleHandle("engine.dll");
	POS RES = WindowSize(GetWindowHandle(GetCurrentProcessId()));
	bool InGame();
	
}
class Player {
public:
	void* address;
	Player(int index);
	Player(void* address);
	~Player();
	CSGO_INFO::POS GetHead();
	CSGO_INFO::POS GetRoot();
	int GetHealth();
	int GetTeam();
	bool IsEnemy();
	bool IsAlive();
	bool IsSleeping();
	bool IsLocalPlayer();
	bool IsNULL();
	CSGO_INFO::ViewMatrix GetViewMatrix();
	CSGO_INFO::POS WorldToScreen(BONE bone);
	static int MAX();
	static Player GetLocalPlayer();

private:
	
};

Player::Player(int index) {
	this->address =  *(void**)((int)CSGO_INFO::CLIENT + hazedumper::signatures::dwEntityList + (index * 0x10));
}
Player::Player(void* address) {
	this->address = address;
}
Player::~Player(){}

bool Player::IsNULL() {
	return this->address == 0 || 
	this->GetHealth()>100 || 
	this->GetHealth() <0 || 
	(this->GetTeam()!=2 && this->GetTeam()!=3);
}
CSGO_INFO::POS Player::GetHead() {
	CSGO_INFO::POS pos;
	void* bone = *(void**)((int)this->address + hazedumper::netvars::m_dwBoneMatrix);
	pos.x = *(float*)((int)bone + 0x180 + 12);
	pos.y = *(float*)((int)bone + 0x180 + 28);
	pos.z = *(float*)((int)bone + 0x180 + 44);
	return pos;
}

CSGO_INFO::POS Player::GetRoot() {
	CSGO_INFO::POS pos = { 0 };
	pos.x = *(float*)((int)this->address + 0x138);
	pos.y = *(float*)((int)this->address + 0x138 + 4);
	pos.z = *(float*)((int)this->address + 0x138 + 8);
	return pos;
}

int Player::GetHealth() {
	return *(int*)((int)this->address + hazedumper::netvars::m_iHealth);
}

Player Player::GetLocalPlayer() {
	void* addr =  *(void**)((int)CSGO_INFO::CLIENT + hazedumper::signatures::dwLocalPlayer);
	return Player(addr);
}

bool Player::IsLocalPlayer() {
	return this->address == Player::GetLocalPlayer().address;
}

int Player::GetTeam() {
	return *(int*)((int)this->address + hazedumper::netvars::m_iTeamNum);
}
bool Player::IsEnemy() {
	return this->GetTeam() != Player::GetLocalPlayer().GetTeam();
}

bool Player::IsAlive() {
	return this->GetHealth() <= 100 && this->GetHealth() > 0;
}

bool Player::IsSleeping() {
	return *(bool*)((int)this->address + hazedumper::signatures::m_bDormant);
}


CSGO_INFO::ViewMatrix Player::GetViewMatrix() {
	return *(CSGO_INFO::ViewMatrix*)((int)CSGO_INFO::CLIENT + hazedumper::signatures::dwViewMatrix);
}

int Player::MAX() {
	void* clientstate = *(void**)((int)CSGO_INFO::ENGINE + hazedumper::signatures::dwClientState);
	return *(int*)((int)clientstate + hazedumper::signatures::dwClientState_MaxPlayer);
}


CSGO_INFO::POS Player::WorldToScreen(BONE P) {
	CSGO_INFO::POS player_pos;
	if (P == BONE::HEAD) {
		player_pos = this->GetHead();
	}
	else if (P == BONE::ROOT) {
		player_pos = this->GetRoot();
	}
	else {
		return { 0 , 0 ,0 };
	}
	CSGO_INFO::ViewMatrix viewMatrix = Player::GetViewMatrix();
    CSGO_INFO::POS out;
	float _x = viewMatrix.floats[0] * player_pos.x + viewMatrix.floats[1] * player_pos.y + viewMatrix.floats[2] * player_pos.z + viewMatrix.floats[3];
	float _y = viewMatrix.floats[4] * player_pos.x + viewMatrix.floats[5] * player_pos.y + viewMatrix.floats[6] * player_pos.z + viewMatrix.floats[7];
	out.z = viewMatrix.floats[12] * player_pos.x + viewMatrix.floats[13] * player_pos.y + viewMatrix.floats[14] * player_pos.z + viewMatrix.floats[15];
	if (out.z <= 0) {
		return {0 , 0 , 0};
	}
	_x *= 1.f / out.z;
	_y *= 1.f / out.z;

	out.x = CSGO_INFO::RES.x * .5f;
	out.y = CSGO_INFO::RES.y * .5f;

	out.x += 0.5f * _x * CSGO_INFO::RES.x + 0.5f;
	out.y -= 0.5f * _y * CSGO_INFO::RES.y + 0.5f;

	return out;
}

//根据窗口句柄获取窗口尺寸
CSGO_INFO::POS CSGO_INFO::WindowSize(HWND hwnd) {
	RECT rect;
	GetWindowRect(hwnd, &rect);
	return { (float)(rect.right - rect.left) , (float)(rect.bottom - rect.top) , 0 };
}

//回调函数
BOOL CALLBACK CSGO_INFO::enumFunc(HWND hwnd, LPARAM data) {
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
HWND CSGO_INFO::GetWindowHandle(DWORD PID) {
	EnumWindows(enumFunc, (LPARAM)&PID);
	return *(HWND*)&PID;
}


bool CSGO_INFO::InGame()
{
	void* clientstate = *(void**)((int)CSGO_INFO::ENGINE + hazedumper::signatures::dwClientState);
	return *(LONG_PTR*)((LONG_PTR)clientstate + hazedumper::signatures::dwClientState_State) == 6;		
}	
	
	

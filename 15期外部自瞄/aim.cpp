

#include "aim.h"
DWORD PID = GetPID("csgo.exe");
HANDLE csgo = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
int CLIENT = (int)GetModule("client.dll", csgo);
int ENGINE = (int)GetModule("engine.dll", csgo);
POS RES = WindowSize(GetWindowHandle(PID));

DWORD GetPID(const char* name)
{
	PROCESSENTRY32 pe = { sizeof(pe) };
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	bool success = Process32First(snapshot, &pe);
	while (success)
	{
		success = Process32Next(snapshot, &pe);
		if (strcmp(name, pe.szExeFile) == 0)
		{
			return pe.th32ProcessID;
		}
	}
}

HMODULE GetModule(const char* name , HANDLE process) {
	HMODULE mod[1024];
	DWORD need;
	char buffer[MAX_PATH];
	EnumProcessModules(process, mod, sizeof(HMODULE) * 1024, &need);
	for (size_t i = 0; i < need; i++)
	{
		GetModuleBaseName(process, mod[i], buffer, sizeof(char) * MAX_PATH);
		if (strcmp(name, buffer) == 0)
		{
			return mod[i];
		}
	}
}


Player::Player(int index) {
	void* addr;
	ReadProcessMemory(csgo, (void*)(CLIENT + hazedumper::signatures::dwEntityList + (index * 0x10)),
		&addr, sizeof(void*), NULL);
	this->address = addr;
}
Player::Player(void* address) {
	this->address = address;
}
Player::~Player() {}

bool Player::IsNULL() {
	return this->address == 0 ||
		this->GetHealth() > 100 ||
		this->GetHealth() < 0 ||
		(this->GetTeam() != 2 && this->GetTeam() != 3);
}
POS Player::GetHead() {
POS pos;
    void* bone;
	ReadProcessMemory(csgo, (void*)((int)this->address + hazedumper::netvars::m_dwBoneMatrix), &bone, sizeof(void*), NULL);
	ReadProcessMemory(csgo, (void*)((int)bone + 0x180 + 12), (void*)&(pos.x), sizeof(float), NULL);
	ReadProcessMemory(csgo, (void*)((int)bone + 0x180 + 28), (void*)&(pos.y), sizeof(float), NULL);
	ReadProcessMemory(csgo, (void*)((int)bone + 0x180 + 44), (void*)&(pos.z), sizeof(float), NULL);
	return pos;
}

POS Player::GetRoot() {
	POS pos = { 0 };
	ReadProcessMemory(csgo, (void*)((int)this->address + 0x138), (void*)&pos, sizeof(POS), NULL);
	return pos;
}

int Player::GetHealth() {
	int health;
	ReadProcessMemory(csgo, 
		(void*)((int)this->address + hazedumper::netvars::m_iHealth), 
		&health,
		sizeof(int),
		NULL);
	return health;
}

Player Player::GetLocalPlayer() {
	void* player;
	ReadProcessMemory(csgo,
		(void*)(CLIENT + hazedumper::signatures::dwLocalPlayer),
		&player, sizeof(void*),
		NULL);
	return player;
}

void Player::SetViewAngle(POS pos) {
	void* state;
	ReadProcessMemory(csgo, (void*)(ENGINE + hazedumper::signatures::dwClientState),
		&state, sizeof(void*), NULL);
	WriteProcessMemory(csgo, (void*)((int)state + hazedumper::signatures::dwClientState_ViewAngles),
		&pos, sizeof(POS), NULL);
}

POS Player::GetLocalViewAngle() {
	void* state;
	ReadProcessMemory(csgo, (void*)(ENGINE + hazedumper::signatures::dwClientState),
		&state, sizeof(void*), NULL);
	POS pos;
	ReadProcessMemory(csgo, (void*)((int)state + hazedumper::signatures::dwClientState_ViewAngles),
		&pos, sizeof(POS), NULL);
	return pos;
}

POS Player::GetAngle(BONE bone) {
	POS back = { 0 };
	POS local_head = Player::GetLocalPlayer().GetHead();
	POS pos;
	if (bone == BONE::HEAD) {
		pos = this->GetHead();
	}
	else {
		pos = this->GetRoot();
	}

	float a = pos.x - local_head.x;
	float b = pos.y - local_head.y;
	float c = sqrt(a * a + b * b);
	float angles = atan(b / a);
	if (a > 0) {
		back.y = angles * (180 / 3.14159265359);
	}
	else {
		back.y = angles * (180 / 3.14159265359) + 180;
	}

	b = pos.z - local_head.z;
	angles = atan(b / c);
	back.x = -angles * (180 / 3.14159265359);
	if (back.y > 180) {
		back.y = back.y - 360;
	}
	return back;
}

bool Player::IsLocalPlayer() {
	return this->address == Player::GetLocalPlayer().address;
}

int Player::GetTeam() {
	int team;
	ReadProcessMemory(csgo, (void*)((int)this->address + hazedumper::netvars::m_iTeamNum),
		&team, sizeof(int), NULL);
	return team;
}
bool Player::IsEnemy() {
	return this->GetTeam() != Player::GetLocalPlayer().GetTeam();
}

bool Player::IsAlive() {
	return this->GetHealth() <= 100 && this->GetHealth() > 0;
}

bool Player::IsSleeping() {
	bool sleeping;
	ReadProcessMemory(csgo, (void*)((int)this->address + hazedumper::signatures::m_bDormant),
		&sleeping, sizeof(bool), NULL);
	return sleeping;
}


ViewMatrix Player::GetViewMatrix() {
	ViewMatrix matrix;
	ReadProcessMemory(csgo, (void*)(CLIENT + hazedumper::signatures::dwViewMatrix),
		&matrix, sizeof(ViewMatrix), NULL);
	return matrix;
}

int Player::MAX() {
	void* clientstate;
	ReadProcessMemory(csgo, (void*)(ENGINE + hazedumper::signatures::dwClientState),
		&clientstate, sizeof(void*), NULL);
	int max;
	ReadProcessMemory(csgo, (void*)((int)clientstate + hazedumper::signatures::dwClientState_MaxPlayer),
		&max, sizeof(int), NULL);
	return max;
}


POS Player::WorldToScreen(BONE P) {
	POS player_pos;
	if (P == BONE::HEAD) {
		player_pos = this->GetHead();
	}
	else if (P == BONE::ROOT) {
		player_pos = this->GetRoot();
	}
	else {
		return { 0 , 0 ,0 };
	}
	ViewMatrix viewMatrix = Player::GetViewMatrix();
	POS out;
	float _x = viewMatrix.floats[0] * player_pos.x + viewMatrix.floats[1] * player_pos.y + viewMatrix.floats[2] * player_pos.z + viewMatrix.floats[3];
	float _y = viewMatrix.floats[4] * player_pos.x + viewMatrix.floats[5] * player_pos.y + viewMatrix.floats[6] * player_pos.z + viewMatrix.floats[7];
	out.z = viewMatrix.floats[12] * player_pos.x + viewMatrix.floats[13] * player_pos.y + viewMatrix.floats[14] * player_pos.z + viewMatrix.floats[15];
	if (out.z <= 0) {
		return { 0 , 0 , 0 };
	}
	_x *= 1.f / out.z;
	_y *= 1.f / out.z;

	out.x = RES.x * .5f;
	out.y = RES.y * .5f;

	out.x += 0.5f * _x * RES.x + 0.5f;
	out.y -= 0.5f * _y * RES.y + 0.5f;

	return out;
}
int Player::DistanceTo(BONE bone)
{
	POS b = this->WorldToScreen(bone);
	float x = RES.x / 2 - b.x;
	float y = RES.y / 2 - b.y;
	return sqrt(x * x + y * y);
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
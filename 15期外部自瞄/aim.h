#pragma once

#include <cmath>
#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include "csgo.hpp"
#include <Windows.h>

struct POS {
	float x, y, z;
};
struct ViewMatrix {
	float floats[16];
};

DWORD GetPID(const char* name);
HMODULE GetModule(const char* name , HANDLE process);
POS WindowSize(HWND hwnd);
BOOL CALLBACK enumFunc(HWND hwnd, LPARAM data);
HWND GetWindowHandle(DWORD PID);
enum BONE {
	HEAD,
	ROOT
};

class Player {
public:
	void* address;
	Player(int index);
	Player(void* address);
	~Player();
	POS GetHead();
	POS GetRoot();
	int GetHealth();
	int GetTeam();
	bool IsEnemy();
	bool IsAlive();
	bool IsSleeping();
	bool IsLocalPlayer();
	bool IsNULL();
	ViewMatrix GetViewMatrix();
	POS WorldToScreen(BONE bone);
	static int MAX();
	static Player GetLocalPlayer();
	static void SetViewAngle(POS);
	static POS GetLocalViewAngle();
	POS GetAngle(BONE);
	int DistanceTo(BONE);

private:

};




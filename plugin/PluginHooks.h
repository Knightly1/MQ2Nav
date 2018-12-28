//
// PluginHooks.h
//

#pragma once

#include "plugin/MQ2Navigation.h"

#include <d3dx9.h>

//----------------------------------------------------------------------------

// indicates whether imgui is ready to be used
extern bool g_imguiReady;

// indicate whether imgui should render a new frame
extern bool g_imguiRender;

// current mouse location
extern POINT MouseLocation;

// whether mouse is blocked from overlay capture or not
extern bool MouseBlocked;

extern IDirect3DDevice9* g_pDevice;

extern bool g_deviceAcquired;

// Mouse state, pointed to by EQADDR_DIMOUSECOPY
struct MouseStateData {
	DWORD x;
	DWORD y;
	DWORD Scroll;
	DWORD relX;
	DWORD relY;
	DWORD InWindow;
};
extern MouseStateData* MouseState;

// Better MOUSEINFO that has scroll too
struct MOUSEINFO2 {
	DWORD X;
	DWORD Y;
	DWORD SpeedX;
	DWORD SpeedY;
	DWORD Scroll;
};
extern MOUSEINFO2* MouseInfo;

//----------------------------------------------------------------------------

enum class HookStatus {
	Success,
	MissingOffset,
	MissingDevice,
	AlreadyInstalled,
};

HookStatus InitializeHooks();

void ShutdownHooks();

//----------------------------------------------------------------------------

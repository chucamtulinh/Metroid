#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define BUTTONDOWN(name, key) (name.rgbButtons[key] & 0x80) 

class diManager
{
public:
	~diManager(void);

	// singleton stuff
	static diManager& getInstance() { static diManager pInstance; return pInstance;}

	bool initDirectInput(HINSTANCE hInst, HWND wndHandle);
	void shutdown(void);

	void getInput(void);

	bool isButtonDown(int which);

	inline int getCurMouseX(void) { return mouseState.lX; }
	inline int getCurMouseY(void) { return mouseState.lY; }
private:
	diManager(void);

	LPDIRECTINPUT8        g_lpDI;		// the direct input object
	LPDIRECTINPUTDEVICE8  g_lpDIDevice; // the direct input device

	DIMOUSESTATE mouseState;			// The current state of the mouse device
};

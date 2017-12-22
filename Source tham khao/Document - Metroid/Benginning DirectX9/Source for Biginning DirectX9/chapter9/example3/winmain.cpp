#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <string>
#include <vector>
using namespace std;
HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

LPDIRECTINPUT8        g_lpDI;	// the direct input object
LPDIRECTINPUTDEVICE8  g_lpDIDevice; // the direct input device

vector<string> DeviceList;

// The callback function to enumerate the installed devices
BOOL CALLBACK    EnumDevicesCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr; 

	// Create the DirectInput object. 
    hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, 
                            IID_IDirectInput8, (void**)&g_lpDI, NULL); 

	if FAILED(hr) return FALSE; 

	// grab the input devices attached
	hr = g_lpDI->EnumDevices( DI8DEVCLASS_ALL, EnumDevicesCallback,
                              NULL, DIEDFL_ATTACHEDONLY );
	if FAILED (hr) return FALSE;

	// create a temporary string
	string tempString;

	// Loop through the vector of device names and put them in tempstring
	tempString = "List of Installed Devices\n";
	for (unsigned int i=0; i<DeviceList.size(); i++)
	{
		tempString += DeviceList[i];
		tempString += "\n";
	}

	// throw up a message box just to display the names of the devices
	MessageBox(NULL, tempString.c_str(), "Devices", MB_OK);
	
	if (g_lpDI) 
    { 
        if (g_lpDIDevice) 
        { 
        // Always unacquire device before calling Release(). 
            g_lpDIDevice->Unacquire(); 
            g_lpDIDevice->Release();
            g_lpDIDevice = NULL; 
        } 
        g_lpDI->Release();
        g_lpDI = NULL; 
    } 

	return 0;
}

BOOL CALLBACK EnumDevicesCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext )
{
    HRESULT hr;

    hr = g_lpDI->CreateDevice( pdidInstance->guidInstance, &g_lpDIDevice, NULL );

    if( FAILED(hr) ) 
        return DIENUM_STOP;
	else
	{
		// add the current device to the vector
		DeviceList.push_back(pdidInstance->tszInstanceName);
	}
    return DIENUM_CONTINUE;
}

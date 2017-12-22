#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <string>
#include <vector>
using namespace std;
HINSTANCE hInst;						// holds the instance for this app
HWND wndHandle;							// global window handle

LPDIRECTINPUT8        g_lpDI;			// the direct input object
LPDIRECTINPUTDEVICE8  g_joystickDevice; // the direct input device

int curX = 320;
int curY = 240;

// The callback function to enumerate the installed devices
BOOL CALLBACK EnumDevicesCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
BOOL CALLBACK    EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr; 

	MessageBox(NULL, "You must have a game controller plugged in", "MESSAGE", MB_OK);

	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return false;
	}

	// Create the DirectInput object. 
    hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, 
                            IID_IDirectInput8, (void**)&g_lpDI, NULL); 

	if FAILED(hr) 
	{
		MessageBox(NULL, "Unable to init DirectInput", "ERROR", MB_OK);
		return 0; 
	}

	hr = g_lpDI->EnumDevices( DI8DEVCLASS_GAMECTRL, EnumDevicesCallback,
                              NULL, DIEDFL_ATTACHEDONLY );
	if FAILED(hr) 
	{
		MessageBox(NULL, "No devices", "ERROR", MB_OK);
		return 0; 
	}

	if( FAILED( hr = g_joystickDevice->SetDataFormat( &c_dfDIJoystick2 ) ) )
	{
		MessageBox(NULL, "Controller failed to initialize", "ERROR", MB_OK);
        return hr;
	}

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
    if( FAILED( hr = g_joystickDevice->SetCooperativeLevel( wndHandle, DISCL_EXCLUSIVE | 
                                                             DISCL_FOREGROUND ) ) )
        return hr;

	if( FAILED( hr = g_joystickDevice->EnumObjects( EnumObjectsCallback, 
                                                NULL, DIDFT_ALL ) ) )
        return hr;

	g_joystickDevice->Acquire();

		// Main message loop:
	// Enter the message loop
    MSG msg; 
    ZeroMemory( &msg, sizeof(msg) );
    while( msg.message!=WM_QUIT )
    {
		// check for messages
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
			TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		// this is called when no messages are pending
		else
		{
			DIJOYSTATE2 js;           // DInput joystick state 
			hr = g_joystickDevice->Poll(); 

			if( FAILED(hr) )  
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = g_joystickDevice->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_joystickDevice->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        continue;
    }

			if( FAILED( hr = g_joystickDevice->GetDeviceState( sizeof(DIJOYSTATE2), &js ) ) )
				return hr; // The device should have been acquired during the Poll()

			curX += js.lX;
			curY += js.lY;
			
			if (curX > 620) curX = 620;
			if (curX < 0) curX = 0;
			if (curY < 0) curY = 0;
			if (curY > 440) curY = 440;
			InvalidateRect(wndHandle, NULL, TRUE); 
		}
	}
	
	if (g_lpDI) 
    { 
        if (g_joystickDevice) 
        { 
        // Always unacquire device before calling Release(). 
            g_joystickDevice->Unacquire(); 
            g_joystickDevice->Release();
            g_joystickDevice = NULL; 
        } 
        g_lpDI->Release();
        g_lpDI = NULL; 
    } 

	return (int) msg.wParam;
}

BOOL CALLBACK EnumDevicesCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext )
{
    HRESULT hr;

    hr = g_lpDI->CreateDevice( pdidInstance->guidInstance, &g_joystickDevice, NULL );

    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

bool initWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "DirectXExample";
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);

	wndHandle = CreateWindow("DirectXExample", 
							 "DirectXExample", 
							 WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT, 
							 CW_USEDEFAULT, 
							 640, 
							 480, 
							 NULL, 
							 NULL, 
							 hInstance, 
							 NULL);
   if (!wndHandle)
      return false;
   
   ShowWindow(wndHandle, SW_SHOW);
   UpdateWindow(wndHandle);

   return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_ERASEBKGND:
			return 1;
		break;
		case WM_PAINT:
			RECT rect;
			rect.top = curY;
			rect.bottom = rect.top + 64;
			rect.left = curX;
			rect.right = rect.left + 64;

			hdc = BeginPaint(hWnd, &ps); 
			HDC backbuffer = CreateCompatibleDC(hdc);
			HBITMAP backbufferBMAP = CreateCompatibleBitmap(hdc, 640, 480);
			SelectObject(backbuffer, backbufferBMAP);

			FillRect(backbuffer, &rect, (HBRUSH) (COLOR_WINDOW+13));
			BitBlt(hdc, 0, 0, 640, 480, backbuffer, 0, 0, SRCCOPY);

			EndPaint(hWnd, &ps); 
			DeleteObject(backbufferBMAP);
			DeleteDC(backbuffer);
            return 0L; 
		break;

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext )
{
    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = -1; 
        diprg.lMax              = +1; 
    
        // Set the range for the axis
        if( FAILED( g_joystickDevice->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;

		DIPROPDWORD center;
		center.diph.dwSize = sizeof(DIPROPDWORD);
		center.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		center.diph.dwHow = DIPH_BYID;
		center.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
		center.dwData = 100;
		if( FAILED( g_joystickDevice->SetProperty( DIPROP_DEADZONE,  &center.diph) ) ) 
            return DIENUM_STOP;         
    }
	return DIENUM_CONTINUE;
}
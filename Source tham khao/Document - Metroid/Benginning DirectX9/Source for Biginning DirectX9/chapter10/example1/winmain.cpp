#include <windows.h>
#include <dxerr9.h>
#include <dsound.h>

#include <string>
using namespace std;

////////////////////////////////////////////// forward declarations
bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LPDIRECTSOUND        g_pDS        = NULL;

BOOL CALLBACK DSCallback( GUID* pGUID, LPSTR strDesc, LPSTR strDrvName, VOID* pContext );

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr; 

	// ennumerate through the sound devices
	hr = DirectSoundEnumerate((LPDSENUMCALLBACK)DSCallback,0);
	
	if FAILED ( hr )
	{
		MessageBox(NULL, "Unable to init DirectSound", "ERROR", MB_OK);
        return hr;
	}
	return 0;
}

BOOL CALLBACK DSCallback( GUID* pGUID, LPSTR strDesc, LPSTR strDrvName, VOID* pContext )
{
	string tempString;

	tempString = "Device name = ";
	tempString += strDesc;
	tempString += "\nDriver name = ";
	tempString += strDrvName;

	MessageBox(NULL, tempString.c_str(), "message", MB_OK);

	return true;
}
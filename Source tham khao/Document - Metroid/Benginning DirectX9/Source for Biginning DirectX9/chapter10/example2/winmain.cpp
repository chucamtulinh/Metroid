#include <windows.h>
#include <dxerr9.h>
#include <dsound.h>

#include "dsutil.h"

#include <string>
using namespace std;

// Window functions and variables
HINSTANCE hInst;				// holds the instance for this app
HWND wndHandle;					// global window handle

bool    initWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// DirectSound functions and variables
LPDIRECTSOUND8        g_pDS;	// The DirectSound Device
		
bool initDirectSound(HWND hwnd);
void shutdownDirectSound(void);
LPDIRECTSOUNDBUFFER LoadWaveToSoundBuffer(std::string wavFilename);
void playSound(LPDIRECTSOUNDBUFFER whichBuffer);
void playSoundLoop(LPDIRECTSOUNDBUFFER whichBuffer);
void stopSound(LPDIRECTSOUNDBUFFER whichBuffer);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int curVolume = 0;
	int volChange = 1000;

	// call our function to init and create our window
	if (!initWindow(hInstance))
	{
		MessageBox(NULL, "Unable to create window", "ERROR", MB_OK);
		return 0;
	}

	// initialize DirectSound
	if (!initDirectSound(wndHandle))
	{
		MessageBox(NULL, "Unable to init DirectSound", "ERROR", MB_OK);
		return 0;
	}

	// load a wave file into a directsound buffer
	LPDIRECTSOUNDBUFFER DSBuffer = LoadWaveToSoundBuffer("sound1.wav");

	if (!DSBuffer)
	{
		MessageBox(NULL, "Unable to load sound1.wav", "ERROR", MB_OK);
		return 0;
	}

	// play this sound
	playSoundLoop(DSBuffer);

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
		else
		{
			curVolume+= volChange;
			if ((curVolume <= 10000) || (curVolume >= 10000))
				volChange *= -1;
			//DSBuffer->SetVolume(-10000);
		}
	}

	// Release the DirectSound buffer created above
	if (DSBuffer)
	{
		DSBuffer->Release();
		DSBuffer = NULL;
	}

	// shutdown DirectSound
	shutdownDirectSound();

	return (int) msg.wParam;
}

/*******************************************************************
* shutdownDirectSound
* Releases the DirecSound device
*******************************************************************/
void shutdownDirectSound(void)
{
	if (g_pDS)
	{
		g_pDS->Release();
		g_pDS = NULL;
	}
}

/*******************************************************************
* initDirectSound
* Initializes DirectSound
*******************************************************************/
bool initDirectSound(HWND hwnd)
{
	HRESULT hr;

	hr = DirectSoundCreate8( NULL, &g_pDS, NULL );
	if FAILED (hr)
		return false;

	// Set DirectSound cooperative level 
    hr = g_pDS->SetCooperativeLevel( hwnd, DSSCL_PRIORITY );
	if FAILED ( hr )
		return false;

	return true;
}

/*******************************************************************
* LoadWaveToSoundBuffer
* Loads a wave file into a DirectSound Buffer
*******************************************************************/
LPDIRECTSOUNDBUFFER LoadWaveToSoundBuffer(std::string wavFilename)
{
	LPDIRECTSOUNDBUFFER apDSBuffer = NULL;
	CWaveFile *wavFile;
	HRESULT hr;

	wavFile = new CWaveFile();
	wavFile->Open((char*)wavFilename.c_str(), NULL, WAVEFILE_READ );
	if( wavFile->GetSize() == 0 )
	{
		MessageBox(wndHandle, "invalid file", "ERROR", MB_OK);
		return false;
	}

	DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = 0;
    dsbd.dwBufferBytes   = wavFile->GetSize();
    dsbd.guid3DAlgorithm = GUID_NULL;
	dsbd.lpwfxFormat     = wavFile->m_pwfx;

	hr = g_pDS->CreateSoundBuffer( &dsbd, &apDSBuffer, NULL );
	if FAILED (hr)
	{
		MessageBox(NULL, "unable to create sound buffer", "ERROR", MB_OK);
		return NULL;
	}

	VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 
	
	hr = apDSBuffer->Lock( 0, wavFile->GetSize(), 
						   &pDSLockedBuffer, &dwDSLockedBufferSize, 
                           NULL, NULL, 0L );
	if FAILED (hr)
        return NULL;

    // Reset the wave file to the beginning 
    wavFile->ResetFile();

	// Read the wave file
    hr = wavFile->Read( (BYTE*) pDSLockedBuffer,
                       dwDSLockedBufferSize, 
                       &dwWavDataRead );
	// Check to make sure that this was successful
	if FAILED (hr)
        return NULL;

	// Check to make sure the wav file is not empty
    if( dwWavDataRead == 0 )
    {
        // Wav is blank, so just fill with silence
        FillMemory( (BYTE*) pDSLockedBuffer, 
                    dwDSLockedBufferSize, 
                    (BYTE)(wavFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
    }
    else if( dwWavDataRead < dwDSLockedBufferSize )
    {
	    // Don't repeat the wav file, just fill in silence 
        FillMemory( (BYTE*) pDSLockedBuffer + dwWavDataRead, 
                   dwDSLockedBufferSize - dwWavDataRead, 
                   (BYTE)(wavFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0 ) );
    }

    // Unlock the buffer, we don't need it anymore.
    apDSBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

	// Clean up
	delete wavFile;

	return apDSBuffer;
}

/*******************************************************************
* playSound
* plays a sound currently in a buffer only once
*******************************************************************/
void playSound(LPDIRECTSOUNDBUFFER whichBuffer)
{
	whichBuffer->Play( 0, 0, 0);
}

/*******************************************************************
* playSoundLoop
* plays a sound in a buffer repeatedly
*******************************************************************/
void playSoundLoop(LPDIRECTSOUNDBUFFER whichBuffer)
{
	whichBuffer->Play( 0, 0, DSBPLAY_LOOPING );
}

/*******************************************************************
* stopSound
* stops the sound in this buffer from playing
*******************************************************************/
void stopSound(LPDIRECTSOUNDBUFFER whichBuffer)
{
	whichBuffer->Stop();
}

/*******************************************************************
* initWindow
* inits and creates the window
*******************************************************************/
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

/*******************************************************************
* WndProc
* The window procedure for this window
*******************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
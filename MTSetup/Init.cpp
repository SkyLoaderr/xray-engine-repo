// Init.cpp
#include "stdafx.h"
#include <windows.h>
#include "MTSetup.h"

extern DWORD	g_dwOSVerFlags;
extern CRITICAL_SECTION	g_cs;
extern DWORD g_dwMainThreadId;
extern HANDLE		g_hAppMutex;

BOOL Init( HINSTANCE hInstance,      //in: handle to the instance
		   HINSTANCE hPrevInstance,  //in: always NULL in Win32
		   LPSTR     lpszCmdLine,    //in: string with ANSI cmd line args
		   int       nCmdShow)        //in: show minimized/normal
{
	BOOL		bRetVal = FALSE;
	HWND		hWnd;
	WNDCLASS	wndclass = {
					0,
					WndProc,
					0,
					0,
					hInstance,
					0,
					0,
					0,
					0,
					g_szAppName};

	g_hAppMutex = CreateMutex(NULL, FALSE, g_szAppName);
	if(ERROR_ALREADY_EXISTS == GetLastError())
	{
		// Another instance is running
		return FALSE;
	}

	g_dwOSVerFlags = GetOperatingSystemVer();
	if(g_dwOSVerFlags == 0)
		return FALSE;

	// Get the main thread ID to PostThreadMessage() on completion.
	g_dwMainThreadId = GetCurrentThreadId();

	InitializeCriticalSection(&g_cs);

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT"), g_szAppName, MB_ICONERROR);
		return FALSE;
	}

	hWnd = CreateWindow(g_szAppName,
						TEXT("The Wrapper Program"),
						WS_OVERLAPPED,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						NULL,
						NULL,
						hInstance,
						NULL);
	//ShowWindow(hWnd, nCmdShow);
	return TRUE;
}
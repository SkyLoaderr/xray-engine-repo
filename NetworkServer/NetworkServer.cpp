// NetworkServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "network.h"
#include "netserver.h"
#include "net_messages.h"
#include "ftimer.h"
#include "resource.h"
#include "log.h"

CTimer	Timer;
float	fTimeDelta		=	0;
bool	bMustExit		=	false;
DWORD	dwServerFlags	=	DPSESSION_OPTIMIZELATENCY | 
							DPSESSION_DIRECTPLAYPROTOCOL | 
							DPSESSION_NODATAMESSAGES;


static BOOL CALLBACK startDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		CheckRadioButton(hw, IDC_R_TCP, IDC_R_MODEM, IDC_R_TCP);
		CheckDlgButton	(hw, IDC_KEEPALIVE, BST_CHECKED);
		break;		
	case WM_DESTROY:
		if (IsDlgButtonChecked(hw, IDC_KEEPALIVE)==BST_CHECKED) dwServerFlags|=DPSESSION_KEEPALIVE;
		if (IsDlgButtonChecked(hw, IDC_MULTICAST)==BST_CHECKED) dwServerFlags|=DPSESSION_MULTICASTSERVER;
		break;
	case WM_CLOSE:
		EndDialog(hw, IDCANCEL);
		break;
	case WM_COMMAND:
		if( LOWORD(wp)==IDCANCEL ) {
			EndDialog(hw, IDCANCEL);
		}
		if( LOWORD(wp)==IDC_START) {
			EndDialog(hw, IDOK);
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    MSG         msg;
	BOOL		bGotMsg;

	// Startup options
	if (strstr(GetCommandLine(),"-quiet")==0) {
		int res = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_STARTUP), 0, startDlgProc );
		if (res!=IDOK) return 0;
	}

	// Real work
    CoInitialize	(NULL);
	CreateLog		();
	
	char cName [MAX_COMPUTERNAME_LENGTH + 1]; DWORD cSize = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(cName, &cSize);	cName[cSize+1]=0; cName[14]=0;
	
	net_RegisterMessages();
	CNetworkServer * pNS = new CNetworkServer(dpTCP, cName, dwServerFlags);
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	while( WM_QUIT != msg.message  &&  !bMustExit)
	{
		bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		if( bGotMsg )
		{
			TranslateMessage( &msg );
			DispatchMessage	( &msg );
		}
		else
		{
			fTimeDelta = Timer.GetAsync(); 
			Timer.Start();
			pNS->OnFrame();
			Sleep(1);
		}
	}
	_DELETE			(pNS);
	
    CoUninitialize	();
	CloseLog		();
	return 0;
}




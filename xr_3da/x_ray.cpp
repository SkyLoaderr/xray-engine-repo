//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	AlexMX		- Alexander Maksimchuk
//	Diver		- Oles Shishkovtsov
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "xr_input.h"
#include "xr_sndman.h"
#include "xr_creator.h"
#include "xr_ioconsole.h"
#include "x_ray.h"
#include "xr_ini.h"
#include "xr_smallfont.h"
#include "xr_trims.h"
#include "3dsoundrender.h"
#include "xr_streamsnd.h"
#include "scripting\script.h"
#include "std_classes.h"

// global variables
ENGINE_API	CApplication*	pApp			= NULL;
ENGINE_API	CCreator*		pCreator		= NULL;

// externs
extern void					InitMath			(void);
extern BOOL					StartGame			(DWORD num);

// startup point
void Startup()
{
	Engine.Initialize			( );
	Device.Initialize			( );
	Console.Initialize			( );
	Engine.External.Initialize	( );
	Console.Execute				("cfg_load startup.ltx");

	// Creation
	pSettings					= new CInifile		("system.ltx",FALSE);

#ifdef DEBUG
	pInput						= new CInput		(FALSE);
#else
	pInput						= new CInput		(TRUE);
#endif
	pSounds						= new CSoundManager	( );

	pApp						= new CApplication	( );

	// ...command line for auto level loading
	char *	pCmdLine			= GetCommandLine();
	char *	pLevelName			= strstr(pCmdLine,"-server ");
	char	cmd	[128];
	if (pLevelName) 
	{
		strconcat		(cmd,"server ",pLevelName+8);
		Console.Execute	(cmd);
	} else {
		pLevelName				= strstr(pCmdLine,"-client ");
		if (pLevelName)	{
			strconcat		(cmd,"client ",pLevelName+8);
			Console.Execute	(cmd);
		}
	}

	// Main cycle
	Device.Run					( );
	_DELETE						( pApp			);
	Engine.Event.Dump			( );

	// Destroying
	_DELETE						( pSounds		);
	_DELETE						( pInput		);
	_DELETE						( pSettings		);

	Console.Destroy				( );
	Device.ShutDown				( );
	Engine.Destroy				( );
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     char *    lpCmdLine,
                     int       nCmdShow)
{
    // Init COM so we can use CoCreateInstance
    CoInitializeEx	(NULL, COINIT_MULTITHREADED);

	mmgrInitialize	(2);

	CreateLog	(strstr(lpCmdLine,"-Q") || strstr(lpCmdLine,"-q"));
	Debug.Start	();

#ifndef DEBUG
	__try {
#endif
		Startup();

#ifndef DEBUG
	} __except(Debug.LogStack(GetExceptionInformation())) {
		MessageBox(0,"Unhandled exception. See ENGINE.LOG for details.","Error",MB_OK|MB_ICONSTOP);
	}
#endif

	Debug.Stop();
	CloseLog();
	mmgrDone();

	// check for need to execute something external
	if (strstr(lpCmdLine,"-exec ")) 
	{
		char *N = strstr(lpCmdLine,"-exec ")+6;
		return _execl(N,N,0);
	}
	return 0;
}

CApplication::CApplication()
{
	ll_dwReference	= 0;

	// events
	eQuit			= Engine.Event.Handler_Attach("KERNEL:quit",this);
	eStartServer	= Engine.Event.Handler_Attach("KERNEL:server",this);
	eStartClient	= Engine.Event.Handler_Attach("KERNEL:client",this);
	eDisconnect		= Engine.Event.Handler_Attach("KERNEL:disconnect",this);

	// levels
	Level_Current				= 0;
	Level_Scan					( );

	// Font
	pFont						= new CFontSmall	( );
	Device.seqRender.Add		( pFont, REG_PRIORITY_LOW-1000 );

	// Register us
	Device.seqFrame.Add			(this, REG_PRIORITY_HIGH+1000);
	Console.Show				( );
}

CApplication::~CApplication()
{
	Console.Hide				( );
	// font
	Device.seqRender.Remove		( pFont		);
	_DELETE						( pFont		);

	// events
	Engine.Event.Handler_Detach	(eDisconnect,this);
	Engine.Event.Handler_Detach	(eStartClient,this);
	Engine.Event.Handler_Detach	(eStartServer,this);
	Engine.Event.Handler_Detach	(eQuit,this);
}


BOOL StartGame(DWORD num) 
{
	R_ASSERT(pCreator==NULL);
	pCreator = (CCreator*)NEW_INSTANCE(CLSID_LEVEL);
	Console.Hide();
	if (!pCreator->Load(num)) {
		Log("! Error loading level");
		DEL_INSTANCE(pCreator);
		Console.Show();
	} else {
	}
	return true;
}

void CApplication::OnEvent(EVENT E, DWORD P1, DWORD P2)
{
	if (E==eQuit)	{
		PostQuitMessage	(0);
		
		for (DWORD i=0; i<Levels.size(); i++)
		{
			_FREE(Levels[i].folder	);
			_FREE(Levels[i].name	);
		}
	} else if (E==eStartServer) {
		Console.Hide();
		LPSTR		Name = LPSTR(P1);
		R_ASSERT	(0==pCreator);
		pCreator	= (CCreator*)	NEW_INSTANCE(CLSID_LEVEL);
		R_ASSERT	(pCreator->net_Server(Name));
		_FREE		(Name);
	} else if (E==eStartClient) {
		Console.Hide();
		LPSTR		Name = LPSTR(P1);
		R_ASSERT	(0==pCreator);
		pCreator	= (CCreator*)	NEW_INSTANCE(CLSID_LEVEL);
		R_ASSERT	(pCreator->net_Client(Name));
		_FREE		(Name);
	} else if (E==eDisconnect) {
		if (pCreator) {
			Console.Hide	();
			pCreator->net_Disconnect();
			DEL_INSTANCE	(pCreator);
			Console.Show	();
		}
	}
}

void CApplication::LoadBegin()
{
	ll_dwReference++;
	if (1==ll_dwReference) {
		ll_pStream	= Device.Streams.Create	(FVF::F_TL,4);
		ll_hLogo	= Device.Shader.Create	("font","ui\\logo",false);
	}
}

void CApplication::LoadEnd()
{
	ll_dwReference--;
	if (0==ll_dwReference) {
		Device.Shader.Delete(ll_hLogo);
		ll_pStream = 0;
	}
}

void CApplication::LoadTitle(char *S, char *S2)
{
	VERIFY(ll_dwReference);

	Device.Begin();

	// Draw logo
	DWORD C		=0xffffffff;
	DWORD Offset;
	FVF::TL* pv = (FVF::TL*) ll_pStream->Lock(4,Offset);
	pv->set(0, float(Device.dwHeight), 1, 1, C, 0, 1); pv++;
	pv->set(0, 0, 1, 1, C, 0, 0); pv++;
	pv->set(float(Device.dwWidth), float(Device.dwHeight), 1, 1, C, 1, 1); pv++;
	pv->set(float(Device.dwWidth), 0, 1, 1, C, 1, 0); pv++;
	ll_pStream->Unlock(4);

	Device.Shader.Set		(ll_hLogo);
	Device.Primitive.Draw	(ll_pStream,4,2,Offset,Device.Streams_QuadIB);

	// Draw title
	Log(S,S2);
	R_ASSERT(pFont);
	pFont->Clear();
	pFont->Color(D3DCOLOR_RGBA(192,192,192,255));
	pFont->Size	(0.02f);
	char *F = "~%s";
	if (S2) F="~%s%s";
	pFont->Out	(0.f,0.93f,F,S,S2);
	pFont->OnRender();

	Device.End	();
}

void CApplication::OnFrame( )
{
	Engine.Event.OnFrame();
}

void CApplication::Level_Scan()
{
	vector<char*>	folder;
	Engine.FS.List	(folder,Path.Levels,FS_ListFolders);
	R_ASSERT		(!folder.empty());
	for (DWORD i=0; i<folder.size(); i++)
	{
		FILE_NAME	N,F;
		strconcat	(N,Path.Levels,folder[i]);
		if	(
			Engine.FS.Exist(F,N,"level")		&&
			Engine.FS.Exist(F,N,"level.ltx")
			)
		{
			if (!Engine.FS.Exist(F,N,"level.ai"))
				Msg("! Warning: partially compiled level: %s",folder[i]);
			sLevelInfo			LI;
			LI.folder			= folder[i];
			LI.name				= "";
			Levels.push_back	(LI);
		} else {
			Msg("! Level not compiled: %s",folder[i]);
			free(folder[i]);
		}
	}
}
void CApplication::Level_Set(DWORD L)
{
	if (L>=Levels.size()) return;
	Level_Current = L;
	strconcat(Path.Current,Path.Levels,Levels[L].folder);
}
int CApplication::Level_ID(LPCSTR name)
{
	char buffer	[256];
	strconcat	(buffer,name,"\\");
	for (DWORD I=0; I<Levels.size(); I++)
	{
		if (0==stricmp(buffer,Levels[I].folder))	return int(I);
	}
	return -1;
}

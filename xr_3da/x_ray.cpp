//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	Oles		- Oles Shishkovtsov
//	AlexMX		- Alexander Maksimchuk
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "xr_input.h"
#include "xr_sndman.h"
#include "xr_creator.h"
#include "xr_ioconsole.h"
#include "x_ray.h"
#include "xr_ini.h"
#include "xr_trims.h"
#include "3dsoundrender.h"
#include "xr_streamsnd.h"
#include "std_classes.h"
#include "GameFont.h"
#include <crtdbg.h>
  
// global variables
ENGINE_API	CApplication*	pApp			= NULL;
ENGINE_API	CCreator*		pCreator		= NULL;

// externs
extern void					InitMath			(void);
extern BOOL					StartGame			(u32 num);

// startup point
void Startup()
{
	Engine.Initialize			( );
	Device.Initialize			( );

	// Creation
	pSettings					= new CInifile		("GameData\\system.ltx",TRUE);

	Console.Initialize			( );
	Engine.External.Initialize	( );
	
	// Execute script
	strcpy						(Console.ConfigFile,"user.ltx");
	if (strstr(Engine.Params,"-ltx ")) {
		string64				c_name;
		sscanf					(strstr(Engine.Params,"-ltx ")+5,"%[^ ] ",c_name);
		strcpy					(Console.ConfigFile,c_name);
	}
	Console.ExecuteScript		(Console.ConfigFile);

	// Name
	if (strstr(Engine.Params,"-name ")) {
		string64				c_name,cmd;
		sscanf					(strstr(Engine.Params,"-name ")+6,"%[^ ] ",c_name);
		strconcat				(cmd,"net_name ",c_name);
		Console.Execute			(cmd);
	}

	BOOL bCaptureInput			= !strstr(Engine.Params,"-i");
#ifdef DEBUG
	bCaptureInput				= FALSE;
#endif

	pInput						= new CInput		(bCaptureInput);
	pSounds						= new CSoundManager	( );

	pApp						= new CApplication	( );

	// ...command line for auto level loading
	char *	pLevelName			= strstr(Engine.Params,"-server ");
	char	cmd	[128];
	if (pLevelName) 
	{
		strconcat				(cmd,"server ",pLevelName+8);
		Console.Execute			(cmd);
	} else {
		pLevelName				= strstr(Engine.Params,"-client ");
		if (pLevelName)	{
			strconcat			(cmd,"client ",pLevelName+8);
			Console.Execute		(cmd);
		}
	}

	if (strstr(Engine.Params,"-demo ")) {
		string64				cmd;
		string64				pDemoName;
		sscanf					(strstr(Engine.Params,"-demo ")+6,"%[^ ] ",pDemoName);
		strconcat				(cmd,"demo_play ",pDemoName);
		Console.Execute			(cmd);
	}

	// Main cycle
	// Sleep					(1000);
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

	// mmgrInitialize	(0);

	BOOL bCaptureExceptions	= !(strstr(lpCmdLine,"-E") || strstr(lpCmdLine,"-e"));
#ifdef DEBUG
	bCaptureExceptions		= FALSE;
#endif
	
	CreateLog		(!(strstr(lpCmdLine,"-Q") || strstr(lpCmdLine,"-q")));
	Debug.Start	();

	// _CrtSetDbgFlag( _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	if (bCaptureExceptions)	
	{
		__try {
			Startup	();
			
		} __except(Debug.LogStack(GetExceptionInformation())) {
			MessageBox(0,"Unhandled exception. See ENGINE.LOG for details.","Error",MB_OK|MB_ICONSTOP);
		}
	} else {
		Startup();
	}
	
	Debug.Stop	();
	CloseLog	();
	// mmgrDone	();

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
	eStartServerLoad= Engine.Event.Handler_Attach("KERNEL:server_load",this);
	eStartClient	= Engine.Event.Handler_Attach("KERNEL:client",this);
	eDisconnect		= Engine.Event.Handler_Attach("KERNEL:disconnect",this);

	// levels
	Level_Current				= 0;
	Level_Scan					( );

	// Font
	pFontSystem					= new CGameFont	("startup_font",CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
	Device.seqRender.Add		( pFontSystem, REG_PRIORITY_LOW-1000 );

	// Register us
	Device.seqFrame.Add			(this, REG_PRIORITY_HIGH+1000);
	Console.Show				( );
}

CApplication::~CApplication()
{
	Console.Hide				( );
	// font
	Device.seqRender.Remove		( pFontSystem		);
	_DELETE						( pFontSystem		);

	// events
	Engine.Event.Handler_Detach	(eDisconnect,this);
	Engine.Event.Handler_Detach	(eStartClient,this);
	Engine.Event.Handler_Detach	(eStartServerLoad,this);
	Engine.Event.Handler_Detach	(eStartServer,this);
	Engine.Event.Handler_Detach	(eQuit,this);
}

/*
BOOL StartGame(u32 num) 
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
*/

void CApplication::OnEvent(EVENT E, u32 P1, u32 P2)
{
	if (E==eQuit)	{
		PostQuitMessage	(0);
		
		for (u32 i=0; i<Levels.size(); i++)
		{
			_FREE(Levels[i].folder	);
			_FREE(Levels[i].name	);
		}
	} else if (E==eStartServerLoad) 
	{
		/*
		Console.Hide();
		LPSTR		Name = LPSTR(P1);
		R_ASSERT	(0==pCreator);
		pCreator	= (CCreator*)	NEW_INSTANCE(CLSID_LEVEL);
		R_ASSERT	(pCreator->net_Server(Name,TRUE));
		_FREE		(Name);
		*/
	} else if (E==eStartServer) {
		Console.Hide();
		LPSTR		Name = LPSTR(P1);
		R_ASSERT	(0==pCreator);
		pCreator	= (CCreator*)	NEW_INSTANCE(CLSID_LEVEL);
		R_ASSERT	(pCreator->net_Start(Name,TRUE));
		_FREE		(Name);
	} else if (E==eStartClient) {
		Console.Hide();
		LPSTR		Name = LPSTR(P1);
		R_ASSERT	(0==pCreator);
		pCreator	= (CCreator*)	NEW_INSTANCE(CLSID_LEVEL);
		if			(!pCreator->net_Start(Name,FALSE))
		{
			Msg						("! FAILED TO CONNECT");
			DEL_INSTANCE			(pCreator);
			Console.Show			();
		} 
		_FREE		(Name);
	} else if (E==eDisconnect) {
		if (pCreator) {
			Console.Hide			();
			pCreator->net_Stop		();
			DEL_INSTANCE			(pCreator);
			Console.Show			();
		}
	}
}

/*
LPSTR Name	= LPSTR(P1);
int id		= Level_ID(Name);
if (id<0)	
{
_FREE		(Name);
return;
}
Level_Set	(id);

*/
void CApplication::LoadBegin()
{
	ll_dwReference++;
	if (1==ll_dwReference) {
		ll_hVS		= Device.Shader._CreateVS	(FVF::F_TL);
		ll_hLogo1	= Device.Shader.Create		("font","ui\\logo");
		ll_hLogo2	= Device.Shader.Create		("font","ui\\ui_logo_nv");
		ll_hLogo	= ll_hLogo2;
	}
}

void CApplication::LoadEnd()
{
	ll_dwReference--;
	if (0==ll_dwReference) {
		Device.Shader.Delete	(ll_hLogo2);
		Device.Shader.Delete	(ll_hLogo1);
		Device.Shader._DeleteVS	(ll_hVS);
	}
}

void CApplication::LoadTitle	(char *S, char *S2)
{
	VERIFY(ll_dwReference);

	Device.Begin();

	// Draw logo
	u32	Offset;
	u32	C						= 0xffffffff;
	u32	_w						= Device.dwWidth;
	u32	_h						= Device.dwHeight;
	FVF::TL* pv						= (FVF::TL*) Device.Streams.Vertex.Lock(4,ll_hVS->dwStride,Offset);
	pv->set(0, float(_h), 1, 1, C, 0, 1);			pv++;
	pv->set(0, 0, 1, 1, C, 0, 0);					pv++;
	pv->set(float(_w), float(_h), 1, 1, C, 1, 1);	pv++;
	pv->set(float(_w), 0, 1, 1, C, 1, 0);			pv++;
	Device.Streams.Vertex.Unlock	(4,ll_hVS->dwStride);
	Device.Shader.set_Shader		(ll_hLogo);
	Device.Primitive.setVertices	(ll_hVS->dwHandle,ll_hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(Offset,Device.Streams.QuadIB);;
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);

	// Draw title
	Log			(S,S2);
	R_ASSERT	(pFontSystem);
	pFontSystem->Clear();
	pFontSystem->Color(D3DCOLOR_RGBA(192,192,192,255));
	char *F = "~%s";
	if (S2) F="~%s%s";
	pFontSystem->Out	(0.f,0.93f,F,S,S2);
	pFontSystem->OnRender();

	Device.End	();
}

void CApplication::LoadSwitch()
{
	if (ll_hLogo == ll_hLogo1)	ll_hLogo = ll_hLogo2;
	else						ll_hLogo = ll_hLogo1;
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
	for (u32 i=0; i<folder.size(); i++)
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
			LI.name				= 0;
			Levels.push_back	(LI);
		} else {
			Msg		("! Level not compiled: %s",folder[i]);
			xr_free	(folder[i]);
		}
	}
}

void CApplication::Level_Set(u32 L)
{
	if (L>=Levels.size()) return;
	Level_Current = L;
	strconcat	(Path.Current,Path.Levels,Levels[L].folder);
	strlwr		(Path.Current);
}
int CApplication::Level_ID(LPCSTR name)
{
	char buffer	[256];
	strconcat	(buffer,name,"\\");
	for (u32 I=0; I<Levels.size(); I++)
	{
		if (0==stricmp(buffer,Levels[I].folder))	return int(I);
	}
	return -1;
}

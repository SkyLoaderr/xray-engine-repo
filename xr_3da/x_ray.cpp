//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	Oles		- Oles Shishkovtsov
//	AlexMX		- Alexander Maksimchuk
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "xr_input.h"
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
extern BOOL					StartGame			(u32 num);

// startup point
void Startup				()
{
	//
	if (strstr(Core.Params,"-external"))	Path	= Path_ExternalBuild;
	else									Path	= Path_InternalBuild;

	// initialization
	Engine.Initialize			( );
	Device.Initialize			( );

	// Creation
	string256 fname; strconcat	(fname,Path.GameData,"system.ltx");
	pSettings					= xr_new<CInifile>	(fname,TRUE);

	Console.Initialize			( );
	Engine.External.Initialize	( );
	
	// Execute script
	strcpy						(Console.ConfigFile,"user.ltx");
	if (strstr(Core.Params,"-ltx ")) {
		string64				c_name;
		sscanf					(strstr(Core.Params,"-ltx ")+5,"%[^ ] ",c_name);
		strcpy					(Console.ConfigFile,c_name);
	}
	Console.ExecuteScript		(Console.ConfigFile);

	BOOL bCaptureInput			= !strstr(Core.Params,"-i");
#ifdef DEBUG
	bCaptureInput				= !bCaptureInput;
#endif

	pInput						= xr_new<CInput>		(bCaptureInput);
	Sound->Initialize			();

	// ...command line for auto start
	LPCSTR	pStartup			= strstr		(Core.Params,"-start ");
	if (pStartup)				Console.Execute	(pStartup+1);

	// Main cycle
	Device.Create				( );
	pApp						= xr_new<CApplication>	();
	Device.Run					( );
	xr_delete					( pApp			);
	Engine.Event.Dump			( );

	// Destroying
	Sound->Destroy				( );
	xr_delete					( pInput		);
	xr_delete					( pSettings		);

	Console.Destroy				( );
	Engine.Destroy				( );
	Device.Destroy				( );
}

WORD getFPUsw() 
{
	WORD SW;
	__asm fstcw SW;
	return SW;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     char *    lpCmdLine,
                     int       nCmdShow)
{
    // Init COM so we can use CoCreateInstance
    CoInitializeEx			(NULL, COINIT_MULTITHREADED);
	Core._initialize		("xray");
	FPU::m24r				();

	CreateLog		(!(strstr(lpCmdLine,"-Q") || strstr(lpCmdLine,"-q")));

	Startup			();
	
	CloseLog		();

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
	eQuit						= Engine.Event.Handler_Attach("KERNEL:quit",this);
	eStart						= Engine.Event.Handler_Attach("KERNEL:start",this);
	eStartLoad					= Engine.Event.Handler_Attach("KERNEL:load",this);
	eDisconnect					= Engine.Event.Handler_Attach("KERNEL:disconnect",this);

	// levels
	Level_Current				= 0;
	Level_Scan					( );

	// Font
	pFontSystem					= xr_new<CGameFont>	("startup_font",CGameFont::fsGradient|CGameFont::fsDeviceIndependent);
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
	xr_delete						( pFontSystem		);

	// events
	Engine.Event.Handler_Detach	(eDisconnect,this);
	Engine.Event.Handler_Detach	(eStartLoad,this);
	Engine.Event.Handler_Detach	(eStart,this);
	Engine.Event.Handler_Detach	(eQuit,this);
}

void CApplication::OnEvent(EVENT E, u32 P1, u32 P2)
{
	if (E==eQuit)	{
		PostQuitMessage	(0);
		
		for (u32 i=0; i<Levels.size(); i++)
		{
			xr_free(Levels[i].folder	);
			xr_free(Levels[i].name	);
		}
	} else if (E==eStart) {
		Console.Hide();
		LPSTR		op_server		= LPSTR	(P1);
		LPSTR		op_client		= LPSTR	(P2);
		R_ASSERT	(0==pCreator);
		pCreator	= (CCreator*)	NEW_INSTANCE(CLSID_LEVEL);
		R_ASSERT	(pCreator->net_Start(op_server,op_client));
		xr_free		(op_server);
		xr_free		(op_client);

		// start any console command
		if (strstr(Core.Params,"-$")) {
			string64			buf,cmd,param;
			sscanf				(strstr(Core.Params,"-$")+2,"%[^ ] %[^ ] ",cmd,param);
			strconcat			(buf,cmd," ",param);
			Console.Execute		(buf);
		}
	} else if (E==eDisconnect) {
		if (pCreator) {
			Console.Hide			();
			pCreator->net_Stop		();
			DEL_INSTANCE			(pCreator);
			Console.Show			();
		}
	}
}

void CApplication::LoadBegin()
{
	ll_dwReference++;
	if (1==ll_dwReference) {
		ll_hGeom		= Device.Shader.CreateGeom	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
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
		Device.Shader.DeleteGeom	(ll_hGeom);
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
	FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock(4,ll_hGeom->vb_stride,Offset);
	pv->set						(0, float(_h), 1, 1, C, 0, 1);			pv++;
	pv->set						(0, 0, 1, 1, C, 0, 0);					pv++;
	pv->set						(float(_w), float(_h), 1, 1, C, 1, 1);	pv++;
	pv->set						(float(_w), 0, 1, 1, C, 1, 0);			pv++;
	RCache.Vertex.Unlock		(4,ll_hGeom->vb_stride);
	
	RCache.set_Shader			(ll_hLogo);
	RCache.set_Geometry			(ll_hGeom);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

	// Draw title
	Log			(S,S2);
	R_ASSERT	(pFontSystem);
	pFontSystem->Clear();
	pFontSystem->SetColor(D3DCOLOR_RGBA(192,192,192,255));
	pFontSystem->SetAligment(CGameFont::alCenter);
	char *F = "%s";
	if (S2) F="%s%s";
	pFontSystem->Out		(0.f,0.93f,F,S,S2);
	pFontSystem->OnRender	();

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
			/*
			if (!Engine.FS.Exist(F,N,"level.ai"))
				Msg("! Warning: partially compiled level: %s",folder[i]);
			*/
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

// xrLC.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "math.h"
#include "build.h"

#pragma comment(linker,"/STACK:0x800000,0x400000")
//#pragma comment(linker,"/HEAP:0x70000000,0x10000000")

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"X:\\xrProgressive.lib")
#pragma comment(lib,"X:\\xrCDB.lib")
#pragma comment(lib,"X:\\FreeImage.lib")
#pragma comment(lib,"X:\\xrHemisphere.lib")
#pragma comment(lib,"X:\\xrCore.lib")

CBuild*	pBuild		= NULL;

extern void __cdecl logThread(void *dummy);
extern volatile BOOL bClose;

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h	== this help\n"
	"-o			== modify build options\n"
	"-f<NAME>	== compile level in GameData\\Levels\\<NAME>\\\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help()
{
	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION);
}

typedef int __cdecl xrOptions(b_params* params, DWORD version, bool bRunBuild);

void Startup(LPSTR     lpCmdLine)
{
	char cmd[512],name[256];
	BOOL bModifyOptions		= FALSE;

	strcpy(cmd,lpCmdLine);
	strlwr(cmd);
	if (strstr(cmd,"-?") || strstr(cmd,"-h"))			{ Help(); return; }
	if (strstr(cmd,"-f")==0)							{ Help(); return; }
	if (strstr(cmd,"-o"))								bModifyOptions	= TRUE;
	if (strstr(cmd,"-r2"))								b_R2			= TRUE;
	
	// Give a LOG-thread a chance to startup
	//_set_sbh_threshold(1920);
	InitCommonControls	();
	_beginthread		(logThread,	0,0);
	Sleep				(150);
	
	// Faster FPU 
	DWORD					dwStartupTime	= timeGetTime();
	SetPriorityClass		(GetCurrentProcess(),IDLE_PRIORITY_CLASS);

	/*
	DWORD	dwMin			= 1800*(1024*1024);
	DWORD	dwMax			= 1900*(1024*1024);
	if (0==SetProcessWorkingSetSize(GetCurrentProcess(),dwMin,dwMax))
	{
		clMsg("*** Failed to expand working set");
	};
	*/
	
	// Load project
	name[0]=0;				sscanf(strstr(cmd,"-f")+2,"%s",name);
	string256				prjName;
	FS.update_path			(prjName,"$game_levels$",strconcat(prjName,name,"\\build.prj"));
	Phase					((string("Reading project [")+string(name)+string("]...")).c_str());

	IReader*	F			= FS.r_open(prjName);

	// Version
	DWORD version;
	F->r_chunk			(EB_Version,&version);
	R_ASSERT(XRCL_CURRENT_VERSION==version);

	// Header
	b_params				Params;
	F->r_chunk			(EB_Parameters,&Params);

	// Show options if needed
	if (bModifyOptions)		
	{
		Phase		("Project options...");
		HMODULE		L = LoadLibrary		("xrLC_Options.dll");
		void*		P = GetProcAddress	(L,"_frmScenePropertiesRun");
		R_ASSERT	(P);
		xrOptions*	O = (xrOptions*)P;
		int			R = O(&Params,version,false);
		FreeLibrary	(L);
		if (R==2)	{
			ExitProcess(0);
		}
	}
	
	// Conversion
	Phase					("Converting data structures...");
	pBuild					= xr_new<CBuild>();
	pBuild->Load			(Params,*F);
	xr_delete				(F);
	
	// Call for builder
	pBuild->Run				("gamedata\\levels\\"+string(name));
	xr_delete				(pBuild);

	// Show statistic
	char	stats[256];
	extern	string make_time(DWORD sec);
	extern  HWND logWindow;

	DWORD	dwEndTime		= timeGetTime();
	sprintf					(stats,"Time elapsed: %s",make_time((dwEndTime-dwStartupTime)/1000).c_str());
	clMsg					("Build succesful!\n%s",stats);
	MessageBox				(logWindow,stats,"Congratulation!",MB_OK|MB_ICONINFORMATION);

	// Close log
	bClose					= TRUE;
	Sleep					(500);
}

int APIENTRY WinMain(HINSTANCE hInst,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// Initialize debugging
	Core._initialize	("xrLC");
	Startup				(lpCmdLine);
	Core._destroy		();
	
	return 0;
}

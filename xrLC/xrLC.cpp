// xrLC.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "math.h"
#include "debugkernel.h"
#include "build.h"

#pragma comment(linker,"/STACK:0x800000,0x400000")
//#pragma comment(linker,"/HEAP:0x40000000,0x10000000")

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx8.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"X:\\xrProgressive.lib")
#pragma comment(lib,"X:\\xrCDB.lib")
#pragma comment(lib,"X:\\FreeImage.lib")
#pragma comment(lib,"X:\\xrHemisphere.lib")

CBuild*	pBuild		= NULL;

extern void __cdecl logThread(void *dummy);
extern volatile BOOL bClose;

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h	== this help\n"
	"-f<NAME>	== compile level in x:\\game\\data\\<NAME>\\\n"
	"-o			== modify build options\n"
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
	if (strstr(cmd,"-o"))								bModifyOptions = TRUE;
	

	// Give a LOG-thread a chance to startup
	//_set_sbh_threshold(1920);
	InitCommonControls	();
	_beginthread		(logThread,	0,0);
	Sleep				(150);
	
	// Faster FPU 
	InitMath				();
	DWORD					dwStartupTime	= timeGetTime();
	SetPriorityClass		(GetCurrentProcess(),IDLE_PRIORITY_CLASS);
	
	// Load project
	name[0]=0;				sscanf(strstr(cmd,"-f")+2,"%s",name);
	string prjName			= "game\\data\\levels\\"+string(name)+"\\build.prj";
	Phase					("Reading project...");
	CCompressedStream*		F	= new CCompressedStream(prjName.c_str(),	"xrLC");
	CStream&				FS	= *F;

	// Version
	DWORD version;
	FS.ReadChunk			(EB_Version,&version);
	R_ASSERT(XRCL_CURRENT_VERSION==version);

	// Header
	b_params				Params;
	FS.ReadChunk			(EB_Parameters,&Params);

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
	pBuild					= new CBuild(Params,FS);
	_DELETE					(F);
	
	// Call for builder
	pBuild->Run				("game\\data\\levels\\"+string(name));
	delete					pBuild;

	// Show statistic
	char	stats[256];
	extern	string make_time(DWORD sec);
	extern  HWND logWindow;

	DWORD	dwEndTime		= timeGetTime();
	sprintf					(stats,"Time elapsed: %s",make_time((dwEndTime-dwStartupTime)/1000).c_str());
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
	Debug.Start		();

#ifndef _DEBUG
	__try
	{
#endif
		Startup		(lpCmdLine);
#ifndef _DEBUG
	}
	__except(Debug.LogStack(GetExceptionInformation()))
	{
		MessageBox(0,"Access violation occured. See ENGINE.LOG for details.","Error",MB_OK|MB_ICONSTOP);
	}
#endif
	
	Debug.Stop		();
	
	return 0;
}

// xrLC.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "math.h"
#include "debugkernel.h"
#include "build.h"

#pragma comment(linker,"/STACK:0x800000,0x400000")
#pragma comment(linker,"/HEAP:0x40000000,0x8000000")

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx8.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"X:\\xrProgressive.lib")
#pragma comment(lib,"X:\\xrOcclusion.lib")
#pragma comment(lib,"X:\\xrStrips.lib")
#pragma comment(lib,"X:\\xrDXTC.lib")
#pragma comment(lib,"X:\\xrSpherical.lib")
#pragma comment(lib,"X:\\FreeImage.lib")

CBuild*	pBuild = NULL;

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
	InitCommonControls	();
	_beginthread		(logThread,	0,0);
	Sleep				(150);
	
	// Faster FPU 
	InitMath			();
	DWORD				dwStartupTime	= timeGetTime();
	
	// Load project
	name[0]=0;			sscanf(strstr(cmd,"-f")+2,"%s",name);
	string prjName		= "x:\\game\\data\\levels\\"+string(name)+"\\build.prj";
	Phase				("Reading project...");
	CVirtualFileStream	FS(prjName.c_str());
	void*				data = FS.Pointer();

	b_transfer	Header		= *((b_transfer *) data);
	R_ASSERT(XRCL_CURRENT_VERSION==Header._version);
	Header.faces			= (b_face*)		(DWORD(data)+DWORD(Header.faces));
	Header.vertices			= (b_vertex*)	(DWORD(data)+DWORD(Header.vertices));
	Header.material			= (b_material*)	(DWORD(data)+DWORD(Header.material));
	Header.shaders			= (b_shader*)	(DWORD(data)+DWORD(Header.shaders));
	Header.textures			= (b_texture*)	(DWORD(data)+DWORD(Header.textures));
	Header.lights			= (b_light*)	(DWORD(data)+DWORD(Header.lights));
	Header.light_keys		= (Flight*)		(DWORD(data)+DWORD(Header.light_keys));
	Header.glows			= (b_glow*)		(DWORD(data)+DWORD(Header.glows));
	Header.particles		= (b_particle*)	(DWORD(data)+DWORD(Header.particles));
	Header.occluders		= (b_occluder*)	(DWORD(data)+DWORD(Header.occluders));
	Header.portals			= (b_portal*)	(DWORD(data)+DWORD(Header.portals));

	// Show options if needed
	if (bModifyOptions)		{
		Phase		("Project options...");
		HMODULE		L = LoadLibrary("xrLC_Options.dll");
		void*		P = GetProcAddress(L,"_frmScenePropertiesRun");
		R_ASSERT	(P);
		xrOptions*	O = (xrOptions*)P;
		int			R = O(&Header.params,Header._version,false);
		FreeLibrary	(L);
		if (R==2)	{
			ExitProcess(0);
		}
	}
	
	pBuild	= new CBuild(&Header);
	
	// Call for builder
	pBuild->Run();
	delete pBuild;

	// Show statistic
	char	stats[256];
	extern	string make_time(DWORD sec);
	extern  HWND logWindow;

	DWORD	dwEndTime = timeGetTime();
	sprintf(stats,"Time elapsed: %s",make_time((dwEndTime-dwStartupTime)/1000).c_str());
	MessageBox(logWindow,stats,"Congratulation!",MB_OK|MB_ICONINFORMATION);

	// Close log
	bClose = TRUE;
	Sleep	(500);
	
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

// xrAI.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "xr_ini.h"
#include "process.h"
#include "xrAI.h"

#include "xrGraph.h"
#include "xr_graph_merge.h"
#include "xr_spawn_merge.h"
#include "xrCrossTable.h"
#include "path_test.h"
#include <mmsystem.h>

#pragma comment(linker,"/STACK:0x800000,0x400000")

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"x:/xrcdb.LIB")
#pragma comment(lib,"x:/MagicFM.LIB")
#pragma comment(lib,"x:/xrCore.LIB")
#pragma comment(lib,"x:/xrSE_Factory.LIB")

extern void	xrCompiler			(LPCSTR name, bool draft_mode);
extern void __cdecl logThread	(void *dummy);
extern volatile BOOL bClose;
extern void test_smooth_path	(LPCSTR name);
extern void test_hierarchy		(LPCSTR name);
extern void	xrConvertMaps		();
extern void	test_goap			();
extern void	smart_cover			(LPCSTR name);
extern void	verify_level_graph	(LPCSTR name, bool verbose);

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h   == this help\n"
	"-f<NAME>   == compile level in gamedata/levels/<NAME>/\n"
	"-o         == modify build options\n"
	"-g<NAME>   == build off-line AI graph and cross-table to ai-map in gamedata/levels/<NAME>/\n"
	"-m         == merge level graphs\n"
	"-s         == build game spawn data\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help()
{	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION); }

char INI_FILE[256];

extern  HWND logWindow;

void Startup(LPSTR     lpCmdLine)
{
	char cmd[512],name[256];
	BOOL bModifyOptions		= FALSE;

	strcpy(cmd,lpCmdLine);
	strlwr(cmd);
	if (strstr(cmd,"-?") || strstr(cmd,"-h"))			{ Help(); return; }
	if ((strstr(cmd,"-f")==0) && (strstr(cmd,"-g")==0) && (strstr(cmd,"-m")==0) && (strstr(cmd,"-s")==0) && (strstr(cmd,"-t")==0) && (strstr(cmd,"-c")==0) && (strstr(cmd,"-verify")==0))	{ Help(); return; }
	if (strstr(cmd,"-o"))								bModifyOptions = TRUE;

	// Give a LOG-thread a chance to startup
	InitCommonControls	();
	Sleep				(150);
	_beginthread		(logThread,	1024*1024,0);
	while				(!logWindow)	Sleep		(150);
	
	// Load project
	name[0]=0; 
	if (strstr(cmd,"-f"))
		sscanf	(strstr(cmd,"-f")+2,"%s",name);
	else
		if (strstr(cmd,"-g"))
			sscanf	(strstr(cmd,"-g")+2,"%s",name);
		else
			if (strstr(cmd,"-s"))
				sscanf	(strstr(cmd,"-s")+2,"%s",name);
			else
				if (strstr(cmd,"-t"))
					sscanf	(strstr(cmd,"-t")+2,"%s",name);
				else
					if (strstr(cmd,"-verify"))
						sscanf	(strstr(cmd,"-verify")+xr_strlen("-verify"),"%s",name);

	if (xr_strlen(name))
		strcat			(name,"\\");
	string4096			prjName;
	prjName				[0] = 0;
	FS.update_path		(prjName,"$game_levels$",name);

	u32				dwStartupTime	= timeGetTime();
	
	FS.update_path		(INI_FILE,"$game_config$","game.ltx");
	
	if (strstr(cmd,"-f"))
		xrCompiler			(prjName,!!strstr(cmd,"-draft"));
	else
		if (strstr(cmd,"-g")) {
			xrBuildGraph		(prjName);
			xrBuildCrossTable	(prjName);
		}
		else {
			if (strstr(cmd,"-m")) {
				xrMergeGraphs		(prjName);
			}
			else
				if (strstr(cmd,"-s")) {
					if (xr_strlen(name))
						name[xr_strlen(name) - 1] = 0;
					char				*output = strstr(cmd,"-out");
					if (output) {
						output			+= xr_strlen("-out");
						_TrimLeft		(output);
					}
					char				*start = strstr(cmd,"-start");
					if (start) {
						start			+= xr_strlen("-start");
						_TrimLeft		(start);
					}
					xrMergeSpawns		(name,output,start);
				}
				else
					if (strstr(cmd,"-t")) {
//						path_test			(prjName);
//						test_smooth_path	(prjName);
//						test_hierarchy		(prjName);
						test_goap			();
//						smart_cover			(prjName);
					}
					else
						if (strstr(cmd,"-c")) {
//							xrConvertMaps	();
						}
						else
							if (strstr(cmd,"-verify")) {
								verify_level_graph	(prjName,!strstr(cmd,"-noverbose"));
							}
		}
	// Show statistic
	char	stats[256];
	extern	std::string make_time(u32 sec);
	extern  HWND logWindow;
	u32				dwEndTime = timeGetTime();
	sprintf				(stats,"Time elapsed: %s",make_time((dwEndTime-dwStartupTime)/1000).c_str());
	MessageBox			(logWindow,stats,"Congratulation!",MB_OK|MB_ICONINFORMATION);

	bClose				= TRUE;
	FlushLog			();
	Sleep				(500);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	Startup					(lpCmdLine);
	return					(0);
}
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

#pragma comment(linker,"/STACK:0x800000,0x400000")

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"x:\\xrcdb.LIB")
#pragma comment(lib,"x:\\MagicFM.LIB")
#pragma comment(lib,"x:\\xrCore.LIB")

extern void	xrCompiler			(LPCSTR name);
extern void __cdecl logThread	(void *dummy);
extern volatile BOOL bClose;

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h   == this help\n"
	"-f<NAME>   == compile level in gamedata\\levels\\<NAME>\\\n"
	"-o         == modify build options\n"
	"-g<NAME>   == build off-line AI graph in gamedata\\levels\\<NAME>\\\n"
	"-m         == merge level graphs\n"
	"-s         == build game spawn data\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help()
{	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION); }

char SYSTEM_LTX[256];
char INI_FILE[256];

void Startup(LPSTR     lpCmdLine)
{
	char cmd[512],name[256];
	BOOL bModifyOptions		= FALSE;

	strcpy(cmd,lpCmdLine);
	strlwr(cmd);
	if (strstr(cmd,"-?") || strstr(cmd,"-h"))			{ Help(); return; }
	if ((strstr(cmd,"-f")==0) && (strstr(cmd,"-g")==0) && (strstr(cmd,"-c")==0)&& (strstr(cmd,"-m")==0) && (strstr(cmd,"-s")==0))	{ Help(); return; }
	if (strstr(cmd,"-o"))								bModifyOptions = TRUE;

	// Give a LOG-thread a chance to startup
	InitCommonControls	();
	_beginthread		(logThread,	0,0);
	Sleep				(150);
	
	// Load project
	name[0]=0; 
	if (strstr(cmd,"-f"))
		sscanf	(strstr(cmd,"-f")+2,"%s",name);
	else
		if (strstr(cmd,"-g"))
			sscanf	(strstr(cmd,"-g")+2,"%s",name);
		else
			if (strstr(cmd,"-c"))
				sscanf	(strstr(cmd,"-c")+2,"%s",name);

	if (strlen(name))
		strcat			(name,"\\");
	char				prjName[256];
	prjName				[0] = 0;
	FS.update_path		(prjName,"$game_levels$",name);

	u32				dwStartupTime	= timeGetTime();
	
	FS.update_path		(SYSTEM_LTX,"$game_data$","system.ltx");
	FS.update_path		(INI_FILE,"$game_data$","game.ltx");
	
	pSettings			= xr_new<CInifile>(SYSTEM_LTX);

	if (strstr(cmd,"-f"))
		xrCompiler			(prjName);
	else
		if (strstr(cmd,"-g"))
			xrBuildGraph		(prjName);
		else
			if (strstr(cmd,"-c"))
				xrBuildCrossTable	(prjName);
			else {
				if (strstr(cmd,"-m")) {
					xrMergeGraphs		(prjName);
				}
				else
					if (strstr(cmd,"-s")) {
						//Ini					= xr_new<CInifile>(INI_FILE);
						xrMergeSpawns		(prjName);
						//xr_delete			(Ini);
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
	Sleep				(500);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// Initialize debugging
 	Core._initialize		("xrAI",NULL);

	Startup					(lpCmdLine);
	
	return 0;
}




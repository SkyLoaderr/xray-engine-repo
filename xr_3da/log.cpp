#include "stdafx.h"
#include "errors.h"
#include "vector.h"
#include "resource.h"
#include "log.h"

static string64			logFName		= "engine.log";
static HWND				logWindow		= NULL;
static HWND				logoWindow		= NULL;
static HWND				logControl		= NULL;
static CCriticalSection	logCS;
std::vector <LPCSTR>	LogFile;

void AddOne				(const char *split) 
{
	logCS.Enter			();

#ifdef _DEBUG
	OutputDebugString	(split);
	OutputDebugString	("\n");
#endif

	FILE	*f = fopen	(logFName, "at");
	if (f) {
		fprintf			(f, "%s\n", split);
		fclose			(f);
	}

	LogFile.push_back	(split);

	if (logControl) 
	{
		SendMessage	( logControl, LB_ADDSTRING, 0, (LPARAM)split );
		u32 dwCnt = SendMessage	( logControl, LB_GETCOUNT, 0, 0);
		SendMessage	( logControl, LB_SETTOPINDEX, dwCnt-1, 0);
		UpdateWindow( logWindow);
	}

	logCS.Leave			();
}

void Log(const char *s) 
{
	int		i,j;
	char	split[1024];

	for (i=0,j=0; s[i]!=0; i++) {
		if (s[i]=='\n') {
			split[j]=0;	// end of line
			if (split[0]==0) { split[0]=' '; split[1]=0; }
			AddOne(xr_strdup(split));
			j=0;
		} else {
			split[j++]=s[i];
		}
	}
	split[j]=0;
	AddOne(xr_strdup(split));
}

void __cdecl Msg( const char *format, ...)
{
	va_list mark;
	char buf[1024];
	va_start( mark, format );
	vsprintf( buf, format, mark );
	Log		( buf );
}

void Log(const char *msg, const char *dop) {
	char buf[1024];

	if (dop)	sprintf(buf,"%s %s",msg,dop);
	else		sprintf(buf,"%s",msg);

	Log		(buf);
}

void Log(const char *msg, u32 dop) {
	char buf[1024];

	sprintf	(buf,"%s %d",msg,dop);
	Log		(buf);
}

void Log(const char *msg, int dop) {
	char buf[1024];

	sprintf	(buf,"%s %d",msg,dop);
	Log		(buf);
}

void Log(const char *msg, float dop) {
	char buf[1024];

	sprintf	(buf,"%s %f",msg,dop);
	Log		(buf);
}

void Log(const char *msg, const Fvector &dop) {
	char buf[1024];

	sprintf	(buf,"%s (%f,%f,%f)",msg,dop.x,dop.y,dop.z);
	Log		(buf);
}

void Log(const char *msg, const Fmatrix &dop) {
	char buf[1024];

	sprintf	(buf,"Matrix:\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n"	,dop.i.x,dop.i.y,dop.i.z,dop._14_
																				,dop.j.x,dop.j.y,dop.j.z,dop._24_
																				,dop.k.x,dop.k.y,dop.k.z,dop._34_
																				,dop.c.x,dop.c.y,dop.c.z,dop._44_);
	Log		(buf);
}

static BOOL CALLBACK logDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
		case WM_DESTROY:
			logControl= 0;
			logWindow = 0;
			break;
		case WM_CLOSE:
			DestroyWindow( hw );
			break;
		case WM_COMMAND:
			if( LOWORD(wp)==IDCANCEL )
				DestroyWindow( hw );
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

static char *month[12] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};
static int day_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


void CreateLog(BOOL bQuiet)
{
	strconcat			(logFName,"engine_",strlwr(psSystemUserName),".log");

	FILE *f;
	f = fopen(logFName, "wt");
	if (f==NULL) abort();
	fprintf(f,"\n");
	fclose(f);

	if (!bQuiet) {
		logWindow = CreateDialog(
			GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDD_LOG),
			0, logDlgProc );
		
		logControl = GetDlgItem( logWindow, IDC_LIST );
	} else {
		logoWindow = CreateDialog(
			GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDD_STARTUP),
			0, logDlgProc );
	}

	// Calculating build
	long Time;
	time(&Time);
	int build=0, mnum=0, dnum, ynum, mcnt;
	char mon[4];
	char buf[128];
	strcpy(buf,__DATE__);
	sscanf(buf,"%s %d %d",mon,&dnum, &ynum);
	for (int i=0; i<12; i++) {
		if (stricmp(month[i],mon)==0) mnum=i;
	}
	for (mcnt=6; mcnt<mnum; mcnt++) build+=day_in_month[mcnt];
	build+=dnum;
	Msg("\"%s\" build %d\nCompilation date: %s\n",
		"XRay Engine",build+(ynum-1999)*365, __DATE__);
}

void CloseLog(void)
{
	CloseLogWindow();
	// Don't cleanup log
	/*
	for (u32 i=0; i<LogFile.size(); i++) {
		_FREE(LogFile[i]);
	}
	*/
	LogFile.clear();
}

void CloseLogWindow(void) {
	if (logWindow) {
		DestroyWindow(logWindow);
		logWindow	= NULL;
		logControl	= NULL;
	}
	if (logoWindow) {
		DestroyWindow(logoWindow);
		logoWindow	= NULL;
	}
}

/*
#ifndef ENGINE_BUILD
void ENGINE_API	__cdecl Status	(const char *format, ...)
{
	if (logWindow) {
		HWND S = GetDlgItem( logWindow, IDC_TEXT );
		SetWindowText(S,format);
	}
}
#endif
*/
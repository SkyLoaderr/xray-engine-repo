#include "stdafx.h"
#include "errors.h"
#include "vector.h"
#include "resource.h"
#include "log.h"

static const char *	logFName	= "visibility.log";
HWND				logWindow	= NULL;
HWND				logControl	= NULL;
static DWORD		dwMainThreadID = 0;
vector <char *>		LogFile;
vector <char *>		LogFileAddons;

void AddOne(char *split) {
	LogFile.push_back(split);
	OutputDebugString(split);
	OutputDebugString("\n");
	if (GetCurrentThreadId()==dwMainThreadID) {
		if (LogFileAddons.size()) {
			// firstly we transfer other threads output to window
			if (logControl) {
				for (int i=0; i<LogFileAddons.size(); i++) {
					SendMessage	(
						logControl, LB_ADDSTRING,
						0, (LPARAM)LogFileAddons[i] );
				}
			}
			LogFileAddons.clear();
		}
		// now transfer the string itself
		if (logControl) {
			SendMessage	( logControl, LB_ADDSTRING, 0, (LPARAM)split );
			DWORD dwCnt = SendMessage	( logControl, LB_GETCOUNT, 0, 0);
			SendMessage	( logControl, LB_SETTOPINDEX, dwCnt-1, 0);
			UpdateWindow( logWindow);
		}
	} else {
		LogFileAddons.push_back(split);
	}
}

void Log(const char *s) {
	FILE	*f;
	int		i,j;
	char	split[256];


	f = fopen(logFName, "at");
	if (f) {
		fprintf(f, "%s\n", s);
		fclose(f);
	}

	for (i=0,j=0; s[i]!=0; i++) {
		if (s[i]=='\n') {
			split[j]=0;	// end of line
			if (split[0]==0) { split[0]=' '; split[1]=0; }
			AddOne(strdup(split));
			j=0;
		} else {
			split[j++]=s[i];
		}
	}
	split[j]=0;
	AddOne(strdup(split));
}

void __cdecl Msg( const char *format, ...)
{
	va_list mark;
	char buf[256];
	va_start( mark, format );
	vsprintf( buf, format, mark );
	Log		( buf );
}

void Log(const char *msg, char *dop) {
	char buf[256];

	sprintf(buf,"%s %s",msg,dop);
	Log(buf);
}

void Log(const char *msg, DWORD dop) {
	char buf[256];

	sprintf(buf,"%s %d",msg,dop);
	Log(buf);
}

void Log(const char *msg, int dop) {
	char buf[256];

	sprintf(buf,"%s %d",msg,dop);
	Log(buf);
}

void Log(const char *msg, float dop) {
	char buf[256];

	sprintf(buf,"%s %f",msg,dop);
	Log(buf);
}

void Log(const char *msg, Fvector &dop) {
	char buf[256];

	sprintf(buf,"%s (%f,%f,%f)",msg,dop.x,dop.y,dop.z);
	Log(buf);
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



void CreateLog(void)
{
	dwMainThreadID = GetCurrentThreadId();

	FILE *f;
	f = fopen(logFName, "wt");
	if (f==NULL) abort();
	fprintf(f,"\n");
	fclose(f);

	logWindow = CreateDialog(
		GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDD_LOG),
		0, logDlgProc );

	logControl = GetDlgItem( logWindow, IDC_LIST );

	// Calculating build
	long Time;
	time(&Time);
	int build=0, mnum, dnum, ynum, mcnt;
	char mon[4];
	char buf[128];
	strcpy(buf,__DATE__);
	sscanf(buf,"%s %d %d",mon,&dnum, &ynum);
	for (int i=0; i<12; i++) {
		if (stricmp(month[i],mon)==0) mnum=i;
	}
	for (mcnt=6; mcnt<mnum; mcnt++) build+=day_in_month[mcnt];
	build+=dnum;
	Msg("\"%s\" demo build %d\nEngine compilation date: %s\n",
		NAME_OF_THE_APP,build+(ynum-1999)*365, __DATE__);
}

void CloseLog(void)
{
	CloseLogWindow();
	for (int i=0; i<LogFile.size(); i++) {
		_FREE(LogFile[i]);
	}
	LogFile.clear();
	LogFileAddons.clear();
}

void CloseLogWindow(void) {
	if (logWindow) {
		DestroyWindow(logWindow);
		logWindow	= NULL;
		logControl	= NULL;
	}
}

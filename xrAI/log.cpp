#include "stdafx.h"
#include "vector.h"
#include "resource.h"
#include "log.h"

//************************* Log-thread data
static CRITICAL_SECTION	csLog;

volatile BOOL			bClose = FALSE;

static char				status	[1024	]="";
static char				phase	[256	]="";
static char				phase_x	[256	]="";
static float			progress = 0.0f;
static vector<string>	strings;
static DWORD			phase_start_time=0;
static BOOL				bStatusChange	= FALSE;
static BOOL				bPhaseChange	= FALSE;
static DWORD			phase_total_time=0;
//************************* Log-thread data

static BOOL CALLBACK logDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
		case WM_DESTROY:
			break;
		case WM_CLOSE:
			ExitProcess		(0);
			bClose = TRUE;
			break;
		case WM_COMMAND:
			if( LOWORD(wp)==IDCANCEL )
			{
				ExitProcess	(0);
				bClose = TRUE;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}
static VOID _process_messages(VOID)
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void __cdecl Status	(const char *format, ...)
{
	EnterCriticalSection(&csLog);
	va_list mark;
	va_start( mark, format );
	vsprintf( status, format, mark );
	bStatusChange = TRUE;
	LeaveCriticalSection(&csLog);
//	Msg("~~~ %s",status);
}

void Progress		(const float F)
{
	// No critical section usage
	progress		= F;
	/*
	LONG* target = (LONG *)(&progress);
	LONG  src    = *( (LONG *)(&F)  );
	InterlockedExchange(target, src);
	*/
}

void Phase			(const char *phase_name)
{
	EnterCriticalSection(&csLog);
	bPhaseChange		= TRUE;
	phase_total_time	= timeGetTime()-phase_start_time;
	phase_start_time	= timeGetTime();
	strcpy				(phase_x,phase);
	strcpy				(phase,phase_name);
	Progress			(0);
	LeaveCriticalSection(&csLog);
	Msg("\n* New phase started: %s",phase_name);
}

void LOut(const char *s) {
	int		i,j;
	char	split[256];


	EnterCriticalSection(&csLog);

	for (i=0,j=0; s[i]!=0; i++) {
		if (s[i]=='\n') {
			split[j]=0;	// end of line
			if (split[0]==0) { split[0]=' '; split[1]=0; }
			strings.push_back(split);
			j=0;
		} else {
			split[j++]=s[i];
		}
	}
	split[j]=0;
	strings.push_back(split);

	LeaveCriticalSection(&csLog);
};

string make_time(DWORD sec)
{
	char buf[64];
	sprintf(buf,"%2.0d:%2.0d:%2.0d",sec/3600,(sec%3600)/60,sec%60);
	int len = strlen(buf);
	for (int i=0; i<len; i++) if (buf[i]==' ') buf[i]='0';
	return string(buf);
}

HWND logWindow=0;
void __cdecl logThread(void *dummy)
{
	// Startup
	InitializeCriticalSection(&csLog);

	FILE *F = fopen("x:\\build.log", "wt");
	R_ASSERT(F);

	logWindow = CreateDialog(
		HINSTANCE(GetModuleHandle(0)),
		MAKEINTRESOURCE(IDD_LOG),
		0, logDlgProc );
	if (!logWindow) {
		DWORD e=GetLastError();
		char errmsg[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,e,0,errmsg,1024,0);
		__asm int 3;
		R_ASSERT(0);
	};
	SetWindowPos(logWindow,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	HWND hwLog		= GetDlgItem(logWindow, IDC_LOG);
	HWND hwProgress	= GetDlgItem(logWindow, IDC_PROGRESS);
	HWND hwInfo		= GetDlgItem(logWindow, IDC_INFO);
	HWND hwStage	= GetDlgItem(logWindow, IDC_STAGE);
	HWND hwTime		= GetDlgItem(logWindow, IDC_TIMING);
	HWND hwPText	= GetDlgItem(logWindow, IDC_P_TEXT);
	HWND hwPhaseTime= GetDlgItem(logWindow, IDC_PHASE_TIME);

    SendMessage(hwProgress, PBM_SETRANGE,	0, MAKELPARAM(0, 1000)); 
    SendMessage(hwProgress, PBM_SETPOS,		0, 0); 

	Msg("\"LevelBuilder v3.0\" beta build\nCompilation date: %s\n",__DATE__);
	{
		char tmpbuf[128];
		Msg("Startup time: %s",_strtime(tmpbuf));
	}

	// Main cycle
	DWORD	LogSize = 0;
	float	PrSave	= 0;
	while (TRUE)
	{
		// transfer data
		EnterCriticalSection(&csLog);
		if (progress>1.f) progress=1.f;

		BOOL bWasChanges = FALSE;
		char tbuf	[256];
		if (LogSize!=strings.size())
		{
			bWasChanges = TRUE;
			for (; LogSize<strings.size(); LogSize++)
			{
				const char *S = strings[LogSize].c_str();
				SendMessage	( hwLog, LB_ADDSTRING, 0, (LPARAM) S);
				fputs		( S, F );
				fputc		( '\n',F);
			}
			fflush			( F );
			SendMessage		( hwLog, LB_SETTOPINDEX, LogSize-1, 0);
		}
		if (fabsf(PrSave-progress)>EPS_L) {
			bWasChanges = TRUE;
			PrSave = progress;
			SendMessage		( hwProgress, PBM_SETPOS, DWORD(progress*1000.f), 0);

			// timing
			if (progress>0.005f) {
				DWORD dwCurrentTime = timeGetTime();
				DWORD dwTimeDiff	= dwCurrentTime-phase_start_time;
				DWORD secElapsed	= dwTimeDiff/1000;
				DWORD secRemain		= DWORD(float(secElapsed)/progress)-secElapsed;
				sprintf(tbuf,
					"Elapsed: %s\n"
					"Remain:  %s",
					make_time(secElapsed).c_str(),
					make_time(secRemain).c_str()
					);
				SetWindowText	( hwTime, tbuf );
			} else {
				SetWindowText	( hwTime, "" );
			}

			// percentage text
			sprintf(tbuf,"%3.2f%%",progress*100.f);
			SetWindowText	( hwPText, tbuf );
		}

		if (bPhaseChange) {
			bWasChanges		= TRUE;
			bPhaseChange	= FALSE;
			SetWindowText	( hwStage,	phase );
			sprintf			( tbuf,"%s : %s",make_time(phase_total_time/1000).c_str(),	phase_x);
			SendMessage		( hwPhaseTime, LB_ADDSTRING, 0, (LPARAM) tbuf);
//			LOut			( tbuf );
		}
		if (bStatusChange) {
			bWasChanges		= TRUE;
			bStatusChange	= FALSE;
			SetWindowText	( hwInfo,	status);
		}
		if (bWasChanges) {
			UpdateWindow	( logWindow);
			bWasChanges		= FALSE;
		}
		LeaveCriticalSection(&csLog);

		_process_messages	();
		if (bClose)			break;
		Sleep				(100);
	}

	// Cleanup
	DestroyWindow(logWindow);
	strings.clear();
//	DeleteCriticalSection(&csLog);
}

void Log(const char *s) {
	LOut(s);
}

void __cdecl Msg( const char *format, ...)
{
	va_list mark;
	char buf[256];
	va_start( mark, format );
	vsprintf( buf, format, mark );
	LOut	( buf );
}

void Log(const char *msg, const char *dop) {
	char buf[256];

	sprintf(buf,"%s %s",msg,dop);
	LOut(buf);
}

void Log(const char *msg, DWORD dop) {
	char buf[256];

	sprintf(buf,"%s %d",msg,dop);
	LOut(buf);
}

void Log(const char *msg, int dop) {
	char buf[256];

	sprintf(buf,"%s %d",msg,dop);
	LOut(buf);
}

void Log(const char *msg, float dop) {
	char buf[256];

	sprintf(buf,"%s %f",msg,dop);
	LOut(buf);
}

void Log(const char *msg, Fvector &dop) {
	char buf[256];

	sprintf(buf,"%s (%f,%f,%f)",msg,dop.x,dop.y,dop.z);
	LOut(buf);
}

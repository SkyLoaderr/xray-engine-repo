#include "stdafx.h"
#include "vector.h"
#include "resource.h"
#include "xrSyncronize.h"
#include "log.h"

//************************* Log-thread data
static CCriticalSection	csLog;

volatile BOOL			bClose = FALSE;

static char				status	[1024	]="";
static char				phase	[256	]="";
static float			progress		= 0.0f;
static vector<string>	strings;
static DWORD			phase_start_time= 0;
static BOOL				bStatusChange	= FALSE;
static BOOL				bPhaseChange	= FALSE;
static DWORD			phase_total_time= 0;

static HWND hwLog		= 0;
static HWND hwProgress	= 0;
static HWND hwInfo		= 0;
static HWND hwStage		= 0;
static HWND hwTime		= 0;
static HWND hwPText		= 0;
static HWND hwPhaseTime	= 0;
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

string make_time(DWORD sec)
{
	char buf[64];
	sprintf(buf,"%2.0d:%2.0d:%2.0d",sec/3600,(sec%3600)/60,sec%60);
	int len = strlen(buf);
	for (int i=0; i<len; i++) if (buf[i]==' ') buf[i]='0';
	return string(buf);
}

void __cdecl Status	(const char *format, ...)
{
	csLog.Enter			();
	va_list mark;
	va_start( mark, format );
	vsprintf( status, format, mark );
	bStatusChange = TRUE;
	csLog.Leave			();
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
	csLog.Enter			();
	while (!(hwPhaseTime && hwStage)) Sleep(1);

	// Replace phase name with TIME:Name 
	char	tbuf		[512];
	bPhaseChange		= TRUE;
	phase_total_time	= timeGetTime()-phase_start_time;
	sprintf				( tbuf,"%s : %s",make_time(phase_total_time/1000).c_str(),	phase);
	SendMessage			( hwPhaseTime, LB_DELETESTRING, SendMessage(hwPhaseTime,LB_GETCOUNT,0,0)-1,0);
	SendMessage			( hwPhaseTime, LB_ADDSTRING, 0, (LPARAM) tbuf);

	// Start new phase
	phase_start_time	= timeGetTime();
	strcpy				(phase,  phase_name);
	SetWindowText		( hwStage,		phase_name );
	sprintf				( tbuf,"--:--:-- * %s",phase);
	SendMessage			( hwPhaseTime,  LB_ADDSTRING, 0, (LPARAM) tbuf);
	SendMessage			( hwPhaseTime,	LB_SETTOPINDEX, SendMessage(hwPhaseTime,LB_GETCOUNT,0,0)-1,0);
	Progress			(0);

	// Release focus
	csLog.Leave			();
	Msg("\n* New phase started: %s",phase_name);
}

void LOut(const char *s) {
	int		i,j;
	char	split[256];

	csLog.Enter			();

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

	csLog.Leave			();
};

HWND logWindow=0;
void __cdecl logThread(void *dummy)
{
	SetProcessPriorityBoost	(GetCurrentProcess(),TRUE);

	// Startup
	string256	log_name,log_user;
	DWORD		buffer_size		= 128;
	GetUserName	(log_user,&buffer_size);
	strconcat	(log_name,"build_",strlwr(log_user),".log");
	FILE *F = fopen(log_name, "wt");
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
	hwLog		= GetDlgItem(logWindow, IDC_LOG);
	hwProgress	= GetDlgItem(logWindow, IDC_PROGRESS);
	hwInfo		= GetDlgItem(logWindow, IDC_INFO);
	hwStage		= GetDlgItem(logWindow, IDC_STAGE);
	hwTime		= GetDlgItem(logWindow, IDC_TIMING);
	hwPText		= GetDlgItem(logWindow, IDC_P_TEXT);
	hwPhaseTime	= GetDlgItem(logWindow, IDC_PHASE_TIME);

    SendMessage(hwProgress, PBM_SETRANGE,	0, MAKELPARAM(0, 1000)); 
    SendMessage(hwProgress, PBM_SETPOS,		0, 0); 

	Msg("\"LevelBuilder v3.0\" beta build\nCompilation date: %s\n",__DATE__);
	{
		char tmpbuf[128];
		Msg("Startup time: %s",_strtime(tmpbuf));
	}

	BOOL		bHighPriority	= FALSE;
	string256	u_name;
	DWORD		u_size	= sizeof(u_name)-1;
	GetUserName	(u_name,&u_size);
	_strlwr		(u_name);
	if ((0==strcmp(u_name,"oles"))||(0==strcmp(u_name,"alexmx")))	bHighPriority	= TRUE;

	// Main cycle
	DWORD	LogSize = 0;
	float	PrSave	= 0;
	while (TRUE)
	{
		SetPriorityClass	(GetCurrentProcess(),bHighPriority?NORMAL_PRIORITY_CLASS:IDLE_PRIORITY_CLASS);
		
		// transfer data
		while (!csLog.TryEnter())	{
			_process_messages	( );
			Sleep				(1);
		}
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

		/*
		if (bPhaseChange) {
			bWasChanges		= TRUE;
			bPhaseChange	= FALSE;
			SetWindowText	( hwStage,	phase );
			sprintf			( tbuf,"%s : %s",make_time(phase_total_time/1000).c_str(),	phase_x);
			SendMessage		( hwPhaseTime, LB_ADDSTRING, 0, (LPARAM) tbuf);
		}
		*/

		if (bStatusChange) {
			bWasChanges		= TRUE;
			bStatusChange	= FALSE;
			SetWindowText	( hwInfo,	status);
		}
		if (bWasChanges) {
			UpdateWindow	( logWindow);
			bWasChanges		= FALSE;
		}
		csLog.Leave			();

		_process_messages	();
		if (bClose)			break;
		Sleep				(200);
	}

	// Cleanup
	DestroyWindow(logWindow);
	strings.clear();
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

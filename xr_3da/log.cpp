#include "stdafx.h"
#include "errors.h"
#include "vector.h"
#include "resource.h"
#include "log.h"

static string64				logFName		= "engine.log";
static xrCriticalSection	logCS;
std::vector <LPCSTR>		LogFile;
static LogCallback			LogCB			= 0;

void FlushLog			()
{
	FILE	*f = fopen	(logFName, "wt");
	if (f) {
		for (u32 it=0; it<LogFile.size(); it++)
			fprintf			(f, "%s\n", LogFile[it]);
		fclose			(f);
	}
}

void AddOne				(const char *split) 
{
	logCS.Enter			();

#ifdef _DEBUG
	OutputDebugString	(split);
	OutputDebugString	("\n");
#endif

	LogFile.push_back	(split);

	//exec CallBack
	if (LogCB)			LogCB(split);

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

static char *month[12] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};
static int day_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void CreateLog(LogCallback cb)
{
	LogCB				= cb;
	strconcat			(logFName,"logs\\",Core.ApplicationName,"_",Core.UserName,".log");
    VerifyPath			(logFName);

	FILE *f;
	f		= fopen		(logFName, "wt");
	if		(f==NULL) abort();
	fclose	(f);

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
	Msg("'%s' build %d, %s\n","xrCore",build+(ynum-1999)*365, __DATE__);
}

void CloseLog(void)
{
	FlushLog		();
	LogFile.clear	();
}

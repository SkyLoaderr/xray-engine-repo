#ifndef logH
#define logH

#define VPUSH(a)	a.x,a.y,a.z

void XRCORE_API __cdecl		Msg			(LPCSTR format, ...);
void XRCORE_API	__stdcall	Log			(LPCSTR msg);
void XRCORE_API	__stdcall	Log			(LPCSTR msg);
void XRCORE_API	__stdcall	Log			(LPCSTR msg, LPCSTR			dop);
void XRCORE_API	__stdcall	Log			(LPCSTR msg, u32			dop);
void XRCORE_API	__stdcall	Log			(LPCSTR msg, int  			dop);
void XRCORE_API	__stdcall	Log			(LPCSTR msg, float			dop);
void XRCORE_API	__stdcall	Log			(LPCSTR msg, const Fvector& dop);
void XRCORE_API	__stdcall	Log			(LPCSTR msg, const Fmatrix& dop);

typedef void (__stdcall * LogCallback) (LPCSTR string);
void 						CreateLog	(LogCallback cb=0);
void 						CloseLog	();
void XRCORE_API	__stdcall	FlushLog	();

extern 		XRCORE_API	xr_vector<LPCSTR>			LogFile;
extern 		XRCORE_API	BOOL						LogExecCB;

#endif


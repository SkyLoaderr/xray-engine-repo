#ifndef _log_H__
#define _log_H__

#define VPUSH(a)	a.x,a.y,a.z

void ENGINE_API __cdecl Msg		(const char *format, ...);
void ENGINE_API			Log		(const char *msg);
void ENGINE_API			Log		(const char *msg);
void ENGINE_API			Log		(const char *msg, const char*	dop);
void ENGINE_API			Log		(const char *msg, DWORD			dop);
void ENGINE_API			Log		(const char *msg, int  			dop);
void ENGINE_API			Log		(const char *msg, float			dop);
void ENGINE_API			Log		(const char *msg, const Fvector& dop);
void ENGINE_API			Log		(const char *msg, const Fmatrix& dop);
void ENGINE_API	__cdecl Status	(const char *format, ...);

void CreateLog					(BOOL bQ=FALSE);
void CloseLog					(void);
void CloseLogWindow				(void);

#endif


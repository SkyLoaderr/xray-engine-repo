#ifndef _log_H__
#define _log_H__

struct _vector;

void ENGINE_API __cdecl Msg		(const char *format, ...);
void ENGINE_API			Log		(const char *msg);
void ENGINE_API			Log		(const char *msg);
void ENGINE_API			Log		(const char *msg, char *	dop);
void ENGINE_API			Log		(const char *msg, DWORD		dop);
void ENGINE_API			Log		(const char *msg, int  		dop);
void ENGINE_API			Log		(const char *msg, float		dop);
void ENGINE_API			Log		(const char *msg, _vector &	dop);
void ENGINE_API	__cdecl Status	(const char *format, ...);

void CreateLog					(void);
void CloseLog					(void);
void CloseLogWindow				(void);

#endif


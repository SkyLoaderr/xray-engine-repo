#ifndef _log_H__
#define _log_H__

void ENGINE_API __cdecl Msg		(const char *format, ...);
void ENGINE_API			Log		(const char *msg);
void ENGINE_API			Log		(const char *msg);
void ENGINE_API			Log		(const char *msg, const char* dop);
void ENGINE_API			Log		(const char *msg, DWORD		dop);
void ENGINE_API			Log		(const char *msg, int  		dop);
void ENGINE_API			Log		(const char *msg, float		dop);
void ENGINE_API			Log		(const char *msg, Fvector &	dop);

void CreateLog					(void);
void CloseLog					(void);
void CloseLogWindow				(void);

void __cdecl Status	(const char *format, ...);
void Progress		(const float F);
void Phase			(const char *phase_name);

#endif


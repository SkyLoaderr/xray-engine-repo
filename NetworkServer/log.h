#ifndef _log_H__
#define _log_H__

void  __cdecl	Msg		(const char *format, ...);
void 			Log		(const char *msg);
void 			Log		(const char *msg);
void 			Log		(const char *msg, char *	dop);
void 			Log		(const char *msg, DWORD		dop);
void 			Log		(const char *msg, int  		dop);
void 			Log		(const char *msg, float		dop);
void __cdecl	Status	(const char *format, ...);
void			Timestamp(DWORD t);

void CreateLog					(void);
void CloseLog					(void);
void CloseLogWindow				(void);

#endif


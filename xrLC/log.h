#ifndef cl_log_H__
#define cl_log_H__

void __cdecl clMsg	(const char *format, ...);
void __cdecl Status	(const char *format, ...);
void Progress		(const float F);
void Phase			(const char *phase_name);

#endif

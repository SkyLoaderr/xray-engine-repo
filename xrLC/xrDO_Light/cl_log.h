#pragma	once

void __cdecl	clMsg		(const char *format, ...);
void __cdecl	Status		(const char *format, ...);
void __stdcall	Progress	(const float F);
void __stdcall	Phase		(const char *phase_name);

void __cdecl	logThread	(void *dummy);
void __stdcall	logCallback	(LPCSTR c);

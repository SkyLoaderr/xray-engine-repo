#ifndef xrDebugH
#define xrDebugH
#pragma once

class XRCORE_API	xrDebug
{
public:
	void		backend		(const char* reason, const char* file, int line);

public:
	void		_initialize	();
	void		_destroy	();
	
public:
	std::string	error2string(long  code	);
	void		fail		(const char *e1, const char *file, int line);
	void		fail		(const char *e1, const char *e2, const char *file, int line);
	void		fail		(const char *e1, const char *e2, const char *e3, const char *file, int line);
	void		error		(long  code, const char* e1, const char *file, int line);
	void _cdecl	fatal		(const char* F,...);
};

extern XRCORE_API	xrDebug		Debug;

#include "xrDebug_macros.h"

#endif

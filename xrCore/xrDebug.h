#ifndef xrDebugH
#define xrDebugH
#pragma once

typedef	void		crashhandler		(void);
class XRCORE_API	xrDebug
{
private:
	crashhandler*	handler	;

public:
	void			_initialize			();
	void			_destroy			();
	
public:
	crashhandler*	get_crashhandler	()							{ return handler;	};
	void			set_crashhandler	(crashhandler* _handler)	{ handler=_handler;	};
	LPCSTR			error2string		(long  code	);
#ifndef __BORLANDC__
	void			fail				(const char *e1, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *e3, const char *file, int line, const char *function, bool &ignore_always);
	void			fail				(const char *e1, const char *e2, const char *e3, const char *e4, const char *file, int line, const char *function, bool &ignore_always);
	void			error				(long  code, const char* e1, const char *file, int line, const char *function, bool &ignore_always);
	void _cdecl		fatal				(const char *file, int line, const char *function, const char* F,...);
	void			backend				(const char* reason, const char* expression, const char *argument0, const char *argument1, const char* file, int line, const char *function, bool &ignore_always);
#else
	void			fail				(const char *e1, const char *file, int line);
	void			fail				(const char *e1, const char *e2, const char *file, int line);
	void			fail				(const char *e1, const char *e2, const char *e3, const char *file, int line);
	void			error				(long  code, const char* e1, const char *file, int line);
	void _cdecl		fatal				(const char* F,...);
	void			backend				(const char* reason, const char* file, int line);
#endif
};

extern XRCORE_API	xrDebug		Debug;

#include "xrDebug_macros.h"

#endif

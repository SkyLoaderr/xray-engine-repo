#include "stdafx.h"

xrCriticalSection::xrCriticalSection	()
{
	pmutex							= xr_malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection		( (CRITICAL_SECTION*)pmutex	);
};
xrCriticalSection::~xrCriticalSection	()
{
	DeleteCriticalSection			( (CRITICAL_SECTION*)pmutex	);
	xr_free							( pmutex		);
};
void	xrCriticalSection::Enter	()
{
	EnterCriticalSection			( (CRITICAL_SECTION*)pmutex );
};
void	xrCriticalSection::Leave	()
{
	LeaveCriticalSection			( (CRITICAL_SECTION*)pmutex );
};
BOOL	xrCriticalSection::TryEnter	()
{
	return TryEnterCriticalSection	( (CRITICAL_SECTION*)pmutex );
};

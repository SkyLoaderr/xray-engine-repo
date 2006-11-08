#include "stdafx.h"

#ifdef PROFILE_CRITICAL_SECTIONS
static add_profile_portion_callback	add_profile_portion = 0;
void set_add_profile_portion	(add_profile_portion_callback callback)
{
	add_profile_portion		= callback;
}

#endif // PROFILE_CRITICAL_SECTIONS

struct profiler {
	LPCSTR					m_timer_id;
	CTimer					m_timer;

	IC	profiler::profiler	(LPCSTR timer_id)
	{
		m_timer_id			= timer_id;
		m_timer.Start		();
	}

	IC	profiler::~profiler	()
	{
		if (!add_profile_portion)
			return;

		u64					time = m_timer.GetElapsed_ticks();
		(*add_profile_portion)(m_timer_id,time);
	}
};

#ifdef PROFILE_CRITICAL_SECTIONS
xrCriticalSection::xrCriticalSection	(LPCSTR id) : m_id(id)
#else // PROFILE_CRITICAL_SECTIONS
xrCriticalSection::xrCriticalSection	()
#endif // PROFILE_CRITICAL_SECTIONS
{
	pmutex							= xr_alloc<CRITICAL_SECTION>(1);
	InitializeCriticalSection		( (CRITICAL_SECTION*)pmutex	);
}

xrCriticalSection::~xrCriticalSection	()
{
	DeleteCriticalSection			( (CRITICAL_SECTION*)pmutex	);
	xr_free							( pmutex		);
}

void	xrCriticalSection::Enter	()
{
#ifdef PROFILE_CRITICAL_SECTIONS
	profiler						temp(m_id);
#endif // PROFILE_CRITICAL_SECTIONS
	EnterCriticalSection			( (CRITICAL_SECTION*)pmutex );
}

void	xrCriticalSection::Leave	()
{
	LeaveCriticalSection			( (CRITICAL_SECTION*)pmutex );
}

BOOL	xrCriticalSection::TryEnter	()
{
	return TryEnterCriticalSection	( (CRITICAL_SECTION*)pmutex );
};

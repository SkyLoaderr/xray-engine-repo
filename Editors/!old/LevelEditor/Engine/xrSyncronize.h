#ifndef xrSyncronizeH
#define xrSyncronizeH
#pragma once

// Desc: Simple wrapper for critical section
class xrCriticalSection
{
private:
	CRITICAL_SECTION    m_CritSec;
public:
    xrCriticalSection	()
    {
        InitializeCriticalSection	( &m_CritSec );
    };

    ~xrCriticalSection	()
    {
        DeleteCriticalSection		( &m_CritSec );
    };

    IC void		Enter	()
    {
        EnterCriticalSection		( &m_CritSec );
    };

    IC void		Leave	()
    {
        LeaveCriticalSection		( &m_CritSec );
    };
	IC BOOL		TryEnter()
	{
		return TryEnterCriticalSection( &m_CritSec );
	};
};
#endif

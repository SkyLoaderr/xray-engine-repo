#pragma once

//-----------------------------------------------------------------------------
// Name: 
// Desc: Simple wrapper for critical section
//-----------------------------------------------------------------------------
class CCriticalSection
{
public:
    CCriticalSection ()
    {
        InitializeCriticalSection	( &m_CritSec );
    };

    ~CCriticalSection()
    {
        DeleteCriticalSection		( &m_CritSec );
    };

    IC void		Enter()
    {
        EnterCriticalSection		( &m_CritSec );
    };

    IC void		Leave()
    {
        LeaveCriticalSection		( &m_CritSec );
    };
	IC BOOL		TryEnter ()
	{
		return TryEnterCriticalSection( &m_CritSec );
	};
private:
    CRITICAL_SECTION    m_CritSec;
};

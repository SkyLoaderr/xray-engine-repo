#pragma once

//-----------------------------------------------------------------------------
// Name: 
// Desc: Simple wrapper for critical section
//-----------------------------------------------------------------------------
class ENGINE_API CCriticalSection
{
public:
    CCriticalSection( DWORD spincount = 2000 )
    {
        InitializeCriticalSection	( &m_CritSec );
    };

    ~CCriticalSection()
    {
        DeleteCriticalSection		( &m_CritSec );
    };

    IC void    Enter()
    {
        EnterCriticalSection		( &m_CritSec );
    };

    IC void    Leave()
    {
        LeaveCriticalSection		( &m_CritSec );
    };
private:
    CRITICAL_SECTION    m_CritSec;
};

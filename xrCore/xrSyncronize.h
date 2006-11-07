#ifndef xrSyncronizeH
#define xrSyncronizeH
#pragma once

#if 0//def DEBUG
#	define PROFILE_CRITICAL_SECTIONS
#endif // DEBUG

#ifdef PROFILE_CRITICAL_SECTIONS
	typedef void __stdcall add_profile_portion_callback(LPCSTR id, const u64 &time);
	extern XRCORE_API add_profile_portion_callback	*add_profile_portion;
#endif // PROFILE_CRITICAL_SECTIONS

// Desc: Simple wrapper for critical section
class XRCORE_API		xrCriticalSection
{
private:
	void*				pmutex;
#ifdef PROFILE_CRITICAL_SECTIONS
	LPCSTR				m_id;
#endif // PROFILE_CRITICAL_SECTIONS

public:
#ifdef PROFILE_CRITICAL_SECTIONS
    xrCriticalSection	(LPCSTR id);
#else // PROFILE_CRITICAL_SECTIONS
    xrCriticalSection	();
#endif // PROFILE_CRITICAL_SECTIONS
    ~xrCriticalSection	();

    void				Enter	();
    void				Leave	();
	BOOL				TryEnter();
};
#endif

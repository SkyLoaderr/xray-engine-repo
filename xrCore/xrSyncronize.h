#ifndef xrSyncronizeH
#define xrSyncronizeH
#pragma once

// Desc: Simple wrapper for critical section
class xrCriticalSection
{
private:
	void*				pmutex;
public:
    xrCriticalSection	();
    ~xrCriticalSection	();

    void				Enter	();
    void				Leave	();
	BOOL				TryEnter();
};
#endif

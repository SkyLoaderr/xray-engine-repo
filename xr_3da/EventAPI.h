#pragma once

#include "xrSyncronize.h"

class ENGINE_API	CEvent;
typedef CEvent*		EVENT;

//---------------------------------------------------------------------
class ENGINE_API CEventBase
{
public:
	virtual void	OnEvent(EVENT E, u32 P1, u32 P2) = 0;
};
//---------------------------------------------------------------------
class ENGINE_API CEventAPI
{
	struct Deferred
	{
		EVENT		E;
		u32		P1;
		u32		P2;
	};
private:
	vector<EVENT>		Events;
	vector<Deferred>	Events_Deferred;
	CCriticalSection	CS;
public:
	EVENT	Create	(const char* N);
	void	Destroy	(EVENT& E);

	EVENT	Handler_Attach(const char* N, CEventBase* H);
	void	Handler_Detach(EVENT& E, CEventBase* H);

	void	Signal	(EVENT	E, u32 P1=0, u32 P2=0);
	void	Signal	(LPCSTR E, u32 P1=0, u32 P2=0);
	void	Defer	(EVENT	E, u32 P1=0, u32 P2=0);
	void	Defer	(LPCSTR E, u32 P1=0, u32 P2=0);

	void	OnFrame	();
	void	Dump	();
};

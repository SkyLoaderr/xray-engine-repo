#pragma once

class ENGINE_API	CEvent;
typedef CEvent*		EVENT;

//---------------------------------------------------------------------
class ENGINE_API CEventBase
{
public:
	virtual void	OnEvent(EVENT E, DWORD P1, DWORD P2) = 0;
};
//---------------------------------------------------------------------
class ENGINE_API CEventAPI
{
	struct Deferred
	{
		EVENT		E;
		DWORD		P1;
		DWORD		P2;
	};
private:
	vector<EVENT>		Events;
	vector<Deferred>	Events_Deferred;

public:
	EVENT	Create	(const char* N);
	void	Destroy	(EVENT& E);

	EVENT	Handler_Attach(const char* N, CEventBase* H);
	void	Handler_Detach(EVENT& E, CEventBase* H);

	void	Signal	(EVENT	E, DWORD P1=0, DWORD P2=0);
	void	Signal	(LPCSTR E, DWORD P1=0, DWORD P2=0);
	void	Defer	(EVENT	E, DWORD P1=0, DWORD P2=0);
	void	Defer	(LPCSTR E, DWORD P1=0, DWORD P2=0);

	void	OnFrame	();
	void	Dump	();
};

#include "stdafx.h"
#include "eventapi.h"

//---------------------------------------------------------------------
class ENGINE_API CEvent
{
	friend class CEventAPI;
private:
	char*					Name;
	vector<CEventBase*>		Handlers;
	DWORD					dwRefCount;
public:
	CEvent	(const char* S);
	~CEvent	();

	LPCSTR	GetFull()
	{	return Name; }
	DWORD	RefCount()
	{	return dwRefCount; }

	BOOL	Equal(CEvent& E)
	{ return stricmp(Name,E.Name)==0; }

	void	Attach(CEventBase* H)
	{
		if (find(Handlers.begin(),Handlers.end(),H)==Handlers.end())
			Handlers.push_back(H);
	}
	void	Detach(CEventBase* H)
	{
		vector<CEventBase*>::iterator I = find(Handlers.begin(),Handlers.end(),H);
		if (I!=Handlers.end())
			Handlers.erase(I);
	}
	void	Signal(DWORD P1, DWORD P2)
	{
		for (DWORD I=0; I<Handlers.size(); I++)
			Handlers[I]->OnEvent(this,P1,P2);
	}
};
//-----------------------------------------
CEvent::CEvent(const char* S)
{	
	Name=strdup(S); 
	_strupr(Name);
	dwRefCount=1; 
}
CEvent::~CEvent()
{	_FREE(Name); }

//-----------------------------------------
IC bool ev_sort(CEvent*E1, CEvent*E2)
{	return E1->GetFull() < E2->GetFull(); }

void CEventAPI::Dump()
{
	std::sort(Events.begin(),Events.end(),ev_sort);
	for (DWORD i=0; i<Events.size(); i++)
		Msg("* [%d] %s",Events[i]->RefCount(),Events[i]->GetFull());
}

EVENT	CEventAPI::Create(const char* N)
{
	CEvent E(N);
	for (vector<CEvent*>::iterator I=Events.begin(); I!=Events.end(); I++)
		if ((*I)->Equal(E)) {
			EVENT F = *I;
			F->dwRefCount++;
			return F;
		}
		EVENT X = new CEvent(N);
		Events.push_back(X);
		return X;
}
void	CEventAPI::Destroy(EVENT& E)
{
	E->dwRefCount--;
	if (E->dwRefCount == 0) 
	{
		vector<CEvent*>::iterator I = find(Events.begin(),Events.end(),E);
		R_ASSERT(I!=Events.end());
		Events.erase(I);
		delete E; E=0;
	}
}
EVENT	CEventAPI::Handler_Attach(const char* N, CEventBase* H)
{
	EVENT	E = Create(N);
	E->Attach(H);
	return E;
}
void	CEventAPI::Handler_Detach(EVENT& E, CEventBase* H)
{
	E->Detach(H);
	Destroy(E);
}
void	CEventAPI::Signal(EVENT E, DWORD P1, DWORD P2)
{
	E->Signal(P1,P2);	
}
void	CEventAPI::Signal(LPCSTR N, DWORD P1, DWORD P2)
{
	EVENT	E = Create(N);
	Signal	(E,P1,P2);
	Destroy	(E);
}
void	CEventAPI::Defer(EVENT E, DWORD P1, DWORD P2)
{
	E->dwRefCount++;
	Events_Deferred.push_back	(Deferred());
	Events_Deferred.back().E	= E;
	Events_Deferred.back().P1	= P1;
	Events_Deferred.back().P2	= P2;
}
void	CEventAPI::Defer(LPCSTR N, DWORD P1, DWORD P2)
{
	EVENT	E = Create(N);
	Defer	(E,P1,P2);
	Destroy	(E);
}

void	CEventAPI::OnFrame	()
{
	if (Events_Deferred.empty())	return;
	for (DWORD I=0; I<Events_Deferred.size(); I++)
	{
		Deferred&	DEF = Events_Deferred[I];
		Signal		(DEF.E,DEF.P1,DEF.P2);
		Destroy		(Events_Deferred[I].E);
	}
	Events_Deferred.clear();
}

#include "stdafx.h"
#include "eventapi.h"
#include "xr_ioconsole.h"

extern	void msRead			();
extern	void msCreate		(LPCSTR name);

//---------------------------------------------------------------------
class ENGINE_API CEvent
{
	friend class CEventAPI;
private:
	char*					Name;
	vector<CEventBase*>		Handlers;
	u32					dwRefCount;
public:
	CEvent	(const char* S);
	~CEvent	();

	LPCSTR	GetFull()
	{	return Name; }
	u32	RefCount()
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
	void	Signal(u32 P1, u32 P2)
	{
		for (u32 I=0; I<Handlers.size(); I++)
			Handlers[I]->OnEvent(this,P1,P2);
	}
};
//-----------------------------------------
CEvent::CEvent(const char* S)
{	
	Name=xr_strdup(S); 
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
	for (u32 i=0; i<Events.size(); i++)
		Msg("* [%d] %s",Events[i]->RefCount(),Events[i]->GetFull());
}

EVENT	CEventAPI::Create(const char* N)
{
	CS.Enter	();
	CEvent	E	(N);
	for (vector<CEvent*>::iterator I=Events.begin(); I!=Events.end(); I++)
	{
		if ((*I)->Equal(E)) {
			EVENT F		= *I;
			F->dwRefCount++;
			CS.Leave	();
			return		F;
		}
	}

	EVENT X = new CEvent(N);
	Events.push_back	(X);
	CS.Leave			( );
	return X;
}
void	CEventAPI::Destroy(EVENT& E)
{
	CS.Enter	();
	E->dwRefCount--;
	if (E->dwRefCount == 0) 
	{
		vector<CEvent*>::iterator I = find(Events.begin(),Events.end(),E);
		R_ASSERT(I!=Events.end());
		Events.erase(I);
		delete E; E=0;
	}
	CS.Leave	();
}

EVENT	CEventAPI::Handler_Attach(const char* N, CEventBase* H)
{
	CS.Enter	();
	EVENT	E = Create(N);
	E->Attach(H);
	CS.Leave	();
	return E;
}

void	CEventAPI::Handler_Detach(EVENT& E, CEventBase* H)
{
	CS.Enter	();
	E->Detach	(H);
	Destroy		(E);
	CS.Leave	();
}
void	CEventAPI::Signal(EVENT E, u32 P1, u32 P2)
{
	CS.Enter	();
	E->Signal	(P1,P2);	
	CS.Leave	();
}
void	CEventAPI::Signal(LPCSTR N, u32 P1, u32 P2)
{
	CS.Enter	();
	EVENT		E = Create(N);
	Signal		(E,P1,P2);
	Destroy		(E);
	CS.Leave	();
}
void	CEventAPI::Defer(EVENT E, u32 P1, u32 P2)
{
	CS.Enter	();
	E->dwRefCount++;
	Events_Deferred.push_back	(Deferred());
	Events_Deferred.back().E	= E;
	Events_Deferred.back().P1	= P1;
	Events_Deferred.back().P2	= P2;
	CS.Leave	();
}
void	CEventAPI::Defer(LPCSTR N, u32 P1, u32 P2)
{
	CS.Enter	();
	EVENT	E	= Create(N);
	Defer		(E,P1,P2);
	Destroy		(E);
	CS.Leave	();
}


void msParse			(LPCSTR c)
{
	if ((0==stricmp(c,"quit")) || (0==stricmp(c,"exit"))) 
	{
		Console.Execute			("quit");
	}
}

void	CEventAPI::OnFrame	()
{
	msRead		();
	CS.Enter	();
	if (Events_Deferred.empty())	return;
	for (u32 I=0; I<Events_Deferred.size(); I++)
	{
		Deferred&	DEF = Events_Deferred[I];
		Signal		(DEF.E,DEF.P1,DEF.P2);
		Destroy		(Events_Deferred[I].E);
	}
	Events_Deferred.clear();
	CS.Leave	();


}

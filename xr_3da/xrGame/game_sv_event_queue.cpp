#include "stdafx.h"
#include "game_sv_event_queue.h"


// 
GameEventQueue::GameEventQueue()		
{
	unused.reserve	(128);
	for (int i=0; i<16; i++)
		unused.push_back	(xr_new<GameEvent>());
}
GameEventQueue::~GameEventQueue()
{
	cs.Enter		();
	u32				it;
	for				(it=0; it<unused.size(); it++)	xr_delete(unused[it]);
	for				(it=0; it<ready.size(); it++)	xr_delete(ready[it]);
	cs.Leave		();
}
GameEvent*		GameEventQueue::Create	()
{
	GameEvent*	ge			= 0;
	cs.Enter		();
	if (unused.empty())	
	{
		ready.push_back		(xr_new<GameEvent> ());
		ge					= ready.back	();
	} else {
		ready.push_back		(unused.back());
		unused.pop_back		();
		ge					= ready.back	();
	}
	cs.Leave		();
	return	ge;
}
GameEvent*		GameEventQueue::Create	(NET_Packet& P, u16 type, u32 time, ClientID clientID)
{
	GameEvent*	ge			= 0;
	cs.Enter		();
	if (unused.empty())	
	{
		ready.push_back		(xr_new<GameEvent> ());
		ge					= ready.back	();
	} else {
		ready.push_back		(unused.back());
		unused.pop_back		();
		ge					= ready.back	();
	}
	Memory.mem_copy	(&(ge->P),&P,sizeof(NET_Packet));
	ge->sender	= clientID;
	ge->time	= time;
	ge->type	= type;

	cs.Leave		();
	return			ge;
}
GameEvent*		GameEventQueue::Retreive	()
{
	GameEvent*	ge			= 0;
	cs.Enter		();
	if (!ready.empty())		ge = ready.front();
	cs.Leave		();
	return	ge;
}
void			GameEventQueue::Release	()
{
	cs.Enter		();
	R_ASSERT		(!ready.empty());
	unused.push_back(ready.front());
	ready.pop_front	();
	cs.Leave		();
}

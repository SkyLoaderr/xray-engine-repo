// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
#define AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_
#pragma once

#include "net_queue.h"

class CGameObject : public CObject  
{
public:
	// AI connection
	DWORD										AI_NodeID;
	NodeCompressed*								AI_Node;
	NET_Queue_Event								net_Events;
	
	// Methods
	virtual BOOL			net_Spawn			(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags);
	virtual void			net_Event			(NET_Packet& P);
	virtual void			Sector_Detect		();
	virtual void			OnVisible			();
	virtual float			Ambient				();

	// State flags
	virtual BOOL			net_Relevant		()	{ return net_Local && getActive();	}	// send messages only if active and local
	virtual BOOL			Ready				()	{ return net_Ready && getActive();	}	// update only if active and fully initialized by/for network
	
	// Position stack
	virtual	SavedPosition	ps_Element			(DWORD ID);

	// Game-specific events
	CGameObject();
	virtual ~CGameObject();
};

#endif // !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)

// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
#define AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_
#pragma once

#include "net_queue.h"

class CGameObject : public CObject  
{
	typedef CObject inherited;
public:
	// AI connection
	DWORD										AI_NodeID;
	NodeCompressed*								AI_Node;
	NET_Queue_Event								net_Events;
	DWORD										respawnPhantom;

	// Utilities
	void					u_EventGen			(NET_Packet& P, u32 type, u32 dest	);
	void					u_EventSend			(NET_Packet& P, BOOL sync=TRUE		);
	
	// Methods
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			net_Event			(NET_Packet& P);
	virtual BOOL			net_Relevant		()	{ return net_Local && getActive();	}	// send messages only if active and local

	virtual BOOL			Ready				()	{ return net_Ready && getActive();	}	// update only if active and fully initialized by/for network
	virtual void			Sector_Detect		();
	virtual float			Ambient				();

	virtual void			OnVisible			();
	virtual void			OnEvent				(NET_Packet& P, u16 type)	{};
	virtual void			UpdateCL			();

	// Position stack
	virtual	SavedPosition	ps_Element			(DWORD ID);

	// Game-specific events
	CGameObject();
	virtual ~CGameObject();
};

#endif // !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)

// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
#define AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_
#pragma once

class CGameObject : public CObject  
#ifdef DEBUG
	, public pureRender
#endif
{
	typedef CObject inherited;
public:
	// AI connection
	u32											AI_NodeID;
	NodeCompressed*								AI_Node;
	u32											respawnPhantom;

	// Utilities
	void					u_EventGen			(NET_Packet& P, u32 type, u32 dest	);
	void					u_EventSend			(NET_Packet& P, BOOL sync=TRUE		);
	
	// Methods
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual BOOL			net_Relevant		()	{ return getLocal() && getActive();	}	// send messages only if active and local

	virtual BOOL			Ready				()	{ return getReady() && getActive();	}	// update only if active and fully initialized by/for network
	virtual void			Sector_Detect		();
	virtual float			Ambient				();

	virtual void			OnVisible			();
	virtual void			OnEvent				(NET_Packet& P, u16 type);
	virtual void			UpdateCL			();
	
	virtual	void			Hit					(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space)=0;
	// Position stack
	virtual	SavedPosition	ps_Element			(u32 ID);

	// Game-specific events
	CGameObject();
	virtual ~CGameObject();

#ifdef DEBUG
	virtual void			OnRender			();
#endif
};

#endif // !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)

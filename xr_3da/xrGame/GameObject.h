// GameObject.h: interface for the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)
#define AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_
#pragma once

class CGameObject : public CObject  
{
public:
	// AI connection
	DWORD								AI_NodeID;
	NodeCompressed*						AI_Node;
	float								AI_Lighting;
	BOOL								bActive;		// was it activated or not - sleeping, not updating, no network messages etc.
	
	// Methods
	virtual BOOL						Spawn			(BOOL bLocal, int server_id, Fvector4& o_pos);
	virtual void						Sector_Detect	();
	virtual float						OnVisible		();

	// State flags
	virtual BOOL						net_Relevant	()	{ return net_Local && bActive;	}	// send messages only if active and local
	virtual BOOL						Ready			()	{ return net_Ready && bActive;	}	// update only if active and fully initialized by/for network

	CGameObject();
	virtual ~CGameObject();
};

#endif // !defined(AFX_GAMEOBJECT_H__3DA72D03_C759_4688_AEBB_89FA812AA873__INCLUDED_)

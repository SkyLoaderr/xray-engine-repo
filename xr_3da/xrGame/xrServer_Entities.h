#ifndef _SRV_ENTITIES_
#define _SRV_ENTITIES_

#include "xrMessages.h"
#ifdef _EDITOR
	#include "net_utils.h"
	#include "PropertiesListTypes.h"
#endif

// refs
class xrServerEntity;
class xrClientData;

// t-defs
struct	SRotation
{
	float  yaw, pitch;
	SRotation() { yaw=pitch=0; }
};

// 
#pragma pack(push,1)
class xrServerEntity_DESC
{
public:
	WORD	version;

	xrServerEntity_DESC() : version(0) {};
};
#pragma pack(pop)

//
class xrServerEntity
{
public:
	xrServerEntity_DESC		desc;
public:
	BOOL					net_Ready;

	u16						ID;				// internal ID
	u16						ID_Parent;		// internal ParentID, 0xffff means no parent
	xrClientData*			owner;

	// spawn data
	string64				s_name;
	string64				s_name_replace;
	u8						s_RP;
	u16						s_flags;		// state flags

	// update data
	Fvector					o_Position;
	Fvector					o_Angle;

	virtual void			UPDATE_Read		(NET_Packet& P)				= 0;
	virtual void			UPDATE_Write	(NET_Packet& P)				= 0;
	virtual void			STATE_Read		(NET_Packet& P, u16 size)	= 0;
	virtual void			STATE_Write		(NET_Packet& P)				= 0;

	virtual u8				g_team()		{ return 0;	}
	virtual u8				g_squad()		{ return 0;	}
	virtual u8				g_group()		{ return 0;	}

	// utils
	void					Spawn_Write		(NET_Packet& P, BOOL bLocal);
	void					Spawn_Read		(NET_Packet& P);

	// editor integration
	virtual void			P_Write			(CFS_Base& FS);
	virtual void			P_Read			(CStream& FS);
#ifdef _EDITOR
    virtual void			FillProp		(PropValueVec& values);
#endif

	xrServerEntity			()
	{
		net_Ready			= FALSE;
		ID					= 0xffff;
        ID_Parent			= 0xffff;
		owner				= 0;
        s_flags				= M_SPAWN_OBJECT_ACTIVE;
		ZeroMemory			(s_name,		sizeof(string64));
		ZeroMemory			(s_name_replace,sizeof(string64));
        o_Angle.set			(0.f,0.f,0.f);
        o_Position.set		(0.f,0.f,0.f);
	}
	~xrServerEntity			()
	{
	}
};

xrServerEntity*	F_entity_Create		(LPCSTR name);
void			F_entity_Destroy	(xrServerEntity* P);

#endif
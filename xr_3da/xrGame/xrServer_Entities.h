#ifndef _SRV_ENTITIES_
#define _SRV_ENTITIES_

#include "xrMessages.h"

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
class xrServerEntity
{
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
	void					Spawn_Write		(NET_Packet& P, BOOL bLocal)
	{
		// generic
		P.w_begin			(M_SPAWN		);
		P.w_string			(s_name			);
		P.w_string			(s_name_replace	);
		P.w_u8				(0xFE			);	// No need for RP, use supplied (POS,ANGLEs)
		P.w_vec3			(o_Position		);
		P.w_vec3			(o_Angle		);
		P.w_u16				(ID				);
		P.w_u16				(ID_Parent		);
		if (bLocal)			P.w_u16(s_flags	| M_SPAWN_OBJECT_LOCAL );
		else				P.w_u16(s_flags );

		// write specific data
		u32	position		= P.w_tell		();
		P.w_u16				(0				);
		STATE_Write			(P				);
		u16 size			= u16			(P.w_tell()-position);
		P.w_seek			(position,&size,sizeof(u16));
	}
	void					Spawn_Read		(NET_Packet& P)
	{
		u16					dummy16;
		// generic
		P.r_begin			(dummy16		);
		P.r_string			(s_name			);
		P.r_string			(s_name_replace	);
		P.r_u8				(s_RP			);
		P.r_vec3			(o_Position		);
		P.r_vec3			(o_Angle		);
		P.r_u16				(ID				);
		P.r_u16				(ID_Parent		);
		P.r_u16				(s_flags		); s_flags&=~M_SPAWN_OBJECT_LOCAL;

		// read specific data
		u16					size;
		P.r_u16				(size			);	// size
		STATE_Read			(P,size			);
	}

	xrServerEntity			()
	{
		net_Ready			= FALSE;
		ID					= 0xffff;
		owner				= 0;
		ZeroMemory			(s_name,		sizeof(string64));
		ZeroMemory			(s_name_replace,sizeof(string64));
	}
	~xrServerEntity			()
	{
	}
};


#endif
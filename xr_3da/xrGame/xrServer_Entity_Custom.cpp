#include "stdafx.h"
#pragma hdrstop
#include "xrServer_Entity_Custom.h"
#include "game_base.h"

// EDITOR, NETWORK, SAVE, LOAD, DEMO
void	xrServerEntity::Spawn_Write		(NET_Packet& P, BOOL bLocal)
{
	// generic
	P.w_begin			(M_SPAWN);
	P.w_string			(s_name			);
	P.w_string			(s_name_replace	);
	P.w_u8				(s_gameid		);
	P.w_u8				(s_RP			);
	P.w_vec3			(o_Position		);
	P.w_vec3			(o_Angle		);
	P.w_u16				(RespawnTime	);
	P.w_u16				(ID				);
	P.w_u16				(ID_Parent		);
	P.w_u16				(ID_Phantom		);
	
	s_flags.set			(M_SPAWN_VERSION,TRUE);
	if (bLocal)			P.w_u16(u16(s_flags.flags|M_SPAWN_OBJECT_LOCAL) );
	else				P.w_u16(u16(s_flags.flags&~(M_SPAWN_OBJECT_LOCAL|M_SPAWN_OBJECT_ASPLAYER)));
	
	P.w_u8				(m_ucVersion = SPAWN_VERSION);

	// write specific data
	u32	position		= P.w_tell		();
	P.w_u16				(0				);
	STATE_Write			(P				);
	u16 size			= u16			(P.w_tell()-position);
	P.w_seek			(position,&size,sizeof(u16));
}

void	xrServerEntity::Spawn_Read		(NET_Packet& P)
{
	u16					dummy16;
	// generic
	P.r_begin			(dummy16);	
	R_ASSERT			(M_SPAWN==dummy16);
	P.r_string			(s_name			);
	P.r_string			(s_name_replace	);
	P.r_u8				(s_gameid		);
	P.r_u8				(s_RP			);
	P.r_vec3			(o_Position		);
	P.r_vec3			(o_Angle		);
	P.r_u16				(RespawnTime	);
	P.r_u16				(ID				);
	P.r_u16				(ID_Parent		);
	P.r_u16				(ID_Phantom		);
	P.r_u16				(s_flags.flags	); 
	if (s_flags.is(M_SPAWN_VERSION))
		P.r_u8			(m_ucVersion);	
	// read specific data
	u16					size;
	P.r_u16				(size			);	// size
	STATE_Read			(P,size			);
}

#ifdef _EDITOR
#include "xr_tokens.h"
#include "PropertiesListHelper.h"

xr_token game_types[]={
	{ "Any game",		GAME_ANY		},
	{ "Single",			GAME_SINGLE		},
	{ "Deathmatch",		GAME_DEATHMATCH },
	{ "CTF",			GAME_CTF		},
	{ "Assault",		GAME_ASSAULT	},
	{ "Counter Strike",	GAME_CS			},
	{ 0,				0				}
};
void	xrServerEntity::FillProp	(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateToken	(items,	PHelper.PrepareKey(pref,"Game Type"),			&s_gameid,		game_types, 1);
    PHelper.CreateFlag16(items,	PHelper.PrepareKey(pref, "Active"),				&s_flags, 		M_SPAWN_OBJECT_ACTIVE);
    PHelper.CreateU16	(items,	PHelper.PrepareKey(pref, "Respawn Time (s)"),	&RespawnTime,	0,43200);
}
#endif


#include "stdafx.h"
#include "gameobject.h"
#include "xrServer_entities.h"

void CLevel::g_cl_Spawn		(LPCSTR name, u8 rp, u16 flags)
{
	// Create
	xrServerEntity*		E	= F_entity_Create(name);
	R_ASSERT			(E);

	// Fill
	strcpy				(E->s_name,name);
	strcpy				(E->s_name_replace,"");
	E->s_gameid			=	u8(GAME);
	E->s_RP				=	rp;
	E->ID				=	0xffff;
	E->ID_Parent		=	0xffff;
	E->s_flags			=	flags;

	// Send
	NET_Packet			P;
	E->Spawn_Write		(P,TRUE);
	Send				(P,net_flags(TRUE));

	// Destroy
	F_entity_Destroy	(E);
}

void CLevel::g_sv_Spawn		(NET_Packet* Packet)
{
	// Begin analysis
	NET_Packet&	P = *Packet;
	u16			type;
	P.r_begin	(type);
	R_ASSERT	(type==M_SPAWN);

	// Read definition
	char		s_name[128],s_replace[128];
	u8			s_rp,s_game;
	u16			s_server_id,s_server_parent_id,s_server_phantom_id,s_data_size,s_flags,s_respawn;
	Fvector		o_pos,o_angle;
	P.r_u16		(s_respawn);
	P.r_string	(s_name);
	P.r_string	(s_replace);
	P.r_u8		(s_game);
	P.r_u8		(s_rp);
	P.r_vec3	(o_pos);
	P.r_vec3	(o_angle);
	P.r_u16		(s_server_id);
	P.r_u16		(s_server_parent_id);
	P.r_u16		(s_server_phantom_id);
	P.r_u16		(s_flags);
	P.r_u16		(s_data_size);

	// Real spawn
	CObject*	O = Objects.LoadOne	(s_name);
	if (O)	
	{
		O->cName_set		(s_name);
		O->cNameSect_set	(s_name);
		if (s_replace[0])	O->cName_set		(s_replace);
	}
	if (0==O || (!O->net_Spawn(s_flags&M_SPAWN_OBJECT_LOCAL, s_server_id, o_pos, o_angle, P, s_flags))) 
	{
		Objects.DestroyObject(O);
		Msg("! Failed to spawn entity '%s'",s_name);
	} else {
		if ((s_flags&M_SPAWN_OBJECT_LOCAL) && (s_flags&M_SPAWN_OBJECT_ASPLAYER))	SetEntity		(O);
		if (s_flags&M_SPAWN_OBJECT_ACTIVE)											O->OnActivate	( );
		if (0xffff != s_server_parent_id)	
		{
			// Generate ownership-event
			NET_Packet		GEN;
			GEN.w_begin		(M_EVENT);
			GEN.w_u32		(Level().timeServer()-NET_Latency);
			GEN.w_u16		(GE_OWNERSHIP_TAKE);
			GEN.w_u16		(s_server_parent_id);
			GEN.w_u16		(u16(O->ID()));

			// Simulate event arrival
			for (;;) 
			{
				CObject*	 uO	= Objects.net_Find	(s_server_parent_id);
				R_ASSERT		(uO);
				CGameObject* GO = dynamic_cast<CGameObject*>(uO);
				if (0==GO)		break;
				GO->net_Event	(GEN);
				break;
			}
		}
	}
}

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
	E->s_gameid			=	u8(Game.type);
	E->s_RP				=	rp;
	E->ID				=	0xffff;
	E->ID_Parent		=	0xffff;
	E->ID_Phantom		=	0xffff;
	E->s_flags			=	flags;
	E->RespawnTime		=	0;

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
	NET_Packet&	P		= *Packet;
	u16					dummy;
	string64			s_name;
	P.r_begin			(dummy);	R_ASSERT	(dummy==M_SPAWN);
	P.r_string			(s_name);

	// Create DC (xrSE)
	xrServerEntity*		E	= F_entity_Create(s_name);
	R_ASSERT			(E);
	E->Spawn_Read		(P);

	// Client spawn
	CObject*	O		= Objects.LoadOne	(s_name);
	if (0==O || (!O->net_Spawn	(E))) 
	{
		O->net_Destroy			( );
		Objects.DestroyObject	(O);
		Msg						("! Failed to spawn entity '%s'",s_name);
	} else {
		if ((E->s_flags&M_SPAWN_OBJECT_LOCAL) && (E->s_flags&M_SPAWN_OBJECT_ASPLAYER))	SetEntity		(O);
		if (E->s_flags&M_SPAWN_OBJECT_ACTIVE)											O->OnActivate	( );
		O->OnDeviceCreate		( );

		if (0xffff != E->ID_Parent)	
		{
			// Generate ownership-event
			NET_Packet			GEN;
			GEN.w_begin			(M_EVENT);
			GEN.w_u32			(Level().timeServer()-NET_Latency);
			GEN.w_u16			(GE_OWNERSHIP_TAKE);
			GEN.w_u16			(E->ID_Parent);
			GEN.w_u16			(u16(O->ID()));

			// Simulate event arrival
			for (;;) 
			{
				CObject*	 uO	= Objects.net_Find	(E->ID_Parent);
				R_ASSERT		(uO);
				CGameObject* GO = dynamic_cast<CGameObject*>(uO);
				if (0==GO)		break;
				GO->net_Event	(GEN);
				break;
			}
		}
	}

	//
	F_entity_Destroy		(E);
}

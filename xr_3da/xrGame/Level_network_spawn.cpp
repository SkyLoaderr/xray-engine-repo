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
	E->s_gameid			=	u8(GameID());
	E->s_RP				=	rp;
	E->ID				=	0xffff;
	E->ID_Parent		=	0xffff;
	E->ID_Phantom		=	0xffff;
	E->s_flags.set		(flags);
	E->RespawnTime		=	0;

	// Send
	NET_Packet			P;
	E->Spawn_Write		(P,TRUE);
	Send				(P,net_flags(TRUE));

	// Destroy
	F_entity_Destroy	(E);
}

void CLevel::g_sv_Spawn		(xrServerEntity* E)
{
	CTimer		T;

#ifdef DEBUG
	Msg					("CLIENT: Spawn: %s, ID=%d", E->s_name, E->ID);
#endif

	// Client spawn
	T.Start		();
	CObject*	O		= Objects.Create	(E->s_name);
	// Msg				("--spawn--CREATE: %f ms",1000.f*T.GetAsync());

	T.Start		();
	if (0==O || (!O->net_Spawn	(E))) 
	{
		O->net_Destroy			( );
		Msg						("! Failed to spawn entity '%s'",E->s_name);
	} else {
		//Msg			("--spawn--SPAWN: %f ms",1000.f*T.GetAsync());
		if ((E->s_flags.is(M_SPAWN_OBJECT_LOCAL)) && (E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER)))	SetEntity		(O);
		if (E->s_flags.is(M_SPAWN_OBJECT_ACTIVE))											O->OnActivate	( );

		if (0xffff != E->ID_Parent)	
		{
			// Generate ownership-event
			NET_Packet			GEN;
			GEN.w_begin			(M_EVENT);
			GEN.w_u32			(Level().timeServer()-NET_Latency);
			GEN.w_u16			(GE_OWNERSHIP_TAKE);
			GEN.w_u16			(E->ID_Parent);
			GEN.w_u16			(u16(O->ID()));
			game_events.insert	(GEN);
		}
	}
}

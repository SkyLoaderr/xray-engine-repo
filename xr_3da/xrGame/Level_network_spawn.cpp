#include "stdafx.h"
#include "xrServer_Objects_ALife_All.h"
#include "level.h"
#include "game_cl_base.h"
#include "net_queue.h"
#include "ai_space.h"
#include "game_level_cross_table.h"
#include "client_spawn_manager.h"
#include "../xr_object.h"

void CLevel::g_cl_Spawn		(LPCSTR name, u8 rp, u16 flags)
{
	// Create
	CSE_Abstract*		E	= F_entity_Create(name);
	VERIFY				(E);

	// Fill
	E->s_name			= name;
	strcpy				(E->s_name_replace,"");
	E->s_gameid			=	u8(GameID());
	E->s_RP				=	rp;
	E->ID				=	0xffff;
	E->ID_Parent		=	0xffff;
	E->ID_Phantom		=	0xffff;
	E->s_flags.assign	(flags);
	E->RespawnTime		=	0;

	// Send
	NET_Packet			P;
	E->Spawn_Write		(P,TRUE);
	Send				(P,net_flags(TRUE));

	// Destroy
	F_entity_Destroy	(E);
}

void CLevel::g_sv_Spawn		(CSE_Abstract* E)
{
	CTimer		T;

#ifdef DEBUG
	//Msg					("* CLIENT: Spawn: %s, ID=%d", E->s_name, E->ID);
#endif

	// Optimization for single-player only	- minimize traffic between client and server
	if	(GameID()	== GAME_SINGLE)		psNET_Flags.set	(NETFLAG_MINIMIZEUPDATES,TRUE);
	else									psNET_Flags.set	(NETFLAG_MINIMIZEUPDATES,FALSE);

	// Client spawn
	T.Start		();
	CObject*	O		= Objects.Create	(*E->s_name);
	// Msg				("--spawn--CREATE: %f ms",1000.f*T.GetAsync());

	T.Start		();
	if (0==O || (!O->net_Spawn	(E))) 
	{
		O->net_Destroy			( );
		client_spawn_manager().clear(O->ID());
		Objects.Destroy			(O);
		Msg						("! Failed to spawn entity '%s'",E->s_name);
	} else {
		client_spawn_manager().callback(O);
		//Msg			("--spawn--SPAWN: %f ms",1000.f*T.GetAsync());
		if ((E->s_flags.is(M_SPAWN_OBJECT_LOCAL)) && (E->s_flags.is(M_SPAWN_OBJECT_ASPLAYER)))	{
			SetEntity		(	O	);
			SetControlEntity(	O	);
			if (net_Syncronised)	net_Syncronize	();	// start sync-thread again
		}

		if (0xffff != E->ID_Parent)	
		{
			// Generate ownership-event
			NET_Packet			GEN;
			GEN.w_begin			(M_EVENT);
			GEN.w_u32			(Level().timeServer());//-NET_Latency);
			GEN.w_u16			(GE_OWNERSHIP_TAKE);
			GEN.w_u16			(E->ID_Parent);
			GEN.w_u16			(u16(O->ID()));
			game_events->insert	(GEN);
		}
	}
}

CSE_Abstract *CLevel::spawn_item		(LPCSTR section, const Fvector &position, u32 level_vertex_id, u16 parent_id, bool return_item)
{
	CSE_Abstract			*abstract = F_entity_Create(section);
	R_ASSERT3				(abstract,"Cannot find item with section",section);
	CSE_ALifeDynamicObject	*dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(abstract);
	if (dynamic_object && ai().get_level_graph()) {
		dynamic_object->m_tNodeID	= level_vertex_id;
		if (ai().get_cross_table())
			dynamic_object->m_tGraphID	= ai().cross_table().vertex(level_vertex_id).game_vertex_id();
	}

	//оружие спавним с полным магазинои
	CSE_ALifeItemWeapon* weapon = smart_cast<CSE_ALifeItemWeapon*>(abstract);
	if(weapon)
		weapon->a_elapsed = weapon->get_ammo_magsize();
	
	// Fill
	abstract->s_name		= section;
	strcpy					(abstract->s_name_replace,section);
	abstract->s_gameid		= u8(GameID());
	abstract->o_Position	= position;
	abstract->s_RP			= 0xff;
	abstract->ID			= 0xffff;
	abstract->ID_Parent		= parent_id;
	abstract->ID_Phantom	= 0xffff;
	abstract->s_flags.assign(M_SPAWN_OBJECT_LOCAL);
	abstract->RespawnTime	= 0;
	
	if (!return_item) {
		NET_Packet				P;
		abstract->Spawn_Write	(P,TRUE);
		Send					(P,net_flags(TRUE));
		F_entity_Destroy		(abstract);
		return					(0);
	}
	else
		return				(abstract);
}

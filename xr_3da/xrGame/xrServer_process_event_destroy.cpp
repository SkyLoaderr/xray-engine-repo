#include "stdafx.h"
#include "xrServer.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "xrserver_objects.h"
#include "game_base.h"
#include "level.h"
#include "game_cl_base.h"
#include "ai_space.h"
#include "alife_object_registry.h"

void xrServer::Process_event_destroy	(NET_Packet& P, DPNID sender, u32 time, u16 ID)
{
	u32								MODE = net_flags(TRUE,TRUE);
	// Parse message
	u16								id_dest	= ID;
	CSE_Abstract*					e_dest = game->get_entity_from_eid	(id_dest);	// кто должен быть уничтожен
	if (!e_dest) {
		Msg							("!SV:ge_destroy: [%d]",id_dest);
		return;
	};

	R_ASSERT						(e_dest);
	xrClientData					*c_dest = e_dest->owner;				// клиент, чей юнит
	R_ASSERT						(c_dest);
	xrClientData					*c_from = ID_to_client(sender);	// клиент, кто прислал
	R_ASSERT						(c_dest == c_from);							// assure client ownership of event
	u16								parent_id = e_dest->ID_Parent;

	if ((0xffff == parent_id) || !Process_event_reject(P,sender,time,parent_id,ID,false)) {
		SendBroadcast				(0xffffffff,P,MODE);
	}
	else {
		NET_Packet					P2;
		P2.w_begin					(M_EVENT);
		P2.w_u32					(time);
		P2.w_u16					(GE_DESTROY_REJECT);
		P2.w_u16					(parent_id);
		P2.w_u16					(id_dest);
		SendBroadcast				(0xffffffff,P2,MODE);
	}

	// Everything OK, so perform entity-destroy
	if (e_dest->m_bALifeControl && ai().get_alife()) {
		game_sv_Single				*_game = dynamic_cast<game_sv_Single*>(game);
		VERIFY						(_game);
		if (ai().alife().objects().object(id_dest,true))
			_game->alife().release	(e_dest,false);
	}

	if (game)
		game->OnDestroyObject		(e_dest->ID);

	entity_Destroy					(e_dest);
}

#include "stdafx.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "xrServer_Objects.h"
#include "xrServer.h"
#include "xrmessages.h"
#include "ai_space.h"

void xrServer::Perform_destroy	(CSE_Abstract* object, u32 mode)
{
	VERIFY					(object);

	while (!object->children.empty()) {
		CSE_Abstract		*child = game->get_entity_from_eid(object->children.back());
		R_ASSERT			(child);
		Perform_reject		(child,object);
		Perform_destroy		(child,mode);
	}

	u16						object_id = object->ID;
	entity_Destroy			(object);

	NET_Packet				P;
	P.w_begin				(M_EVENT);
	P.w_u32					(Device.dwTimeGlobal-NET_Latency*2*0);
	P.w_u16					(GE_DESTROY);
	P.w_u16					(object_id);
	ClientID				clientID;
	clientID.setBroadcast	();
	SendBroadcast			(clientID,P,mode);
}

void xrServer::SLS_Clear		()
{
	// Collect entities
	u32						mode = net_flags(TRUE,TRUE);
	while (!entities.empty())
		Perform_destroy		((*entities.begin()).second,mode);
}

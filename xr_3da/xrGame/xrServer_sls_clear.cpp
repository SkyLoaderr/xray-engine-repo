#include "stdafx.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "xrServer_Objects.h"
#include "xrServer.h"
#include "xrmessages.h"
#include "ai_space.h"

void xrServer::Perform_destroy	(CSE_Abstract* object, u32 mode)
{
	R_ASSERT				(object);
	R_ASSERT				(object->ID_Parent == 0xffff);

#ifdef DEBUG
//	verify_entities			();
#endif

	while (!object->children.empty()) {
		CSE_Abstract		*child = game->get_entity_from_eid(object->children.back());
		R_ASSERT			(child);
//		Msg					("SLS-CLEAR : REJECT  [%s][%s] FROM [%s][%s]",child->name(),child->name_replace(),object->name(),object->name_replace());
		Perform_reject		(child,object,2*NET_Latency);
#ifdef DEBUG
//		verify_entities			();
#endif
		Perform_destroy		(child,mode);
	}

//	Msg						("SLS-CLEAR : DESTROY [%s][%s]",object->name(),object->name_replace());
	u16						object_id = object->ID;
	entity_Destroy			(object);

#ifdef DEBUG
//	verify_entities			();
#endif

	NET_Packet				P;
	P.w_begin				(M_EVENT);
	P.w_u32					(Device.dwTimeGlobal - 2*NET_Latency);
	P.w_u16					(GE_DESTROY);
	P.w_u16					(object_id);
	ClientID				clientID;
	clientID.setBroadcast	();
	SendBroadcast			(clientID,P,mode);
}

void xrServer::SLS_Clear		()
{
#if 0
	Msg									("SLS-CLEAR : %d objects");
	xrS_entities::const_iterator		I = entities.begin();
	xrS_entities::const_iterator		E = entities.end();
	for ( ; I != E; ++I)
		Msg								("entity to destroy : [%d][%s][%s]",(*I).second->ID,(*I).second->name(),(*I).second->name_replace());
#endif

	u32									mode = net_flags(TRUE,TRUE);
	while (!entities.empty()) {
		bool							found = false;
		xrS_entities::const_iterator	I = entities.begin();
		xrS_entities::const_iterator	E = entities.end();
		for ( ; I != E; ++I) {
			if ((*I).second->ID_Parent != 0xffff)
				continue;
			found						= true;
			Perform_destroy				((*I).second,mode);
			break;
		}
		R_ASSERT						(found);
	}
}

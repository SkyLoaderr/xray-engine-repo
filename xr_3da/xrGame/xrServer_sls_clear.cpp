#include "stdafx.h"
#include "game_sv_single.h"
#include "ai_alife.h"
#include "xrServer_Objects.h"

void xrServer::Perform_destroy	(CSE_Abstract* tpSE_Abstract, u32 mode)
{
	VERIFY				(tpSE_Abstract);
	xr_vector<u16>::const_iterator	I = tpSE_Abstract->children.begin	();
	xr_vector<u16>::const_iterator	E = tpSE_Abstract->children.end		();
	for ( ; I != E; ++I) {
		CSE_Abstract	*e_dest	= game->get_entity_from_eid(*I);
		Perform_destroy	(e_dest,mode);
	}

	NET_Packet			P;
	P.w_begin			(M_EVENT);
	P.w_u32				(Device.dwTimeGlobal-NET_Latency*2);
	P.w_u16				(GE_DESTROY);
	P.w_u16				(tpSE_Abstract->ID);

	SendBroadcast		(0xffffffff,P,mode);

	entity_Destroy		(tpSE_Abstract);
}

void xrServer::SLS_Clear		()
{
	// Don't do anything if game in ALife mode
	if (game->type == GAME_SINGLE) {
		game_sv_Single *tpGame	= dynamic_cast<game_sv_Single*>(game);
		if (tpGame && tpGame->m_tpALife)
			return;
	}

	// Collect entities
	u32					mode	= net_flags(TRUE,TRUE);
	while (entities.begin() != entities.end())
		Perform_destroy			(entities.begin()->second,mode);
}

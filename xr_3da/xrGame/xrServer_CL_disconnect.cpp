#include "stdafx.h"
#include "hudmanager.h"


void xrServer::OnCL_Disconnected	(IClient* CL)
{
	Level().HUD()->outMessage(0xffffffff,"SERVER","Player '%s' disconnected",CL->Name);

	NET_Packet			P;
	DWORD				mode			= net_flags(TRUE,TRUE);

	// Collect entities
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*	E	= I->second;
		if (E->owner == CL)	{
			P.w_begin			(M_EVENT);
			P.w_u32				(Device.dwTimeGlobal);
			P.w_u16				(GE_DESTROY);
			P.w_u16				(E->ID);

			SendBroadcast		(CL->ID,P,mode);
			entities.erase		(E->ID);
			entity_Destroy		(E);
		}
	}

	// Game config (all, info excludes deleted player)
	Perform_game_export		();
}

#include "stdafx.h"

void xrServer::SLS_Save	(CFS_Base& FS)
{
	// Generate spawn+update
	NET_Packet		P;
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (u32 C=0; I!=E; I++,C++)
	{
		xrServerEntity*	E	= I->second;

		FS.open_chunk		(C);

		// Spawn
		E->Spawn_Write		(P,TRUE);
		FS.Wword			(P.B.count);
		FS.write			(P.B.data,P.B.count);

		// Update
		P.w_begin			(M_UPDATE);
		P.w_u16				(E->ID);
		E->UPDATE_Write		(P);
		FS.Wword			(P.B.count);
		FS.write			(P.B.data,P.B.count);

		FS.close_chunk		();
	}
}

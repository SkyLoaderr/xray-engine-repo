#include "stdafx.h"

void xrServer::SLS_Save	(CFS_Base& FS)
{
	// Generate spawn+update
	NET_Packet		P;
	xrS_entities::iterator	I=entities.begin(),E=entities.end();
	for (; I!=E; I++)
	{
		xrServerEntity*	E	= I->second;

		// Spawn
		E->Spawn_Write		(P,TRUE);
		FS.write			(P.B.data,P.B.count);

		// Update
		P.w_begin			(M_UPDATE);
		E->UPDATE_Write		(P);
		FS.write			(P.B.data,P.B.count);
	}
}

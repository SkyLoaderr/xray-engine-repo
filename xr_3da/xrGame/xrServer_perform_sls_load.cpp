#include "stdafx.h"
#include "xrserver.h"
#include "xrmessages.h"

void xrServer::SLS_Load	(IReader& fs)
{
	// Generate spawn+update
	NET_Packet		P;
	u16				u_id	= 0xffff;
	for (u32 C=0; ; ++C)
	{
		IReader* F			= fs.open_chunk(C);
		if (0==F)			break;

		// Spawn
		P.B.count			= F->r_u16();
		F->r				(P.B.data,P.B.count);
		P.r_begin			(u_id);	R_ASSERT	(M_SPAWN==u_id);
		ClientID	clientID;clientID.set(0);
		Process_spawn		(P,clientID);

		// Update
		P.B.count			= F->r_u16();
		F->r				(P.B.data,P.B.count);
		P.r_begin			(u_id);	R_ASSERT	(M_UPDATE==u_id);
		
		clientID.set(0);
		Process_update		(P,clientID);

		// Close
		F->close			();
	}
}

#include "stdafx.h"

void xrServer::SLS_Load	(CStream& FS)
{
	// Generate spawn+update
	NET_Packet		P;
	for (u32 C=0; ; C++)
	{
		CStream* F			= FS.OpenChunk(C);
		if (0==F)			break;

		// Spawn
		P.B.count			= F->Rword();
		F->Read				(P.B.data,P.B.count);
		Process_spawn		(P,0);

		// Update
		u16 u_id			= 0xffff;
		P.B.count			= F->Rword();
		F->Read				(P.B.data,P.B.count);
		P.r_begin			(u_id);	R_ASSERT	(M_UPDATE==u_id);
		Process_update		(P,0);

		// Close
		F->Close			();
	}
}

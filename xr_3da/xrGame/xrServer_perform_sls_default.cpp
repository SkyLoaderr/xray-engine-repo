#include "stdafx.h"

void xrServer::SLS_Default	()
{
	// Spawn all other objects
	FILE_NAME			fn_spawn;
	if (Engine.FS.Exist(fn_spawn, Path.Current, "level.spawn"))
	{
		CStream*		SP	= Engine.FS.Open(fn_spawn);
		NET_Packet		P;
		CStream*		S		= 0;
		int				S_id	= 0;
		while (0!=(S = SP->OpenChunk(S_id)))
		{
			P.B.count	=	S->Length();
			S->Read			(P.B.data,P.B.count);
			S->Close		();

			u16				ID;
			P.r_begin		(ID);		R_ASSERT(M_SPAWN==ID);
			Process_spawn	(P,0);

			S_id		+=	1;
		}
		Engine.FS.Close	(SP);
	}
}

#include "stdafx.h"

void xrServer::Disconnect()
{
	IPureServer::Disconnect	();
	SLS_Clear				();
	xr_delete				(game);
}

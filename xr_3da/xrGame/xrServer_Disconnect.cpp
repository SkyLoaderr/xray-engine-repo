#include "stdafx.h"

void xrServer::Disconnect()
{
	IPureServer::Disconnect	();
	xr_delete				(game);
}

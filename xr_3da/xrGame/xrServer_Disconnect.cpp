#include "stdafx.h"

void xrServer::Disconnect()
{
	IPureServer::Disconnect	();
	_DELETE					(game);
}

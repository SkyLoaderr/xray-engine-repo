#include "stdafx.h"
#include "playerlist.h"

CPlayers::CPlayers(void)
{
}

CPlayers::~CPlayers(void)
{
}

void CPlayers::add(NET_Packet& P)
{
	u32				ID;
	P.r_u32			(ID);

	Item			I;
	P.r_string		(I.name);
	I.score			= 0;
	items.insert	(make_pair(ID,I));
}

void CPlayers::remove(NET_Packet& P)
{
	u32				ID;
	P.r_u32			(ID);

	items.erase		(ID);
}

CPlayers::Item& CPlayers::access(DWORD ID)
{
	return items.find	(ID);
}

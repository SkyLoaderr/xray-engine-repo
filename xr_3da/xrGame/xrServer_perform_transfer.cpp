#include "stdafx.h"

void xrServer::Perform_transfer(xrServerEntity* what, xrServerEntity* from, xrServerEntity* to)
{
	// Sanity check
	R_ASSERT	(what && from && to);
	R_ASSERT	(from != to);
	R_ASSERT	(what->ID_Parent == from->ID);
	NET_Packet	P;
	u32			time		= Device.dwTimeGlobal;
	DWORD		MODE		= net_flags(TRUE,TRUE);

	// 1. Perform migration if need it
	if (from->owner != to->owner)	PerformMigration(what,from->owner,to->owner);
	Log						("B");

	// 2. Detach "FROM"
	vector<u16>& C			= from->children;
	vector<u16>::iterator c	= find	(C.begin(),C.end(),what->ID);
	R_ASSERT				(c!=C.end());
	C.erase					(c);
	P.w_begin				(M_EVENT);
	P.w_u32					(time);
	P.w_u16					(GE_OWNERSHIP_REJECT);
	P.w_u16					(from->ID);
	P.w_u16					(what->ID);
	SendBroadcast			(0xffffffff,P,MODE);
	Log						("A");

	// 3. Attach "TO"
	what->ID_Parent			= to->ID;
	to->children.push_back	(what->ID);
	P.w_begin				(M_EVENT);
	P.w_u32					(time+1);
	P.w_u16					(GE_OWNERSHIP_TAKE);
	P.w_u16					(to->ID);
	P.w_u16					(what->ID);
	SendBroadcast			(0xffffffff,P,MODE);
	Log						("C");
}

void xrServer::Perform_reject(xrServerEntity* what, xrServerEntity* from)
{
	R_ASSERT	(what && from);
	R_ASSERT	(what->ID_Parent == from->ID);

	NET_Packet	P;
	u32			time		= Device.dwTimeGlobal;
	u16			dummy;

	P.w_begin				(M_EVENT);
	P.w_u32					(time);
	P.w_u16					(GE_OWNERSHIP_REJECT);
	P.w_u16					(from->ID);
	P.w_u16					(what->ID);

	P.r_begin				(dummy);
	P.r_u32					(time);
	P.r_u16					(dummy);
	P.r_u16					(dummy);
	Process_event_reject	(P,0xffffffff,time,from->ID);
}

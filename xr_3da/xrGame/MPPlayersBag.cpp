#include "stdafx.h"

#include "MPPlayersBag.h"
#include "Level.h"
//#include "xrMessages.h"

CMPPlayersBag::CMPPlayersBag()
{
};

CMPPlayersBag::~CMPPlayersBag()
{
};

void CMPPlayersBag::OnEvent(NET_Packet& P, u16 type) 
{
	CInventoryItemObject::OnEvent		(P,type);
	u16						id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : 
			{
				P.r_u16(id);
				CObject* O = Level().Objects.net_Find(id);
				O->H_SetParent(this);
				O->Position().set(Position());
			}break;
		case GE_OWNERSHIP_REJECT : 
			{
				P.r_u16			(id);
				CObject* O = Level().Objects.net_Find(id);
				O->H_SetParent(0);
			}break;

	}
}
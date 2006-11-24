#include "stdafx.h"

#include "MPPlayersBag.h"
#include "Level.h"
//#include "xrMessages.h"
#include "game_base_space.h"

#define BAG_REMOVE_TIME		60000

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
				O->H_SetParent(0,!P.r_eof() && P.r_u8());
			}break;

	}
}

extern INT g_iWeaponRemove;
bool CMPPlayersBag::NeedToDestroyObject()	const
{
	if (GameID() == GAME_SINGLE) return false;
	if (Remote()) return false;
	if (H_Parent()) return false;
	if (g_iWeaponRemove == -1) return false;
	if (g_iWeaponRemove == 0) return true;
	if (TimePassedAfterIndependant() > BAG_REMOVE_TIME)
		return true;

	return false;
}
#include "stdafx.h"
#include "actor.h"
#include "targetcs.h"
#include "customdetector.h"
#include "uigamesp.h"
#include "hudmanager.h"
#include "weapon.h"
#include "artifact.h"
#include "scope.h"
#include "silencer.h"
#include "grenadelauncher.h"
#include "inventory.h"

IC BOOL BE	(BOOL A, BOOL B)
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CActor::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	u16 id;
	switch (type)
	{
	case GE_TRADE_BUY:
	case GE_OWNERSHIP_TAKE:
		{
			// Log("CActor::OnEvent - TAKE - ", *cName());
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			if(inventory().Take(dynamic_cast<CGameObject*>(O))) 
			{
				O->H_SetParent(this);
				

				//добавить новый артефакт в меню, если
				//мы работали с устройством сочетания 
				CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
				
				CArtifact* pArtifact = dynamic_cast<CArtifact*>(O);
				if(pGameSP && pArtifact)
				{
					if(pGameSP->m_pUserMenu == &pGameSP->InventoryMenu &&
						pGameSP->InventoryMenu.IsArtifactMergeShown())
					{
						pGameSP->InventoryMenu.AddArtifactToMerger(pArtifact);
					}
				}


/*				CScope* pScope = dynamic_cast<CScope*>(O);
				CSilencer* pSilencer = dynamic_cast<CSilencer*>(O);
				CGrenadeLauncher* pGrenadeLauncher = dynamic_cast<CGrenadeLauncher*>(O);
*/
				//добавить отсоединенный аддон в инвентарь
				if(pGameSP/* && (pScope || pSilencer || pGrenadeLauncher)*/)
				{
					if(pGameSP->m_pUserMenu == &pGameSP->InventoryMenu)
					{
						pGameSP->InventoryMenu.AddItemToBag(dynamic_cast<CInventoryItem*>(O));
					}
				}

			} 
			else 
			{
				NET_Packet P;
				u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
				P.w_u16(u16(O->ID()));
				u_EventSend(P);
			}

		}
		break;
	case GE_TRADE_SELL:
	case GE_OWNERSHIP_REJECT:
		{
			// Log			("CActor::OnEvent - REJECT - : ", *cName());

			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			
			if (inventory().Drop(dynamic_cast<CGameObject*>(O)) && !O->getDestroy()) 
			{
				O->H_SetParent(0);
				feel_touch_deny(O,2000);
			}

		}
		break;
	case GE_INV_ACTION:
		{
			s32 cmd;
			P.r_s32		(cmd);
			u32 flags;
			P.r_u32		(flags);
			
			if (flags & CMD_START)
				IR_OnKeyboardPress(cmd);
			else
				IR_OnKeyboardRelease(cmd);
//			inventory().Action(cmd, flags);
		}
		break;
	}
}

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
#include "level.h"
#include "xr_level_controller.h"
#include "FoodItem.h"

#include "CameraLook.h"
#include "CameraFirstEye.h"

IC BOOL BE	(BOOL A, BOOL B)
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CActor::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent			(P,type);
	CInventoryOwner::OnEvent	(P,type);

	u16 id;
	switch (type)
	{
	case GE_TRADE_BUY:
	case GE_OWNERSHIP_TAKE:
		{
			// Log("CActor::OnEvent - TAKE - ", *cName());
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			if (!O)
			{
				Msg("! Error: No object to take/buy [%d]", id);
				break;
			}

			CFoodItem* pFood = dynamic_cast<CFoodItem*>(O);
			if(pFood)
				pFood->m_eItemPlace = eItemPlaceRuck;

			if(inventory().Take(dynamic_cast<CGameObject*>(O), false, true)) 
			{
				O->H_SetParent(dynamic_cast<CObject*>(this));
				

				//добавить новый артефакт в меню, если
				//мы работали с устройством сочетания 
				CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
				
				CArtefact* pArtefact = dynamic_cast<CArtefact*>(O);
				if(pGameSP && pArtefact)
				{
					if(pGameSP->MainInputReceiver() == &pGameSP->InventoryMenu &&
						pGameSP->InventoryMenu.IsArtefactMergeShown())
					{
						pGameSP->InventoryMenu.AddArtefactToMerger(pArtefact);
					}
				}


/*				CScope* pScope = dynamic_cast<CScope*>(O);
				CSilencer* pSilencer = dynamic_cast<CSilencer*>(O);
				CGrenadeLauncher* pGrenadeLauncher = dynamic_cast<CGrenadeLauncher*>(O);
*/
				//добавить отсоединенный аддон в инвентарь
				if(pGameSP/* && (pScope || pSilencer || pGrenadeLauncher)*/)
				{
					if(pGameSP->MainInputReceiver() == &pGameSP->InventoryMenu)
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
			if (!O)
			{
				Msg("! Error: No object to reject/sell [%d]", id);
				break;
			}
			
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
			s32 RandomSeed;
			P.r_s32		(RandomSeed);
						
			if (flags & CMD_START)
			{
				if (cmd == kWPN_ZOOM)
					SetZoomRndSeed(RandomSeed);
				IR_OnKeyboardPress(cmd);
			}
			else
				IR_OnKeyboardRelease(cmd);
//			inventory().Action(cmd, flags);
		}
		break;
	case GEG_PLAYER_ITEM2SLOT:
	case GEG_PLAYER_ITEM2BELT:
	case GEG_PLAYER_ITEM2RUCK:
	case GEG_PLAYER_ITEMDROP:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			if (!O) break;
			switch (type)
			{
			case GEG_PLAYER_ITEM2SLOT:	inventory().Slot(dynamic_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEM2BELT:	inventory().Belt(dynamic_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEM2RUCK:	inventory().Ruck(dynamic_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEMDROP:	
				{
					CInventoryItem* pIItem = dynamic_cast<CInventoryItem*>(O);
					if (!pIItem) break;
					pIItem->Drop();
				}break;
			}
		}break;
	case GEG_PLAYER_BUYMENU_OPEN:
	case GEG_PLAYER_INVENTORYMENU_OPEN:
		{
			if (OnServer())
			{
				u32 InventorySlot = inventory().GetActiveSlot();
				if( InventorySlot != NO_ACTIVE_SLOT && InventorySlot <= PDA_SLOT &&
					inventory().m_slots[InventorySlot].m_pIItem)
				{
					inventory().SetPrevActiveSlot(InventorySlot);
					inventory().Activate(NO_ACTIVE_SLOT);
				}
			};
		}break;
	case GEG_PLAYER_BUYMENU_CLOSE:
	case GEG_PLAYER_INVENTORYMENU_CLOSE:
		{
			if (OnServer())
			{
				u32 InventorySlot = inventory().GetPrevActiveSlot();
				if( InventorySlot != NO_ACTIVE_SLOT && InventorySlot <= PDA_SLOT &&
					inventory().m_slots[InventorySlot].m_pIItem)
				{
					inventory().Activate(inventory().GetPrevActiveSlot());
				}
			};
		}break;
	case GE_MOVE_ACTOR:
		{
			Fvector NewPos, NewRot;
			P.r_vec3(NewPos);
			P.r_vec3(NewRot);
			Fmatrix	M = Level().CurrentControlEntity()->XFORM();
			M.translate(NewPos);
			r_model_yaw				= NewRot.y;
			r_torso.yaw				= NewRot.y;
			r_torso.pitch			= -NewRot.x;
			unaffected_r_torso_yaw	 = r_torso.yaw;
			unaffected_r_torso_pitch = r_torso.pitch;
			cam_Active()->Set		(-unaffected_r_torso_yaw,unaffected_r_torso_pitch,0);
			Level().CurrentControlEntity()->ForceTransform(M);
		}break;
	}
}

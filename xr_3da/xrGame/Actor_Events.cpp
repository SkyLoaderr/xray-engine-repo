#include "stdafx.h"
#include "actor.h"
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

			CFoodItem* pFood = smart_cast<CFoodItem*>(O);
			if(pFood)
				pFood->m_eItemPlace = eItemPlaceRuck;

			if(inventory().Take(smart_cast<CGameObject*>(O), false, true)) 
			{
				O->H_SetParent(smart_cast<CObject*>(this));
				

				//добавить новый артефакт в меню, если
				//мы работали с устройством сочетания 
				CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
				
				CArtefact* pArtefact = smart_cast<CArtefact*>(O);
				if(pGameSP && pArtefact)
				{
					if(pGameSP->MainInputReceiver() == &pGameSP->InventoryMenu &&
						pGameSP->InventoryMenu.IsArtefactMergeShown())
					{
						pGameSP->InventoryMenu.AddArtefactToMerger(pArtefact);
					}
				}


/*				CScope* pScope = smart_cast<CScope*>(O);
				CSilencer* pSilencer = smart_cast<CSilencer*>(O);
				CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(O);
*/
				//добавить отсоединенный аддон в инвентарь
				if(pGameSP/* && (pScope || pSilencer || pGrenadeLauncher)*/)
				{
					if(pGameSP->MainInputReceiver() == &pGameSP->InventoryMenu)
					{
						pGameSP->InventoryMenu.AddItemToBag(smart_cast<CInventoryItem*>(O));
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
			
			if (inventory().Drop(smart_cast<CGameObject*>(O)) && !O->getDestroy()) 
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
			s32 ZoomRndSeed = P.r_s32();
			s32 ShotRndSeed = P.r_s32();
									
			if (flags & CMD_START)
			{
				if (cmd == kWPN_ZOOM)
					SetZoomRndSeed(ZoomRndSeed);
				if (cmd == kWPN_FIRE)
					SetShotRndSeed(ShotRndSeed);
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
	case GEG_PLAYER_ITEM_EAT:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			if (!O) break;
			switch (type)
			{
			case GEG_PLAYER_ITEM2SLOT:	inventory().Slot(smart_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEM2BELT:	inventory().Belt(smart_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEM2RUCK:	inventory().Ruck(smart_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEM_EAT:	inventory().Eat(smart_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEMDROP:	
				{
					CInventoryItem* pIItem = smart_cast<CInventoryItem*>(O);
					if (!pIItem) break;
					pIItem->Drop();
				}break;
			}
		}break;
	case GEG_PLAYER_BUYMENU_OPEN:
	case GEG_PLAYER_INVENTORYMENU_OPEN:
	case GEG_PLAYER_DEACTIVATE_CURRENT_SLOT:
	case GEG_PLAYER_SPRINT_START:
		{
			if (OnServer())
			{
				m_iCurWeaponHideState |= 1<<((type - GEG_PLAYER_INVENTORYMENU_OPEN)/2);
			};
		}break;
	case GEG_PLAYER_BUYMENU_CLOSE:
	case GEG_PLAYER_INVENTORYMENU_CLOSE:
	case GEG_PLAYER_RESTORE_CURRENT_SLOT:
	case GEG_PLAYER_SPRINT_END:
		{
			if (OnServer())
			{
				m_iCurWeaponHideState &= ~(1<<((type - GEG_PLAYER_INVENTORYMENU_OPEN)/2));
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
			unaffected_r_torso.yaw	= r_torso.yaw;
			unaffected_r_torso.pitch= r_torso.pitch;
			unaffected_r_torso.roll	= r_torso.roll;
			cam_Active()->Set		(-unaffected_r_torso.yaw,unaffected_r_torso.pitch,unaffected_r_torso.roll);
			Level().CurrentControlEntity()->ForceTransform(M);
		}break;
	}
}

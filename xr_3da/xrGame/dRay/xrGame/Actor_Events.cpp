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
#include "ActorCondition.h"
#include "Grenade.h"

#include "CameraLook.h"
#include "CameraFirstEye.h"
#include "holder_custom.h"
#include "ui/uiinventoryWnd.h"
#ifdef DEBUG
#include "PHDebug.h"
#endif
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
			//Log("CActor::OnEvent - TAKE - ", *cName());
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

			CGameObject* _GO = smart_cast<CGameObject*>(O);
			
			if( inventory().CanTakeItem(smart_cast<CInventoryItem*>(_GO)) ){

				O->H_SetParent(smart_cast<CObject*>(this));

				inventory().Take(_GO, false, true);

				CUIGameSP* pGameSP = NULL;
				CUI* ui = HUD().GetUI();
				if( ui&&ui->UIGame() )
				{
					pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
					if (Level().CurrentViewEntity() == this)
							HUD().GetUI()->UIGame()->ReInitShownUI();
				};
				
				//�������� ������������� ����� � ���������
				if(pGameSP/* && (pScope || pSilencer || pGrenadeLauncher)*/)
				{
					if(pGameSP->MainInputReceiver() == pGameSP->InventoryMenu)
					{
						pGameSP->InventoryMenu->AddItemToBag(smart_cast<CInventoryItem*>(O));
					}
				}
				
//				CWeapon* pWeapon = smart_cast<CWeapon*>(O);
				CWeapon* pWeapon = smart_cast<CWeapon*>(O);
				CGrenade* pGrenade = smart_cast<CGrenade*>(O);
				if (pWeapon || pGrenade) SelectBestWeapon();
#ifdef DEBUG
//				Msg("OnEvent - %s[%d] - TAKE - %s[%d]0x%X", *cName(), ID(), *O->cName(), id, smart_cast<CInventoryItem*>(O));
#endif

			} 
			else 
			{
				NET_Packet P;
				u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
				P.w_u16(u16(O->ID()));
				u_EventSend(P);
#ifdef DEBUG
//				Msg("! OnEvent - %s[%d] - TAKE - %s[%d]0x%X - FAILED", *cName(), ID(), *O->cName(), id, smart_cast<CInventoryItem*>(O));
#endif
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
#ifdef DEBUG			
//			Msg("OnEvent - %s[%d] - REJECT - %s[%d]0x%X", *cName(), ID(), *O->cName(), id, smart_cast<CInventoryItem*>(O));
#endif
			if (inventory().Drop(smart_cast<CGameObject*>(O)) && !O->getDestroy()) 
			{
				O->H_SetParent(0);
				feel_touch_deny(O,2000);
			}

			CWeapon* pWeapon = smart_cast<CWeapon*>(O);
			CGrenade* pGrenade = smart_cast<CGrenade*>(O);
			CArtefact* pArtefact = smart_cast<CArtefact*>(O);
			if (pWeapon || pGrenade || pArtefact) SelectBestWeapon();

			if (Level().CurrentViewEntity() == this && HUD().GetUI() && HUD().GetUI()->UIGame())
				HUD().GetUI()->UIGame()->ReInitShownUI();
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
	case GEG_PLAYER_ACTIVATEARTEFACT:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			if (!O) break;
			if (O->getDestroy()) 
			{
#ifdef _DEBUG
				Msg("! something to destroyed object - %s[%d]0x%X", *O->cName(), id, smart_cast<CInventoryItem*>(O));
#endif
				break;
			}
			switch (type)
			{
			case GEG_PLAYER_ITEM2SLOT:	/*Msg("GEG_PLAYER_ITEM2SLOT - %s[%d]0x%X", *O->cName(), id, smart_cast<CInventoryItem*>(O));*/ inventory().Slot(smart_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEM2BELT:	/*Msg("GEG_PLAYER_ITEM2BELT - %s[%d]0x%X", *O->cName(), id, smart_cast<CInventoryItem*>(O));*/ inventory().Belt(smart_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEM2RUCK:	/*Msg("GEG_PLAYER_ITEM2RUCK - %s[%d]0x%X", *O->cName(), id, smart_cast<CInventoryItem*>(O));*/ inventory().Ruck(smart_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ITEM_EAT:	/*Msg("GEG_PLAYER_ITEM_EAT -  %s[%d]0x%X", *O->cName(), id, smart_cast<CInventoryItem*>(O));*/ inventory().Eat(smart_cast<CInventoryItem*>(O)); break;
			case GEG_PLAYER_ACTIVATEARTEFACT:
				{
					CArtefact* pArtefact = smart_cast<CArtefact*>(O);
					if (!pArtefact) break;
					pArtefact->ActivateArtefact();
				}break;
			case GEG_PLAYER_ITEMDROP:	
				{
					CInventoryItem* pIItem = smart_cast<CInventoryItem*>(O);
					if (!pIItem) break;
					pIItem->Drop();
				}break;
			}
		}break;
	case GEG_PLAYER_ACTIVATE_SLOT:
		{
			u32							slot_id;
			P.r_u32						(slot_id);

			inventory().Activate		(slot_id);
								  
		}break;

	case GEG_PLAYER_WEAPON_HIDE_STATE:
		{
			u32 State		= P.r_u32();
			BOOL	Set		= !!P.r_u8();
#ifdef DEBUG
			if(ph_dbg_draw_mask.test(phDbgLadder)&&(INV_STATE_LADDER==State) )
			{
				if(Set)Msg("OnEvent   GEG_PLAYER_WEAPON_HIDE_STATE ON LADDER");
				else Msg("OnEvent  GEG_PLAYER_WEAPON_HIDE_STATE OFF LADDER");
			}
#endif
			inventory().SetSlotsBlocked	((u16)State, !!Set);
		}break;
	case GE_MOVE_ACTOR:
		{
			Fvector NewPos, NewRot;
			P.r_vec3(NewPos);
			P.r_vec3(NewRot);
			
			MoveActor(NewPos, NewRot);
		}break;
	case GE_ACTOR_MAX_POWER:
		{
			conditions().MaxPower();
			conditions().ClearWounds();
			ClearBloodWounds();
		}break;
	case GEG_PLAYER_ATTACH_HOLDER:
		{
			u32 id = P.r_u32();
			CObject* O	= Level().Objects.net_Find	(id);
			if (!O){
				Msg("! Error: No object to attach holder [%d]", id);
				break;
			}
			VERIFY(m_holder==NULL);
			CHolderCustom*	holder = smart_cast<CHolderCustom*>(O);
			if(!holder->Engaged())	use_Holder		(holder);

		}break;
	case GEG_PLAYER_DETACH_HOLDER:
		{
			if			(!m_holder)	break;
			u32 id			= P.r_u32();
			CGameObject*	GO	= smart_cast<CGameObject*>(m_holder);
			VERIFY			(id==GO->ID());
			use_Holder		(NULL);
		}break;
	case GEG_PLAYER_PLAY_HEADSHOT_PARTICLE:
		{
			OnPlayHeadShotParticle(P);
		}break;
	case GE_ACTOR_JUMPING:
		{
			/*
			Fvector dir;
			P.r_dir(dir);
			float jump = P.r_float();
			NET_SavedAccel = dir;
			extern float NET_Jump;
			NET_Jump = jump;
			m_bInInterpolation = false;
			mstate_real |= mcJump;
			*/
		}break;
	}
}

void			CActor::MoveActor		(Fvector NewPos, Fvector NewDir)
{
	Fmatrix	M = XFORM();
	M.translate(NewPos);
	r_model_yaw				= NewDir.y;
	r_torso.yaw				= NewDir.y;
	r_torso.pitch			= -NewDir.x;
	unaffected_r_torso.yaw	= r_torso.yaw;
	unaffected_r_torso.pitch= r_torso.pitch;
	unaffected_r_torso.roll	= 0;//r_torso.roll;

	r_torso_tgt_roll		= 0;
	cam_Active()->Set		(-unaffected_r_torso.yaw,unaffected_r_torso.pitch,unaffected_r_torso.roll);
	ForceTransform(M);

	m_bInInterpolation = false;	
}
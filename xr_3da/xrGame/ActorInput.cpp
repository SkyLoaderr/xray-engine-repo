#include "stdafx.h"
#include "Actor.h"
#include "Torch.h"
#include "trade.h"
#include "../CameraBase.h"
#include "Car.h"
#include "HudManager.h"
#include "UIGameSP.h"
#include "inventory.h"

void CActor::IR_OnKeyboardPress(int cmd)
{
	if (Remote())												return;

	if (GAME_PHASE_PENDING == Game().phase || !g_Alive())
	{
		if (kWPN_FIRE == cmd)	
		{
			// Switch our "ready" status
			NET_Packet			P;
			u_EventGen			(P,GEG_PLAYER_READY,ID());
			u_EventSend			(P);
		}
		return;
	}

	if (!g_Alive()) return;

	if (cmd == kUSE) 
	{
		if(m_pPersonWeLookingAt)
		{
			CEntityAlive* pEntityAliveWeLookingAt = 
							dynamic_cast<CEntityAlive*>(m_pPersonWeLookingAt);
			
			R_ASSERT(pEntityAliveWeLookingAt);

			if(pEntityAliveWeLookingAt->g_Alive())
			{
				if(!IsTalking())
				{
					//���������� ���������� � ����
					if(m_pPersonWeLookingAt->OfferTalk(this))
					{	
						StartTalk(m_pPersonWeLookingAt);

						//������ ���� ��������� � ������ single
						CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
						if(pGameSP)pGameSP->StartTalk();
					}
				}
			}
			//����� �����
			else if(Level().IR_GetKeyState(DIK_LSHIFT))
			{
				//������ ���� ��������� � ������ single
				CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
				if(pGameSP)pGameSP->StartCarBody(&inventory(), this,
												 &m_pPersonWeLookingAt->inventory(),
												 dynamic_cast<CGameObject*>(m_pPersonWeLookingAt));
			}
		}
		else if(m_pCarWeLookingAt && Level().IR_GetKeyState(DIK_LSHIFT))
		{
			//������ ���� ��������� � ������ single
			CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
			if(pGameSP)pGameSP->StartCarBody(&inventory(), this,
											 m_pCarWeLookingAt->GetInventory(),
											 dynamic_cast<CGameObject*>(m_pCarWeLookingAt));

		}
	}

	if(m_vehicle && kUSE != cmd)
	{
		m_vehicle->IR_OnKeyboardPress(cmd);
		return;
	}

	if(inventory().Action(cmd, CMD_START))						return;


	switch(cmd){
	case kACCEL:	mstate_wishful |= mcAccel;					break;
	case kR_STRAFE:	mstate_wishful |= mcRStrafe;				break;
	case kL_STRAFE:	mstate_wishful |= mcLStrafe;				break;
	case kFWD:		mstate_wishful |= mcFwd;					break;
	case kBACK:		mstate_wishful |= mcBack;					break;
	case kJUMP:		mstate_wishful |= mcJump;					break;
	case kCROUCH:	mstate_wishful |= mcCrouch;					break;

	case kCAM_1:	cam_Set			(eacFirstEye);				break;
	case kCAM_2:	cam_Set			(eacLookAt);				break;
	case kCAM_3:	cam_Set			(eacFreeLook);				break;

	case kWPN_FIRE:	g_fireStart		(); 						break;
	case kWPN_ZOOM:	g_fire2Start	();							break;
	case kTORCH:{ 
		PIItem I = inventory().Get("device_torch",false); 
		if (I){
			CTorch* torch = dynamic_cast<CTorch*>(I);
			if (torch) torch->Switch();
		}
		}break;
//	case kWPN_ZOOM:	Weapons->Zoom	(TRUE);						break;
	case kWPN_1:	
	case kWPN_2:	
	case kWPN_3:	
	case kWPN_4:	
	case kWPN_5:	
	case kWPN_6:	
	case kWPN_7:	
	case kWPN_8:	
	case kWPN_9:	
		//Weapons->ActivateWeaponID	(cmd-kWPN_1);			
		break;
	case kBINOCULARS:
		//Weapons->ActivateWeaponID	(Weapons->WeaponCount()-1);
		break;
	case kWPN_RELOAD:
		//Weapons->Reload			();
		break;
	case kUSE:
		ActorUse();
		break;
	case kDROP:
		b_DropActivated			= TRUE;
		f_DropPower				= 0;
		break;
	}
}

void CActor::IR_OnKeyboardRelease(int cmd)
{
	if (Remote())		return;

	if (g_Alive())	
	{
		if(inventory().Action(cmd, CMD_STOP)) return;

		if(m_vehicle)
		{
			m_vehicle->IR_OnKeyboardRelease(cmd);
			return;
		}

		switch(cmd)
		{
		case kACCEL:	mstate_wishful &=~mcAccel;		break;
		case kR_STRAFE:	mstate_wishful &=~mcRStrafe;	break;
		case kL_STRAFE:	mstate_wishful &=~mcLStrafe;	break;
		case kFWD:		mstate_wishful &=~mcFwd;		break;
		case kBACK:		mstate_wishful &=~mcBack;		break;
		case kJUMP:		mstate_wishful &=~mcJump;		break;
		case kCROUCH:	mstate_wishful &=~mcCrouch;		break;

		case kWPN_FIRE:	g_fireEnd();					break;
		case kWPN_ZOOM:	g_fire2End();					break;
//		case kWPN_ZOOM:	Weapons->Zoom(FALSE);			break;

		case kDROP:		if(GAME_PHASE_INPROGRESS == Game().phase) g_PerformDrop();				break;
		}
	}
}

void CActor::IR_OnKeyboardHold(int cmd)
{
	if (Remote() || !g_Alive())		return;

	if(m_vehicle)
	{
		m_vehicle->IR_OnKeyboardHold(cmd);
		return;
	}

	switch(cmd)
	{
	case kUP:
	case kDOWN: 
	case kCAM_ZOOM_IN: 
	case kCAM_ZOOM_OUT: 
		cam_Active()->Move(cmd); break;
	case kLEFT:
	case kRIGHT:
		if (eacFreeLook!=cam_active) cam_Active()->Move(cmd); break;
	}
}

void CActor::IR_OnMouseMove(int dx, int dy)
{
	if (Remote())	return;

	if(m_vehicle) 
	{
		m_vehicle->IR_OnMouseMove(dx,dy);
		return;
	}

	CCameraBase* C	= cameras	[cam_active];
	float scale		= (C->f_fov/DEFAULT_FOV)*psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d = float(dx)*scale;
		cam_Active()->Move((d<0)?kLEFT:kRIGHT, _abs(d));
	}
	if (dy){
		float d = ((psMouseInvert.test(1))?-1:1)*float(dy)*scale*3.f/4.f;
		cam_Active()->Move((d>0)?kUP:kDOWN, _abs(d));
	}
}

void CActor::ActorUse()
{
	if(Level().IR_GetKeyState(DIK_LSHIFT)) return;

	u16 element=BI_NONE;
	CGameObject* object =pick_Object(element);	
	if(use_Vehicle(object)) return;
	if(!m_PhysicMovementControl.PHCapture())
		m_PhysicMovementControl.PHCaptureObject(object,element);
	else
		m_PhysicMovementControl.PHReleaseObject();
	
}
//void CActor::IR_OnMousePress(int btn)
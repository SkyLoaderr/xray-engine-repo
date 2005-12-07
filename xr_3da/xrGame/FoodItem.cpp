///////////////////////////////////////////////////////////////
// FoodItem.cpp
// FoodItem - ��� ����������
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "eatable_item.h"
#include "FoodItem.h"
#include "WeaponHud.h"
#include "PhysicsShell.h"
#include "xr_level_controller.h"
#include "entity_alive.h"

#include "actor.h"
#include "../skeletoncustom.h"

#define PLAYING_ANIM_TIME 10000

CFoodItem::CFoodItem() 
{
	m_bReadyToEat = false;
}

CFoodItem::~CFoodItem() 
{
}

DLL_Pure *CFoodItem::_construct	()
{
	CEatableItemObject::_construct();
	CHudItem::_construct	();
	return					(this);
}

BOOL CFoodItem::net_Spawn(CSE_Abstract* DC) 
{
	return		(
        CHudItem::net_Spawn(DC) &&
		CEatableItemObject::net_Spawn(DC)
	);
}

void CFoodItem::Load(LPCSTR section) 
{
	CEatableItemObject::Load(section);
	CHudItem::Load		(section);

	if(m_pHUD)
	{
		m_sAnimIdle		= pSettings->r_string(*hud_sect, "anim_idle");
		m_sAnimShow		= pSettings->r_string(*hud_sect, "anim_show");
		m_sAnimHide		= pSettings->r_string(*hud_sect, "anim_hide");
		m_sAnimPlay		= pSettings->r_string(*hud_sect, "anim_play");
		m_sAnimPrepare	= pSettings->r_string(*hud_sect, "anim_prepare");
		m_sAnimEat		= pSettings->r_string(*hud_sect, "anim_eat");
	}

//	Fvector	position_offset, angle_offset;
//	position_offset		= pSettings->r_fvector3(section,"position_offset");
//	angle_offset		= pSettings->r_fvector3(section,"angle_offset");
//	m_offset.setHPB			(VPUSH(angle_offset));
//	m_offset.translate_over	(position_offset);
}

void CFoodItem::net_Destroy() 
{
	CHudItem::net_Destroy		();
	CEatableItemObject::net_Destroy	();
}

void CFoodItem::shedule_Update(u32 dt) 
{
	CEatableItemObject::shedule_Update(dt);
}

void CFoodItem::UpdateCL() 
{
	CEatableItemObject::UpdateCL();
	CHudItem::UpdateCL();

	if(STATE == FOOD_IDLE && m_dwStateTime > PLAYING_ANIM_TIME) 
		OnStateSwitch(FOOD_PLAYING);
}

u32 CFoodItem::GetSlot	() const
{
	if(H_Parent() && smart_cast<const CActor*>(H_Parent()))
	{
		return NO_ACTIVE_SLOT;
	}

	return CEatableItemObject::GetSlot();
}

void CFoodItem::OnH_A_Chield() 
{
	CHudItem::OnH_A_Chield				();
	CEatableItemObject::OnH_A_Chield	();

	m_bReadyToEat = false;
	STATE = FOOD_HIDDEN;
}

void CFoodItem::OnH_B_Independent() 
{
	CHudItem::OnH_B_Independent();
	CEatableItemObject::OnH_B_Independent();
}

void CFoodItem::OnH_A_Independent	()
{
	CEatableItemObject::OnH_A_Independent();
}

void CFoodItem::OnH_B_Chield		()
{
	CEatableItemObject::OnH_B_Chield();
	CHudItem::OnH_B_Chield();
}

void CFoodItem::renderable_Render() 
{
	CHudItem::renderable_Render				();
}

void CFoodItem::Show()
{
	m_bReadyToEat = false;
	SwitchState(FOOD_SHOWING);
}

void CFoodItem::Hide() 
{
	SwitchState(FOOD_HIDING);
}



bool CFoodItem::Action(s32 cmd, u32 flags) 
{
	if (CEatableItemObject::Action(cmd, flags) || CHudItem::Action(cmd,flags))
		return					(true);

	switch(cmd) 
	{
	case kWPN_FIRE:
		{
        	if(flags&CMD_START) 
			{
				if(STATE == FOOD_IDLE || STATE == FOOD_PLAYING)
				{
                    if(m_bReadyToEat)
                        SwitchState(FOOD_EATING);
					else
						SwitchState(FOOD_PREPARE);
				}
			} 
		}
        return true;
	}
	return false;
}

void CFoodItem::OnAnimationEnd() 
{
	m_bPending = false;

	switch(STATE) 
	{
	case FOOD_HIDING:
		{
			OnStateSwitch(FOOD_HIDDEN);
		} break;
	case FOOD_PREPARE:
		m_bReadyToEat = true;
	case FOOD_SHOWING:
	case FOOD_EATING:
	case FOOD_PLAYING:
		{
			OnStateSwitch(FOOD_IDLE);
		} break;
	}
}

void CFoodItem::OnStateSwitch	(u32 S)
{
	CHudItem::OnStateSwitch	(S);

	switch(STATE)
	{
		case FOOD_HIDDEN:
			m_bPending = false;
			setVisible(FALSE);
			break;
		case FOOD_SHOWING:
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimShow), true, this);
			break;
		case FOOD_IDLE:
			m_bPending = false;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimIdle), true, this);
			break;
		case FOOD_PREPARE:
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimPrepare), true, this);
			break;
		case FOOD_EATING:
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimEat), true, this);
			break;
		case FOOD_HIDING:
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimHide), true, this);
			break;
		case FOOD_PLAYING:
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimPlay), true, this);
			break;
		default: NODEFAULT;
	}
}

void CFoodItem::UpdateXForm	()
{
	CEatableItemObject::renderable_Render	();
/**
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame = Device.dwFrame;

		if (0==H_Parent())	return;

		// Get access to entity and its visual
		CEntityAlive*	E		= smart_cast<CEntityAlive*>(H_Parent());

		if(!E) return;

		R_ASSERT		(E);
		CKinematics*	V		= smart_cast<CKinematics*>	(E->Visual());
		VERIFY			(V);

		// Get matrices
		int				boneL,boneR,boneR2;
		E->g_WeaponBones(boneL,boneR,boneR2);

		boneL = boneR2;

		V->CalculateBones	();
		Fmatrix& mL			= V->LL_GetTransform(u16(boneL));
		Fmatrix& mR			= V->LL_GetTransform(u16(boneR));

		// Calculate
		Fmatrix			mRes;
		Fvector			R,D,N;
		D.sub			(mL.c,mR.c);	D.normalize_safe();
		R.crossproduct	(mR.j,D);		R.normalize_safe();
		N.crossproduct	(D,R);			N.normalize_safe();
		mRes.set		(R,N,D,mR.c);
		mRes.mulA_43	(E->XFORM());
		XFORM().mul		(mRes, offset());
	}
/**/
}

void CFoodItem::OnEvent		(NET_Packet& P, u16 type)
{
	CEatableItemObject::OnEvent(P,type);
	CHudItem::OnEvent			(P,type);
}

bool CFoodItem::Activate	()
{
	return CHudItem::Activate();
}

void CFoodItem::Deactivate	()
{
	CHudItem::Deactivate	();
}

bool CFoodItem::Useful		() const
{
	return CEatableItemObject::Useful();
}

bool CFoodItem::IsPending	() const
{
	return CHudItem::IsPending();
}

void CFoodItem::on_renderable_Render	()
{
	CEatableItemObject::renderable_Render();
}

///////////////////////////////////////////////////////////////
// FoodItem.cpp
// FoodItem - еда персонажей
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "eatable_item.h"
#include "FoodItem.h"
#include "WeaponHud.h"
#include "PhysicsShell.h"


#define PLAYING_ANIM_TIME 10000

CFoodItem::CFoodItem(void) 
{
	m_bReadyToEat = false;
}

CFoodItem::~CFoodItem(void) 
{
}


BOOL CFoodItem::net_Spawn(LPVOID DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CFoodItem::Load(LPCSTR section) 
{
	inherited::Load(section);
	CEatableItem::Load(section);


	if(m_pHUD)
	{
		m_sAnimIdle		= pSettings->r_string(*hud_sect, "anim_idle");
		m_sAnimShow		= pSettings->r_string(*hud_sect, "anim_show");
		m_sAnimHide		= pSettings->r_string(*hud_sect, "anim_hide");
		m_sAnimPlay		= pSettings->r_string(*hud_sect, "anim_play");
		m_sAnimPrepare	= pSettings->r_string(*hud_sect, "anim_prepare");
		m_sAnimEat		= pSettings->r_string(*hud_sect, "anim_eat");
	}
}

void CFoodItem::net_Destroy() 
{
	inherited::net_Destroy();
}

void CFoodItem::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);

}

void CFoodItem::UpdateCL() 
{
	inherited::UpdateCL();

	if(STATE == FOOD_IDLE && m_dwStateTime > PLAYING_ANIM_TIME) 
		OnStateSwitch(FOOD_PLAYING);
}

void CFoodItem::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();

	m_bReadyToEat = false;
	STATE = FOOD_HIDDEN;
}

void CFoodItem::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CFoodItem::renderable_Render() 
{
	inherited::renderable_Render();
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
	if(inherited::Action(cmd, flags)) return true;

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
	inherited::OnStateSwitch	(S);


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
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
	BOOL res = inherited::net_Spawn(DC);

	CSkeletonRigid* V = PSkeletonRigid(Visual());
	R_ASSERT(V);


	if (m_pPhysicsShell==0) 
	{
		// Physics (Box)
		Fobb obb; 
		Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); 
		obb.m_rotate.identity();
		
		// Physics (Elements)
		CPhysicsElement* E = P_create_Element(); 
		R_ASSERT(E); 
		E->add_Box(obb);
		// Physics (Shell)
		m_pPhysicsShell = P_create_Shell(); 
		R_ASSERT(m_pPhysicsShell);
		m_pPhysicsShell->add_Element(E);
		m_pPhysicsShell->setDensity(2000.f);
		
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity();
		m_pPhysicsShell->fDesiredStrength = 0.f;
	}

	setVisible(true);
	setEnabled(true);

	return res;
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
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	
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


	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}


void CFoodItem::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();

	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();

	m_bReadyToEat = false;
}

void CFoodItem::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();

	setVisible(true);
	setEnabled(true);
	
	CObject* E = dynamic_cast<CObject*>(H_Parent()); 
	R_ASSERT(E);
	
	XFORM().set(E->XFORM());

	if(m_pPhysicsShell) 
	{
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM()); l_p1.c.add(l_up); l_up.mul(1.2f); 
		l_p2.set(XFORM()); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
	}
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
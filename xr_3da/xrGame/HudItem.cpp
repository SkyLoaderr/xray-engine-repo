//////////////////////////////////////////////////////////////////////
// HudItem.cpp: ����� �������� ��� ���� ��������� �������
//				����������� HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HudItem.h"
#include "WeaponHUD.h"
#include "Actor.h"

CHudItem::CHudItem(void)
{
	m_pHUD = NULL;
	hud_mode = FALSE;
	m_dwStateTime = 0;
}
CHudItem::~CHudItem(void)
{
	xr_delete			(m_pHUD);
}

void CHudItem::Load(LPCSTR section)
{
	inherited::Load		(section);

	//��������� hud, ���� �� �����
	if(pSettings->line_exist(section,"hud"))
		hud_sect = pSettings->r_string		(section,"hud");

	if(*hud_sect)
	{
		m_pHUD				= xr_new<CWeaponHUD> (this);
		m_pHUD->Load		(*hud_sect);
	}
	else
	{
		m_pHUD = NULL;
		//���� hud �� �����, �� ����� ����, �� ������
		R_ASSERT2(m_slot == NO_ACTIVE_SLOT, "active slot is set, but hud for food item is not available");
	}
}


void CHudItem::renderable_Render()
{
	if (m_pHUD)	PSkeletonAnimated(m_pHUD->Visual())->Update	();
	
	CActor *pActor = dynamic_cast<CActor*>(H_Parent());
	if (m_pHUD && pActor && hud_mode && !m_pHUD->IsHidden())
	{ 
		// HUD render
		UpdateHudPosition	();
		::Render->set_Transform		(&m_pHUD->Transform());
		::Render->add_Visual		(m_pHUD->Visual());
	}
	else if(!pActor || !hud_mode)
	{
		// normal render
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}

bool CHudItem::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	return false;
}
void CHudItem::OnAnimationEnd()
{
}

void CHudItem::SwitchState(u32 S)
{
	if (Local()/* && (S!=NEXT_STATE)*/)	
	{
		// !!! Just single entry for given state !!!
		NEXT_STATE		= S;	// Very-very important line of code!!! :)
		NET_Packet		P;
		u_EventGen		(P,GE_WPN_STATE_CHANGE,ID());
		P.w_u8			(u8(S));
		u_EventSend		(P);
	}
}

void CHudItem::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);
	
	switch (type)
	{
	case GE_WPN_STATE_CHANGE:
		{
			u8				S;
			P.r_u8			(S);
			OnStateSwitch	(u32(S));
		}
		break;
	}
}

void CHudItem::OnStateSwitch	(u32 S)
{
	m_dwStateTime = 0;
	
	STATE = S;
	if(Remote()) NEXT_STATE = S;
}


bool CHudItem::Activate() 
{
	Show();
	return true;
}

void CHudItem::Deactivate() 
{
	Hide();
}



void CHudItem::UpdateHudPosition	()
{
	if (Device.dwFrame == dwHudUpdate_Frame) return;
	
	dwHudUpdate_Frame = Device.dwFrame;

	if (m_pHUD && hud_mode)
	{
		Fmatrix							trans;
		Level().Cameras.affected_Matrix	(trans);
		m_pHUD->UpdatePosition			(trans);
	}
}

void CHudItem::UpdateCL()
{
	inherited::UpdateCL();
	m_dwStateTime += Device.dwTimeDelta;
}
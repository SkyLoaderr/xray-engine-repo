//////////////////////////////////////////////////////////////////////
// HudItem.cpp: класс родитель для всех предметов имеющих
//				собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HudItem.h"
#include "WeaponHUD.h"
#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "xrmessages.h"
#include "level.h"
#include "inventory.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CHudItem::CHudItem(void)
{
	m_pHUD = NULL;
	hud_mode = FALSE;
	m_dwStateTime = 0;
	m_bRenderHud = true;
}
CHudItem::~CHudItem(void)
{
	xr_delete			(m_pHUD);
}


void CHudItem::Load(LPCSTR section)
{
	inherited::Load		(section);

	//загрузить hud, если он нужен
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
		//если hud не задан, но задан слот, то ошибка
		R_ASSERT2(m_slot == NO_ACTIVE_SLOT, "active slot is set, but hud for food item is not available");
	}
}

void CHudItem::net_Destroy()
{
	if(m_pHUD)
		m_pHUD->net_DestroyHud	();

	hud_mode = FALSE;
	m_dwStateTime = 0;

	inherited::net_Destroy();
}


void CHudItem::PlaySound(HUD_SOUND& hud_snd,
						 const Fvector& position)
{
	HUD_SOUND::PlaySound(hud_snd, position, H_Root(), !!hud_mode);
}

BOOL  CHudItem::net_Spawn(LPVOID DC) 
{
	BOOL l_res = inherited::net_Spawn(DC);
	return l_res;
}


void CHudItem::renderable_Render()
{
	UpdateXForm();
	if(hud_mode && !m_pHUD->IsHidden() && !IsHidden())
	{ 
		// HUD render
		if(m_bRenderHud)
		{
			::Render->set_Transform		(&m_pHUD->Transform());
			::Render->add_Visual		(m_pHUD->Visual());
		}
	}
	else
		if(!H_Parent() || (!hud_mode && m_pHUD && !m_pHUD->IsHidden() && !IsHidden()))
		{
			inherited::renderable_Render();
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
	NEXT_STATE		= S;	// Very-very important line of code!!! :)
	if (Local() && !getDestroy()/* && (S!=NEXT_STATE)*/)	
	{
		// !!! Just single entry for given state !!!
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
	if(m_pHUD) 
		m_pHUD->Init();

	Show();
	return true;
}

void CHudItem::Deactivate() 
{
	Hide();
}



void CHudItem::UpdateHudPosition	()
{
	if (m_pHUD && hud_mode)
	{
		Fmatrix							trans;

		CActor* pActor = smart_cast<CActor*>(H_Parent());
		if(pActor)
		{
			pActor->EffectorManager().affected_Matrix	(trans);
			m_pHUD->UpdatePosition						(trans);
		}
	}
}

void CHudItem::UpdateCL()
{
	inherited::UpdateCL();
	m_dwStateTime += Device.dwTimeDelta;

	if(m_pHUD) m_pHUD->UpdateHud();
	UpdateHudPosition	();
}

void CHudItem::OnH_A_Chield		()
{
	hud_mode = FALSE;
	
	if (m_pHUD) {
		if(Level().CurrentEntity() == H_Parent() && smart_cast<CActor*>(H_Parent()))
			m_pHUD->SetCurrentEntityHud(true);
		else
			m_pHUD->SetCurrentEntityHud(false);
	}

	inherited::OnH_A_Chield		();
}

void CHudItem::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();

	if (m_pInventory && m_pInventory->ActiveItem() == smart_cast<PIItem>(this))
		OnActiveItem ();
	else
		OnHiddenItem ();
}

void CHudItem::OnH_B_Independent	()
{
	hud_mode = FALSE;
	if (m_pHUD)
		m_pHUD->SetCurrentEntityHud(false);
	
	StopHUDSounds();

	inherited::OnH_B_Independent	();
}

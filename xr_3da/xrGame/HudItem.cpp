//////////////////////////////////////////////////////////////////////
// HudItem.cpp: класс родитель для всех предметов имеющих
//				собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HudItem.h"

#include "WeaponHUD.h"

CHudItem::CHudItem(void)
{
	m_pHUD				= xr_new<CWeaponHUD> (this);
	hud_mode			= FALSE;
}
CHudItem::~CHudItem(void)
{
	xr_delete			(m_pHUD);
}

void CHudItem::Load		(LPCSTR section)
{
	inherited::Load		(section);

	hud_sect			= pSettings->r_string		(section,"hud");
	m_pHUD->Load		(*hud_sect);
}


void CHudItem::renderable_Render		()
{
	if (m_pHUD)	PSkeletonAnimated(m_pHUD->Visual())->Update	();
}



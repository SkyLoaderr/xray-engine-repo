///////////////////////////////////////////////////////////////
// CustomOutfit.cpp
// CustomOutfit - защитные костюмы для персонажей
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "customoutfit.h"
#include "PhysicsShell.h"
#include "inventory_space.h"

CCustomOutfit::CCustomOutfit()
{
	m_slot = OUTFIT_SLOT;

	m_bUsingCondition = true;

	m_HitTypeK.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
	{
		m_HitTypeK[i] = 1.0f;
	}
}

CCustomOutfit::~CCustomOutfit() 
{
}


BOOL CCustomOutfit::net_Spawn(LPVOID DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CCustomOutfit::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_HitTypeK[ALife::eHitTypeBurn]			= pSettings->r_float(section,"burn_protection");
	m_HitTypeK[ALife::eHitTypeStrike]		= pSettings->r_float(section,"strike_protection");
	m_HitTypeK[ALife::eHitTypeShock]		= pSettings->r_float(section,"shock_protection");
	m_HitTypeK[ALife::eHitTypeWound]		= pSettings->r_float(section,"wound_protection");
	m_HitTypeK[ALife::eHitTypeRadiation]	= pSettings->r_float(section,"radiation_protection");
	m_HitTypeK[ALife::eHitTypeTelepatic]	= pSettings->r_float(section,"telepatic_protection");
	m_HitTypeK[ALife::eHitTypeChemicalBurn] = pSettings->r_float(section,"chemical_burn_protection");
	m_HitTypeK[ALife::eHitTypeExplosion]	= pSettings->r_float(section,"explosion_protection");
	m_HitTypeK[ALife::eHitTypeFireWound]	= pSettings->r_float(section,"fire_wound_protection");

	m_iOutfitIconX = pSettings->r_u32(section, "full_scale_icon_x");
	m_iOutfitIconY = pSettings->r_u32(section, "full_scale_icon_y");
}

void CCustomOutfit::net_Destroy() 
{
	inherited::net_Destroy();
}

void CCustomOutfit::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CCustomOutfit::UpdateCL() 
{
	inherited::UpdateCL();
}


void CCustomOutfit::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CCustomOutfit::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CCustomOutfit::renderable_Render() 
{
	inherited::renderable_Render();
}

void CCustomOutfit::Hit(float P, Fvector &dir,	
		    CObject* who, s16 element,
		    Fvector position_in_object_space, 
		    float impulse, 
		    ALife::EHitType hit_type)
{
	inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);
}


float CCustomOutfit::GetHitTypeK(ALife::EHitType hit_type)
{
	return m_HitTypeK[hit_type];
}
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

	m_fOutfitBurn = pSettings->r_float(section,"burn_protection");
	m_fOutfitStrike = pSettings->r_float(section,"strike_protection");
	m_fOutfitShock = pSettings->r_float(section,"shock_protection");
	m_fOutfitWound = pSettings->r_float(section,"wound_protection");
	m_fOutfitRadiation = pSettings->r_float(section,"radiation_protection");
	m_fOutfitTelepatic = pSettings->r_float(section,"telepatic_protection");
	m_fOutfitChemicalBurn = pSettings->r_float(section,"chemical_burn_protection");
	m_fOutfitExplosion = pSettings->r_float(section,"explosion_protection");
	m_fOutfitFireWound = pSettings->r_float(section,"fire_wound_protection");


	m_fK_Burn = pSettings->r_float(section,"burn_immunity");
	m_fK_Strike = pSettings->r_float(section,"strike_immunity");
	m_fK_Shock = pSettings->r_float(section,"shock_immunity");
	m_fK_Wound = pSettings->r_float(section,"wound_immunity");
	m_fK_Radiation = pSettings->r_float(section,"radiation_immunity");
	m_fK_Telepatic = pSettings->r_float(section,"telepatic_immunity");
	m_fK_ChemicalBurn = pSettings->r_float(section,"chemical_burn_immunity");
	m_fK_Explosion = pSettings->r_float(section,"explosion_immunity");
	m_fK_FireWound = pSettings->r_float(section,"fire_wound_immunity");

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
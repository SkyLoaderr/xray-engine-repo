///////////////////////////////////////////////////////////////
// CustomOutfit.cpp
// CustomOutfit - �������� ������� ��� ����������
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "customoutfit.h"
#include "PhysicsShell.h"
#include "inventory_space.h"
#include "Inventory.h"
#include "Actor.h"
#include "game_cl_base.h"
#include "Level.h"
#include "BoneProtections.h"


CCustomOutfit::CCustomOutfit()
{
	m_slot = OUTFIT_SLOT;

	m_flags.set(FUsingCondition, TRUE);

	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeProtection[i] = 1.0f;

	m_boneProtection = xr_new<SBoneProtections>();
}

CCustomOutfit::~CCustomOutfit() 
{
	xr_delete(m_boneProtection);
}


BOOL CCustomOutfit::net_Spawn(CSE_Abstract* DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CCustomOutfit::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_HitTypeProtection[ALife::eHitTypeBurn]		= pSettings->r_float(section,"burn_protection");
	m_HitTypeProtection[ALife::eHitTypeStrike]		= pSettings->r_float(section,"strike_protection");
	m_HitTypeProtection[ALife::eHitTypeShock]		= pSettings->r_float(section,"shock_protection");
	m_HitTypeProtection[ALife::eHitTypeWound]		= pSettings->r_float(section,"wound_protection");
	m_HitTypeProtection[ALife::eHitTypeRadiation]	= pSettings->r_float(section,"radiation_protection");
	m_HitTypeProtection[ALife::eHitTypeTelepatic]	= pSettings->r_float(section,"telepatic_protection");
	m_HitTypeProtection[ALife::eHitTypeChemicalBurn]= pSettings->r_float(section,"chemical_burn_protection");
	m_HitTypeProtection[ALife::eHitTypeExplosion]	= pSettings->r_float(section,"explosion_protection");
	m_HitTypeProtection[ALife::eHitTypeFireWound]	= pSettings->r_float(section,"fire_wound_protection");

	m_iOutfitIconX = pSettings->r_u32(section, "full_scale_icon_x");
	m_iOutfitIconY = pSettings->r_u32(section, "full_scale_icon_y");
		
	if (pSettings->line_exist(section, "actor_visual"))
		m_ActorVisual = pSettings->r_string(section, "actor_visual");
	else
		m_ActorVisual = NULL;

	m_ef_equipment_type		= pSettings->r_u32(section,"ef_equipment_type");
	if (pSettings->line_exist(section, "power_loss"))
		m_fPowerLoss = pSettings->r_float(section, "power_loss");
	else
		m_fPowerLoss = 1.0f;	
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

//void CCustomOutfit::Hit(float P, Fvector &dir,	
//		    CObject* who, s16 element,
//		    Fvector position_in_object_space, 
//		    float impulse, 
//		    ALife::EHitType hit_type)
void	CCustomOutfit::Hit					(SHit* pHDS)
{
//	inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);
	inherited::Hit(pHDS);
}

void CCustomOutfit::Hit(float hit_power, ALife::EHitType hit_type)
{
	hit_power *= m_HitTypeK[hit_type];
	ChangeCondition(-hit_power);
}

float CCustomOutfit::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
	return 1.0f - m_HitTypeProtection[hit_type]*GetCondition();
}

float CCustomOutfit::GetHitTypeProtection(ALife::EHitType hit_type, s16 element)
{
	float fBase = m_HitTypeProtection[hit_type]*GetCondition();
	float bone = m_boneProtection->getBoneProtection(element);
	return 1.0f - fBase*bone;
}

float	CCustomOutfit::HitThruArmour(float hit_power, s16 element, float AP)
{
	float BoneArmour = m_boneProtection->getBoneArmour(element)*GetCondition()*(1-AP);	
	float NewHitPower = hit_power - BoneArmour;
	if (NewHitPower < hit_power*m_boneProtection->m_fHitFrac) return hit_power*m_boneProtection->m_fHitFrac;
	return NewHitPower;
};

void	CCustomOutfit::OnMoveToSlot		()
{
	if (m_pInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pInventory->GetOwner());
		if (pActor)
		{
			if (m_ActorVisual.size())
			{
				shared_str NewVisual = NULL;
				char* TeamSection = Game().getTeamSection(pActor->g_Team());
				if (TeamSection)
				{
					if (pSettings->line_exist(TeamSection, *cNameSect()))
					{
						NewVisual = pSettings->r_string(TeamSection, *cNameSect());
						string256 SkinName;
						std::strcpy(SkinName, pSettings->r_string("mp_skins_path", "skin_path"));
						strcat(SkinName, *NewVisual);
						strcat(SkinName, ".ogf");
						NewVisual._set(SkinName);
					}
				}
				
				if (!NewVisual.size())
					NewVisual = m_ActorVisual;

				pActor->ChangeVisual(NewVisual);
			}
			if(pSettings->line_exist(cNameSect(),"bones_koeff_protection")){
				m_boneProtection->reload( pSettings->r_string(cNameSect(),"bones_koeff_protection"), smart_cast<CKinematics*>(pActor->Visual()) );

			};
		}
	}
};

void	CCustomOutfit::OnMoveToRuck		()
{
	if (m_pInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pInventory->GetOwner());
		if (pActor)
		{
			if (m_ActorVisual.size())
			{
				shared_str DefVisual = pActor->GetDefaultVisualOutfit();
				if (DefVisual.size())
				{
					pActor->ChangeVisual(DefVisual);
				};
			}
		}
	}
};

u32	CCustomOutfit::ef_equipment_type	() const
{
	return		(m_ef_equipment_type);
}

float CCustomOutfit::GetPowerLoss() 
{
	if (m_fPowerLoss<1 && GetCondition() <= 0)
	{
		return 1.0f;			
	};
	return m_fPowerLoss;
};
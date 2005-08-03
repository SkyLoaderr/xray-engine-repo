///////////////////////////////////////////////////////////////
// CustomOutfit.cpp
// CustomOutfit - защитные костюмы для персонажей
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "customoutfit.h"
#include "PhysicsShell.h"
#include "inventory_space.h"
#include "Inventory.h"
#include "Actor.h"
#include "game_cl_base.h"
#include "Level.h"


struct SBoneProtections{
	struct BoneProtection {
		float		koeff;
		float		armour;
	};
	typedef xr_map<s16,BoneProtection>		storage_type;
	typedef storage_type::iterator	storage_it;
						SBoneProtections	()								{m_default.koeff = 1.0f; m_default.armour = 0;}
	BoneProtection		m_default;
	storage_type		m_bones_koeff;
	void				reload				(const shared_str& outfit_section, CKinematics* kinematics);
	float				getBoneProtection	(s16 bone_id);
	float				getBoneArmour		(s16 bone_id);
};

float SBoneProtections::getBoneProtection	(s16 bone_id)
{
	storage_it it = m_bones_koeff.find(bone_id);
	if( it != m_bones_koeff.end() )
		return it->second.koeff;
	else
		return m_default.koeff;
}

float SBoneProtections::getBoneArmour	(s16 bone_id)
{
	storage_it it = m_bones_koeff.find(bone_id);
	if( it != m_bones_koeff.end() )
		return it->second.armour;
	else
		return m_default.armour;
}

void SBoneProtections::reload(const shared_str& outfit_section, CKinematics* kinematics)
{
	VERIFY(kinematics);
	m_bones_koeff.clear();


	if(!pSettings->line_exist(outfit_section,"bones_koeff_protection")) return;
	LPCSTR bone_sect = pSettings->r_string(outfit_section,"bones_koeff_protection");

	CInifile::Sect	&protections = pSettings->r_section(bone_sect);
	for (CInifile::SectIt i=protections.begin(); protections.end() != i; ++i) {
//		float k = (float)atof( *(*i).second );
		string256 buffer;
		float Koeff = (float)atof( _GetItem(*(*i).second, 0, buffer) );
		float Armour = (float)atof( _GetItem(*(*i).second, 1, buffer) );
		
		BoneProtection	BP;
		BP.koeff = Koeff;
		BP.armour = Armour;

		if (!xr_strcmp(*(*i).first,"default"))
		{
			m_default = BP;
		}
		else 
		{
			s16	bone_id				= kinematics->LL_BoneID(i->first);
			R_ASSERT2				(BI_NONE != bone_id, *(*i).first);			
			m_bones_koeff.insert(mk_pair(bone_id,BP));
		}
	}

}

CCustomOutfit::CCustomOutfit()
{
	m_slot = OUTFIT_SLOT;

	m_bUsingCondition = true;

	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
	{
		m_HitTypeProtection[i] = 1.0f;
	}
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
	//  [8/3/2005]
	m_fHitFrac = READ_IF_EXISTS(pSettings, r_float, section, "hit_fraction",	0.1f);
	//  [8/3/2005]
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

void CCustomOutfit::Hit(float hit_power, ALife::EHitType hit_type)
{
	if(!m_bUsingCondition) return;
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

float	CCustomOutfit::HitThruArmour(float hit_power, s16 element)
{
	float BoneArmour = m_boneProtection->getBoneArmour(element)*GetCondition();	
	float NewHitPower = hit_power - BoneArmour/100.0f;
	if (NewHitPower < hit_power*m_fHitFrac) return hit_power*m_fHitFrac;
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
			m_boneProtection->reload( cNameSect(), smart_cast<CKinematics*>(pActor->Visual()) );
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
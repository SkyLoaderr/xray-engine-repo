// Actor_Weapon.cpp:	 для работы с оружием
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "inventory.h"
#include "weapon.h"
#include "map_manager.h"



static const float VEL_MAX		= 10.f;
static const float VEL_A_MAX	= 10.f;

//возвращает текуший разброс стрельбы (в радианах)с учетом движения
float CActor::GetWeaponAccuracy() const
{
	CWeapon* W	= smart_cast<CWeapon*>(inventory().ActiveItem());

	if(m_bZoomAimingMode&&W&&!W->IsRotatingToZoom())
		return m_fDispAim;

	float dispersion = m_fDispBase*W->Get_PDM_Base();

	CEntity::SEntityState state;
	if (g_State(state))
	{
		// angular factor
		dispersion *= (1.f + (state.fAVelocity/VEL_A_MAX)*m_fDispVelFactor*W->Get_PDM_Vel_F());
//		Msg("--- base=[%f] angular disp=[%f]",m_fDispBase, dispersion);
		// linear movement factor
		bool bAccelerated = isActorAccelerated(mstate_real, IsZoomAimingMode());
		if( bAccelerated )
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*m_fDispVelFactor*W->Get_PDM_Vel_F()*(1.f + m_fDispAccelFactor*W->Get_PDM_Accel_F()));
		else
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*m_fDispVelFactor*W->Get_PDM_Vel_F());

		if (state.bCrouch){	
			dispersion *= (1.f + m_fDispCrouchFactor*W->Get_PDM_Crouch());

			if(!bAccelerated )
				dispersion *= (1.f + m_fDispCrouchNoAccelFactor*W->Get_PDM_Crouch_NA());
		}
	}

	return dispersion;
}


void CActor::g_fireParams	(const CHudItem* pHudItem, Fvector &fire_pos, Fvector &fire_dir)
{
	fire_pos = EffectorManager().vPosition;
	fire_dir = EffectorManager().vDirection;

	const CMissile* pMissile = smart_cast <const CMissile*> (pHudItem);
	if (pMissile)
	{
		Fvector offset;
		XFORM().transform_dir(offset, m_vMissileOffset);
		fire_pos.add(offset);
	}


/*
	if(Level().game.type == GAME_SINGLE)
	{
		if (HUDview()) 
		{
			fire_pos = Device.vCameraPosition;
			fire_dir = Device.vCameraDirection;
		}
	}
	else
	{
		Fvector		fire_normal;
		if (cam_Active()->style != eacFreeLook)
			cam_Active()->Get(fire_pos, fire_dir, fire_normal);
		else
		{
			fire_pos = Device.vCameraPosition;
			fire_dir = Device.vCameraDirection;
		}
	}
*/
}

void CActor::g_WeaponBones	(int &L, int &R1, int &R2)
{
	R1				= m_r_hand;
	R2				= m_r_finger2;
	L				= m_l_finger1;
}

BOOL CActor::g_State (SEntityState& state) const
{
	state.bJump			= !!(mstate_real&mcJump);
	state.bCrouch		= !!(mstate_real&mcCrouch);
	state.bFall			= !!(mstate_real&mcFall);
	state.bSprint		= !!(mstate_real&mcSprint);
	state.fVelocity		= m_PhysicMovementControl->GetVelocityActual();
	state.fAVelocity	= fCurAVelocity;
	return TRUE;
}


void		CActor::HideCurrentWeapon		(u32 Msg)//, bool only2handed)
{
	if (g_Alive() || this == Level().CurrentControlEntity())
	{
		NET_Packet	P;
		u_EventGen(P, Msg/*GEG_PLAYER_DEACTIVATE_CURRENT_SLOT*/, ID());
		u_EventSend(P);
	};
};

void		CActor::RestoreHidedWeapon		(u32 Msg)
{
	if (g_Alive() || this == Level().CurrentControlEntity())
	{
		NET_Packet	P;
		u_EventGen(P, Msg/*GEG_PLAYER_RESTORE_CURRENT_SLOT*/, ID());
		u_EventSend(P);
	};
}

void		CActor::Check_Weapon_ShowHideState	()
{
	if (!OnServer()) return;
	if (m_iCurWeaponHideState)
	{
		u32 InventorySlot = inventory().GetActiveSlot();
		if( InventorySlot != NO_ACTIVE_SLOT && InventorySlot <= PDA_SLOT &&
			inventory().m_slots[InventorySlot].m_pIItem &&
			(!(m_iCurWeaponHideState&(1<<2)) || !inventory().m_slots[InventorySlot].m_pIItem->IsSingleHanded()) &&
			(!(m_iCurWeaponHideState&(1<<3) && InventorySlot == KNIFE_SLOT))
			)
		{
			if (inventory().Activate(NO_ACTIVE_SLOT))
			{
				inventory().SetPrevActiveSlot(InventorySlot);
			};
		}
	}
	else
	{
		u32 InventorySlot = inventory().GetPrevActiveSlot();
		if( InventorySlot != NO_ACTIVE_SLOT && InventorySlot <= PDA_SLOT &&
			inventory().m_slots[InventorySlot].m_pIItem)
		{
			if (inventory().Activate(InventorySlot))
			{
				inventory().SetPrevActiveSlot(NO_ACTIVE_SLOT);
			}
		};
	}
};

static	u16 BestWeaponSlots [] = {
	RIFLE_SLOT		,		// 2
	PISTOL_SLOT		,		// 1
	GRENADE_SLOT	,		// 3
	KNIFE_SLOT		,		// 0
};
void CActor::SelectBestWeapon	()
{
	if (GameID() == GAME_SINGLE) return;
	for (int i=0; i<4; i++)
	{
		if (inventory().m_slots[BestWeaponSlots[i]].m_pIItem)
		{
			if (inventory().GetActiveSlot() != BestWeaponSlots[i])
			{
				inventory().Activate(BestWeaponSlots[i]);
			}
			return;
		};
	};
}

#define ENEMY_HIT_SPOT	"mp_hit_sector_location"
BOOL	g_bShowHitSectors	= TRUE;

void	CActor::HitSector(CObject* who, CObject* weapon)
{
	if (!g_bShowHitSectors) return;
	if (!g_Alive()) return;

	bool bShowHitSector = true;
	
	CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(who);

	if (!pEntityAlive || this == who) bShowHitSector = false;

	if (weapon)
	{
		CWeapon* pWeapon = smart_cast<CWeapon*> (weapon);
		if (pWeapon)
		{
			if (pWeapon->IsSilencerAttached())
			{
				bShowHitSector = false;
				if (pWeapon->IsGrenadeLauncherAttached())
				{
				}
			}
		}
	}

	if (!bShowHitSector) return;	
		Level().MapManager().AddMapLocation(ENEMY_HIT_SPOT, who->ID());
}
// Actor_Weapon.cpp:	 ��� ������ � �������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "inventory.h"
#include "weapon.h"
#include "map_manager.h"
#include "level.h"
#include "CharacterPhysicsSupport.h"
#include "EffectorShot.h"
#include "WeaponMagazined.h"
static const float VEL_MAX		= 10.f;
static const float VEL_A_MAX	= 10.f;

#define GetWeaponParam(pWeapon, func_name, def_value)	((pWeapon) ? (pWeapon->func_name) : def_value)

//���������� ������� ������� �������� (� ��������)� ������ ��������
float CActor::GetWeaponAccuracy() const
{
	CWeapon* W	= smart_cast<CWeapon*>(inventory().ActiveItem());
	

	if(m_bZoomAimingMode&&W&&!GetWeaponParam(W, IsRotatingToZoom(), false))
		return m_fDispAim;

	float dispersion = m_fDispBase*GetWeaponParam(W, Get_PDM_Base(), 1.0f);

	CEntity::SEntityState state;
	if (g_State(state))
	{
		// angular factor
		dispersion *= (1.f + (state.fAVelocity/VEL_A_MAX)*m_fDispVelFactor*GetWeaponParam(W, Get_PDM_Vel_F(), 1.0f));
//		Msg("--- base=[%f] angular disp=[%f]",m_fDispBase, dispersion);
		// linear movement factor
		bool bAccelerated = isActorAccelerated(mstate_real, IsZoomAimingMode());
		if( bAccelerated )
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*m_fDispVelFactor*GetWeaponParam(W, Get_PDM_Vel_F(), 1.0f)*(1.f + m_fDispAccelFactor*GetWeaponParam(W, Get_PDM_Accel_F(), 1.0f)));
		else
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*m_fDispVelFactor*GetWeaponParam(W, Get_PDM_Vel_F(), 1.0f));

		if (state.bCrouch){	
			dispersion *= (1.f + m_fDispCrouchFactor*GetWeaponParam(W, Get_PDM_Crouch(), 1.0f));

			if(!bAccelerated )
				dispersion *= (1.f + m_fDispCrouchNoAccelFactor*GetWeaponParam(W, Get_PDM_Crouch_NA(), 1.0f));
		}
	}

	return dispersion;
}


void CActor::g_fireParams	(const CHudItem* pHudItem, Fvector &fire_pos, Fvector &fire_dir)
{
	VERIFY			(inventory().ActiveItem());
	fire_pos		= Cameras().Pos();
	fire_dir		= Cameras().Dir();

	const CMissile	*pMissile = smart_cast <const CMissile*> (pHudItem);
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
	state.fVelocity		= character_physics_support()->movement()->GetVelocityActual();
	state.fAVelocity	= fCurAVelocity;
	return TRUE;
}

/*
void		CActor::HideCurrentWeapon		(u32 Msg)//, bool only2handed)
{
	if (g_Alive() || this == Level().CurrentControlEntity())
	{
		NET_Packet	P;
		u_EventGen(P, Msg, ID());
		u_EventSend(P);
	};
};

void		CActor::RestoreHidedWeapon		(u32 Msg)
{
	if (g_Alive() || this == Level().CurrentControlEntity())
	{
		NET_Packet	P;
		u_EventGen(P, Msg, ID());
		u_EventSend(P);
	};
}
*/
void		CActor::SetWeaponHideState		(u32 State, BOOL	Set)
{
	if (g_Alive() && this == Level().CurrentControlEntity())
	{
		NET_Packet	P;
		u_EventGen(P, GEG_PLAYER_WEAPON_HIDE_STATE, ID());
		P.w_u32(State);
		P.w_u8(u8(Set));
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
			(!(m_iCurWeaponHideState&whs_ON_LEDDER) || !inventory().m_slots[InventorySlot].m_pIItem->IsSingleHanded()) &&
			(!(m_iCurWeaponHideState&whs_CAR) || !(inventory().m_slots[InventorySlot].m_pIItem->IsSingleHanded())||InventorySlot == KNIFE_SLOT) &&
			(!(m_iCurWeaponHideState&whs_SPRINT && InventorySlot == KNIFE_SLOT))
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
	if ( IsGameTypeSingle() ) return;
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

void CActor::on_weapon_shot_start		(CWeapon *weapon)
{	
	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (weapon);
	//*
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>	(Cameras().GetCamEffector(eCEShot)); 
	if (!effector) {
		effector					= 
			(CCameraShotEffector*)Cameras().AddCamEffector(
			xr_new<CCameraShotEffector>(weapon->camMaxAngle,
			weapon->camRelaxSpeed,
			weapon->camMaxAngleHorz,
			weapon->camStepAngleHorz,
			weapon->camDispertionFrac)	);
	}
	R_ASSERT						(effector);

	if (pWM)
	{
		if (effector->IsSingleShot())
			update_camera(effector);

		if (pWM->GetCurrentFireMode() == 1)
		{
			effector->SetSingleShoot(TRUE);
		}
		else
		{
			effector->SetSingleShoot(FALSE);
		}
	};

	effector->SetRndSeed			(GetShotRndSeed());
	effector->SetActor				(this);
	effector->Shot					(weapon->camDispersion + weapon->camDispersionInc*float(weapon->ShotsFired()));

	if (pWM)
	{
		if (pWM->GetCurrentFireMode() != 1)
		{
			effector->SetActive(FALSE);
			update_camera(effector);
		}		
	}
}

void CActor::on_weapon_shot_stop		(CWeapon *weapon)
{
	//---------------------------------------------
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot)); 
	if (effector && effector->IsActive())
	{
		if (effector->IsSingleShot())
			update_camera(effector);
	}
	//---------------------------------------------
	Cameras().RemoveCamEffector(eCEShot);
}

void CActor::on_weapon_hide				(CWeapon *weapon)
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot)); 
	if (effector && !effector->IsActive())
		effector->Clear				();
}

Fvector CActor::weapon_recoil_delta_angle	()
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	Fvector							result = {0.f,0.f,0.f};

	if (effector)
		effector->GetDeltaAngle		(result);

	return							(result);
}

Fvector CActor::weapon_recoil_last_delta()
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	Fvector							result = {0.f,0.f,0.f};

	if (effector)
		effector->GetLastDelta		(result);

	return							(result);
}
//////////////////////////////////////////////////////////////////////////

void	CActor::SpawnAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (OnClient()) return;
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	///	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().GetAny( *(pWM->m_ammoTypes[0]) ));
	//	if (!pAmmo) 
	pWM->SpawnAmmo(0xffffffff, NULL, ID());
};

void	CActor::RemoveAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (OnClient()) return;
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().GetAny(*(pWM->m_ammoTypes[0]) ));
	if (!pAmmo) return;
	//--- �� ����� ������� � �������� ������	
	/*
	//--- ��������� �� �������� �� ��� � ����-�� ���
	bool CanRemove = true;
	TIItemContainer::const_iterator I = inventory().m_all.begin();//, B = I;
	TIItemContainer::const_iterator E = inventory().m_all.end();
	for ( ; I != E; ++I)
	{
	CInventoryItem* pItem = (*I);//->m_pIItem;
	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) continue;
	if (pWM == pIItem) continue;
	if (pWM->m_ammoTypes[0] != pAmmo->CInventoryItem::object().cNameSect()) continue;
	CanRemove = false;
	break;
	};

	if (!CanRemove) return;
	*/
	pAmmo->DestroyObject();
	//	NET_Packet			P;
	//	u_EventGen			(P,GE_DESTROY,pAmmo->ID());
	//	u_EventSend			(P);
};

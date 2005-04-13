// Actor_Weapon.cpp:	 ��� ������ � �������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "inventory.h"
#include "weapon.h"



static const float VEL_MAX		= 10.f;
static const float VEL_A_MAX	= 10.f;

//���������� ������� ������� �������� (� ��������)� ������ ��������
float CActor::GetWeaponAccuracy() const
{
	CWeapon* W	= smart_cast<CWeapon*>(inventory().ActiveItem());

	if(m_bZoomAimingMode&&W&&!W->IsRotatingToZoom())
		return 0.f;

	float dispersion = m_fDispBase;

	CEntity::SEntityState state;
	if (g_State(state))
	{
		// angular factor
		dispersion *= (1.f + (state.fAVelocity/VEL_A_MAX)*m_fDispVelFactor);
//		Msg("--- base=[%f] angular disp=[%f]",m_fDispBase, dispersion);
		// linear movement factor
		bool bAccelerated = isActorAccelerated(mstate_real, IsZoomAimingMode());
		if( bAccelerated )
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*
			m_fDispVelFactor*(1.f + m_fDispAccelFactor));
		else
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*m_fDispVelFactor);

		if (state.bCrouch){	
			dispersion *= (1.f + m_fDispCrouchFactor);

			if(!bAccelerated )
				dispersion *= (1.f + m_fDispCrouchNoAccelFactor);
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
	state.fVelocity		= m_PhysicMovementControl->GetVelocityActual();
	state.fAVelocity	= fCurAVelocity;
	return TRUE;
}

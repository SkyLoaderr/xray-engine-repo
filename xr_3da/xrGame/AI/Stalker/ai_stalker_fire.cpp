////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../WeaponMagazined.h"

#define TIME_FIRE_DELTA		400

float CAI_Stalker::HitScale	(int element)
{
	CKinematics* V		= PKinematics(Visual());			VERIFY(V);
	float scale			= fis_zero(V->LL_GetBoneInstance(u16(element)).get_param(0))?1.f:V->LL_GetBoneInstance(u16(element)).get_param(0);
	return				(m_fHitFactor*scale);
}

void CAI_Stalker::g_fireParams(Fvector& P, Fvector& D)
{
	if (g_Alive()) {
		Center(P);
		D.setHP(-m_head.current.yaw,-m_head.current.pitch);
		D.normalize_safe();
	}
}

void CAI_Stalker::g_WeaponBones	(int &L, int &R1, int &R2)
{
	CKinematics *V	= PKinematics(Visual());
	R1				= V->LL_BoneID("bip01_r_hand");
	R2				= V->LL_BoneID("bip01_r_finger2");
	if (IsLimping() && (m_tMentalState == eMentalStateFree))
		L				= R2;
	else
		L				= V->LL_BoneID("bip01_l_finger1");
}

bool CAI_Stalker::bfCheckIfCanKillTarget(CEntity *tpEntity, Fvector target_pos, float yaw2, float pitch2, float fSafetyAngle) 
{
	Fvector fire_pos = tpEntity->Position();
	fire_pos.sub(target_pos,fire_pos);
	float yaw1,pitch1;
	fire_pos.getHP(yaw1,pitch1);
	yaw1 = angle_normalize_signed(yaw1);
	pitch1 = angle_normalize_signed(pitch1);
	yaw2 = angle_normalize_signed(yaw2);
	pitch2 = angle_normalize_signed(pitch2);
	
	CInventoryOwner *tpInventoryOwner = dynamic_cast<CInventoryOwner*>(tpEntity);
	if (tpInventoryOwner) {
		if (!tpInventoryOwner->m_inventory.ActiveItem() || !dynamic_cast<CWeapon*>(tpInventoryOwner->m_inventory.ActiveItem()) || !dynamic_cast<CWeapon*>(tpInventoryOwner->m_inventory.ActiveItem())->GetAmmoElapsed())
			return(false);
		if (!((angle_difference(yaw1,yaw2) <= fSafetyAngle) && (angle_difference(pitch1,pitch2) <= fSafetyAngle)))
			return(false);

#pragma todo("Dima to Dima : Recover check if can kill target function")
//		setEnabled			(false);
//		Collide::ray_query	l_tRayQuery;
//		l_tRayQuery.O		=	NULL;
//		Fvector				l_tFireDirection;
//		l_tFireDirection.setHP(yaw1,pitch1);
//		g_pGameLevel->ObjectSpace.RayPick(tpEntity->Position(), l_tFireDirection, fire_pos.distance_to(target_pos) + .5f, l_tRayQuery);
//		setEnabled			(true);
//		return				(l_tRayQuery.O && (l_tRayQuery.O->Position().distance_to(target_pos) < EPS_L));
		return				(true);
	}
	else
		return((tpEntity->Position().distance_to(target_pos) < 2.f) && (angle_difference(yaw1,yaw2) <= PI_DIV_2));
}

bool CAI_Stalker::bfCheckIfCanKillEnemy()
{
	return(m_tEnemy.m_enemy && m_tEnemy.m_enemy->g_Alive() && m_tEnemy.m_visible && bfCheckIfCanKillTarget(this,m_tEnemy.m_enemy->Position(),-m_head.current.yaw,-m_head.current.pitch));
}

bool CAI_Stalker::bfCheckIfCanKillMember()
{
	bool bCanKillMember = false;
	
	for (int i=0, iTeam = (int)g_Team(); i<(int)m_tpaVisibleObjects.size(); ++i) {
		CCustomMonster* CustomMonster = dynamic_cast<CCustomMonster*>(m_tpaVisibleObjects[i]);
		if ((CustomMonster) && (CustomMonster->g_Team() == iTeam))
			if (CustomMonster->g_Alive() && bfCheckIfCanKillTarget(this,CustomMonster->Position(),-m_head.current.yaw,-m_head.current.pitch)) {
				bCanKillMember = true;
				break;
			}
	}
	return(bCanKillMember);
}

void CAI_Stalker::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	Fvector D;
	XFORM().transform_dir(D,vLocalDir);
		
	if (g_Alive()) {
//	if (fEntityHealth>0) {
		// Save event
		m_hit_time = Level().timeServer();
		m_hit_direction.set(D);
		m_hit_direction.normalize();
		m_tHitPosition = who->Position();
		
		SHurt	tHurt;
		tHurt.dwTime	= Level().timeServer();
		if (0 != (tHurt.tpEntity = dynamic_cast<CEntity*>(who)))
			vfUpdateHurt(tHurt);

		feel_sound_new(who,SOUND_TYPE_WEAPON_SHOOTING,who->Position(),1.f);

		// Play hit-ref_sound
		ref_sound& S = m_tpSoundHit[::Random.randI((int)m_tpSoundHit.size())];
		if (!S.feedback && g_Alive()) {
			S.play_at_pos(this,eye_matrix.c);
			S.feedback->set_volume(1.f);
		}
		float	yaw, pitch;
		D.getHP(yaw,pitch);
		CSkeletonAnimated *tpKinematics = PSkeletonAnimated(Visual());
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		CMotionDef *tpMotionDef = m_tAnims.A[m_tBodyState].m_tGlobal.A[0].A[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(m_body.current.yaw,-yaw) <= PI_DIV_2 ? 0 : 1))];
		float power_factor = 3.f*amount/100.f; clamp(power_factor,0.f,1.f);
		tpKinematics->PlayFX(tpMotionDef,power_factor);
	}
}

float CAI_Stalker::EnemyHeuristics(CEntity* E)
{
	if (E->g_Team()  == g_Team())	
		return flt_max;		// don't attack our team
	
	if (!E->g_Alive())
		return flt_max;		// don't attack dead enemies
	
	float	g_strength = E->g_Armor()+E->g_Health();
	
	float	f1	= Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strength);
	return  f1*f2;
}

void CAI_Stalker::SelectEnemy(SEnemySelected& S)
{
	// Initiate process
	objVisible&	Known	= Level().Teams[g_Team()].Squads[g_Squad()].KnownEnemys;
	S.m_enemy			= 0;
	S.m_visible			= FALSE;
	S.m_cost			= flt_max-1;
	
	if (Known.size()==0)
		return;
	// Get visible list
	feel_vision_get	(m_tpaVisibleObjects);
	std::sort		(m_tpaVisibleObjects.begin(),m_tpaVisibleObjects.end());
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	
	for (u32 i=0; i<Known.size(); ++i) {
		CEntityAlive*	E = dynamic_cast<CEntityAlive*>(Known[i].key);
		if (!E || !E->g_Alive() || E->getDestroy())
			continue;

		float		H = EnemyHeuristics(E);
		if (H<S.m_cost) {
			bool bVisible = false;
			for (int i=0; i<(int)m_tpaVisibleObjects.size(); ++i)
				if (m_tpaVisibleObjects[i] == E) {
					bVisible = true;
					break;
				}
			float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
			if (cost<S.m_cost)	{
				S.m_enemy		= E;
				S.m_visible		= bVisible;
				S.m_cost		= cost;
				Group.m_bEnemyNoticed = true;
			}
		}
	}
	if (m_tEnemy.m_enemy)
		vfSaveEnemy();
	if (m_tSavedEnemy && m_tSavedEnemy->getDestroy())
		m_tSavedEnemy = 0;
}

bool CAI_Stalker::bfCheckForNodeVisibility(u32 dwNodeID, bool /**bIfRayPick/**/)
{
	Fvector tDirection;
	tDirection.sub(ai().level_graph().vertex_position(dwNodeID),Position());
	tDirection.normalize_safe();
	SRotation tRotation;
	mk_rotation(tDirection,tRotation);
	if (angle_difference(m_head.current.yaw,tRotation.yaw) <= eye_fov*PI/180.f/2.f)
		return(ai().level_graph().check_vertex_in_direction(level_vertex_id(),Position(),dwNodeID));
	else
		return(false);
}

void CAI_Stalker::vfSetWeaponState(EObjectAction tWeaponState)
{
	CWeapon *tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	
	if (!m_inventory.ActiveItem() || !tpWeapon)
		return;

	if (m_tMentalState == eMentalStatePanic) {
		m_inventory.Activate(u32(-1));
		return;
	}

	bool bSafeFire = m_bFiring;

	u32 dwStartFireAmmo, dwFireDelayMin = 0, dwFireDelayMax = 0;
	CWeaponMagazined *tpWeaponMagazined = dynamic_cast<CWeaponMagazined*>(tpWeapon);
	if (tpWeaponMagazined && m_tEnemy.m_enemy) {
		float fDistance = Position().distance_to(m_tEnemy.m_enemy->Position());
		float fDistance1 = tpWeaponMagazined->m_fMaxRadius, fDistance2 = (tpWeaponMagazined->m_fMaxRadius + 0*tpWeaponMagazined->m_fMinRadius)/2,fDistance3 = 0*tpWeaponMagazined->m_fMinRadius;
		u32 dwFuzzyDistance;
		if (_abs(fDistance - fDistance1) <	_abs(fDistance - fDistance2))
			dwFuzzyDistance = 0;
		else
			if (_abs(fDistance - fDistance2) <	_abs(fDistance - fDistance3))
				dwFuzzyDistance = 1;
			else
				dwFuzzyDistance = 2;
		
		u32	dwMinQueueSize = 0, dwMaxQueueSize = 0;
		switch (m_tMovementType) {
			case eMovementTypeRun : {
				switch (dwFuzzyDistance) {
					case 0 : {
						dwMinQueueSize	= 1;
						dwMaxQueueSize	= 3;
						dwFireDelayMin	= 2000;
						dwFireDelayMax	= 4000;
						break;
					}
					case 1 : {
						dwMinQueueSize	= 2;
						dwMaxQueueSize	= 3;
						dwFireDelayMin	= 1000;
						dwFireDelayMax	= 2000;
						break;
					}
					case 2 : {
						dwMinQueueSize	= 3;
						dwMaxQueueSize	= 7;
						dwFireDelayMin	= 700;
						dwFireDelayMax	= 1500;
						break;
					}
					default : NODEFAULT;
				}
				break;
			}
			case eMovementTypeWalk : {
				switch (dwFuzzyDistance) {
					case 0 : {
						dwMinQueueSize	= 1;
						dwMaxQueueSize	= 3;
						dwFireDelayMin	= 1000;
						dwFireDelayMax	= 3000;
						break;
					}
					case 1 : {
						dwMinQueueSize	= 2;
						dwMaxQueueSize	= 4;
						dwFireDelayMin	= 900;
						dwFireDelayMax	= 2000;
						break;
					}
					case 2 : {
						dwMinQueueSize	= 5;
						dwMaxQueueSize	= 10;
						dwFireDelayMin	= 700;
						dwFireDelayMax	= 1200;
						break;
					}
					default : NODEFAULT;
				}
				break;
			}
			case eMovementTypeStand : {
				switch (dwFuzzyDistance) {
					case 0 : {
						dwMinQueueSize	= 1;
						dwMaxQueueSize	= 3;
						dwFireDelayMin	= 1500;
						dwFireDelayMax	= 2500;
						break;
					}
					case 1 : {
						dwMinQueueSize	= 3;
						dwMaxQueueSize	= 5;
						dwFireDelayMin	= 1000;
						dwFireDelayMax	= 1800;
						break;
					}
					case 2 : {
						dwMinQueueSize	= 10;
						dwMaxQueueSize	= 15;
						dwFireDelayMin	= 500;
						dwFireDelayMax	= 1000;
						break;
					}
					default : NODEFAULT;
				}
				break;
			}
			default : {
				m_inventory.Action(kWPN_FIRE, CMD_STOP);
				m_inventory.ActiveItem()->Deactivate();
				dwMaxQueueSize			= 0;
				break;
			}
		}
		if (dwMaxQueueSize)
			if (tpWeaponMagazined->GetAmmoMagSize() > 1)
				tpWeaponMagazined->SetQueueSize(::Random.randI(iFloor(float(dwMinQueueSize)/_min(30,tpWeaponMagazined->GetAmmoMagSize())*float(tpWeaponMagazined->GetAmmoMagSize())),iFloor(float(dwMaxQueueSize)/_min(30,tpWeaponMagazined->GetAmmoMagSize())*float(tpWeaponMagazined->GetAmmoMagSize()))));
			else
				tpWeaponMagazined->SetQueueSize(1);
	}

//	Msg	("Time : %d\nDESIRED : %s\nREAL : %s\nFIRING : %d",m_current_update,(tWeaponState == eObjectActionIdle) ? "IDLE" : "FIRE",(tpWeapon->STATE == CWeapon::eFire) ? "FIRING" : ((tpWeapon->STATE == CWeapon::eIdle) ? "IDLE" : ((tpWeapon->STATE == CWeapon::eReload) ? "RELOAD" : "UNKNOWN")),int(m_bFiring));
	if (tWeaponState == eObjectActionIdle) {
		m_bFiring = false;
		if (tpWeapon->STATE == CWeapon::eFire)
			m_inventory.Action(kWPN_FIRE, CMD_STOP);
		xr_vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin(), B = I;
		xr_vector<CInventorySlot>::iterator E = m_inventory.m_slots.end() - 2;
		s32 best_slot = -1;
		for ( ; I != E; ++I)
			if ((*I).m_pIItem && ((I - B) != (int)m_inventory.GetActiveSlot()) && (!dynamic_cast<CWeaponMagazined*>((*I).m_pIItem) || dynamic_cast<CWeaponMagazined*>((*I).m_pIItem)->IsAmmoAvailable()))
				best_slot = u32(I - B);
		if (best_slot > (int)m_inventory.GetActiveSlot())
			m_inventory.Activate(best_slot);
	}
	else
		if (tWeaponState == eObjectActionPrimaryFire)
			if (tpWeapon->STATE == CWeapon::eFire)
				if (bfCheckIfCanKillEnemy() && !bfCheckIfCanKillMember()) {
					u32 dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
					if (dwStartFireAmmo) {
						m_bFiring = true;
						m_dwNoFireTime = m_current_update + ::Random.randI(dwFireDelayMin,dwFireDelayMax);
					}
					else {
						m_inventory.Action(kWPN_FIRE, CMD_STOP);
						m_bFiring = false;
					}
				}
				else {
					m_inventory.Action(kWPN_FIRE, CMD_STOP);
					m_bFiring = false;
					m_dwNoFireTime = m_current_update;
				}
			else
				if (tpWeapon->STATE == CWeapon::eIdle)
					if (bfCheckIfCanKillEnemy() && !bfCheckIfCanKillMember()) {
						dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
						if (dwStartFireAmmo && (m_current_update > m_dwNoFireTime)) {
							if (m_bFiring && (m_current_update - m_dwStartFireTime > TIME_FIRE_DELTA)) {
								m_inventory.Action(kWPN_FIRE, CMD_START);
//								Msg				("Weapon could fire (%d)",m_current_update);
							}
							else {
								m_dwNoFireTime = m_current_update;
								m_inventory.Action(kWPN_FIRE, CMD_STOP);
							}
							if (!m_bFiring) {
								m_dwStartFireTime	= m_current_update;
//								Msg				("Weapon shouldn't fire (%d)",m_current_update);
							}
							m_bFiring = true;
						}
					}
					else
						m_bFiring = false;
				else
					m_bFiring = false;
		else {
			if (tpWeapon->STATE == CWeapon::eFire)
				m_inventory.Action(kWPN_FIRE,	CMD_STOP);
			m_bFiring = false;
		}
			
	if (tpWeaponMagazined) {
		dwStartFireAmmo = tpWeaponMagazined->GetAmmoElapsed();
		if ((!dwStartFireAmmo) && (CWeapon::eReload != tpWeapon->STATE))
			if (tpWeaponMagazined->IsAmmoAvailable()) {
				m_inventory.Action(kWPN_FIRE,	CMD_START);
				m_inventory.Action(kWPN_FIRE,	CMD_STOP);
			}
			else
				if ((CWeapon::eHidden != tpWeapon->STATE) && (CWeapon::eHiding != tpWeapon->STATE)) {
					if (tpWeaponMagazined->STATE == CWeapon::eFire)
						m_inventory.Action(kWPN_FIRE,	CMD_STOP);
					xr_vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin(), B = I;
					xr_vector<CInventorySlot>::iterator E = m_inventory.m_slots.end() - 2;
					u32 best_slot = u32(-1);
					for ( ; I != E; ++I)
						if ((*I).m_pIItem && ((I - B) != (int)m_inventory.GetActiveSlot()) && (!dynamic_cast<CWeaponMagazined*>((*I).m_pIItem) || dynamic_cast<CWeaponMagazined*>((*I).m_pIItem)->IsAmmoAvailable()))
							best_slot = u32(I - B);
					if (-1 != best_slot)														   
						m_inventory.Activate(best_slot);
//					else {
//						m_inventory.Ruck(tpWeaponMagazined);
//						tpWeaponMagazined->Drop();
//					}
				}
	}

	bool bFiring = (tpWeapon->STATE == CWeapon::eFire) || (tpWeapon->STATE == CWeapon::eFire2);

	CGroup &Group = *getGroup();
	if ((bSafeFire) && (!bFiring))
		--(Group.m_dwFiring);
	else
		if ((!bSafeFire) && (bFiring))
			++Group.m_dwFiring;
}
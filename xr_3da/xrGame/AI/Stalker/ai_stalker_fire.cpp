////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\..\\WeaponMagazined.h"

#define	FIRE_SAFETY_ANGLE				PI/10

void CAI_Stalker::g_fireParams(Fvector& P, Fvector& D)
{
	if (g_Alive()) {
		clCenter(P);
		D.setHP(-r_current.yaw,-r_current.pitch);
		D.normalize_safe();
	}
}

void CAI_Stalker::g_WeaponBones	(int &L, int &R1, int &R2)
{
	CKinematics *V	= PKinematics(Visual());
	R1				= V->LL_BoneID("bip01_r_hand");
	R2				= V->LL_BoneID("bip01_r_finger2");
	L				= V->LL_BoneID("bip01_l_finger1");
}

bool CAI_Stalker::bfCheckIfCanKillTarget(Fvector fire_pos, Fvector target_pos, float yaw2, float pitch2) 
{
	if (!m_inventory.ActiveItem() || !dynamic_cast<CWeapon*>(m_inventory.ActiveItem()) || !dynamic_cast<CWeapon*>(m_inventory.ActiveItem())->GetAmmoElapsed())
		return(false);

	fire_pos.sub(target_pos,fire_pos);
	float yaw1,pitch1;
	fire_pos.getHP(yaw1,pitch1);
	yaw1 = angle_normalize_signed(yaw1);
	pitch1 = angle_normalize_signed(pitch1);
	yaw2 = angle_normalize_signed(yaw2);
	pitch2 = angle_normalize_signed(pitch2);
	return(getAI().bfTooSmallAngle(yaw1,yaw2,FIRE_SAFETY_ANGLE) && getAI().bfTooSmallAngle(pitch1,pitch2,FIRE_SAFETY_ANGLE));
}

bool CAI_Stalker::bfCheckIfCanKillEnemy()
{
	return(m_tEnemy.Enemy && m_tEnemy.Enemy->g_Alive() && m_tEnemy.bVisible && bfCheckIfCanKillTarget(vPosition,m_tEnemy.Enemy->Position(),-r_current.yaw,-r_current.pitch));
}

bool CAI_Stalker::bfCheckIfCanKillMember()
{
	bool bCanKillMember = false;
	
	for (int i=0, iTeam = (int)g_Team(); i<(int)m_tpaVisibleObjects.size(); i++) {
		CCustomMonster* CustomMonster = dynamic_cast<CCustomMonster*>(m_tpaVisibleObjects[i]);
		if ((CustomMonster) && (CustomMonster->g_Team() == iTeam))
			if (CustomMonster->g_Alive() && bfCheckIfCanKillTarget(vPosition,CustomMonster->Position(),-r_current.yaw,-r_current.pitch)) {
				bCanKillMember = true;
				break;
			}
	}
	return(bCanKillMember);
}

void CAI_Stalker::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	m_dwHitTime = Level().timeServer();
	m_tHitDir.set(D);
	m_tHitDir.normalize();
	m_tHitPosition = who->Position();
	
	// Play hit-sound
//	sound& S				= m_tpaSoundHit[Random.randI(SND_HIT_COUNT)];
	
//	if (g_Health() > 0) {
//		if (S.feedback)
//			return;
//		if (Random.randI(2))
//			return;
//		::Sound->play_at_pos		(S,this,vPosition);
//	}
//	if (g_Health() - amount <= 0) {
//		if ((m_tpCurrentGlobalAnimation) && (!m_tpCurrentGlobalBlend->playing))
//			if (m_tpCurrentGlobalAnimation != m_tRatAnimations.tNormal.tGlobal.tpaDeath[0])
//				m_tpCurrentGlobalBlend = PKinematics(pVisual)->PlayCycle(m_tpCurrentGlobalAnimation = m_tRatAnimations.tNormal.tGlobal.tpaDeath[::Random.randI(0,2)]);
//	}
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
	S.Enemy					= 0;
	S.bVisible			= FALSE;
	S.fCost				= flt_max-1;
	
	if (Known.size()==0)
		return;
	// Get visible list
	feel_vision_get	(m_tpaVisibleObjects);
	std::sort		(m_tpaVisibleObjects.begin(),m_tpaVisibleObjects.end());
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	
	for (u32 i=0; i<Known.size(); i++) {
		CEntityAlive*	E = dynamic_cast<CEntityAlive*>(Known[i].key);
		if (!E || !E->g_Alive())
			continue;

		float		H = EnemyHeuristics(E);
		if (H<S.fCost) {
			bool bVisible = false;
			for (int i=0; i<(int)m_tpaVisibleObjects.size(); i++)
				if (m_tpaVisibleObjects[i] == E) {
					bVisible = true;
					break;
				}
			float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
			if (cost<S.fCost)	{
				S.Enemy		= E;
				S.bVisible	= bVisible;
				S.fCost		= cost;
				Group.m_bEnemyNoticed = true;
			}
		}
	}
	if (m_tEnemy.Enemy)
		vfSaveEnemy();
}

bool CAI_Stalker::bfCheckForNodeVisibility(u32 dwNodeID, bool bIfRayPick)
{
	Fvector tDirection;
	tDirection.sub(getAI().tfGetNodeCenter(dwNodeID),vPosition);
	tDirection.normalize_safe();
	SRotation tRotation;
	mk_rotation(tDirection,tRotation);
	if (getAI().bfTooSmallAngle(r_current.yaw,tRotation.yaw,eye_fov*PI/180.f/2.f))
		return(getAI().bfCheckNodeInDirection(AI_NodeID,vPosition,dwNodeID));
	else
		return(false);
}

void CAI_Stalker::vfSetWeaponState(EWeaponState tWeaponState)
{
	CWeapon *tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	
	if (!m_inventory.ActiveItem() || !tpWeapon)
		return;

	bool bSafeFire = m_bFiring;

	if (tWeaponState == eWeaponStateIdle) {
		if (tpWeapon->STATE == CWeapon::eFire)
			m_inventory.Action(kWPN_FIRE, CMD_STOP);
	}
	else
		if (tWeaponState == eWeaponStatePrimaryFire)
			if (tpWeapon->STATE == CWeapon::eFire)
				if (bfCheckIfCanKillEnemy() && !bfCheckIfCanKillMember()) {
					m_dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
					m_inventory.Action(kWPN_FIRE, CMD_STOP);
					if (m_dwStartFireAmmo) {
						m_inventory.Action(kWPN_FIRE, CMD_START);
						m_bFiring = true;
					}
				}
				else {
					m_inventory.Action(kWPN_FIRE, CMD_STOP);
					m_bFiring = false;
					m_dwNoFireTime = m_dwCurrentUpdate;
				}
			else
				if (tpWeapon->STATE == CWeapon::eIdle)
					if (bfCheckIfCanKillEnemy() && !bfCheckIfCanKillMember()) {
						m_dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
						if (m_dwStartFireAmmo) {
							m_inventory.Action(kWPN_FIRE, CMD_START);
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
			
	switch(tpWeapon->STATE) {
		case CWeapon::eIdle		: Msg("%s : idle",tpWeapon->cNameSect());
			break;
		case CWeapon::eFire		: Msg("%s : fire",tpWeapon->cNameSect());
			break;
		case CWeapon::eFire2	: Msg("%s : fire 2",tpWeapon->cNameSect());
			break;
		case CWeapon::eReload	: Msg("%s : recharge",tpWeapon->cNameSect());
			break;
		case CWeapon::eShowing	: Msg("%s : show",tpWeapon->cNameSect());
			break;
		case CWeapon::eHiding	: Msg("%s : hide",tpWeapon->cNameSect());
			break;
		case CWeapon::eHidden	: Msg("%s : hidden",tpWeapon->cNameSect());
			break;
	}
	
	CWeaponMagazined *tpWeaponMagazined = dynamic_cast<CWeaponMagazined*>(tpWeapon);
	if (tpWeaponMagazined) {
		m_dwStartFireAmmo = tpWeaponMagazined->GetAmmoElapsed();
		if ((!m_dwStartFireAmmo) && (tpWeapon->STATE != CWeapon::eReload))
			if (tpWeaponMagazined->IsAmmoAvailable()) {
				m_inventory.Action(kWPN_FIRE,	CMD_START);
				m_inventory.Action(kWPN_FIRE,	CMD_STOP);
			}
			else
				if ((tpWeapon->STATE != CWeapon::eHidden) && (tpWeapon->STATE != CWeapon::eHiding)) {
					if (tpWeaponMagazined->STATE == CWeapon::eFire)
						m_inventory.Action(kWPN_FIRE,	CMD_STOP);
					vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin(), B = I;
					vector<CInventorySlot>::iterator E = m_inventory.m_slots.end();
					u32 best_slot = -1;
					for ( ; I != E; I++)
						if ((*I).m_pIItem && ((I - B) != m_inventory.m_activeSlot) && (!dynamic_cast<CWeaponMagazined*>((*I).m_pIItem) || dynamic_cast<CWeaponMagazined*>((*I).m_pIItem)->IsAmmoAvailable()))
							best_slot = I - B;
					if (best_slot != -1)														   
						m_inventory.Activate(best_slot);
				}
	}

	m_bFiring = (tpWeapon->STATE == CWeapon::eFire) || (tpWeapon->STATE == CWeapon::eFire2);

	CGroup &Group = *getGroup();
	if ((bSafeFire) && (!m_bFiring))
		Group.m_dwFiring--;
	else
		if ((!bSafeFire) && (m_bFiring))
			Group.m_dwFiring++;
}
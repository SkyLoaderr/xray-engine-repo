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

void CAI_Stalker::g_WeaponBones	(int &L, int &R1, int &R2)
{
	CKinematics *V	= PKinematics(Visual());
	R1				= V->LL_BoneID("bip01_r_hand");
	R2				= V->LL_BoneID("bip01_r_finger2");
	L				= V->LL_BoneID("bip01_l_finger1");
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

bool CAI_Stalker::bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint) 
{
	Fvector tMemberDirection;
	tMemberDirection.sub(tMemberPoint,tMyPoint);
	vfNormalizeSafe(tMemberDirection);
	float fAlpha = tFireVector.dotproduct(tMemberDirection);
	clamp(fAlpha,-.99999f,+.99999f);
	fAlpha = acosf(fAlpha);
	return(fAlpha < FIRE_SAFETY_ANGLE);
}

bool CAI_Stalker::bfCheckIfCanKillMember()
{
	Fvector tFireVector, tMyPosition = Position();
	tFireVector.setHP	(-r_torso_current.yaw - m_fAddWeaponAngle,-r_torso_current.pitch);
	
	bool bCanKillMember = false;
	
	for (int i=0, iTeam = (int)g_Team()/**, iSquad = (int)g_Squad(), iGroup = (int)g_Group()/**/; i<(int)m_tpaVisibleObjects.size(); i++) {
		CCustomMonster* CustomMonster = dynamic_cast<CCustomMonster*>(m_tpaVisibleObjects[i]);
		//if ((CustomMonster) && (CustomMonster->g_Team() == iTeam) && (CustomMonster->g_Squad() == iSquad) && (CustomMonster->g_Group() == iGroup))
		if ((CustomMonster) && (CustomMonster->g_Team() == iTeam))
			if ((CustomMonster->g_Health() > 0) && (bfCheckForMember(tFireVector,tMyPosition,CustomMonster->Position()))) {
				bCanKillMember = true;
				break;
			}
	}
	return(bCanKillMember);
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
		m_inventory.Action(kWPN_FIRE, CMD_STOP);
		m_inventory.Action(kWPN_ZOOM, CMD_STOP);
	}
	else
		if (tWeaponState == eWeaponStatePrimaryFire) {
			if (tpWeapon->STATE == CWeapon::eFire)
				if ((int)m_dwStartFireAmmo - (int)tpWeapon->GetAmmoElapsed() > ::Random.randI(m_dwFireRandomMin,m_dwFireRandomMax + 1)) {
					m_inventory.Action(kWPN_FIRE, CMD_STOP);
					m_bFiring = false;
					m_dwNoFireTime = m_dwCurrentUpdate;
				}
				else {
					if (bfCheckIfCanKillEnemy())
						if (!bfCheckIfCanKillMember()) {
							m_dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
							m_inventory.Action(kWPN_FIRE, CMD_STOP);
							m_inventory.Action(kWPN_FIRE, CMD_START);
							m_bFiring = true;
						}
						else {
							m_inventory.Action(kWPN_FIRE, CMD_STOP);
							m_bFiring = false;
							m_dwNoFireTime = m_dwCurrentUpdate;
						}
					else {
						m_inventory.Action(kWPN_FIRE, CMD_STOP);
						m_bFiring = false;
						m_dwNoFireTime = m_dwCurrentUpdate;
					}
				}
			else {
				if (tpWeapon->STATE == CWeapon::eIdle) {
					if ((int)m_dwCurrentUpdate - (int)m_dwNoFireTime > ::Random.randI(m_dwNoFireTimeMin,m_dwNoFireTimeMax + 1))
						if (bfCheckIfCanKillEnemy())
							if (!bfCheckIfCanKillMember()) {
								m_dwStartFireAmmo = tpWeapon->GetAmmoElapsed();
								m_inventory.Action(kWPN_FIRE, CMD_STOP);
								m_inventory.Action(kWPN_FIRE, CMD_START);
								m_bFiring = true;
							}
							else
								m_bFiring = false;
						else
							m_bFiring = false;
					else
						m_bFiring = false;
				}
				else {
					if (tpWeapon->STATE == CWeapon::eFire)
						m_inventory.Action(kWPN_FIRE,	CMD_STOP);
					m_bFiring = false;
				}
			}
		}
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
				if (tpWeaponMagazined->STATE == CWeapon::eFire)
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
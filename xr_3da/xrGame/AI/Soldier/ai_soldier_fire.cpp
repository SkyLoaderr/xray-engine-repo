////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier_fire.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\actor.h"
#include "..\\..\\hudmanager.h"

#define	FIRE_SAFETY_ANGLE				PI/10
#define SPECIAL_SQUAD					6
#define LIGHT_FITTING			

bool CAI_Soldier::bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint) 
{
	Fvector tMemberDirection;
	tMemberDirection.sub(tMemberPoint,tMyPoint);
	vfNormalizeSafe(tMemberDirection);
	float fAlpha = tFireVector.dotproduct(tMemberDirection);
	clamp(fAlpha,-.99999f,+.99999f);
	fAlpha = acosf(fAlpha);
	return(fAlpha < FIRE_SAFETY_ANGLE);
}

bool CAI_Soldier::bfCheckIfCanKillEnemy() 
{
	return(true);
//	Fvector tMyLook;
//	tMyLook.setHP	(-r_torso_current.yaw - m_fAddWeaponAngle,-r_torso_current.pitch);
//	if (Enemy.Enemy) {
//		Fvector tFireVector, tMyPosition = Position(), tEnemyPosition = Enemy.Enemy->Position();
//		tFireVector.sub(tEnemyPosition,tMyPosition);
//		vfNormalizeSafe(tFireVector);
//		float fAlpha = tFireVector.dotproduct(tMyLook);
//		clamp(fAlpha,-.99999f,+.99999f);
//		fAlpha = acosf(fAlpha);
//		return(fAlpha < FIRE_SAFETY_ANGLE);
//	}
//	else
//		return(false);
}

bool CAI_Soldier::bfCheckIfCanKillMember()
{
	Fvector tFireVector, tMyPosition = Position();
	tFireVector.setHP	(-r_torso_current.yaw - m_fAddWeaponAngle,-r_torso_current.pitch);
	
	bool bCanKillMember = false;
	
	for (int i=0, iTeam = g_Team(), iSquad = g_Squad(), iGroup = g_Group(); i<tpaVisibleObjects.size(); i++) {
		CCustomMonster* CustomMonster = dynamic_cast<CCustomMonster*>(tpaVisibleObjects[i]);
		//if ((CustomMonster) && (CustomMonster->g_Team() == iTeam) && (CustomMonster->g_Squad() == iSquad) && (CustomMonster->g_Group() == iGroup))
		if ((CustomMonster) && (CustomMonster->g_Team() == iTeam))
			if ((CustomMonster->g_Health() > 0) && (bfCheckForMember(tFireVector,tMyPosition,CustomMonster->Position()))) {
				bCanKillMember = true;
				break;
			}
	}
	return(bCanKillMember);
}

void CAI_Soldier::g_fireParams(Fvector &fire_pos, Fvector &fire_dir)
{
	if (Weapons->ActiveWeapon()) {
		fire_pos.set(Weapons->ActiveWeapon()->Position());
		/**
		if ((tSavedEnemy) && (tSavedEnemy->g_Health() > 0)) {
			Fvector fTemp;
			fTemp.sub(tSavedEnemy->Position(),fire_pos);
			fTemp.normalize_safe();
			fire_dir.set(fTemp);
		}
		else
			fire_dir.set(eye_matrix.k);
		/**/
			//fire_dir.set(eye_matrix.k);
			//fire_dir.set(tWatchDirection);
		/**
		if (r_torso_current.yaw > PI)
			fire_dir.setHP(-r_torso_current.yaw + m_fAddWeaponAngle,-r_torso_current.pitch);
		else
			fire_dir.setHP(-r_torso_current.yaw,-r_torso_current.pitch);
		/**/
		fire_dir.setHP(-r_torso_current.yaw,-r_torso_current.pitch);
	}
}

float CAI_Soldier::EnemyHeuristics(CEntity* E)
{
	if (E->g_Team()  == g_Team())	
		return flt_max;		// don't attack our team
	
	int	g_strench = E->g_Armor()+E->g_Health();
	
	if (g_strench <= 0)					
		return flt_max;		// don't attack dead enemiyes
	
	float	f1	= Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strench);
	float   f3  = 1;
	if (E==Level().CurrentEntity())  f3 = .5f;
	return  f1*f2*f3;
}

// when someone hit soldier
void CAI_Soldier::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwHitTime = Level().timeServer();
	tHitDir.set(D);
	tHitDir.normalize();
	tHitPosition = who->Position();
	
	INIT_SQUAD_AND_LEADER;
	CGroup &Group = Squad.Groups[g_Group()];
	
	Group.m_dwLastHitTime = dwHitTime;
	Group.m_tLastHitDirection = tHitDir;
	Group.m_tHitPosition = tHitPosition;
	
	if (iHealth > 0) {
		if (dwfRandom(2))
			PKinematics(pVisual)->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageLeft);
		else
			PKinematics(pVisual)->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageRight);
		
		// Play hit-sound
		sound& S	= sndHit[Random.randI(SND_HIT_COUNT)];
		
		if (S.feedback)			
			return;
		pSounds->PlayAtPos	(S,this,vPosition);
	}
}

void CAI_Soldier::SelectEnemy(SEnemySelected& S)
{
	// Initiate process
	objVisible&	Known	= Level().Teams[g_Team()].KnownEnemys;
	S.Enemy					= 0;
	S.bVisible			= FALSE;
	S.fCost				= flt_max-1;
	
#ifdef LIGHT_FITTING
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// this code is dummy
	// only for fitting light coefficients
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	bool bActorInCamera = false;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif

	if (Known.size()==0) {
#ifdef LIGHT_FITTING
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// this code is dummy
		// only for fitting light coefficients
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (!bActorInCamera && (g_Squad() == SPECIAL_SQUAD))
			Level().HUD()->outMessage(0xffffffff,cName(),"I don't see you");
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif
		return;
	}
	
	// Get visible list
	ai_Track.o_get	(tpaVisibleObjects);
	std::sort		(tpaVisibleObjects.begin(),tpaVisibleObjects.end());
	
	INIT_SQUAD_AND_LEADER;
	CGroup &Group = Squad.Groups[g_Group()];
	
	// Iterate on known
	for (DWORD i=0; i<Known.size(); i++)
	{
		CEntity*	E = dynamic_cast<CEntity*>(Known[i].key);
		float		H = EnemyHeuristics(E);
		if (H<S.fCost) {
			if (!Group.m_bEnemyNoticed)
#ifdef LIGHT_FITTING
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// this code is dummy
				// only for fitting light coefficients
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (g_Squad() == SPECIAL_SQUAD) {
					bool bB = bfCheckForVisibility(E);
					CActor *tpActor = dynamic_cast<CActor *>(E);
					if (tpActor) {
						Level().HUD()->outMessage(0xffffffff,cName(),bB ? "I see you" : "I don't see you");
						bActorInCamera = true;
						continue;
					}
					else
						if (!bB)
							continue;
				}
				else
					if (!bfCheckForVisibility(E))
						continue;
#else
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// this code is correct
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				if (!bfCheckForVisibility(E))
					continue;
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif
				// Calculate local visibility
				CObject**	ins	 = lower_bound(tpaVisibleObjects.begin(),tpaVisibleObjects.end(),(CObject*)E);
				bool	bVisible = ((ins==tpaVisibleObjects.end())?FALSE:((E==*ins)?TRUE:FALSE)) && (bfCheckForVisibility(E));
				float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
				if (cost<S.fCost)	{
					S.Enemy		= E;
					S.bVisible	= bVisible;
					S.fCost		= cost;
					Group.m_bEnemyNoticed = true;
				}
		}
	}
#ifdef LIGHT_FITTING
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// this code is dummy
	// only for fitting light coefficients
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (!bActorInCamera && (g_Squad() == SPECIAL_SQUAD))
		Level().HUD()->outMessage(0xffffffff,cName(),"I don't see you");
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif
}

bool CAI_Soldier::bfCheckForDanger()
{
	DWORD dwCurTime = Level().timeServer();

	if (dwCurTime - dwHitTime < HIT_REACTION_TIME)
		return(true);
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	
	if (dwCurTime - Group.m_dwLastHitTime < HIT_REACTION_TIME)
		return(true);

	SelectSound(m_iSoundIndex);
	return(m_iSoundIndex > -1);
}

bool CAI_Soldier::bfCheckIfGroupFightType()
{
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];

	DWORD dwMemberCount = this == Leader ? 0 : 1;
	for (int i=0; i<Group.Members.size(); i++)
		if (Group.Members[i]->g_Health() > 0)
			dwMemberCount++;

	return(dwMemberCount > 0);
}

#define RAT_ENEMY				1.f
#define SOLDIER_ENEMY			20.f
#define ZOMBIE_ENEMY			3.f

#define M134_ENEMY_INACTIVE		100.f
#define FN2000_ENEMY_INACTIVE	30.f
#define AK74_ENEMY_INACTIVE		10.f
#define LR300_ENEMY_INACTIVE	20.f
#define HPSA_ENEMY_INACTIVE		3.f
#define PM_ENEMY_INACTIVE		4.f
#define FORT_ENEMY_INACTIVE		5.f

#define M134_ENEMY_ACTIVE		100.f
#define FN2000_ENEMY_ACTIVE		30.f
#define AK74_ENEMY_ACTIVE		10.f
#define LR300_ENEMY_ACTIVE		20.f
#define HPSA_ENEMY_ACTIVE		3.f
#define PM_ENEMY_ACTIVE			4.f
#define FORT_ENEMY_ACTIVE		5.f

#define RAT_FRIEND				-1.f
#define SOLDIER_FRIEND			-20.f
#define ZOMBIE_FRIEND			-3.f

#define M134_FRIEND_INACTIVE	-100.f
#define FN2000_FRIEND_INACTIVE	-30.f
#define AK74_FRIEND_INACTIVE	-10.f
#define LR300_FRIEND_INACTIVE	-20.f
#define HPSA_FRIEND_INACTIVE	-3.f
#define PM_FRIEND_INACTIVE		-4.f
#define FORT_FRIEND_INACTIVE	-5.f

#define M134_FRIEND_ACTIVE		-100.f
#define FN2000_FRIEND_ACTIVE	-30.f
#define AK74_FRIEND_ACTIVE		-10.f
#define LR300_FRIEND_ACTIVE		-20.f
#define HPSA_FRIEND_ACTIVE		-3.f
#define PM_FRIEND_ACTIVE		-4.f
#define FORT_FRIEND_ACTIVE		-5.f

DWORD CAI_Soldier::tfGetAloneFightType()
{
	objVisible &KnownEnemies = Level().Teams[g_Team()].KnownEnemys;
	float fFightCoefficient = 0.f, fTempCoefficient;
	CCustomMonster *tpCustomMonster = 0;

	for (int i=0; i<KnownEnemies.size(); i++) {
		tpCustomMonster = dynamic_cast<CCustomMonster *>(KnownEnemies[i].key);
		if (!tpCustomMonster)
			continue;
		switch (tpCustomMonster->SUB_CLS_ID) {
			case CLSID_AI_RAT		: {
				fFightCoefficient += RAT_ENEMY*tpCustomMonster->g_Health()/100.f;
				break;
			}
			case CLSID_AI_SOLDIER	: {
				fTempCoefficient = SOLDIER_ENEMY*tpCustomMonster->g_Health()/100.f;
				for (int j=0; j<tpCustomMonster->tpfGetWeapons()->WeaponCount(); j++) {
					CWeapon *tpWeapon = tpCustomMonster->tpfGetWeapons()->GetWeaponByIndex(j);
					int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
					int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
					switch (tpWeapon->SUB_CLS_ID) {
						case CLSID_OBJECT_W_M134		: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= M134_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= M134_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_M134_en		: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= M134_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= M134_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_FN2000		: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= FN2000_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= FN2000_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_AK74		: {	
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= AK74_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= AK74_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_LR300		: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= LR300_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= LR300_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_HPSA		: {	
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= HPSA_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= HPSA_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_PM			: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= PM_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= PM_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_FORT		: {	
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= FORT_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= FORT_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
					}
				}
			}
			case CLSID_AI_ZOMBIE	: {
				fFightCoefficient += ZOMBIE_ENEMY*tpCustomMonster->g_Health()/100.f;
				break;
			}
		}
	}

	INIT_SQUAD_AND_LEADER
	CGroup &Group = Squad.Groups[g_Group()];
	for ( i=0; i<Group.Members.size() + 1; i++) {
		
		if (i<Group.Members.size())
			tpCustomMonster = dynamic_cast<CCustomMonster*>(Group.Members[i]);
		else
			tpCustomMonster = dynamic_cast<CCustomMonster*>(Squad.Leader);
		
//		if ((tpCustomMonster) && (tpCustomMonster != this)) {
		if (tpCustomMonster) {
			switch (tpCustomMonster->SUB_CLS_ID) {
				case CLSID_AI_RAT		: {
					fFightCoefficient += RAT_FRIEND*tpCustomMonster->g_Health()/100.f;
					break;
				}
				case CLSID_AI_SOLDIER	: {
					fTempCoefficient = SOLDIER_FRIEND*tpCustomMonster->g_Health()/100.f;
					for (int j=0; j<tpCustomMonster->tpfGetWeapons()->WeaponCount(); j++) {
						CWeapon *tpWeapon = tpCustomMonster->tpfGetWeapons()->GetWeaponByIndex(j);
						int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
						int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
						switch (tpWeapon->SUB_CLS_ID) {
							case CLSID_OBJECT_W_M134		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= M134_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= M134_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_M134_en		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= M134_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= M134_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_FN2000		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FN2000_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= FN2000_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_AK74		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= AK74_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= AK74_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_LR300		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= LR300_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= LR300_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_HPSA		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= HPSA_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= HPSA_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_PM			: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= PM_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= PM_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_FORT		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FORT_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= FORT_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
						}
					}
				}
				case CLSID_AI_ZOMBIE	: {
					fFightCoefficient += ZOMBIE_FRIEND*tpCustomMonster->g_Health()/100.f;
					break;
				}
			}
			fFightCoefficient += fTempCoefficient;
		}
	}

	if (fFightCoefficient > 400)
		return(FIGHT_TYPE_RETREAT);
	else
		if (fFightCoefficient > 100)
			return(FIGHT_TYPE_DEFEND);
		else
			return(FIGHT_TYPE_ATTACK);
}

DWORD CAI_Soldier::tfGetGroupFightType()
{
	objVisible &KnownEnemies = Level().Teams[g_Team()].KnownEnemys;
	float fFightCoefficient = 0.f, fTempCoefficient;
	CCustomMonster *tpCustomMonster = 0;

	for (int i=0; i<KnownEnemies.size(); i++) {
		tpCustomMonster = dynamic_cast<CCustomMonster *>(KnownEnemies[i].key);
		if (!tpCustomMonster)
			continue;
		switch (tpCustomMonster->SUB_CLS_ID) {
			case CLSID_AI_RAT		: {
				fFightCoefficient += RAT_ENEMY*tpCustomMonster->g_Health()/100.f;
				break;
			}
			case CLSID_AI_SOLDIER	: {
				fTempCoefficient = SOLDIER_ENEMY*tpCustomMonster->g_Health()/100.f;
				for (int j=0; j<tpCustomMonster->tpfGetWeapons()->WeaponCount(); j++) {
					CWeapon *tpWeapon = tpCustomMonster->tpfGetWeapons()->GetWeaponByIndex(j);
					int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
					int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
					switch (tpWeapon->SUB_CLS_ID) {
						case CLSID_OBJECT_W_M134		: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= M134_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= M134_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_M134_en		: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= M134_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= M134_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_FN2000		: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= FN2000_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= FN2000_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_AK74		: {	
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= AK74_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= AK74_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_LR300		: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= LR300_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= LR300_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_HPSA		: {	
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= HPSA_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= HPSA_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_PM			: {
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= PM_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= PM_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
						case CLSID_OBJECT_W_FORT		: {	
							if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
								fTempCoefficient *= FORT_ENEMY_ACTIVE*iAmmoCurrent;
							else
								fTempCoefficient *= FORT_ENEMY_INACTIVE*iAmmoCurrent;
							break;
						}
					}
				}
			}
			case CLSID_AI_ZOMBIE	: {
				fFightCoefficient += ZOMBIE_ENEMY*tpCustomMonster->g_Health()/100.f;
				break;
			}
		}
	}

	INIT_SQUAD_AND_LEADER
	CGroup &Group = Squad.Groups[g_Group()];
	for ( i=0; i<Group.Members.size() + 1; i++) {
		
		if (i<Group.Members.size())
			tpCustomMonster = dynamic_cast<CCustomMonster*>(Group.Members[i]);
		else
			tpCustomMonster = dynamic_cast<CCustomMonster*>(Squad.Leader);
		
//		if ((tpCustomMonster) && (tpCustomMonster != this)) {
		if (tpCustomMonster) {
			switch (tpCustomMonster->SUB_CLS_ID) {
				case CLSID_AI_RAT		: {
					fFightCoefficient += RAT_FRIEND*tpCustomMonster->g_Health()/100.f;
					break;
				}
				case CLSID_AI_SOLDIER	: {
					fTempCoefficient = SOLDIER_FRIEND*tpCustomMonster->g_Health()/100.f;
					for (int j=0; j<tpCustomMonster->tpfGetWeapons()->WeaponCount(); j++) {
						CWeapon *tpWeapon = tpCustomMonster->tpfGetWeapons()->GetWeaponByIndex(j);
						int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
						int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
						switch (tpWeapon->SUB_CLS_ID) {
							case CLSID_OBJECT_W_M134		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= M134_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= M134_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_M134_en		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= M134_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= M134_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_FN2000		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FN2000_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= FN2000_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_AK74		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= AK74_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= AK74_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_LR300		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= LR300_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= LR300_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_HPSA		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= HPSA_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= HPSA_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_PM			: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= PM_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= PM_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
							case CLSID_OBJECT_W_FORT		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FORT_FRIEND_ACTIVE*iAmmoCurrent;
								else
									fTempCoefficient *= FORT_FRIEND_INACTIVE*iAmmoCurrent;
								break;
							}
						}
					}
				}
				case CLSID_AI_ZOMBIE	: {
					fFightCoefficient += ZOMBIE_FRIEND*tpCustomMonster->g_Health()/100.f;
					break;
				}
			}
			fFightCoefficient += fTempCoefficient;
		}
	}

	if (fFightCoefficient > 400)
		return(FIGHT_TYPE_RETREAT);
	else
		if (fFightCoefficient > 100)
			return(FIGHT_TYPE_DEFEND);
		else
			return(FIGHT_TYPE_ATTACK);
}

bool CAI_Soldier::bfSaveFromEnemy(CEntity *tpEntity)
{
	return(true);
}
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
	if (!Enemy.Enemy) {
		Enemy.Enemy = who;
		vfSaveEnemy();
	}
	
	if (iHealth > 0) {
		vfAddHurtToList(who);
		if (::Random.randI(0,2))
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
			if (!Group.m_bEnemyNoticed || !Group.Members.size())
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
	DWORD dwCurTime = m_dwCurrentUpdate;

	if (bfAmIHurt())
		return(true);
	
	CGroup *Group = getGroup();
	
	if (bfIsMemberHurt())
		return(true);

	SelectEnemy(Enemy);
	
	if (Enemy.Enemy)
		return(true);
	
	//SelectSound(m_iSoundIndex);
    
	return(m_iSoundIndex > -1);
}

DWORD CAI_Soldier::tfGetActionType()
{
	if (bfAmIDead())
		return(ACTION_TYPE_DIE);

	SelectEnemy(Enemy);

	if (!Enemy.Enemy && !tSavedEnemy)
		return(ACTION_TYPE_NONE);
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];

	DWORD dwMemberCount = this == Leader ? 0 : 1;
	for (int i=0; i<Group.Members.size(); i++)
		if (Group.Members[i]->g_Health() > 0)
			dwMemberCount++;

//	if (dwMemberCount > 0)
//		return(ACTION_TYPE_GROUP);
//	else
		return(ACTION_TYPE_ALONE);
}

#define RAT_ENEMY				1.f
#define SOLDIER_ENEMY			20.f
#define ZOMBIE_ENEMY			3.f
#define STALKER_ENEMY			25.f

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
#define STALKER_FRIEND			25.f

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
	SelectEnemy(Enemy);
	
	if (bfDoesEnemyExist())
		vfSaveEnemy();

	if (bfAmIHurt())
		return(FIGHT_TYPE_HURT);

	objVisible &KnownEnemies = Level().Teams[g_Team()].KnownEnemys;
	float fFightCoefficient = 0.f, fTempCoefficient;
	CCustomMonster *tpCustomMonster = 0;

	if (Enemy.Enemy) {
		CEntity *tpEntity = dynamic_cast<CEntity *>(KnownEnemies[0].key);
		if ((tpEntity) && (!bfCheckForEntityVisibility(tpEntity)) && !bfNeedRecharge()) 
			if (!bfCheckHistoryForState(aiSoldierAttackAloneFireFire,100000))
				return(FIGHT_TYPE_ATTACK);
	}

	if (!Enemy.Enemy)
		if (tSavedEnemy)
			if (bfCheckHistoryForState(aiSoldierRetreatAloneNonFire,10000) || bfCheckHistoryForState(aiSoldierRetreatAloneFire,10000))
				return(FIGHT_TYPE_RETREAT);
			else			
				return(FIGHT_TYPE_FIND);
		else
			return(FIGHT_TYPE_NONE);

	for (int i=0; i<KnownEnemies.size(); i++) {
		tpCustomMonster = dynamic_cast<CCustomMonster *>(KnownEnemies[i].key);
		if (tpCustomMonster) {
			switch (tpCustomMonster->SUB_CLS_ID) {
				case CLSID_AI_RAT		: {
					fFightCoefficient += RAT_ENEMY*tpCustomMonster->g_Health()/100.f;
					break;
				}
				case CLSID_AI_SOLDIER	: {
					fTempCoefficient = SOLDIER_ENEMY*tpCustomMonster->g_Health()/100.f;
					for (int j=0; j<tpCustomMonster->tpfGetWeapons()->WeaponCount(); j++) {
						CWeapon *tpWeapon = tpCustomMonster->tpfGetWeapons()->getWeaponByIndex(j);
						int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
						int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
						switch (tpWeapon->SUB_CLS_ID) {
							case CLSID_OBJECT_W_M134		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= M134_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= M134_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_FN2000		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FN2000_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= FN2000_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_AK74		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= AK74_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= AK74_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_LR300		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= LR300_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= LR300_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_HPSA		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= HPSA_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= HPSA_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_PM			: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= PM_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= PM_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_FORT		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FORT_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= FORT_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
						}
					}
					break;
				}
				case CLSID_AI_ZOMBIE	: {
					fFightCoefficient += ZOMBIE_ENEMY*tpCustomMonster->g_Health()/100.f;
					break;
				}
			}
		}
		else {
			CActor *tpActor = dynamic_cast<CActor *>(KnownEnemies[i].key);
			if (!tpActor)
				continue;
			
			fTempCoefficient = STALKER_ENEMY*tpActor->g_Health()/100.f;
			for (int j=0; j<tpActor->tpfGetWeapons()->WeaponCount(); j++) {
				CWeapon *tpWeapon = tpActor->tpfGetWeapons()->getWeaponByIndex(j);
				int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
				int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
				switch (tpWeapon->SUB_CLS_ID) {
					case CLSID_OBJECT_W_M134		: {
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= M134_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= M134_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_FN2000		: {
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= FN2000_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= FN2000_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_AK74		: {	
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= AK74_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= AK74_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_LR300		: {
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= LR300_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= LR300_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_HPSA		: {	
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= HPSA_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= HPSA_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_PM			: {
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= PM_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= PM_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_FORT		: {	
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= FORT_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= FORT_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
				}
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
						CWeapon *tpWeapon = tpCustomMonster->tpfGetWeapons()->getWeaponByIndex(j);
						int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
						int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
						switch (tpWeapon->SUB_CLS_ID) {
							case CLSID_OBJECT_W_M134		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= M134_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= M134_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_FN2000		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FN2000_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= FN2000_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_AK74		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= AK74_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= AK74_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_LR300		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= LR300_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= LR300_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_HPSA		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= HPSA_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= HPSA_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_PM			: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= PM_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= PM_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_FORT		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FORT_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= FORT_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
						}
					}
					break;
				}
				case CLSID_AI_ZOMBIE	: {
					fFightCoefficient += ZOMBIE_FRIEND*tpCustomMonster->g_Health()/100.f;
					break;
				}
			}
			fFightCoefficient += fTempCoefficient;
		}
	}

	return(FIGHT_TYPE_DEFEND);
//	if (fFightCoefficient > 400)
//		return(FIGHT_TYPE_RETREAT);
//	else
//		if (fFightCoefficient > 100)
//			return(FIGHT_TYPE_DEFEND);
//		else
//			return(FIGHT_TYPE_ATTACK);
}

DWORD CAI_Soldier::tfGetGroupFightType()
{
	objVisible &KnownEnemies = Level().Teams[g_Team()].KnownEnemys;
	float fFightCoefficient = 0.f, fTempCoefficient;
	CCustomMonster *tpCustomMonster = 0;

	for (int i=0; i<KnownEnemies.size(); i++) {
		tpCustomMonster = dynamic_cast<CCustomMonster *>(KnownEnemies[i].key);
		if (tpCustomMonster) {
			switch (tpCustomMonster->SUB_CLS_ID) {
				case CLSID_AI_RAT		: {
					fFightCoefficient += RAT_ENEMY*tpCustomMonster->g_Health()/100.f;
					break;
				}
				case CLSID_AI_SOLDIER	: {
					fTempCoefficient = SOLDIER_ENEMY*tpCustomMonster->g_Health()/100.f;
					for (int j=0; j<tpCustomMonster->tpfGetWeapons()->WeaponCount(); j++) {
						CWeapon *tpWeapon = tpCustomMonster->tpfGetWeapons()->getWeaponByIndex(j);
						int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
						int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
						switch (tpWeapon->SUB_CLS_ID) {
							case CLSID_OBJECT_W_M134		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= M134_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= M134_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_FN2000		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FN2000_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= FN2000_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_AK74		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= AK74_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= AK74_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_LR300		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= LR300_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= LR300_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_HPSA		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= HPSA_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= HPSA_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_PM			: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= PM_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= PM_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_FORT		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FORT_ENEMY_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= FORT_ENEMY_INACTIVE*iAmmoElapsed;
								break;
							}
						}
					}
					break;
				}
				case CLSID_AI_ZOMBIE	: {
					fFightCoefficient += ZOMBIE_ENEMY*tpCustomMonster->g_Health()/100.f;
					break;
				}
			}
		}
		else {
			CActor *tpActor = dynamic_cast<CActor *>(KnownEnemies[i].key);
			if (!tpActor)
				continue;
			
			fTempCoefficient = STALKER_ENEMY*tpActor->g_Health()/100.f;
			for (int j=0; j<tpActor->tpfGetWeapons()->WeaponCount(); j++) {
				CWeapon *tpWeapon = tpActor->tpfGetWeapons()->getWeaponByIndex(j);
				int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
				int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
				switch (tpWeapon->SUB_CLS_ID) {
					case CLSID_OBJECT_W_M134		: {
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= M134_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= M134_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_FN2000		: {
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= FN2000_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= FN2000_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_AK74		: {	
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= AK74_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= AK74_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_LR300		: {
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= LR300_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= LR300_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_HPSA		: {	
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= HPSA_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= HPSA_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_PM			: {
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= PM_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= PM_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
					case CLSID_OBJECT_W_FORT		: {	
						if (j == tpActor->tpfGetWeapons()->ActiveWeaponID())
							fTempCoefficient *= FORT_ENEMY_ACTIVE*iAmmoElapsed;
						else
							fTempCoefficient *= FORT_ENEMY_INACTIVE*iAmmoElapsed;
						break;
					}
				}
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
						CWeapon *tpWeapon = tpCustomMonster->tpfGetWeapons()->getWeaponByIndex(j);
						int iAmmoCurrent = tpWeapon->GetAmmoCurrent();
						int iAmmoElapsed = tpWeapon->GetAmmoElapsed();
						switch (tpWeapon->SUB_CLS_ID) {
							case CLSID_OBJECT_W_M134		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= M134_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= M134_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_FN2000		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FN2000_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= FN2000_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_AK74		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= AK74_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= AK74_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_LR300		: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= LR300_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= LR300_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_HPSA		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= HPSA_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= HPSA_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_PM			: {
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= PM_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= PM_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
							case CLSID_OBJECT_W_FORT		: {	
								if (j == tpCustomMonster->tpfGetWeapons()->ActiveWeaponID())
									fTempCoefficient *= FORT_FRIEND_ACTIVE*iAmmoElapsed;
								else
									fTempCoefficient *= FORT_FRIEND_INACTIVE*iAmmoElapsed;
								break;
							}
						}
					}
					break;
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
	float fSquare = ffCalcSquare(r_torso_current.yaw,eye_fov/180.f*PI,AI_Node);
	return(fSquare < .1f);
}

bool CAI_Soldier::bfCheckForDangerPlace()
{
	if ((AI_Path.TravelPath.empty()) || (AI_Path.TravelPath.size() - 1 <= AI_Path.TravelStart))
		return(false);
	NodeCompressed *tpCurrentNode = AI_Node;
	NodeCompressed *tpNextNode = 0;
	
	bool bOk = false;
	NodeLink *taLinks = (NodeLink *)((BYTE *)AI_Node + sizeof(NodeCompressed));
	int iCount = AI_Node->links;
	for (int i=0; i<iCount; i++) {
		tpNextNode = Level().AI.Node(Level().AI.UnpackLink(taLinks[i]));
 		if (Level().AI.bfInsideNode(tpNextNode,AI_Path.TravelPath[AI_Path.TravelStart + 1].P,.35f)) {
			bOk = true;
			break;
		}
	}
	if (!bOk)
		return(false);
 	
	float fAngleOfView = eye_fov*PI/180.f;
	float fMaxSquare = .20f, fBestAngle = -1.f;
	for (float fIncrement = 0; fIncrement < PI_MUL_2; fIncrement += PI/40.f) {
		if (ffGetCoverInDirection(fIncrement,tpCurrentNode) > .5f)
			continue;
		float fSquare0 = ffCalcSquare(fIncrement,fAngleOfView,tpCurrentNode);
		float fSquare1 = ffCalcSquare(fIncrement,fAngleOfView,tpNextNode);
		if (fSquare1 - fSquare0 > fMaxSquare) {
			fMaxSquare = fSquare1 - fSquare0;
			fBestAngle = fIncrement;
		}
	}
	
	if (fBestAngle >= -EPS_L) {
		r_torso_target.yaw = r_target.yaw = fBestAngle;
		return(true);
	}
	else
		return(false);
}

bool CAI_Soldier::bfSetLookToDangerPlace()
{
	if ((AI_Path.TravelPath.empty()) || (AI_Path.TravelPath.size() - 1 <= AI_Path.TravelStart))
		return(false);
	NodeCompressed *tpCurrentNode = AI_Node;
	NodeCompressed *tpNextNode = 0;
	
	bool bOk = false;
	NodeLink *taLinks = (NodeLink *)((BYTE *)AI_Node + sizeof(NodeCompressed));
	int iCount = AI_Node->links;
	for (int i=0; i<iCount; i++) {
		tpNextNode = Level().AI.Node(Level().AI.UnpackLink(taLinks[i]));
 		if (Level().AI.bfInsideNode(tpNextNode,AI_Path.TravelPath[AI_Path.TravelStart + 1].P,.35f)) {
			bOk = true;
			break;
		}
	}
	if (!bOk)
		return(false);
 	
	float fAngleOfView = eye_fov*PI/180.f;
	float fMaxSquare = .0f, fBestAngle = -1.f;
	for (float fIncrement = r_torso_current.yaw - MAX_HEAD_TURN_ANGLE; fIncrement <= r_torso_current.yaw + MAX_HEAD_TURN_ANGLE; fIncrement += 2*MAX_HEAD_TURN_ANGLE/60) {
		float fSquare0 = ffCalcSquare(fIncrement,fAngleOfView,tpCurrentNode);
		float fSquare1 = ffCalcSquare(fIncrement,fAngleOfView,tpNextNode);
		if (fSquare1 - fSquare0 > fMaxSquare) {
			fMaxSquare = fSquare1 - fSquare0;
			fBestAngle = fIncrement;
		}
	}
	
	if (fBestAngle >= -EPS_L) {
		r_target.yaw = fBestAngle;
		return(true);
	}
	else
		return(false);
}

float CAI_Soldier::ffGetCoverFromNode(CAI_Space &AI, Fvector &tPosition, NodeCompressed *tNode, float fEyeFov)
{
	Fvector tP0 = AI.tfGetNodeCenter(tNode),tP1;
	tP1.sub(tPosition,tP0);
	tP1.normalize();
	SRotation tRotation;
	mk_rotation(tP1,tRotation);
	return(ffCalcSquare(tRotation.yaw,fEyeFov,tNode));
}

void CAI_Soldier::vfFindAllSuspiciousNodes(DWORD StartNode, Fvector tPointPosition, const Fvector& BasePos, float Range, CGroup &Group)
{
	Device.Statistic.AI_Range.Begin	();

	Group.m_tpaSuspiciousNodes.clear();
	
	BOOL bStop = FALSE;
	
	CAI_Space &AI = Level().AI;
	NodePosition QueryPos;
	AI.PackPosition(QueryPos,BasePos);

	AI.q_stack.clear();
	AI.q_stack.push_back(StartNode);
	AI.q_mark_bit[StartNode] = true;
	
	NodeCompressed*	Base = AI.Node(StartNode);
	
	float range_sqr		= Range*Range;
	float fEyeFov;
	CEntityAlive *tpEnemy = dynamic_cast<CEntityAlive*>(tSavedEnemy);
	if (tpEnemy)
		fEyeFov = tpEnemy->ffGetFov();
	else
		fEyeFov = ffGetFov();
	fEyeFov *=PI/180.f;

	/**
	Fvector tMyDirection;
	SRotation tMyRotation,tEnemyRotation;
	tMyDirection.sub(tSavedEnemyPosition,vPosition);
	mk_rotation(tMyDirection,tMyRotation);
	tMyDirection.sub(tSavedEnemy->Position(),vPosition);
	mk_rotation(tMyDirection,tEnemyRotation);
	if (tEnemyRotation.yaw > tMyRotation.yaw) {
		if (tEnemyRotation.yaw - tMyRotation.yaw > PI)
			tMyRotation.yaw += PI_MUL_2;
	}
	else
		if (tMyRotation.yaw > tEnemyRotation.yaw)
			if (tMyRotation.yaw - tEnemyRotation.yaw > PI)
				tEnemyRotation.yaw += PI_MUL_2;
    bool bRotation = tMyRotation.yaw < tEnemyRotation.yaw;
	/**/
	INIT_SQUAD_AND_LEADER;
	vfMarkVisibleNodes(Leader);
	for (int i=0; i<getGroup()->Members.size(); i++)
		vfMarkVisibleNodes(getGroup()->Members[i]);

	Msg("Nodes being checked for suspicious :");
	for (DWORD it=0; it<AI.q_stack.size(); it++) {
		DWORD ID = AI.q_stack[it];

 		//if (bfCheckForVisibility(ID,tMyRotation,bRotation) && (ID != StartNode))
		//	continue;

		NodeCompressed*	N = AI.Node(ID);
		DWORD L_count	= DWORD(N->links);
		NodeLink* L_it	= (NodeLink*)(LPBYTE(N)+sizeof(NodeCompressed));
		NodeLink* L_end	= L_it+L_count;
		for( ; L_it!=L_end; L_it++) {
			if (bStop)			
				break;
			// test node
			DWORD Test = AI.UnpackLink(*L_it);
			if (AI.q_mark_bit[Test])
				continue;

			NodeCompressed*	T = AI.Node(Test);

			float distance_sqr = AI.u_SqrDistance2Node(BasePos,T);
			if (distance_sqr>range_sqr)	
				continue;

			// register
			AI.q_mark_bit[Test]		= true;
			AI.q_stack.push_back	(Test);

			// estimate
			SSearchPlace tSearchPlace;

			Fvector tDirection, tNodePosition = AI.tfGetNodeCenter(T);
			tDirection.sub(tPointPosition,tNodePosition);
			tDirection.normalize_safe();
			SRotation tRotation;
			mk_rotation(tDirection,tRotation);
			float fCost = ffGetCoverInDirection(tRotation.yaw,T);
			Msg("%d %.2f",Test,fCost);
			if (fCost < .35f) {
				bool bOk = false;
				float fMax = 0.f;
				for (int i=0, iIndex = -1; i<Group.m_tpaSuspiciousNodes.size(); i++) {
					Fvector tP0 = AI.tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID);
					float fDistance = tP0.distance_to(tNodePosition);
					if (fDistance < 10.f) {
						if (fDistance < 3.f) {
							if (fCost < Group.m_tpaSuspiciousNodes[i].fCost) {
								Group.m_tpaSuspiciousNodes[i].dwNodeID = Test;
								Group.m_tpaSuspiciousNodes[i].fCost = fCost;
							}
							bOk = true;
							break;
						}
						tDirection.sub(tP0,tNodePosition);
						tDirection.normalize_safe();
						mk_rotation(tDirection,tRotation);
						if (ffGetCoverInDirection(tRotation.yaw,T) > .3f) {
							if (fCost < Group.m_tpaSuspiciousNodes[i].fCost) {
								Group.m_tpaSuspiciousNodes[i].dwNodeID = Test;
								Group.m_tpaSuspiciousNodes[i].fCost = fCost;
							}
							bOk = true;
							break;
						}
					}
					if (fCost > fMax) {
						fMax = Group.m_tpaSuspiciousNodes[i].fCost;
						iIndex = i;
					}
				}
				if (!bOk) {
					if (Group.m_tpaSuspiciousNodes.size() < MAX_SUSPICIOUS_NODE_COUNT) {
						tSearchPlace.dwNodeID	= Test;
						tSearchPlace.fCost		= fCost;
						tSearchPlace.dwSearched	= 0;
						tSearchPlace.dwGroup	= 0;
						Group.m_tpaSuspiciousNodes.push_back(tSearchPlace);
					}
					else
						if ((fMax > fCost) && (iIndex >= 0)) {
							Group.m_tpaSuspiciousNodes[iIndex].dwNodeID = Test;
							Group.m_tpaSuspiciousNodes[iIndex].fCost = fCost;
						}
				}
			}
		}
		if (bStop)
			break;
	}

//	{
//		vector<DWORD>::iterator it	= AI.q_stack.begin();
//		vector<DWORD>::iterator end	= AI.q_stack.end();
//		for ( ; it!=end; it++)	
//			AI.q_mark_bit[*it] = false;
//	}
	AI.q_mark_bit.assign(AI.GetHeader().count,false);
	
	Msg("Suspicious nodes :");
	for (int k=0; k<Group.m_tpaSuspiciousNodes.size(); k++)
		Msg("%d",Group.m_tpaSuspiciousNodes[k].dwNodeID);

	Device.Statistic.AI_Range.End();
}

#define GROUP_RADIUS	15.f

void CAI_Soldier::vfClasterizeSuspiciousNodes(CGroup &Group)
{
//	DWORD N = Group.m_tpaSuspiciousNodes.size();
//	m_tpaSuspiciousPoints.resize(N);
//	m_tpaSuspiciousForces.resize(N);
//	for (int i=0; i<N; i++) {
//		m_tpaSuspiciousPoints[i] = Level().AI.tfGetNodeCenter(Group.m_tpaSuspiciousNodes[i].dwNodeID);
//		m_tpaSuspiciousForces[i].set(0,0,0);
//	}
//	float fK = .05f;
//	for (int k=0; k<3; k++) {
//		for (int i=0; i<N; i++) {
//			for (int j=0; j<N; j++)
//				if (j != i) {
//					Fvector tDummy;
//					tDummy.sub(m_tpaSuspiciousPoints[j],m_tpaSuspiciousPoints[i]);
//					tDummy.mul(tDummy.magnitude()*fK);
//					m_tpaSuspiciousForces[i].add(tDummy);
//				}
//		}
//		for (int i=0; i<m_tpaSuspiciousPoints.size(); i++) {
//			m_tpaSuspiciousForces[i].div(N);
//			m_tpaSuspiciousPoints[i].add(m_tpaSuspiciousForces[i]);
//			m_tpaSuspiciousForces[i].set(0,0,0);
//		}
//	}
//	for (int i=0, iGroupCounter = 1; i<N; i++, iGroupCounter++) {
//		if (!Group.m_tpaSuspiciousNodes[i].dwGroup) 
//			Group.m_tpaSuspiciousNodes[i].dwGroup = iGroupCounter;
//		for (int j=0; j<N; j++)
//			if (!Group.m_tpaSuspiciousNodes[j].dwGroup && (m_tpaSuspiciousPoints[j].distance_to(m_tpaSuspiciousPoints[i]) < GROUP_RADIUS))
//				Group.m_tpaSuspiciousNodes[j].dwGroup = iGroupCounter;
//	}
//	for ( i=0; i<N; i++)
//		Group.m_tpaSuspiciousNodes[i].dwGroup--;
//	Group.m_tpaSuspiciousGroups.resize(--iGroupCounter);
//	for ( i=0; i<iGroupCounter; i++)
//		Group.m_tpaSuspiciousGroups[i] = 0;

 	DWORD N = Group.m_tpaSuspiciousNodes.size();
	for (int i=0, iGroupCounter = 1; i<N; i++, iGroupCounter++) {
		if (!Group.m_tpaSuspiciousNodes[i].dwGroup) 
			Group.m_tpaSuspiciousNodes[i].dwGroup = iGroupCounter;
		for (int j=0; j<N; j++)
			if (!Group.m_tpaSuspiciousNodes[j].dwGroup && (Level().AI.ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[j].dwNodeID,Group.m_tpaSuspiciousNodes[i].dwNodeID) < GROUP_RADIUS))
				Group.m_tpaSuspiciousNodes[j].dwGroup = iGroupCounter;
	}
	for ( i=0; i<N; i++)
		Group.m_tpaSuspiciousNodes[i].dwGroup--;
	Group.m_tpaSuspiciousGroups.resize(--iGroupCounter);
	for ( i=0; i<iGroupCounter; i++)
		Group.m_tpaSuspiciousGroups[i] = 0;
}

int CAI_Soldier::ifGetSuspiciousAvailableNode(int iLastIndex, CGroup &Group)
{
	int Index = -1;
	float fMin = 1000, fCost;
	DWORD dwNodeID = AI_NodeID;
	if (iLastIndex >= 0) {
		dwNodeID = Group.m_tpaSuspiciousNodes[iLastIndex].dwNodeID;
		int iLastGroup = Group.m_tpaSuspiciousNodes[iLastIndex].dwGroup;
		for (int i=0; i<Group.m_tpaSuspiciousNodes.size(); i++) {
			if ((Group.m_tpaSuspiciousNodes[i].dwGroup != iLastGroup) || Group.m_tpaSuspiciousNodes[i].dwSearched)
				continue;
//			if (Group.m_tpaSuspiciousNodes[i].dwSearched)
//				continue;
//			if (Group.m_tpaSuspiciousNodes[i].fCost < fMin) {
//				fMin = Group.m_tpaSuspiciousNodes[i].fCost;
//				Index = i;
//			}
			if ((fCost = Level().AI.ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[i].dwNodeID,dwNodeID)) < fMin) {
				fMin = fCost;
				Index = i;
			}
		}
	}
	
	if (Index >= 0)
		return(Index);

	for (int i=0; i<Group.m_tpaSuspiciousNodes.size(); i++) {
//		if (Group.m_tpaSuspiciousGroups[Group.m_tpaSuspiciousNodes[i].dwGroup])
//			continue;
		if (Group.m_tpaSuspiciousNodes[i].dwSearched)
			continue;
		if (Group.m_tpaSuspiciousNodes[i].fCost < fMin) {
			fMin = Group.m_tpaSuspiciousNodes[i].fCost;
			Index = i;
		}
//		if ((fCost = Level().AI.ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[i].dwNodeID,dwNodeID)) < fMin) {
//			fMin = fCost;
//			Index = i;
//		}
	}

	if (Index == -1) {
		for (int i=0; i<Group.m_tpaSuspiciousNodes.size(); i++) {
			if (Group.m_tpaSuspiciousNodes[i].dwSearched)
				continue;
			if (Group.m_tpaSuspiciousNodes[i].fCost < fMin) {
				fMin = Group.m_tpaSuspiciousNodes[i].fCost;
				Index = i;
			}
//			if ((fCost = Level().AI.ffGetDistanceBetweenNodeCenters(Group.m_tpaSuspiciousNodes[i].dwNodeID,dwNodeID)) < fMin) {
//				fMin = fCost;
//				Index = i;
//			}
		}
	}
	return(Index);
}

bool CAI_Soldier::bfCheckIfIHaveToChangePosition()
{
	return(true);
}

float CAI_Soldier::ffGetDistanceToNearestMember()
{
	float fDistance = 1000.f;
	INIT_SQUAD_AND_LEADER;
	CGroup &Group = Squad.Groups[g_Group()];
	if (Leader != this)
		fDistance = min(fDistance,vPosition.distance_to(Leader->Position()));
	for (int i=0; i<Group.Members.size(); i++)
		if (Group.Members[i] != this) {
			CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(Group.Members[i]);
			if (!tpCustomMonster || (tpCustomMonster->AI_Path.fSpeed < EPS_L))
				fDistance = min(fDistance,vPosition.distance_to(Group.Members[i]->Position()));
		}
	return(fDistance);
}

void CAI_Soldier::vfMarkVisibleNodes(CEntity *tpEntity)
{
	CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(tpEntity);
	if (!tpCustomMonster)
		return;

	CAI_Space &AI = Level().AI;
	Fvector tDirection;
	float fFov = tpCustomMonster->ffGetFov()*PI/180.f, fRange = tpCustomMonster->ffGetRange();
	
	for (float fIncrement = 0; fIncrement < PI_MUL_2; fIncrement += PI/10.f) {
		tDirection.setHP(fIncrement,0.f);
		AI.ffMarkNodesInDirection(tpCustomMonster->AI_NodeID,tpCustomMonster->Position(),tDirection,AI.q_mark_bit,fRange);
	}
}

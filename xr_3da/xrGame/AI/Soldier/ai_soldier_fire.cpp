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

#define	FIRE_SAFETY_ANGLE				PI/10

bool CAI_Soldier::bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint) 
{
	Fvector tMemberDirection;
	tMemberDirection.sub(tMyPoint,tMemberPoint);
	vfNormalizeSafe(tMemberDirection);
	float fAlpha = acosf(tFireVector.dotproduct(tMemberDirection));
	//return(false);
	return(fAlpha < FIRE_SAFETY_ANGLE);
}

bool CAI_Soldier::bfCheckIfCanKillEnemy() 
{
	Fvector tMyLook;
	tMyLook.setHP	(r_torso_current.yaw + PI/6,r_torso_current.pitch);
	if (Enemy.Enemy) {
		Fvector tFireVector, tMyPosition = Position(), tEnemyPosition = Enemy.Enemy->Position();
		tFireVector.sub(tMyPosition,tEnemyPosition);
		vfNormalizeSafe(tFireVector);
		float fAlpha = acosf(tFireVector.dotproduct(tMyLook));
		return(true);
		//return(fAlpha < FIRE_ANGLE);
	}
	else
		return(true);
}

bool CAI_Soldier::bfCheckIfCanKillMember()
{
	Fvector tFireVector, tMyPosition = Position();
	tFireVector.setHP	(r_torso_current.yaw,r_torso_current.pitch);
	
	bool bCanKillMember = false;
	
	for (int i=0, iTeam = g_Team(), iSquad = g_Squad(), iGroup = g_Group(); i<tpaVisibleObjects.size(); i++) {
		CCustomMonster* CustomMonster = dynamic_cast<CCustomMonster*>(tpaVisibleObjects[i]);
		if ((CustomMonster) && (CustomMonster->g_Team() == iTeam) && (CustomMonster->g_Squad() == iSquad) && (CustomMonster->g_Group() == iGroup))
			if ((CustomMonster->g_Health() > 0) && (bfCheckForMember(tFireVector,tMyPosition,CustomMonster->Position()))) {
				bCanKillMember = true;
				break;
			}
	}
	return(bCanKillMember);
}

void CAI_Soldier::g_fireParams(Fvector &fire_pos, Fvector &fire_dir)
{
	//Weapons->GetFireParams(fire_pos, fire_dir);
	if (Weapons->ActiveWeapon()) {
		fire_pos.set(Weapons->ActiveWeapon()->Position());
		fire_dir.set(eye_matrix.k);
		SRotation sRotation;
		mk_rotation(fire_dir,sRotation);
		sRotation.yaw += 29*PI/180;
		fire_dir.setHP(-sRotation.yaw,-sRotation.pitch);
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
		if (::Random.randI(0,2))
			PKinematics(pVisual)->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageLeft);
		else
			PKinematics(pVisual)->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageRight);
	}
	
	// Play hit-sound
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	
	if (S.feedback)			
		return;
	//if (Random.randI(2))	
	//	return;
	pSounds->Play3DAtPos	(S,this,vPosition);
}

void CAI_Soldier::SelectEnemy(SEnemySelected& S)
{
	// Initiate process
	objVisible&	Known	= Level().Teams[g_Team()].KnownEnemys;
	S.Enemy					= 0;
	S.bVisible			= FALSE;
	S.fCost				= flt_max-1;
	if (Known.size()==0)	return;
	
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
				if (!bfCheckForVisibility(E))
					continue;
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
}

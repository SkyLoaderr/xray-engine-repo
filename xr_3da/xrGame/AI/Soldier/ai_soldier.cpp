////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\..\\3dsound.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\..\\..\\bodyinstance.h"
#include "..\\..\\..\\xr_trims.h"

#define WRITE_LOG

CAI_Soldier::CAI_Soldier()
{
	dwHitTime = 0;
	tHitDir.set(0,0,1);
	dwSenseTime = 0;
	tSenseDir.set(0,0,1);
	tSavedEnemy = 0;
	tSavedEnemyPosition.set(0,0,0);
	tpSavedEnemyNode = 0;
	dwSavedEnemyNodeID = -1;
	dwLostEnemyTime = 0;
	bBuildPathToLostEnemy = false;
	eCurrentState = aiSoldierFollowLeader;
	m_dwLastRangeSearch = 0;
	m_dwLastSuccessfullSearch = 0;
	m_fAggressiveness = ::Random.randF(0,1);
	m_fTimorousness = ::Random.randF(0,1);
	m_bFiring = false;
}

CAI_Soldier::~CAI_Soldier()
{
	// removing all data no more being neded 
	for (int i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void __stdcall CAI_Soldier::HeadSpinCallback(CBoneInstance* B)
{
	CAI_Soldier*		A = dynamic_cast<CAI_Soldier*> (static_cast<CObject*>(B->Callback_Param));

	Fmatrix				spin;
	spin.setXYZ			(A->NET_Last.o_torso.yaw - A->r_current.yaw, A->r_current.pitch, 0);
	B->mTransform.mul_43(spin);
}

void CAI_Soldier::vfLoadSounds()
{
	pSounds->Create3D(sndHit[0],"actor\\bhit_flesh-1");
	pSounds->Create3D(sndHit[1],"actor\\bhit_flesh-2");
	pSounds->Create3D(sndHit[2],"actor\\bhit_flesh-3");
	pSounds->Create3D(sndHit[3],"actor\\bhit_helmet-1");
	pSounds->Create3D(sndHit[4],"actor\\bullet_hit1");
	pSounds->Create3D(sndHit[5],"actor\\bullet_hit2");
	pSounds->Create3D(sndHit[6],"actor\\ric_conc-1");
	pSounds->Create3D(sndHit[7],"actor\\ric_conc-2");
	pSounds->Create3D(sndDie[0],"actor\\die0");
	pSounds->Create3D(sndDie[1],"actor\\die1");
	pSounds->Create3D(sndDie[2],"actor\\die2");
	pSounds->Create3D(sndDie[3],"actor\\die3");
}

void CAI_Soldier::vfLoadSelectors(CInifile *ini, const char *section)
{
	SelectorAttack.Load(ini,section);
	SelectorDefend.Load(ini,section);
	SelectorFindEnemy.Load(ini,section);
	SelectorFollowLeader.Load(ini,section);
	SelectorFreeHunting.Load(ini,section);
	SelectorMoreDeadThanAlive.Load(ini,section);
	SelectorNoWeapon.Load(ini,section);
	SelectorPatrol.Load(ini,section);
	SelectorPursuit.Load(ini,section);
	SelectorReload.Load(ini,section);
	SelectorRetreat.Load(ini,section);
	SelectorSenseSomething.Load(ini,section);
	SelectorUnderFire.Load(ini,section);
}

void CAI_Soldier::vfAssignBones(CInifile *ini, const char *section)
{
	int head_bone = PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_head"));
	PKinematics(pVisual)->LL_GetInstance(head_bone).set_callback(HeadSpinCallback,this);
}

void CAI_Soldier::vfCheckForPatrol(CInifile *ini)
{
	if (Level().pLevel->LineExists("patrol_path","path_00")) {
		
		LPCSTR buf = Level().pLevel->ReadSTRING("patrol_path","path_00"), buf2 = buf;
		
		int path_count = _GetItemCount(buf);

		R_ASSERT(path_count && (path_count % 3 == 0));

		path_count /= 3;
		m_tpaPatrolPoints.resize(path_count);
		m_tpaPointDeviations.resize(path_count);

		for (int i=0; i<path_count; i++) {
			sscanf(buf2,"%f,%f,%f",&(m_tpaPatrolPoints[i].x),&(m_tpaPatrolPoints[i].y),&(m_tpaPatrolPoints[i].z));
			for (int komas=0; komas<3; buf2++)
				if (*buf2 == ',')
					komas++;
		}

		m_dwStartPatrolNode = Level().AI.q_LoadSearch(m_tpaPatrolPoints[0]);
		AI_Path.bNeedRebuild = TRUE;
	}
}

void CAI_Soldier::Load(CInifile* ini, const char* section)
{ 
	// load parameters from ".ini" file
	inherited::Load	(ini,section);
	
	// initialize start position
	Fvector			P = vPosition;
	P.x				+= ::Random.randF();
	P.z				+= ::Random.randF();

	vfLoadSounds();
	vfLoadAnimations();
	vfLoadSelectors(ini,section);
	vfAssignBones(ini,section);
	vfCheckForPatrol(ini);
}

void CAI_Soldier::g_fireParams(Fvector &fire_pos, Fvector &fire_dir)
{
	//Weapons->GetFireParams(fire_pos, fire_dir);
	if (Weapons->ActiveWeapon()) {
		fire_pos.set	(Weapons->ActiveWeapon()->Position());
		fire_dir.set	(eye_matrix.k);
	}
}

void CAI_Soldier::OnVisible()
{
	inherited::OnVisible();

	Weapons->OnRender(FALSE);
	//return(0);
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

	// Play hit-sound
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->Play3DAtPos(S,vPosition);
	
	if (iHealth > 0)
		if (!m_cCrouched)
			if (::Random.randI(0,2))
				PKinematics(pVisual)->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageLeft);
			else
				PKinematics(pVisual)->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageRight);
		else
			if (::Random.randI(0,2))
				PKinematics(pVisual)->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageLeft);
			else
				PKinematics(pVisual)->PlayFX(tSoldierAnimations.tNormal.tTorso.tpDamageRight);
}

// when someone hit soldier
void CAI_Soldier::SenseSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwSenseTime = Level().timeServer();
	tSenseDir.set(D);
}

// when soldier is dead
void CAI_Soldier::Death()
{
	// perform death operations
	inherited::Death( );
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	eCurrentState = aiSoldierDie;

	// removing from group
	//Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Member_Remove(this);

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

// soldier update
void CAI_Soldier::Update(DWORD DT)
{
	inherited::Update(DT);
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

#define MOVEMENT_IDLE_TIME			1000
#define MAX_INVISIBLE_SPEED			 2.f
#define MAX_VIEWABLE_SPEED			10.f

#define MOVEMENT_SPEED_WEIGHT		20.f
#define DISTANCE_WEIGHT				20.f
#define SPEED_WEIGHT				-2.f
//#define LIGHTNESS_WEIGHT			40.f

bool CAI_Soldier::bfCheckForVisibility(CEntity* tpEntity)
{
	float fResult = 0.f;
	
	// computing maximum viewable distance in the specified direction
	Fvector tCurrentWatchDirection, tTemp;
	tCurrentWatchDirection.direct(r_current.yaw,r_current.pitch);
	tCurrentWatchDirection.normalize();
	tTemp.sub(tpEntity->Position(),vPosition);
	tTemp.normalize();
	//float fAlpha = tCurrentWatchDirection.dotproduct(tTemp), fEyeFov = eye_fov*PI/180.f;
	float fAlpha = tWatchDirection.dotproduct(tTemp), fEyeFov = eye_fov*PI/180.f;
	clamp(fAlpha,-.99999f,+.99999f);
	fAlpha = acosf(fAlpha);
	float fMaxViewableDistanceInDirection = eye_range*(1 - fAlpha/(fEyeFov/2.f));
	
	// computing distance weight
	tTemp.sub(vPosition,tpEntity->Position());
	fResult += tTemp.magnitude() >= fMaxViewableDistanceInDirection ? 0.f : DISTANCE_WEIGHT*(1.f - tTemp.magnitude()/fMaxViewableDistanceInDirection);
	
	// computing movement speed weight
	if (tpEntity->ps_Size() > 1) {
		DWORD dwTime = tpEntity->ps_Element(tpEntity->ps_Size() - 1).dwTime;
		if (dwTime < MOVEMENT_IDLE_TIME) {
			tTemp.sub(tpEntity->ps_Element(tpEntity->ps_Size() - 2).vPosition,tpEntity->ps_Size() - 1);
			float fSpeed = tTemp.magnitude()/dwTime;
			fResult += fSpeed < MAX_INVISIBLE_SPEED ? MOVEMENT_SPEED_WEIGHT*fSpeed/MAX_INVISIBLE_SPEED : MOVEMENT_SPEED_WEIGHT;
		}
	}

	// computing lightness weight
	fResult *= float(tpEntity->AI_Node->light)/255.f;
	
	// computing enemy state
	switch (m_cCrouched) {
		case 0: {
			break;
		}
		case 1: {
			fResult *= .85f;
			break;
		}
		case 2: {
			fResult *= .66f;
			break;
		}
	}

	// computing my ability to view the enemy
	//if ()
	//fResult += m_fCurSpeed < MAX_VIEWABLE_SPEED ? SPEED_WEIGHT*(1.f - m_fCurSpeed/MAX_VIEWABLE_SPEED) : SPEED_WEIGHT;

	return(fResult >= 6.f);
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

	// Iterate on known
	for (DWORD i=0; i<Known.size(); i++)
	{
		CEntity*	E = dynamic_cast<CEntity*>(Known[i].key);
		float		H = EnemyHeuristics(E);
		if (H<S.fCost) {
			if (!bfCheckForVisibility(E))
				continue;
			// Calculate local visibility
			CObject**	ins	 = lower_bound(tpaVisibleObjects.begin(),tpaVisibleObjects.end(),(CObject*)E);
			bool	bVisible = (ins==tpaVisibleObjects.end())?FALSE:((E==*ins)?TRUE:FALSE);
			float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
			if (cost<S.fCost)	{
				S.Enemy		= E;
				S.bVisible	= bVisible;
				S.fCost		= cost;
			}
		}
	}
}

IC bool CAI_Soldier::bfCheckForMember(Fvector &tFireVector, Fvector &tMyPoint, Fvector &tMemberPoint) 
{
	Fvector tMemberDirection;
	tMemberDirection.sub(tMyPoint,tMemberPoint);
	vfNormalizeSafe(tMemberDirection);
	float fAlpha = acosf(tFireVector.dotproduct(tMemberDirection));
	return(fAlpha < FIRE_SAFETY_ANGLE);
}

bool CAI_Soldier::bfCheckIfCanKillMember()
{
	Fvector tFireVector, tMyPosition = Position();
	tFireVector.direct(r_torso_current.yaw,r_torso_current.pitch);
	
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

IC bool CAI_Soldier::bfCheckIfCanKillEnemy() 
{
	Fvector tMyLook;
	tMyLook.direct(r_torso_current.yaw,r_torso_current.pitch);
	if (Enemy.Enemy) {
		Fvector tFireVector, tMyPosition = Position(), tEnemyPosition = Enemy.Enemy->Position();
		tFireVector.sub(tMyPosition,tEnemyPosition);
		vfNormalizeSafe(tFireVector);
		float fAlpha = acosf(tFireVector.dotproduct(tMyLook));
		return(true);
		return(fAlpha < FIRE_ANGLE);
	}
	else
		return(true);
}

bool CAI_Soldier::bfCheckPath(AI::Path &Path) {
	const vector<BYTE> &q_mark = Level().AI.tpfGetNodeMarks();
	for (int i=1; i<Path.Nodes.size(); i++) 
		if (q_mark[Path.Nodes[i]])
			return(false);
	return(true);
}

float ffCalcSquare(float fAngle, float fAngleOfView, float _b0, float _b1, float _b2, float _b3)
{
	fAngle = fAngle >= PI ? 2*PI - fAngle : fAngle;
	float fSquare;
	if ((fAngle >= -PI/2 + fAngleOfView/2) && (fAngle < fAngleOfView/2))
		if (fAngle < PI/2 - fAngleOfView/2) {
			float b0 = _b0, b1 = _b1, b2 = _b2, b3 = _b3, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;
			fSquare = (CUBE(PI)*(SQR(a1) + SQR(a2))/24.f + SQR(PI)*(a1*b1 + a2*b2)/4.f + PI*(SQR(b1) + SQR(b2))/2.f - (CUBE(fAngle)*SQR(a1)/3.f + SQR(fAngle)*a1*b1 + fAngle*SQR(b1) + CUBE(fKsi)*SQR(a2)/3.f + SQR(fKsi)*a2*b2 + fKsi*SQR(b2)))/2.f;
		}
		else {
			float b0 = _b1, b1 = _b2, b2 = _b3, b3 = _b0, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
			fSquare = (CUBE(PI)*(SQR(a3) + SQR(a2))/24.f + SQR(PI)*(a3*b3 + a2*b2)/4.f + PI*(SQR(b3) + SQR(b2))/2.f - (CUBE(fAngle)*SQR(a1)/3.f + SQR(fAngle)*a1*b1 + fAngle*SQR(b1) + CUBE(fKsi)*SQR(a3)/3.f + SQR(fKsi)*a3*b3 + fKsi*SQR(b3)))/2.f;
		}
	else
		if ((fAngle - PI/2 >= -PI/2 + fAngleOfView/2) && (fAngle - PI/2 < fAngleOfView/2))
			if (fAngle - PI/2 < PI/2 - fAngleOfView/2) {
				float b0 = _b1, b1 = _b2, b2 = _b3, b3 = _b0, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
				fSquare = (CUBE(PI)*(SQR(a1) + SQR(a2))/24.f + SQR(PI)*(a1*b1 + a2*b2)/4.f + PI*(SQR(b1) + SQR(b2))/2.f - (CUBE(fAngle)*SQR(a1)/3.f + SQR(fAngle)*a1*b1 + fAngle*SQR(b1) + CUBE(fKsi)*SQR(a2)/3.f + SQR(fKsi)*a2*b2 + fKsi*SQR(b2)))/2.f;
			}
			else {
				float b0 = _b2, b1 = _b3, b2 = _b0, b3 = _b1, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
				fSquare = (CUBE(PI)*(SQR(a3) + SQR(a2))/24.f + SQR(PI)*(a3*b3 + a2*b2)/4.f + PI*(SQR(b3) + SQR(b2))/2.f - (CUBE(fAngle)*SQR(a1)/3.f + SQR(fAngle)*a1*b1 + fAngle*SQR(b1) + CUBE(fKsi)*SQR(a3)/3.f + SQR(fKsi)*a3*b3 + fKsi*SQR(b3)))/2.f;
			}
		else
			if ((fAngle - PI >= -PI/2 + fAngleOfView/2) && (fAngle - PI < fAngleOfView/2))
				if (fAngle - PI < PI/2 - fAngleOfView/2) {
					float b0 = _b2, b1 = _b3, b2 = _b0, b3 = _b1, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
					fSquare = (CUBE(PI)*(SQR(a1) + SQR(a2))/24.f + SQR(PI)*(a1*b1 + a2*b2)/4.f + PI*(SQR(b1) + SQR(b2))/2.f - (CUBE(fAngle)*SQR(a1)/3.f + SQR(fAngle)*a1*b1 + fAngle*SQR(b1) + CUBE(fKsi)*SQR(a2)/3.f + SQR(fKsi)*a2*b2 + fKsi*SQR(b2)))/2.f;
				}
				else {
					float b0 = _b3, b1 = _b0, b2 = _b1, b3 = _b2, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
					fSquare = (CUBE(PI)*(SQR(a3) + SQR(a2))/24.f + SQR(PI)*(a3*b3 + a2*b2)/4.f + PI*(SQR(b3) + SQR(b2))/2.f - (CUBE(fAngle)*SQR(a1)/3.f + SQR(fAngle)*a1*b1 + fAngle*SQR(b1) + CUBE(fKsi)*SQR(a3)/3.f + SQR(fKsi)*a3*b3 + fKsi*SQR(b3)))/2.f;
				}
			else
				if (fAngle - 3*PI/2 < PI/2 - fAngleOfView/2) {
					float b0 = _b3, b1 = _b0, b2 = _b1, b3 = _b2, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), fKsi = fAngleOfView - fAngle;;
					fSquare = (CUBE(PI)*(SQR(a1) + SQR(a2))/24.f + SQR(PI)*(a1*b1 + a2*b2)/4.f + PI*(SQR(b1) + SQR(b2))/2.f - (CUBE(fAngle)*SQR(a1)/3.f + SQR(fAngle)*a1*b1 + fAngle*SQR(b1) + CUBE(fKsi)*SQR(a2)/3.f + SQR(fKsi)*a2*b2 + fKsi*SQR(b2)))/2.f;
				}
				else {
					float b0 = _b0, b1 = _b1, b2 = _b2, b3 = _b3, a1 = (b1 - b0)/(PI/2.f), a2 = (b1 - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fKsi = PI- fAngleOfView - fAngle;
					fSquare = (CUBE(PI)*(SQR(a3) + SQR(a2))/24.f + SQR(PI)*(a3*b3 + a2*b2)/4.f + PI*(SQR(b3) + SQR(b2))/2.f - (CUBE(fAngle)*SQR(a1)/3.f + SQR(fAngle)*a1*b1 + fAngle*SQR(b1) + CUBE(fKsi)*SQR(a3)/3.f + SQR(fKsi)*a3*b3 + fKsi*SQR(b3)))/2.f;
				}
	return(fSquare);
}

void CAI_Soldier::SetDirectionLook()
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.square_magnitude() > EPS_L) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
			r_target.yaw = r_torso_target.yaw + PI_DIV_6;
		}
	}
	r_target.pitch = 0;
	r_torso_target.pitch = 0;
}

void CAI_Soldier::SetLessCoverLook(NodeCompressed *tNode)
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.square_magnitude() > 0.000001) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
			
			float fAngleOfView = eye_fov/180.f*PI, fMaxSquare = -1.f, fBestAngle;
			
			for (float fIncrement = r_torso_current.yaw - MAX_HEAD_TURN_ANGLE; fIncrement <= r_torso_current.yaw + MAX_HEAD_TURN_ANGLE; fIncrement += 2*MAX_HEAD_TURN_ANGLE/60.f) {
				float fSquare = ffCalcSquare(fIncrement,fAngleOfView,FN(1),FN(2),FN(3),FN(0));
				if (fSquare > fMaxSquare) {
					fMaxSquare = fSquare;
					fBestAngle = fIncrement;
				}
			}
			
			r_target.yaw = fBestAngle;
			q_look.o_look_speed=_FB_look_speed;
			//r_torso_speed = _FB_look_speed;//(r_torso_target.yaw - r_torso_current.yaw);
			r_target.yaw += PI/6.f;
		}
	}
	r_target.pitch = 0;
	r_torso_target.pitch = 0;
}

void CAI_Soldier::SetSmartLook(NodeCompressed *tNode, Fvector &tEnemyDirection)
{
	Fvector tLeft;
	Fvector tRight;
	Fvector tFront;
	Fvector tBack;
	tLeft.set(-1,0,0);
	tRight.set(1,0,0);
	tFront.set(0,0,1);
	tBack.set(0,0,-1);

	tEnemyDirection.normalize();
		
	/**
	float fCover = 0;
	if ((tEnemyDirection.x < 0) && (tEnemyDirection.z > 0)) {
		float fAlpha = acosf(tEnemyDirection.dotproduct(tFront));
		fCover = cosf(fAlpha)*tNode->cover[0] + sinf(fAlpha)*tNode->cover[1];
	}
	else 
		if ((tEnemyDirection.x > 0) && (tEnemyDirection.z > 0)) {
			float fAlpha = acosf(tEnemyDirection.dotproduct(tFront));
			fCover = cosf(fAlpha)*tNode->cover[1] + sinf(fAlpha)*tNode->cover[2];
		}
		else 
			if ((tEnemyDirection.x > 0) && (tEnemyDirection.z < 0)) {
				float fAlpha = acosf(tEnemyDirection.dotproduct(tBack));
				fCover = cosf(fAlpha)*tNode->cover[3] + sinf(fAlpha)*tNode->cover[2];
			}
			else 
				if ((tEnemyDirection.x < 0) && (tEnemyDirection.z < 0)) {
					float fAlpha = acosf(tEnemyDirection.dotproduct(tBack));
					fCover = cosf(fAlpha)*tNode->cover[3] + sinf(fAlpha)*tNode->cover[0];
				}
	

	//Msg("%8.2f",fCover);
	if (fCover > -1.0f*255.f) {
	/**/
		//q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tEnemyDirection), 1000);
		mk_rotation(tEnemyDirection,r_torso_target);
		r_target.yaw = r_torso_target.yaw;
		r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
		r_target.pitch *= -1;
		q_look.o_look_speed=8*_FB_look_speed;
	/**
	}
	else {
		SetLessCoverLook(tNode);
		q_look.o_look_speed=8*_FB_look_speed;
	}
	/**/
}

void CAI_Soldier::vfSaveEnemy()
{
	tSavedEnemy = Enemy.Enemy;
	tSavedEnemyPosition = Enemy.Enemy->Position();
	tpSavedEnemyNode = Enemy.Enemy->AI_Node;
	dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
}

void CAI_Soldier::vfInitSelector(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
	// checking if leader is dead then make myself a leader
	if (Leader->g_Health() <= 0)
		Leader = this;
	// setting watch mode to false
	bool bWatch = false;
	// if i am not a leader then assign leader
	if (Leader != this) {
		S.m_tLeader = Leader;
		S.m_tLeaderPosition = Leader->Position();
		S.m_tpLeaderNode = Leader->AI_Node;
		S.m_tLeaderNode = Leader->AI_NodeID;
	}
	// otherwise assign leader to null
	else {
		S.m_tLeader = 0;
		S.m_tLeaderPosition.set(0,0,0);
		S.m_tpLeaderNode = NULL;
		S.m_tLeaderNode = -1;
	}
	S.m_tHitDir			= tHitDir;
	S.m_dwHitTime		= dwHitTime;
	
	S.m_dwCurTime		= Level().timeServer();
	//Msg("%d : %d",S.m_dwHitTime,S.m_dwCurTime);
	
	S.m_tMe				= this;
	S.m_tpMyNode		= AI_Node;
	S.m_tMyPosition		= Position();
	
	if (Enemy.Enemy) {
		bBuildPathToLostEnemy = false;
		// saving an enemy
		vfSaveEnemy();

		S.m_tEnemy			= Enemy.Enemy;
		S.m_tEnemyPosition	= Enemy.Enemy->Position();
		S.m_tpEnemyNode		= Enemy.Enemy->AI_Node;
	}
	else {
		S.m_tEnemy			= tSavedEnemy;
		S.m_tEnemyPosition	= tSavedEnemyPosition;
		S.m_tpEnemyNode		= tpSavedEnemyNode;
	}
	
	S.taMembers = &(Squad.Groups[g_Group()].Members);
	
	//if (S.m_tLeader)
	//	S.taMembers.push_back(S.m_tLeader);
}

void CAI_Soldier::vfBuildPathToDestinationPoint(CSoldierSelectorAttack *S)
{
	// building a path from and to
	if (S)
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path,*(S->m_tpEnemyNode),S->fOptEnemyDistance);
	else
		Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
	
	if (AI_Path.Nodes.size() > 2) {
	// if path is long enough then build travel line
		AI_Path.BuildTravelLine(Position());
		AI_Path.TravelStart = 0;
	}
	else {
	// if path is too short then clear it (patch for ExecMove)
		AI_Path.TravelPath.clear();
		AI_Path.bNeedRebuild = FALSE;
	}
}

void CAI_Soldier::vfSearchForBetterPosition(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
	if ((!m_dwLastRangeSearch) || ((S.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SEARCH_TIME_INTERVAL) && (::Random.randF(0,1) < float(S.m_dwCurTime - m_dwLastRangeSearch)/MAX_TIME_RANGE_SEARCH))) {
		
		bool bLastSearch = m_dwLastRangeSearch != 0;
		DWORD dwTimeDifference = S.m_dwCurTime - m_dwLastSuccessfullSearch;
		m_dwLastRangeSearch = S.m_dwCurTime;
		Device.Statistic.AI_Node.Begin();
		Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
		Device.Statistic.AI_Node.End();
		// search for the best node according to the 
		// selector evaluation function in the radius N meteres
		float fOldCost;
		if (bLastSearch)
			Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost,dwTimeDifference);
		else
			Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
		// if search has found new best node then 
		if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
			AI_Path.DestNode		= S.BestNode;
			AI_Path.bNeedRebuild	= TRUE;
		} 
		
		if (AI_Path.Nodes.size() <= 2)
			AI_Path.bNeedRebuild = TRUE;

		if (AI_Path.bNeedRebuild)
			m_dwLastSuccessfullSearch = S.m_dwCurTime;
	}
}

void CAI_Soldier::vfSearchForBetterPositionWTime(CAISelectorBase &S, CSquad &Squad, CEntity* &Leader)
{
	DWORD dwTimeDifference = S.m_dwCurTime - m_dwLastSuccessfullSearch;
	m_dwLastRangeSearch = S.m_dwCurTime;
	Device.Statistic.AI_Node.Begin();
	Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
	Device.Statistic.AI_Node.End();
	// search for the best node according to the 
	// selector evaluation function in the radius N meteres
	float fOldCost;
	Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
	// if search has found new best node then 
	//if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
	if ((AI_Path.DestNode != S.BestNode) && (S.BestCost < (fOldCost - S.fLaziness))){
		AI_Path.DestNode		= S.BestNode;
		AI_Path.bNeedRebuild	= TRUE;
	} 
	
	if (AI_Path.Nodes.size() <= 2)
		AI_Path.bNeedRebuild = TRUE;

	if (AI_Path.bNeedRebuild)
		m_dwLastSuccessfullSearch = S.m_dwCurTime;
}

void CAI_Soldier::vfAimAtEnemy()
{
	Fvector	pos;
	Enemy.Enemy->svCenter(pos);
	tWatchDirection.sub(pos,eye_matrix.c);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	q_look.o_look_speed=8*_FB_look_speed;
}

static BOOL __fastcall SoldierQualifier(CObject* O, void* P)
{
	if (O->CLS_ID!=CLSID_ENTITY)			
		return FALSE;
	else
		return TRUE;
}

objQualifier* CAI_Soldier::GetQualifier	()
{
	return(&SoldierQualifier);
}

void CAI_Soldier::vfSetFire(bool bFire, CGroup &Group)
{
	bool bSafeFire = m_bFiring;

	if (bFire) {
		if (bfCheckIfCanKillEnemy())
			if (!bfCheckIfCanKillMember()) {
				q_action.setup(AI::AIC_Action::FireBegin);
				m_bFiring = true;
			}
			else {
				q_action.setup(AI::AIC_Action::FireEnd);
				//Weapons->ActiveWeapon()->Reload();
				m_bFiring = false;
			}
		else {
			q_action.setup(AI::AIC_Action::FireEnd);
			m_bFiring = false;
			//Weapons->ActiveWeapon()->Reload();
		}
	}
	else {
		q_action.setup(AI::AIC_Action::FireEnd);
		//if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() < 10))
		//	Weapons->ActiveWeapon()->Reload();
		m_bFiring = false;
	}
	
	if ((bSafeFire) && (!m_bFiring))
		Group.m_dwFiring--;
	else
		if ((!bSafeFire) && (m_bFiring))
			Group.m_dwFiring++;
		
	//Msg("firing : %d",Group.m_dwFiring);
}

void CAI_Soldier::vfSetMovementType(bool bCrouched, float fSpeed)
{
	if (bCrouched) {
		Squat();
		m_fCurSpeed = m_fCrouchCoefficient*fSpeed;
	}
	else {
		StandUp();
		m_fCurSpeed = fSpeed;
	}
	
}

void CAI_Soldier::vfCheckForSavedEnemy()
{
	if (!tSavedEnemy) {
		tSavedEnemy = Enemy.Enemy;
		tSavedEnemyPosition = Enemy.Enemy->Position();
		tpSavedEnemyNode = Enemy.Enemy->AI_Node;
		dwSavedEnemyNodeID = Enemy.Enemy->AI_NodeID;
	}
	else {
		tSavedEnemy = 0;
		tSavedEnemyPosition.set(0,0,0);
		tpSavedEnemyNode = 0;
		dwSavedEnemyNodeID = -1;
	}
}

void CAI_Soldier::AttackFire()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Shooting enemy...");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
	
	SelectEnemy(Enemy);
	
	if (!(Enemy.Enemy)) {
		if (((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy)) {
			eCurrentState = tStateStack.top();
			tStateStack.pop();
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		else {
			dwLostEnemyTime = Level().timeServer();
			eCurrentState = aiSoldierPursuit;
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		return;
	}
	
	if (!(Enemy.bVisible)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierFindEnemy;
		m_dwLastRangeSearch = 0;
		return;
	}
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());

	if ((tDistance.square_magnitude() >= 25.f) && ((Group.m_dwFiring > 1) || ((Group.m_dwFiring == 1) && (!m_bFiring)))) {
		eCurrentState = aiSoldierAttackRun;
		m_dwLastRangeSearch = 0;
		return;
	}

	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierReload;
		m_dwLastRangeSearch = 0;
		return;
	}

	vfSaveEnemy();

	AI_Path.TravelPath.clear();
	
	vfAimAtEnemy();
	
	vfSetFire(true,Group);

	vfSetMovementType(true,m_fMaxSpeed);

	bStopThinking = true;
}

void CAI_Soldier::AttackRun()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Attack enemy");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
	
	SelectEnemy(Enemy);
	
	if (!(Enemy.Enemy)) {
		if (((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy)) {
			eCurrentState = tStateStack.top();
			tStateStack.pop();
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		else {
			dwLostEnemyTime = Level().timeServer();
			eCurrentState = aiSoldierPursuit;
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		return;
	}
	
	if (!(Enemy.bVisible)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierFindEnemy;
		m_dwLastRangeSearch = 0;
		return;
	}
		
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	Fvector tDistance;
	tDistance.sub(Position(),Enemy.Enemy->Position());
	
	if ((tDistance.square_magnitude() < 25.f) || ((!(Group.m_dwFiring)) && (Weapons->ActiveWeapon() && (Weapons->ActiveWeapon()->GetAmmoElapsed() > 0)))) {
	//if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() > 0)){
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorAttack,Squad,Leader);
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(&SelectorAttack);
	else
		vfSearchForBetterPosition(SelectorAttack,Squad,Leader);
	
	vfAimAtEnemy();
	
	vfSetFire(false,Group);

	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() <= Weapons->ActiveWeapon()->GetAmmoMagSize()*0.2f))
		Weapons->ActiveWeapon()->Reload();
	
	vfSetMovementType(false,m_fMaxSpeed);

	bStopThinking = true;
}

void CAI_Soldier::Defend()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Defend from enemy");
#endif
}

void CAI_Soldier::Die()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Dying...");
#endif
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	AI_Path.TravelPath.clear();
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	//bActive = false;
	bEnabled = false;
	
	bStopThinking = true;
}

void CAI_Soldier::FindEnemy()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Looking for enemy");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
	
	SelectEnemy(Enemy);
	
	if (!(Enemy.Enemy)) {
		if (((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) || (!tSavedEnemy)) {
			eCurrentState = tStateStack.top();
			tStateStack.pop();
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		else {
			dwLostEnemyTime = Level().timeServer();
			eCurrentState = aiSoldierPursuit;
			q_action.setup(AI::AIC_Action::FireEnd);
			m_dwLastRangeSearch = 0;
		}
		return;
	}
	
	if (Enemy.bVisible) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		return;
	}
		
	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() < AMMO_NEED_RELOAD)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierReload;
		m_dwLastRangeSearch = 0;
		return;
	}

	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorFindEnemy,Squad,Leader);
	
	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFindEnemy,Squad,Leader);
	
	vfAimAtEnemy();
	
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];

	vfSetFire(false,Group);
	
	vfSetMovementType(false,m_fMaxSpeed);

	bStopThinking = true;
	return;
}

void CAI_Soldier::FollowLeader()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Following leader");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
		
	SelectEnemy(Enemy);
	// do I see the enemies?
	if (Enemy.Enemy)		{
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	DWORD dwCurTime = Level().timeServer();
	
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierSenseSomething;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {
		tHitDir = Group.m_tLastHitDirection;
		dwHitTime = Group.m_dwLastHitTime;
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	if (Leader == this) {
		if (m_tpaPatrolPoints.size())
			eCurrentState = aiSoldierPatrolRoute;
		else
			eCurrentState = aiSoldierFreeHunting;
		return;
	}
	else
		if (m_tpaPatrolPoints.size())
			eCurrentState = aiSoldierFollowLeaderPatrol;

	vfInitSelector(SelectorFollowLeader,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFollowLeader,Squad,Leader);
		
	SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMinSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::FreeHunting()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Free hunting");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
		
	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	DWORD dwCurTime = Level().timeServer();
	
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierSenseSomething;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {
		tHitDir = Group.m_tLastHitDirection;
		dwHitTime = Group.m_dwLastHitTime;
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	vfInitSelector(SelectorFreeHunting,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFreeHunting,Squad,Leader);

	SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMinSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::Injuring()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Feeling pain...");
#endif
}

void CAI_Soldier::Jumping()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Jumping...");
#endif
}

void CAI_Soldier::MoreDeadThanAlive()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"More dead than alive");
#endif
}

void CAI_Soldier::NoWeapon()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Searching for weapon");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}
		
	SelectEnemy(Enemy);
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	vfInitSelector(SelectorNoWeapon,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else {
		m_dwLastRangeSearch = 0;
		vfSearchForBetterPosition(SelectorNoWeapon,Squad,Leader);
	}

	if (Enemy.Enemy)
		vfAimAtEnemy();
	else
		SetDirectionLook();

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMaxSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::Pursuit()
{
	// if no more health then soldier is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Pursuiting");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}

	SelectEnemy(Enemy);
	
	if (Enemy.Enemy) {
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	DWORD dwCurTime = Level().timeServer();
	
	if (dwCurTime - dwLostEnemyTime > LOST_ENEMY_REACTION_TIME) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		q_action.setup(AI::AIC_Action::FireEnd);
		m_fCurSpeed = m_fMaxSpeed;
		bStopThinking = true;
		return;
	}
				
	if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierPatrolUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierSenseSomething;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	if ((dwCurTime - Group.m_dwLastHitTime < HIT_JUMP_TIME) && (Group.m_dwLastHitTime)) {
		tHitDir = Group.m_tLastHitDirection;
		dwHitTime = Group.m_dwLastHitTime;
		tHitPosition = Group.m_tHitPosition;
		tHitPosition = Group.m_tHitPosition;
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierUnderFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	if (!tSavedEnemy) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		return;
	}

	vfInitSelector(SelectorPursuit,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorPursuit,Squad,Leader);

	tWatchDirection.sub(tSavedEnemyPosition,Position());
	
	if (tWatchDirection.magnitude() > 0.0001f)
		SetSmartLook(AI_Node,tWatchDirection);
	else
		SetLessCoverLook(AI_Node);

	vfSetFire(false,Group);

	vfSetMovementType(false,m_fMaxSpeed);
	
	bStopThinking = true;
}

void CAI_Soldier::Reload()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Recharging...");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}

	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == Weapons->ActiveWeapon()->GetAmmoMagSize())) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		return;
	}
	
	if ((Weapons->ActiveWeapon()) && (!(Weapons->ActiveWeapon()->GetAmmoCurrent()))) {
		eCurrentState = aiSoldierNoWeapon;
		m_dwLastRangeSearch = 0;
		return;
	}
	
	SelectEnemy(Enemy);

	/**
	if (!Enemy.Enemy) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		q_action.setup(AI::AIC_Action::FireEnd);
		m_dwLastRangeSearch = 0;
		return;
	}
	/**/
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	vfInitSelector(SelectorReload,Squad,Leader);

	if (Enemy.Enemy) {
		if (AI_Path.bNeedRebuild)
			vfBuildPathToDestinationPoint(0);
		else
			vfSearchForBetterPosition(SelectorReload,Squad,Leader);
		tWatchDirection.sub(Enemy.Enemy->Position(),Position());
		if (tWatchDirection.magnitude() > 0.0001f)
			SetSmartLook(AI_Node,tWatchDirection);
		else
			SetLessCoverLook(AI_Node);
	}
	else {
		//SetLessCoverLook(AI_Node);
	}
	
	vfSetFire(false,Group);
	
	if (Weapons->ActiveWeapon())
		Weapons->ActiveWeapon()->Reload();

	//if (AI_Path.TravelPath.size() <= AI_Path.TravelStart)
		vfSetMovementType(false,m_fMinSpeed);
	//else
	//	vfSetMovementType(true,m_fMinSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::Retreat()
{
}

void CAI_Soldier::SenseSomething()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Sense something...");
#endif
	//bStopThinking = true;
	//dwSenseTime = 0;
	// setting up a look to watch at the least safe direction
	q_look.setup(AI::AIC_Look::Look,AI::t_Direction,&tSenseDir,1000);
	// setting up look speed
	//q_look.o_look_speed=_FB_look_speed;

	eCurrentState = tStateStack.top();
	tStateStack.pop();
	bStopThinking = true;
}

void CAI_Soldier::UnderFire()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Under fire...");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiSoldierDie;
		return;
	}

	SelectEnemy(Enemy);

	if (Enemy.Enemy)		{
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierAttackFire;
		m_dwLastRangeSearch = 0;
		return;
	}

	DWORD dwCurTime = Level().timeServer();

	/**
	if (dwCurTime - dwHitTime > HIT_REACTION_TIME) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		return;
	}
	/**/
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	Group.m_dwLastHitTime = dwHitTime;
	Group.m_tLastHitDirection = tHitDir;
	Group.m_tHitPosition = tHitPosition;

	if ((dwCurTime - Group.m_dwLastHitTime > HIT_REACTION_TIME) && (Group.m_dwLastHitTime)) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		return;
	}
	
	vfInitSelector(SelectorUnderFire,Squad,Leader);

	SelectorUnderFire.m_tEnemyPosition = tHitPosition;

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		if (m_dwLastSuccessfullSearch <= Group.m_dwLastHitTime)
			vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);

	mk_rotation(tHitDir,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	//r_target.pitch = 0;
	//r_torso_target.pitch = 0;
	
	//q_look.o_look_speed=8*_FB_look_speed;

	vfSetFire(dwCurTime - dwHitTime < 1000,Group);

	vfSetMovementType(false,m_fMaxSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::Think()
{
	bStopThinking = false;
	do {
		ESoldierStates ePreviousState = eCurrentState;
		switch(eCurrentState) {
			case aiSoldierAttackRun : {
				AttackRun();
				break;
			}
			case aiSoldierAttackFire : {
				AttackFire();
				break;
			}
			case aiSoldierDefend : {
				Defend();
				break;
			}
			case aiSoldierDie : {
				Die();
				break;
			}
			case aiSoldierFindEnemy : {
				FindEnemy();
				break;
			}
			case aiSoldierFollowLeader : {
				FollowLeader();
				break;
			}
			case aiSoldierFreeHunting : {
				FreeHunting();
				break;
			}
			case aiSoldierInjuring : {
				Injuring();
				break;
			}
			case aiSoldierJumping : {
				Jumping();
				break;
			}
			case aiSoldierMoreDeadThanAlive : {
				MoreDeadThanAlive();
				break;
			}
			case aiSoldierNoWeapon : {
				NoWeapon();
				break;
			}
			case aiSoldierPatrolRoute : {
				Patrol();
				break;
										}
			case aiSoldierPatrolReturnToRoute : {
				PatrolReturn();
				break;
										}
			case aiSoldierFollowLeaderPatrol : {
				FollowLeaderPatrol();
				break;
			}
			case aiSoldierPatrolUnderFire : {
				PatrolUnderFire();
				break;
			}
			case aiSoldierPatrolHurt : {
				PatrolHurt();
				break;
			}
			case aiSoldierPursuit : {
				Pursuit();
				break;
			}
			case aiSoldierReload : {
				Reload();
				break;
			}
			case aiSoldierRetreat : {
				Retreat();
				break;
			}
			case aiSoldierSenseSomething : {
				SenseSomething();
				break;
			}
			case aiSoldierUnderFire : {
				UnderFire();
				break;
			}
		}
		m_bStateChanged = ePreviousState != eCurrentState;
	}
	while (!bStopThinking);
}

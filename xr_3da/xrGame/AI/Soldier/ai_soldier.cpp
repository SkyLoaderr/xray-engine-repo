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

#define WRITE_LOG
#define MIN_RANGE_SEARCH_TIME_INTERVAL	500.f
#define MAX_TIME_RANGE_SEARCH			150000.f
#define	FIRE_ANGLE						PI/30
#define	FIRE_SAFETY_ANGLE				PI/30
#define CUBE(x)							((x)*(x)*(x))
#define LEFT_NODE(Index)				((Index + 3) & 3)
#define RIGHT_NODE(Index)				((Index + 5) & 3)
#define FN(i)							(float(tNode->cover[(i)])/255.f)
#define	AMMO_NEED_RELOAD				6
#define	MAX_HEAD_TURN_ANGLE				(PI/3.f)
#define VECTOR_DIRECTION(i)				(i == 0 ? tLeft : (i == 1 ? tForward : (i == 2 ? tRight : tBack)))
#define EYE_WEAPON_DELTA				(-PI/30.f)

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

/**/
	Fmatrix				spin;
	spin.setXYZ			(A->NET_Last.o_torso.yaw - A->r_current.yaw, A->r_current.pitch, 0);
	B->mTransform.mul_43(spin);
/**
	Fmatrix				spin;
	float				bone_yaw	= A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta;
	float				bone_pitch	= A->r_torso.pitch;
	clamp				(bone_pitch,-PI_DIV_8,PI_DIV_4);
	spin.setXYZ			(bone_yaw,bone_pitch,0);
	B->mTransform.mul_43(spin);
/**/
}

void CAI_Soldier::Load(CInifile* ini, const char* section)
{ 
	// load parameters from ".ini" file
	inherited::Load	(ini,section);
	
	Fvector			P = vPosition;
	P.x				+= ::Random.randF();
	P.z				+= ::Random.randF();

	// sounds
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

	SelectorAttack.Load(ini,section);
	SelectorDefend.Load(ini,section);
	SelectorFindEnemy.Load(ini,section);
	SelectorFollowLeader.Load(ini,section);
	SelectorFreeHunting.Load(ini,section);
	SelectorMoreDeadThanAlive.Load(ini,section);
	SelectorNoWeapon.Load(ini,section);
	SelectorPursuit.Load(ini,section);
	SelectorReload.Load(ini,section);
	SelectorRetreat.Load(ini,section);
	SelectorSenseSomething.Load(ini,section);
	SelectorUnderFire.Load(ini,section);

	m_tpaDeathAnimations[0] = m_death;
	m_tpaDeathAnimations[1] = PKinematics(pVisual)->ID_Cycle_Safe("norm_death_2");
	
	m_crouch_walk.Create(PKinematics(pVisual),"cr_walk");
	m_crouch_run.Create(PKinematics(pVisual),"cr_run");
	m_crouch_idle = PKinematics(pVisual)->ID_Cycle_Safe("cr_idle");
	m_crouch_death = PKinematics(pVisual)->ID_Cycle_Safe("cr_death");

	int head_bone = PKinematics(pVisual)->LL_BoneID(ini->ReadSTRING(section,"bone_head"));
	PKinematics(pVisual)->LL_GetInstance(head_bone).set_callback(HeadSpinCallback,this);
}

void CAI_Soldier::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	//R_ASSERT(fsimilar(_view.magnitude(),1));
	//R_ASSERT(fsimilar(_move.magnitude(),1));

	CMotionDef*	S=0;

	if (iHealth<=0) {
		if (m_bCrouched)
			S = m_crouch_death;
		else {
			for (int i=0 ;i<2; i++)
				if (m_tpaDeathAnimations[i] == m_current) {
					S = m_current;
					break;
				}
			if (!S)
				S = m_tpaDeathAnimations[::Random.randI(0,2)];
		}
	} 
	else {
		if (speed<0.2f)
			if (m_bCrouched)	
				S = m_crouch_idle;
			else
				S = m_idle;
		else {
			Fvector view = _view; view.y=0; view.normalize_safe();
			Fvector move = _move; move.y=0; move.normalize_safe();
			float	dot  = view.dotproduct(move);
			
			SAnimState* AState = 0;
			if (m_bCrouched)	
				AState = &m_crouch_walk;
			else 
				AState = &m_walk;
			
			if (m_bCrouched) {
				if (speed >= m_fMaxSpeed*m_fCrounchCoefficient)
					AState = &m_crouch_run;
			}
			else		
				if (speed >= m_fMaxSpeed)
					AState = &m_run;
			
			//S = AState->fwd;
			/**/
			if (dot>0.7f)
				S = AState->fwd;
			else 
				if ((dot<=0.7f)&&(dot>=-0.7f)) {
					Fvector cross; 
					cross.crossproduct(view,move);
					if (cross.y > 0)
						S = AState->rs;
					else
						S = AState->ls;
				}
				else
					S = AState->back;
			/**/
		}
	}
	if (S!=m_current){ 
		m_current = S;
		if (S) PKinematics(pVisual)->PlayCycle(S);
	}
}

void CAI_Soldier::g_fireParams(Fvector &fire_pos, Fvector &fire_dir)
{
	//Weapons->GetFireParams(fire_pos, fire_dir);
	if (Weapons->ActiveWeapon()) {
		fire_pos.set	(Weapons->ActiveWeapon()->Position());
		fire_dir.set	(eye_matrix.k);
	}
}

float CAI_Soldier::OnVisible()
{
	float ret = inherited::OnVisible();

	Weapons->OnRender(FALSE);

	return ret;
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

	// Play hit-sound
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->Play3DAtPos(S,vPosition);
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
	q_action.setup(AI::AIC_Action::FireEnd);
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

bool CAI_Soldier::bfCheckIfCanKillMember(CAISelectorBase &S)
{
	Fvector tFireVector, tMyPosition = S.m_tMyPosition;
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

/**
float ffGetDirection(float fGamma,float fMaxOpened,float b1,float b2, float &fSquare)
{
	float a1 = (fMaxOpened - b1)/(PI/2.f), a2 = (fMaxOpened - b2)/(PI/2.f), fDelta = PI - fGamma, fAlpha;
	
	if (a1 == a2)
		fAlpha = (PI - fGamma)/2.f;
	else {
		
		float fAlpha0 = (-(a1*b1+fDelta*SQR(a2) + a2*b2) - sqrt(SQR(a1*b1 + fDelta*SQR(a2) + a2*b2) - (SQR(a1) - SQR(a2))*(SQR(b1) - SQR(fDelta*a2) - 2*fDelta*a2*b2 - SQR(b2))))/(SQR(a1) - SQR(a2));
		float fAlpha1 = (-(a1*b1+fDelta*SQR(a2) + a2*b2) + sqrt(SQR(a1*b1 + fDelta*SQR(a2) + a2*b2) - (SQR(a1) - SQR(a2))*(SQR(b1) - SQR(fDelta*a2) - 2*fDelta*a2*b2 - SQR(b2))))/(SQR(a1) - SQR(a2));
		
		if (a1 < a2)
			fAlpha = (-(a1*b1+fDelta*SQR(a2) + a2*b2) - sqrt(SQR(a1*b1 + fDelta*SQR(a2) + a2*b2) - (SQR(a1) - SQR(a2))*(SQR(b1) - SQR(fDelta*a2) - 2*fDelta*a2*b2 - SQR(b2))))/(SQR(a1) - SQR(a2));
		else
			fAlpha = (-(a1*b1+fDelta*SQR(a2) + a2*b2) + sqrt(SQR(a1*b1 + fDelta*SQR(a2) + a2*b2) - (SQR(a1) - SQR(a2))*(SQR(b1) - SQR(fDelta*a2) - 2*fDelta*a2*b2 - SQR(b2))))/(SQR(a1) - SQR(a2));
		
		if (fAlpha < 0.f)
			fAlpha = 0.f;
		else
			if (fAlpha > fDelta)
				fAlpha = fDelta;
	}
	
	float fKsi = PI- fGamma - fAlpha;
	fSquare = (CUBE(PI)*(SQR(a1) + SQR(a2))/24.f + SQR(PI)*(a1*b1 + a2*b2)/4.f + PI*(SQR(b1) + SQR(b2))/2.f - (CUBE(fAlpha)*SQR(a1)/3.f + SQR(fAlpha)*a1*b1 + fAlpha*SQR(b1) + CUBE(fKsi)*SQR(a2)/3.f + SQR(fKsi)*a2*b2 + fKsi*SQR(b2)))/2.f;

	return(fGamma/2.f + fAlpha);
}

float ffGetDirection(float fGamma,float fMaxOpened,float b1,float b2, float b3, float &fSquare)
{
	float a1 = (fMaxOpened - b1)/(PI/2.f), a2 = (fMaxOpened - b2)/(PI/2.f), a3 = (b1 - b3)/(PI/2.f), fDelta = PI - fGamma, fAlpha;
	
	if (a1 == a3)
		fAlpha = -(PI - fGamma)/2.f;
	else {
		
		if (a1 < a3)
			fAlpha = (-(a1*b1+fDelta*SQR(a3) + a3*b3) + sqrt(SQR(a1*b1 + fDelta*SQR(a3) + a3*b3) - (SQR(a1) - SQR(a3))*(SQR(b1) - SQR(fDelta*a3) - 2*fDelta*a3*b3 - SQR(b3))))/(SQR(a1) - SQR(a3));
		else
			fAlpha = (-(a1*b1+fDelta*SQR(a3) + a3*b3) - sqrt(SQR(a1*b1 + fDelta*SQR(a3) + a3*b3) - (SQR(a1) - SQR(a3))*(SQR(b1) - SQR(fDelta*a3) - 2*fDelta*a3*b3 - SQR(b3))))/(SQR(a1) - SQR(a3));
		
		if (fAlpha > 0.f)
			fAlpha = 0.f;
		else
			if (fAlpha < -fDelta)
				fAlpha = -fDelta;
	}
	
	float fKsi = PI- fGamma - fAlpha;
	fSquare = (CUBE(PI)*(SQR(a3) + SQR(a2))/24.f + SQR(PI)*(a3*b3 + a2*b2)/4.f + PI*(SQR(b3) + SQR(b2))/2.f - (CUBE(fAlpha)*SQR(a1)/3.f + SQR(fAlpha)*a1*b1 + fAlpha*SQR(b1) + CUBE(fKsi)*SQR(a3)/3.f + SQR(fKsi)*a3*b3 + fKsi*SQR(b3)))/2.f;

	return(fGamma/2.f + fAlpha);
}
/**/

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

void CAI_Soldier::SetLessCoverLook(NodeCompressed *tNode)
{
	/**
	float fAngleOfView = eye_fov/180.f*PI, fMaxSquare = -1.f, fSquare, fBestAngle, fCurrentAngle;

	for (int i=0, iSaveI = -1, iNormal = 0; i<4; i++) {
		fCurrentAngle = ffGetDirection(fAngleOfView,FN(i),FN(RIGHT_NODE(i)),FN(LEFT_NODE(i)),FN(RIGHT_NODE(RIGHT_NODE(i))),fSquare);
		if (fSquare > fMaxSquare) {
			fMaxSquare = fSquare;
			fBestAngle = fCurrentAngle;
			iSaveI = i;
			iNormal = 0;
		}
		fCurrentAngle = ffGetDirection(fAngleOfView,FN(i),FN(RIGHT_NODE(i)),FN(LEFT_NODE(i)),fSquare);
		if (fSquare > fMaxSquare) {
			fMaxSquare = fSquare;
			fBestAngle = fCurrentAngle;
			iSaveI = i;
			iNormal = 1;
		}
	}

	VERIFY(iSaveI >= 0);

	float fSinus,fCosinus;
	_sincos(fBestAngle,fSinus,fCosinus);
	switch (iSaveI) {
		case 0 : {
			tWatchDirection.set(-fSinus,0,fCosinus);
			break;
		}
		case 1 : {
			tWatchDirection.set(fCosinus,0,fSinus);
			break;
		}
		case 2 : {
			tWatchDirection.set(fSinus,0,-fCosinus);
			break;
		}
		case 3 : {
			tWatchDirection.set(-fCosinus,0,-fSinus);
			break;
		}
	}

	q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tWatchDirection), 1000);
	q_look.o_look_speed=1*_FB_look_speed;

	Fvector tTorsoDirection;
	i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tTorsoDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tTorsoDirection.square_magnitude() > 0.000001) {
			tTorsoDirection.normalize();
			SRotation r_temp;
			mk_rotation(tTorsoDirection,r_temp);
			m_fTorsoAngle = r_temp.yaw;
		}
	}
	//m_fTorsoAngle = 0;
	/**/
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.square_magnitude() > 0.000001) {
			tWatchDirection.normalize();
			mk_rotation(tWatchDirection,r_torso_target);
			//r_target.yaw = m_fTorsoAngle;
			
			float fAngleOfView = eye_fov/180.f*PI, fMaxSquare = -1.f, fBestAngle;

			/**
			Fvector tLeft,tRight,tForward,tBack;
			tLeft.set   (-1,0,0);
			tRight.set  (1,0,0);
			tForward.set(0,0,1);
			tBack.set   (0,0,-1);

			float fAngle;
			int iSaveI = -1;

			fBestAngle = 2*PI;

			fAngle = acosf(tWatchDirection.dotproduct(tLeft));
			if (fAngle < fBestAngle) {
				fBestAngle = fAngle;
				iSaveI = 0;
			}

			fAngle = acosf(tWatchDirection.dotproduct(tForward));
			if (fAngle < fBestAngle) {
				fBestAngle = fAngle;
				iSaveI = 1;
			}
			
			fAngle = acosf(tWatchDirection.dotproduct(tRight));
			if (fAngle < fBestAngle) {
				fBestAngle = fAngle;
				iSaveI = 2;
			}
			
			fAngle = acosf(tWatchDirection.dotproduct(tBack));
			if (fAngle < fBestAngle) {
				fBestAngle = fAngle;
				iSaveI = 3;
			}
			
			fCurrentAngle = ffGetDirection(fAngleOfView,FN(iSaveI),FN(RIGHT_NODE(iSaveI)),FN(LEFT_NODE(iSaveI)),fSquare);

			if (fCurrentAngle < MAX_HEAD_TURN_ANGLE)
				fAngle = MAX_HEAD_TURN_ANGLE;
			else
				if (fCurrentAngle > PI - MAX_HEAD_TURN_ANGLE)
					fAngle = PI - MAX_HEAD_TURN_ANGLE;
				else
					fAngle = fCurrentAngle;

			/**/

			/**
			for (int i=0, iSaveI = -1; i<4; i++) {
				//mk_rotation(VECTOR_DIRECTION(RIGHT_NODE(i)),r_temp);
				mk_rotation(VECTOR_DIRECTION(i),r_temp);
				float fAdditionAngle = r_temp.yaw;
				fCurrentAngle = fAdditionAngle + ffGetDirection(fAngleOfView,FN(i),FN(RIGHT_NODE(i)),FN(LEFT_NODE(i)),fSquare);
				if ((fabsf(fCurrentAngle - m_fTorsoAngle) < MAX_HEAD_TURN_ANGLE) && (fSquare > fMaxSquare)) {
					fMaxSquare = fSquare;
					fBestAngle = fCurrentAngle;
					iSaveI = i;
				}
			}
			/**/
			for (float fIncrement = r_torso_current.yaw - MAX_HEAD_TURN_ANGLE; fIncrement <= r_torso_current.yaw + MAX_HEAD_TURN_ANGLE; fIncrement += 2*MAX_HEAD_TURN_ANGLE/60.f) {
				float fSquare = ffCalcSquare(fIncrement,fAngleOfView,FN(1),FN(2),FN(3),FN(0));
				if (fSquare > fMaxSquare) {
					fMaxSquare = fSquare;
					fBestAngle = fIncrement;
				}
			}

			/**
			float fSinus,fCosinus;
			_sincos(fBestAngle,fSinus,fCosinus);
			switch (iSaveI) {
				case 0 : {
					tWatchDirection.set(-fSinus,0,fCosinus);
					break;
				}
				case 1 : {
					tWatchDirection.set(fCosinus,0,fSinus);
					break;
				}
				case 2 : {
					tWatchDirection.set(fSinus,0,-fCosinus);
					break;
				}
				case 3 : {
					tWatchDirection.set(-fCosinus,0,-fSinus);
					break;
				}
			}
			
			q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tWatchDirection), 1000);
			/**/
			r_target.yaw = fBestAngle;
			q_look.o_look_speed=_FB_look_speed;
		}
	}
	//else
		//r_target.yaw = m_fTorsoAngle;
	/**/
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
		q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tEnemyDirection), 1000);
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
		
		DWORD dwTimeDifference = S.m_dwCurTime - m_dwLastSuccessfullSearch;
		m_dwLastRangeSearch = S.m_dwCurTime;
		Device.Statistic.AI_Node.Begin();
		Squad.Groups[g_Group()].GetAliveMemberInfoWithLeader(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this,Leader);
		Device.Statistic.AI_Node.End();
		// search for the best node according to the 
		// selector evaluation function in the radius N meteres
		float fOldCost;
		Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost,dwTimeDifference);
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

void CAI_Soldier::vfAimAtEnemy()
{
	// setting up a look
	/**
	q_look.setup(
		AI::AIC_Look::Look, 
		AI::t_Object, 
		&Enemy.Enemy,
		1000);
	/**/

	//m_fTorsoAngle = r_target.yaw - EYE_WEAPON_DELTA;
	
	Fvector	pos;
	Enemy.Enemy->svCenter(pos);
	tWatchDirection.sub(pos,eye_matrix.c);
	mk_rotation(tWatchDirection,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	//setting turn speed
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

void CAI_Soldier::vfSetFire(bool bFire, CAISelectorBase &S)
{
	if (bFire) {
		if (bfCheckIfCanKillEnemy())
			if (!bfCheckIfCanKillMember(S)) {
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
}

void CAI_Soldier::vfSetMovementType(bool bCrouched, float fSpeed)
{
	if (bCrouched) {
		Squat();
		m_fCurSpeed = m_fCrounchCoefficient*fSpeed;
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
}

#define INIT_SQUAD_AND_LEADER \
	CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];\
	CEntity* Leader = Squad.Leader;\
	if (Leader->g_Health() <= 0)\
		Leader = this;\
	R_ASSERT (Leader);


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
	if ((!m_bFiring) && (Group.m_bFiring)) {
		eCurrentState = aiSoldierAttackRun;
		m_dwLastRangeSearch = 0;
		return;
	}

	if ((Weapons->ActiveWeapon()) && (Weapons->ActiveWeapon()->GetAmmoElapsed() == 0)) {
		if (m_bFiring) {
			Group.m_bFiring = false;
			m_bFiring = false;
		}
		tStateStack.push(eCurrentState);
		eCurrentState = aiSoldierReload;
		m_dwLastRangeSearch = 0;
		return;
	}

	vfSaveEnemy();

	AI_Path.TravelPath.clear();
	
	vfAimAtEnemy();
	
	vfSetFire(true,SelectorAttack);

	if (m_bFiring)
		Group.m_bFiring = true;
	else
		Group.m_bFiring = false;
	
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
	if (!(Group.m_bFiring)) {
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
	
	vfSetFire(false,SelectorAttack);

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
	q_look.setup(0,AI::t_None,0,0	);
	q_action.setup(AI::AIC_Action::FireEnd);
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
	
	vfSetFire(false,SelectorFindEnemy);
	
	vfSetMovementType(true,m_fMaxSpeed);

	bStopThinking = true;
	return;
	/**/
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
		eCurrentState = aiSoldierFreeHunting;
		return;
	}

	vfInitSelector(SelectorFollowLeader,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorFollowLeader,Squad,Leader);
		
	SetLessCoverLook(AI_Node);

	vfSetFire(false,SelectorFollowLeader);

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

	vfSetFire(false,SelectorFreeHunting);

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
		SetLessCoverLook(AI_Node);

	vfSetFire(false,SelectorNoWeapon);

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

	vfSetFire(false,SelectorPursuit);

	vfSetMovementType(true,m_fMaxSpeed);
	
	bStopThinking = true;
}

void CAI_Soldier::Reload()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Reloading weapon...");
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

	if (!Enemy.Enemy) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		q_action.setup(AI::AIC_Action::FireEnd);
		m_dwLastRangeSearch = 0;
		return;
	}
	
	DWORD dwCurTime = Level().timeServer();
	
	INIT_SQUAD_AND_LEADER;

	CGroup &Group = Squad.Groups[g_Group()];
	
	vfInitSelector(SelectorReload,Squad,Leader);

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorReload,Squad,Leader);

	tWatchDirection.sub(Enemy.Enemy->Position(),Position());
	if (tWatchDirection.magnitude() > 0.0001f)
		SetSmartLook(AI_Node,tWatchDirection);
	else
		SetLessCoverLook(AI_Node);

	vfSetFire(false,SelectorReload);
	
	if (Weapons->ActiveWeapon())
		Weapons->ActiveWeapon()->Reload();

	//if (AI_Path.TravelPath.size() <= AI_Path.TravelStart)
		vfSetMovementType(false,m_fMaxSpeed);
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
	q_look.o_look_speed=_FB_look_speed;

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

	if (dwCurTime - dwHitTime > HIT_REACTION_TIME) {
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		m_dwLastRangeSearch = 0;
		return;
	}
	
	INIT_SQUAD_AND_LEADER;
	
	vfInitSelector(SelectorUnderFire,Squad,Leader);

	CGroup &Group = Squad.Groups[g_Group()];
	Group.m_dwLastHitTime = dwHitTime;
	Group.m_tLastHitDirection = tHitDir;

	if (AI_Path.bNeedRebuild)
		vfBuildPathToDestinationPoint(0);
	else
		vfSearchForBetterPosition(SelectorUnderFire,Squad,Leader);

	mk_rotation(tHitDir,r_torso_target);
	r_target.yaw = r_torso_target.yaw;
	r_torso_target.yaw = r_torso_target.yaw - EYE_WEAPON_DELTA;
	
	q_look.o_look_speed=8*_FB_look_speed;

	vfSetFire(dwCurTime - dwHitTime < 1000,SelectorUnderFire);

	vfSetMovementType(true,m_fMaxSpeed);
	// stop processing more rules
	bStopThinking = true;
}

void CAI_Soldier::Think()
{
	bStopThinking = false;
	do {
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
	}
	while (!bStopThinking);
}
////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 23.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\..\\3dsound.h"
#include "ai_rat.h"
#include "ai_rat_selectors.h"
#include "..\\..\\hudmanager.h"

//#define WRITE_LOG

CAI_Rat::CAI_Rat()
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
	m_tpCurrentBlend = 0;
	eCurrentState = aiRatFollowMe;
	m_bMobility = true;
	m_bAttackStarted = false;
	m_bAttackFinished = true;
	m_bAttackGravitation = false;
	m_tpEnemyBeingAttacked = 0;
	m_dwLastUpdate = 0;
	m_fSpin = 0.0;
	m_fMultiplier = sinf(m_fSpin);
	AI_Path.m_bCollision = false;
}

CAI_Rat::~CAI_Rat()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void __stdcall CAI_Rat::SpinCallback(CBoneInstance* B)
{
	CAI_Rat*		M = dynamic_cast<CAI_Rat*> (static_cast<CObject*>(B->Callback_Param));
	Fmatrix				spin;

	/**
	Fvector tTorsoDirection = M->Direction();
	tTorsoDirection.invert();
	float fAlpha = acosf(M->tWatchDirection.dotproduct(tTorsoDirection));
	
	Fvector tOne;
	tOne.set(1,0,0);
	float fAlpha1 = tOne.dotproduct(tTorsoDirection);
	float fAlpha2 = tOne.dotproduct(M->tWatchDirection);
	if (fAlpha1 > fAlpha2)
		spin.rotateZ(fAlpha);
	else 
		spin.rotateZ(-fAlpha);
	/**/
	spin.setXYZ			(0, M->NET_Last.o_torso.pitch, 0);
	B->mTransform.mul_43(spin);
}

void CAI_Rat::Load(CInifile* ini, const char* section)
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
	SelectorFollow.Load(ini,section);
	SelectorFreeHunting.Load(ini,section);
	SelectorPursuit.Load(ini,section);
	SelectorUnderFire.Load(ini,section);
	
	m_fHitPower = ini->ReadFLOAT(section,"hit_power");
	m_dwHitInterval = ini->ReadINT(section,"hit_interval");

	m_tpaAttackAnimations[0] = PKinematics(pVisual)->ID_Cycle_Safe("attack_1");
	m_tpaAttackAnimations[1] = PKinematics(pVisual)->ID_Cycle_Safe("attack_2");
	m_tpaAttackAnimations[2] = PKinematics(pVisual)->ID_Cycle_Safe("attack_3");

	m_tpaDeathAnimations[0] = m_death;
	m_tpaDeathAnimations[1] = PKinematics(pVisual)->ID_Cycle_Safe("norm_death_2");
	
	//int spine_bone			= PKinematics(pVisual)->LL_BoneID("bip01_spine2");
	//PKinematics(pVisual)->LL_GetInstance(spine_bone).set_callback(SpinCallback,this);
}

BOOL CAI_Rat::Hit(int perc, Fvector &dir, CEntity* who) 
{
	if (who->Local()) {
		// *** we are REMOTE/LOCAL, attacker is LOCAL
		// send, update
		// *** signal to everyone
		NET_Packet	P;
		P.w_begin	(M_FIRE_HIT);
		P.w_u16		(u16(net_ID));
		P.w_u16		(u16(who->ID()));
		P.w_u8		(perc	);
		P.w_dir		(dir	);
		Level().Send(P,net_flags(TRUE));
	} 
	else {
		// *** we are REMOTE/LOCAL, attacker is REMOTE
		// update
	}
	
	// *** process hit calculations
	// Calc impulse
	Fvector vLocalDir;
	float m = dir.magnitude();
	R_ASSERT(m>EPS);
	
	float amount			=	2*float(perc)/Movement.GetMass();
	dir.y					+=	0.1f;
	Fvector I; I.direct		(Movement.vExternalImpulse,dir,amount/m);
	Movement.vExternalImpulse.add(I);
	
	// convert impulse into local coordinate system
	Fmatrix mInvXForm;
	mInvXForm.invert		(svTransform);
	mInvXForm.transform_dir	(vLocalDir,dir);
	vLocalDir.invert		();
	
	// Calc HitAmount
	int iHitAmount, iOldHealth=iHealth;
	if (iArmor)
	{
		iHealth		-=	(iMAX_Armor-iArmor)/iMAX_Armor*perc;
		iHitAmount	=	(perc*9)/10;
		iArmor		-=	iHitAmount;
	} else {
		iHitAmount	=	perc;
		iHealth		-=	iHitAmount;
	}
	
	// Update state and play sounds, etc
	if (iHealth<=0 && iOldHealth>0)
	{ 
		if (who!=this)	Level().HUD()->outMessage(0xffffffff,cName(),"Killed by '%s'...",who->cName());
		else			Level().HUD()->outMessage(0xffffffff,cName(),"Crashed...");
		HitSignal		(iHitAmount,vLocalDir,who);
		//Die				();
		return FALSE;
	}
	else
	{
		HitSignal		(iHitAmount,vLocalDir,who);
		return FALSE;
	}
}

// when someone hit rat
void CAI_Rat::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
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

// when someone hit rat
void CAI_Rat::SenseSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	dwSenseTime = Level().timeServer();
	tSenseDir.set(D);
}

// when rat is dead
void CAI_Rat::Death()
{
	// perform death operations
	inherited::Death( );
	q_action.setup(AI::AIC_Action::FireEnd);
	eCurrentState = aiRatDie;

	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

// rat update
void CAI_Rat::Update(DWORD DT)
{
	inherited::Update(DT);
}

float CAI_Rat::EnemyHeuristics(CEntity* E)
{
	if (E->g_Team()  == g_Team())	
		return flt_max;		// don't attack our team
	
	int	g_strench = E->g_Armor()+E->g_Health();
	
	if (g_strench <= 0)					
		return flt_max;		// don't attack dead enemiyes
	
	float	f1	= Position().distance_to_sqr(E->Position());
	float	f2	= float(g_strench);
	return  f1*f2;
}

void CAI_Rat::SelectEnemy(SEnemySelected& S)
{
	// Initiate process
	objVisible&	Known	= Level().Teams[g_Team()].KnownEnemys;
	S.Enemy					= 0;
	S.bVisible			= FALSE;
	S.fCost				= flt_max-1;
	if (Known.size()==0)	return;

	// Get visible list
	objSET		Visible;
	ai_Track.o_get	(Visible);
	std::sort			(Visible.begin(),Visible.end());

	// Iterate on known
	for (DWORD i=0; i<Known.size(); i++)
	{
		CEntity*	E = dynamic_cast<CEntity*>(Known[i].key);
		float		H = EnemyHeuristics(E);
		if (H<S.fCost) {
			// Calculate local visibility
			CObject**	ins	 = lower_bound(Visible.begin(),Visible.end(),(CObject*)E);
			bool	bVisible = (ins==Visible.end())?FALSE:((E==*ins)?TRUE:FALSE);
			float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
			if (cost<S.fCost)	{
				S.Enemy		= E;
				S.bVisible	= bVisible;
				S.fCost		= cost;
			}
		}
	}
}

bool CAI_Rat::bfCheckPath(AI::Path &Path) {
	const vector<BYTE> &q_mark = Level().AI.tpfGetNodeMarks();
	for (int i=1; i<Path.Nodes.size(); i++) 
		if (q_mark[Path.Nodes[i]])
			return(false);
	return(true);
}

IC void CAI_Rat::SetDirectionLook()
{
	int i = ps_Size();
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		//Fvector tTempPosition = tPreviousPosition.vPosition;
		//Msg("%d : %6.2f,%6.2f,%6.2f",this,tTempPosition.x,tTempPosition.y,tTempPosition.z);
		//tTempPosition = tCurrentPosition.vPosition;
		//Msg("%d : %6.2f,%6.2f,%6.2f",this,tTempPosition.x,tTempPosition.y,tTempPosition.z);
		tWatchDirection.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
		if (tWatchDirection.square_magnitude() > 0.000001) {
			tWatchDirection.normalize();
			if (tWatchDirection.y < -0.7f)
				tWatchDirection.y *= -1.f;
			if (i < 3)
				q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tWatchDirection), 1000);
			else {
				Fvector tPreviousDirection;
				tPreviousDirection.sub(ps_Element(i - 3).vPosition,ps_Element(i - 2).vPosition);
				if (tPreviousDirection.square_magnitude() > 0.000001) {
					tPreviousDirection.normalize();
					Fvector tNormal = tWatchDirection;
					tWatchDirection.reflect(tPreviousDirection,tNormal);
					//tWatchDirection.invert();
					q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tWatchDirection), 1000);
				}
				else
					q_look.setup(AI::AIC_Look::Look, AI::t_Direction, &(tWatchDirection), 1000);
			}
			q_look.o_look_speed=_FB_look_speed/1;
		}
		else {
			Msg("something's wrong!");
		}
	}
}

IC bool CAI_Rat::bfInsideSubNode(const Fvector &tCenter, const SSubNode &tpSubNode)
{
	return(((tCenter.x >= tpSubNode.tLeftDown.x) && (tCenter.z >= tpSubNode.tLeftDown.z)) && ((tCenter.x <= tpSubNode.tRightUp.x) && (tCenter.z <= tpSubNode.tRightUp.z)));
}

#define EPSILON 0.001

IC bool CAI_Rat::bfInsideSubNode(const Fvector &tCenter, const float fRadius, const SSubNode &tpSubNode)
{
	float fDist0 = SQR(tCenter.x - tpSubNode.tLeftDown.x) + SQR(tCenter.z - tpSubNode.tLeftDown.z);
	float fDist1 = SQR(tCenter.x - tpSubNode.tLeftDown.x) + SQR(tCenter.z - tpSubNode.tRightUp.z);
	float fDist2 = SQR(tCenter.x - tpSubNode.tRightUp.x) + SQR(tCenter.z - tpSubNode.tLeftDown.z);
	float fDist3 = SQR(tCenter.x - tpSubNode.tRightUp.x) + SQR(tCenter.z - tpSubNode.tRightUp.z);
	return(min(fDist0,min(fDist1,min(fDist2,fDist3))) <= fRadius*fRadius + EPSILON);
}

IC bool CAI_Rat::bfInsideNode(const Fvector &tCenter, const NodeCompressed *tpNode)
{
	/**/
	Fvector tLeftDown;
	Fvector tRightUp;
	Level().AI.UnpackPosition(tLeftDown,tpNode->p0);
	Level().AI.UnpackPosition(tRightUp,tpNode->p1);
	float fSubNodeSize = Level().AI.GetHeader().size;
	return(((tCenter.x >= tLeftDown.x - fSubNodeSize/2.f) && (tCenter.z >= tLeftDown.z - fSubNodeSize/2.f)) && ((tCenter.x <= tRightUp.x + fSubNodeSize/2.f) && (tCenter.z <= tRightUp.z + fSubNodeSize/2.f)));
	/**
	NodePosition tCenterPosition;
	Level().AI.PackPosition(tCenterPosition,tCenter);
	return(((tCenterPosition.x >= tpNode->p0.x) && (tCenterPosition.z >= tpNode->p0.z)) && ((tCenterPosition.x <= tpNode->p1.x) && (tCenterPosition.z <= tpNode->p1.z)));
	/**/
}

IC float CAI_Rat::ffComputeCost(Fvector tLeaderPosition,SSubNode &tCurrentNeighbour, const float fMinDistance)
{
	Fvector tCurrentSubNode;
	tCurrentSubNode.x = (tCurrentNeighbour.tLeftDown.x + tCurrentNeighbour.tRightUp.x)/2.f;
	//tCurrentSubNode.y = (tCurrentNeighbour.tLeftDown.y + tCurrentNeighbour.tRightUp.y)/2.f;
	tCurrentSubNode.z = (tCurrentNeighbour.tLeftDown.z + tCurrentNeighbour.tRightUp.z)/2.f;
	return(SQR(sqrt(SQR(tLeaderPosition.x - tCurrentSubNode.x) + /**0*SQR(tLeaderPosition.y - tCurrentSubNode.y)/**/ + SQR(tLeaderPosition.z - tCurrentSubNode.z)) - fMinDistance));
}

IC float CAI_Rat::ffGetY(NodeCompressed &tNode, float X, float Z)
{
	// unpack plane
	Fvector	DUP, vNorm, v, v1, P0;
	DUP.set(0,1,0);
	pvDecompress(vNorm,tNode.plane);
	Fplane PL; 
	Level().AI.UnpackPosition(P0,tNode.p0);
	PL.build(P0,vNorm);
	v.set(X,P0.y,Z);	
	PL.intersectRayPoint(v,DUP,v1);	
	//v1.direct(v1,PL.n,.01f);
	//return(v1.y + .01f);
	return(v1.y);
}

/**/
IC bool CAI_Rat::bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode)
{
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPSILON))
		return(true);
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPSILON) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPSILON))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPSILON) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPSILON))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPSILON) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPSILON))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPSILON))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPSILON))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPSILON))
		return(true);
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPSILON))
		return(true);
	// otherwise
	return(false);
}

/**
IC bool CAI_Rat::bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode)
{
	// check if it is left node
	if ((fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tRightUp.z) < EPSILON) &&
		(fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPSILON))
		return(true);
	// check if it is front node
	if ((fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z)  < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPSILON))
		return(true);
	// check if it is right node
	if ((fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPSILON) &&
		(fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPSILON))
		return(true);
	// check if it is back node
	if ((fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPSILON) &&
		(fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tRightUp.x) < EPSILON))
		return(true);
	// otherwise
	return(false);
}
/**/

/**
IC bool CAI_Rat::bfNeighbourNode(const SSubNode &tCurrentSubNode, const SSubNode &tMySubNode, const float fSubNodeSize)
{
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPSILON + fSubNodeSize) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPSILON + fSubNodeSize))
		return(true);
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPSILON + fSubNodeSize) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPSILON + fSubNodeSize))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPSILON + fSubNodeSize) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPSILON + fSubNodeSize))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPSILON + fSubNodeSize) &&
		(fabs(tCurrentSubNode.tRightUp.z - tMySubNode.tLeftDown.z) < EPSILON + fSubNodeSize))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPSILON + fSubNodeSize) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPSILON + fSubNodeSize))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tRightUp.x) < EPSILON + fSubNodeSize) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPSILON + fSubNodeSize))
		return(true);
	if ((fabs(tCurrentSubNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPSILON + fSubNodeSize) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPSILON + fSubNodeSize))
		return(true);
	if ((fabs(tCurrentSubNode.tRightUp.x - tMySubNode.tLeftDown.x) < EPSILON + fSubNodeSize) &&
		(fabs(tCurrentSubNode.tLeftDown.z - tMySubNode.tRightUp.z) < EPSILON))
		return(true);
	// otherwise
	return(false);
}
/**/

//#define MAX_NEIGHBOUR_COUNT 5
#define MAX_NEIGHBOUR_COUNT 9
//define MAX_NEIGHBOUR_COUNT 25

int CAI_Rat::ifDivideNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes)
{
	CAI_Space &AI = Level().AI;
	float fSubNodeSize = AI.GetHeader().size;
	SSubNode tNode;
	tpSubNodes.clear();
	int iCount = 0, iResult = -1;
	Fvector tLeftDown;
	Fvector tRightUp;
	AI.UnpackPosition(tLeftDown,tpStartNode->p0);
	AI.UnpackPosition(tRightUp,tpStartNode->p1);
	
	float x = tCurrentPosition.x - (tLeftDown.x - fSubNodeSize/2.f);
	float z = tCurrentPosition.z - (tLeftDown.z - fSubNodeSize/2.f);
	//iResult = floor(x/fSubNodeSize)*(floor((tRightUp.z - tLeftDown.z)/fSubNodeSize) + 1) + floor(z/fSubNodeSize);
	SSubNode tMySubNode;
	tMySubNode.tLeftDown.x = tLeftDown.x - fSubNodeSize/2.f + fSubNodeSize*floor(x/fSubNodeSize);
	tMySubNode.tLeftDown.z = tLeftDown.z - fSubNodeSize/2.f + fSubNodeSize*floor(z/fSubNodeSize);
	tMySubNode.tRightUp.x = tMySubNode.tLeftDown.x + fSubNodeSize;
	tMySubNode.tRightUp.z = tMySubNode.tLeftDown.z + fSubNodeSize;
	for (float i=tLeftDown.x - fSubNodeSize/2.f; i < tRightUp.x; i+=fSubNodeSize) {
		tNode.tLeftDown.x = i;
		tNode.tRightUp.x = i + fSubNodeSize;
		for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
			tNode.tLeftDown.z = j;
			tNode.tRightUp.z = j + fSubNodeSize;
			if (
				(iResult < 0) &&
				(fabs(tNode.tLeftDown.x - tMySubNode.tLeftDown.x) < EPSILON) &&
				(fabs(tNode.tLeftDown.z - tMySubNode.tLeftDown.z) < EPSILON) &&
				(fabs(tNode.tRightUp.x - tMySubNode.tRightUp.x) < EPSILON) &&
				(fabs(tNode.tRightUp.z - tMySubNode.tRightUp.z) < EPSILON)
				) {
				tNode.tLeftDown.y = ffGetY(*tpStartNode,i,j);
				tNode.tRightUp.y = ffGetY(*tpStartNode,i + fSubNodeSize,j + fSubNodeSize);
				tNode.bEmpty = true;
				tpSubNodes.push_back(tNode);
				iResult = iCount;
				iCount++;
			}
			else
				if (bfNeighbourNode(tNode,tMySubNode)) {
					tNode.tLeftDown.y = ffGetY(*tpStartNode,i,j);
					tNode.tRightUp.y = ffGetY(*tpStartNode,i + fSubNodeSize,j + fSubNodeSize);
					tNode.bEmpty = true;
					tpSubNodes.push_back(tNode);
					iCount++;
					if (iCount >= MAX_NEIGHBOUR_COUNT)
						break;
				}
		}
		if (iCount >= MAX_NEIGHBOUR_COUNT)
			break;
	}
	if (iCount < MAX_NEIGHBOUR_COUNT) {
		NodeLink *taLinks = (NodeLink *)((u8 *)tpStartNode + sizeof(NodeCompressed));
		int iLinkCount = tpStartNode->link_count;
		for (int k=0; k<iLinkCount; k++) {
			NodeCompressed *tpCurrentNode = AI.Node(AI.UnpackLink(taLinks[k]));
			AI.UnpackPosition(tLeftDown,tpCurrentNode->p0);
			AI.UnpackPosition(tRightUp,tpCurrentNode->p1);
			for (float i=tLeftDown.x - fSubNodeSize/2.f; i < tRightUp.x; i+=fSubNodeSize) {
				tNode.tLeftDown.x = i;
				tNode.tRightUp.x = i + fSubNodeSize;
				for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
					tNode.tLeftDown.z = j;
					tNode.tRightUp.z = j + fSubNodeSize;
					if (bfNeighbourNode(tNode,tMySubNode)) {
						tNode.tLeftDown.y = ffGetY(*tpCurrentNode,i,j);
						tNode.tRightUp.y = ffGetY(*tpCurrentNode,i + fSubNodeSize,j + fSubNodeSize);
						tNode.bEmpty = true;
						tpSubNodes.push_back(tNode);
						iCount++;
						if (iCount >= MAX_NEIGHBOUR_COUNT)
							break;
					}
				}
				if (iCount >= MAX_NEIGHBOUR_COUNT)
					break;
			}
			if (iCount >= MAX_NEIGHBOUR_COUNT)
				break;
		}
	}
	VERIFY(iResult >= 0);
	return(iResult);
}

int CAI_Rat::ifDivideNearestNode(NodeCompressed *tpStartNode, Fvector tCurrentPosition, vector<SSubNode> &tpSubNodes)
{
	CAI_Space &AI = Level().AI;
	float fSubNodeSize = AI.GetHeader().size;
	SSubNode tNode;
	tpSubNodes.clear();
	int iCount = 0, iMySubNode = -1;
	float fBestCost = 100000000.f;
	Fvector tLeftDown;
	Fvector tRightUp;
	AI.UnpackPosition(tLeftDown,tpStartNode->p0);
	AI.UnpackPosition(tRightUp,tpStartNode->p1);
	for (float i=tLeftDown.x - fSubNodeSize/2.f; i < tRightUp.x; i+=fSubNodeSize) {
		tNode.tLeftDown.x = i;
		tNode.tRightUp.x = i + fSubNodeSize;
		for (float j=tLeftDown.z - fSubNodeSize/2.f; j < tRightUp.z; j+=fSubNodeSize) {
			tNode.tLeftDown.z = j;
			tNode.tRightUp.z = j + fSubNodeSize;
			float fCurrentCost = ffComputeCost(tCurrentPosition,tNode);
			if (fCurrentCost < fBestCost) {
				tNode.tLeftDown.y = ffGetY(*tpStartNode,i,j);
				tNode.tRightUp.y = ffGetY(*tpStartNode,i + fSubNodeSize,j + fSubNodeSize);
				tNode.bEmpty = true;
				tpSubNodes.push_back(tNode);
				fBestCost = fCurrentCost;
				iMySubNode = iCount;
				iCount++;
			}
		}
	}
	VERIFY(iMySubNode >= 0);
	return(iMySubNode);
}

void CAI_Rat::FollowLeader(Fvector &tLeaderPosition, const float fMinDistance, const float fMaxDistance) 
{
	Fvector tCurrentPosition = Position();
	NodeCompressed* tpCurrentNode = AI_Node;
	if (bfInsideNode(tCurrentPosition,tpCurrentNode)) {
		// divide the nearest nodes into the subnodes 0.7x0.7 m^2
		int iMySubNode = ifDivideNode(tpCurrentNode,tCurrentPosition,tpSubNodes);
		VERIFY(iMySubNode >= 0);
		Fvector tCenter;

		int i;
		/**
		SSubNode *tpFrontSubNode=0;
		SSubNode *tpBackSubNode=0;
		SSubNode *tpLeftSubNode=0;
		SSubNode *tpRightSubNode=0;
		SSubNode &tMySubNode = tpSubNodes[iMySubNode];

		
		for ( i=0; i<tpSubNodes.size(); i++) {
			if (i == iMySubNode)
				continue;
			// check if it is left node
			if ((fabs(tpSubNodes[i].tRightUp.z - tMySubNode.tRightUp.z) < EPSILON) &&
				(fabs(tpSubNodes[i].tRightUp.x - tMySubNode.tLeftDown.x) < EPSILON))
				tpLeftSubNode = &(tpSubNodes[i]);
			// check if it is front node
			if ((fabs(tpSubNodes[i].tLeftDown.z - tMySubNode.tRightUp.z)  < EPSILON) &&
				(fabs(tpSubNodes[i].tLeftDown.x - tMySubNode.tLeftDown.x) < EPSILON))
				tpFrontSubNode = &(tpSubNodes[i]);
			// check if it is right node
			if ((fabs(tpSubNodes[i].tLeftDown.z - tMySubNode.tLeftDown.z) < EPSILON) &&
				(fabs(tpSubNodes[i].tLeftDown.x - tMySubNode.tRightUp.x) < EPSILON))
				tpRightSubNode = &(tpSubNodes[i]);
			// check if it is back node
			if ((fabs(tpSubNodes[i].tRightUp.z - tMySubNode.tLeftDown.z) < EPSILON) &&
				(fabs(tpSubNodes[i].tRightUp.x - tMySubNode.tRightUp.x) < EPSILON))
				tpBackSubNode = &(tpSubNodes[i]);
		}
		/**/

		/**/
		// filling the subnodes with the moving objects
		Level().ObjectSpace.GetNearest(Position(),3*Level().AI.GetHeader().size);
		CObjectSpace::NL_TYPE &tpNearestList = Level().ObjectSpace.nearest_list;
		//Msg("%d",tpNearestList.size());
		if (!(tpNearestList.empty())) {
			for (CObjectSpace::NL_IT tppObjectIterator=tpNearestList.begin(); tppObjectIterator!=tpNearestList.end(); tppObjectIterator++) {
				CObject* tpCurrentObject = (*tppObjectIterator)->Owner();
				if ((tpCurrentObject->CLS_ID != CLSID_ENTITY) || (tpCurrentObject == this) || (tpCurrentObject == m_tpEnemyBeingAttacked))
					continue;

				CCustomMonster* M = dynamic_cast<CCustomMonster*>(tpCurrentObject);
				if ((M) && (M->g_Health() <= 0))
					continue;

				float fRadius = 0.35f;//tpCurrentObject->Radius();
				tpCurrentObject->clCenter(tCenter);
				
				//bool bFront=0,bBack=0,bLeft=0,bRight=0;
				for (int j=0; j<tpSubNodes.size(); j++)
					if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[j])) {
						tpSubNodes[j].bEmpty = false;
						//if (&(tpSubNodes[j]) == tpFrontSubNode)
						//	bFront = true;
						//if (&(tpSubNodes[j]) == tpLeftSubNode)
						//	bLeft = true;
						//if (&(tpSubNodes[j]) == tpRightSubNode)
						//	bRight = true;
						//if (&(tpSubNodes[j]) == tpBackSubNode)
						//	bBack = true;
					}

				CAI_Rat *tMember = dynamic_cast<CAI_Rat*>(tpCurrentObject);
				if (tMember)
					if (tMember->AI_Path.TravelPath.size() > tMember->AI_Path.TravelStart + 2) {
						CAI_Space &AI = Level().AI;
						tCenter = tMember->AI_Path.TravelPath[tMember->AI_Path.TravelStart + 1].P;
						for (int j=0; j<tpSubNodes.size(); j++)
							if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[j])) {
								tpSubNodes[j].bEmpty = false;
								//if (&(tpSubNodes[j]) == tpFrontSubNode)
								//	bFront = true;
								//if (&(tpSubNodes[j]) == tpLeftSubNode)
								//	bLeft = true;
								//if (&(tpSubNodes[j]) == tpRightSubNode)
								//	bRight = true;
								//if (&(tpSubNodes[j]) == tpBackSubNode)
								//	bBack = true;
							}
						//if ((bFront && bLeft) || (bFront && bRight) || (bBack && bLeft) || (bBack && bRight)) {						
						//	for (int j=0; j<tpSubNodes.size(); j++)
						//		tpSubNodes[j].bEmpty = false;
						//	break;
						//}
					}
				/**/
			}
		}
		/**/
		// checking the nearest nodes
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;

		//Fvector tLeaderPosition = Leader->Position();
		DWORD dwTime = Level().timeServer();
		int iBestI = -1;
		float fBestCost = SQR(sqrt(SQR(tLeaderPosition.x - tCurrentPosition.x) + 0*SQR(tLeaderPosition.y - tCurrentPosition.y) + SQR(tLeaderPosition.z - tCurrentPosition.z)) - fMinDistance);
		bool bMobility = false;
		for ( i=0; i<tpSubNodes.size(); i++)
			if ((i != iMySubNode) && (tpSubNodes[i].bEmpty)) {
				bMobility = true;
				float fCurCost = ffComputeCost(tLeaderPosition,tpSubNodes[i],fMinDistance);
				if (fCurCost < fBestCost) {
					iBestI = i;
					fBestCost = fCurCost;
				}
			}

		if (bMobility) {
			m_bMobility = true;
			/**/
			if (iBestI < 0)
				if (dwTime - m_dwLastUpdate > 3000) {
					m_dwLastUpdate = dwTime;
					if (tpSubNodes.size() > 1)
						do
							iBestI = ::Random.randI(0,tpSubNodes.size());
						while (iBestI == iMySubNode);

				}
				//else
				//	m_bMobility
			/**/
			if (iBestI >= 0) {
				m_dwLastUpdate = dwTime;
				Fvector tFinishPosition;
				tFinishPosition.x = (tpSubNodes[iBestI].tLeftDown.x + tpSubNodes[iBestI].tRightUp.x)/2.f;
				tFinishPosition.y = (tpSubNodes[iBestI].tLeftDown.y + tpSubNodes[iBestI].tRightUp.y)/2.f;
				tFinishPosition.z = (tpSubNodes[iBestI].tLeftDown.z + tpSubNodes[iBestI].tRightUp.z)/2.f;
				VERIFY(tFinishPosition.x < 1000000.f);
				CTravelNode	tCurrentPoint,tFinishPoint;
				tCurrentPoint.P.set(tCurrentPosition);
				tCurrentPoint.floating = FALSE;
				AI_Path.TravelPath.push_back(tCurrentPoint);
				tFinishPoint.P.set(tFinishPosition);
				tFinishPoint.floating = FALSE;
				AI_Path.TravelPath.push_back(tFinishPoint);
			}
		}
		else {
			m_bMobility = false;
		}
	}
	else {
		int iMySubNode = ifDivideNearestNode(tpCurrentNode,tCurrentPosition,tpSubNodes);
		VERIFY(iMySubNode >= 0);
		Level().ObjectSpace.GetNearest(tpSubNodes[iMySubNode].tLeftDown,3*(Level().AI.GetHeader().size));
		CObjectSpace::NL_TYPE &tpNearestList = Level().ObjectSpace.nearest_list;
		//Msg("%d",tpNearestList.size());
		if (!tpNearestList.empty()) {
			for (CObjectSpace::NL_IT tppObjectIterator=tpNearestList.begin(); tppObjectIterator!=tpNearestList.end(); tppObjectIterator++) {
				CObject* tpCurrentObject = (*tppObjectIterator)->Owner();
				if ((tpCurrentObject->CLS_ID != CLSID_ENTITY) || (tpCurrentObject == this) || (tpCurrentObject == m_tpEnemyBeingAttacked))
					continue;
				float fRadius = tpCurrentObject->Radius();
				Fvector tCenter;
				tpCurrentObject->clCenter(tCenter);
				if (bfInsideSubNode(tCenter,fRadius,tpSubNodes[iMySubNode])) {
					tpSubNodes[iMySubNode].bEmpty = false;
					break;
				}
			}
		}
		AI_Path.TravelPath.clear();
		AI_Path.TravelStart = 0;
		if (tpSubNodes[iMySubNode].bEmpty) {
			Fvector tFinishPosition;
			tFinishPosition.x = (tpSubNodes[iMySubNode].tLeftDown.x + tpSubNodes[iMySubNode].tRightUp.x)/2.f;
			tFinishPosition.y = (tpSubNodes[iMySubNode].tLeftDown.y + tpSubNodes[iMySubNode].tRightUp.y)/2.f;
			tFinishPosition.z = (tpSubNodes[iMySubNode].tLeftDown.z + tpSubNodes[iMySubNode].tRightUp.z)/2.f;
			CTravelNode	tCurrentPoint,tFinishPoint;
			tCurrentPoint.P.set(tCurrentPosition);
			tCurrentPoint.floating = FALSE;
			AI_Path.TravelPath.push_back(tCurrentPoint);
			tFinishPoint.P.set(tFinishPosition);
			tFinishPoint.floating = FALSE;
			AI_Path.TravelPath.push_back(tFinishPoint);
			m_bMobility = true;
		}
		else
			m_bMobility = false;
	}
	DWORD dwCurTime = Level().timeServer();
	m_fSpin += PI/50;
	if (!AI_Path.TravelPath.empty())
		m_fMultiplier *= sinf(m_fSpin);
	else
		m_fMultiplier *= 0.f;
}

void CAI_Rat::InFlight()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"InFlight");
#endif
	if (!m_bAttackFinished) {
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		if (!Enemy.Enemy) {
			m_bAttackStarted = false;
			eCurrentState = tStateStack.top();
			tStateStack.pop();
			return;
		}
		if (m_bAttackGravitation) {
			q_action.setup(AI::AIC_Action::AttackBegin);
			m_tpEnemyBeingAttacked = Enemy.Enemy;
		}
		q_look.setup(AI::AIC_Look::Look, AI::t_Object, &Enemy,1000);
		q_look.o_look_speed=_FB_look_speed;
		// checking flag to stop processing more states
		m_fCurSpeed = m_fMaxSpeed;
		bStopThinking = true;
		SelectAnimation(clTransform.k,tWatchDirection,AI_Path.fSpeed);
		return;
	}
	else {
		m_bAttackStarted = false;
		eCurrentState = tStateStack.top();
		tStateStack.pop();
		return;
	}
}

void CAI_Rat::Attack()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Attack");
#endif
	//Fvector tTempPosition = Position();
	//Msg("%d : %6.2f,%6.2f,%6.2f",this,tTempPosition.x,tTempPosition.y,tTempPosition.z);
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (!(Enemy.Enemy)) {
			// did we kill the enemy ?
			if ((tSavedEnemy) && (tSavedEnemy->g_Health() <= 0)) {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
			}
			//  no, we lost him
			else 
				if (tSavedEnemy) {
					dwLostEnemyTime = Level().timeServer();
					eCurrentState = aiRatPursuit;
				}
				else {
					eCurrentState = tStateStack.top();
					tStateStack.pop();
				}
			return;
		}
		else {
			if (Enemy.bVisible) {
				float fDistance = ffGetDistance(Position(),Enemy.Enemy->Position());
				if ((Enemy.Enemy) && (fDistance < SelectorAttack.fMaxEnemyDistance) && (fDistance > SelectorAttack.fMinEnemyDistance)) {
					m_bAttackGravitation = true;
					m_bAttackStarted = true;
					m_bAttackFinished = false;
					tStateStack.push(eCurrentState);
					eCurrentState = aiRatInFlight;
					return;
				}
				else {
					/**/
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					// determining who is leader
					CEntity* Leader = Squad.Leader;
					if (!Squad.Groups[g_Group()].m_bLeaderViewsEnemy) {
						Squad.Leader = this;
						Squad.Groups[g_Group()].m_bLeaderViewsEnemy = true;
					}
					m_tpEnemyBeingAttacked = Enemy.Enemy;
					FollowLeader(Enemy.Enemy->Position(),SelectorAttack.fMinEnemyDistance);				
					float fDistance = ffGetDistance(Position(),Enemy.Enemy->Position());
					q_action.setup(AI::AIC_Action::AttackEnd);
					if (fDistance > SelectorAttack.fMaxEnemyDistance)
						SetDirectionLook();
					else {
						q_look.setup(AI::AIC_Look::Look, AI::t_Object, &(Enemy.Enemy), 1000);
						q_look.o_look_speed=8*_FB_look_speed;
					}
					// checking flag to stop processing more states
					m_fCurSpeed = m_fMaxSpeed;
					bStopThinking = true;
					return;
					/**/
				}
			}
			else {
				CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
				// determining who is leader
				CEntity* Leader = Squad.Leader;
				if (Squad.Leader == this) {
					Squad.Groups[g_Group()].m_bLeaderViewsEnemy = false;
					eCurrentState = tStateStack.top();
					tStateStack.pop();
					bStopThinking = true;
					return;
				}
				else {
					FollowLeader(Squad.Leader->Position());
					q_action.setup(AI::AIC_Action::AttackEnd);
					SetDirectionLook();
				}
				// checking flag to stop processing more states
				m_fCurSpeed = m_fMaxSpeed;
				bStopThinking = true;
				return;
			}
		}
	}
}

void CAI_Rat::Cover()
{
	bStopThinking = true;
}

void CAI_Rat::Defend()
{
	bStopThinking = true;
}

void CAI_Rat::Die()
{
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);

	if (!m_bAttackFinished)
		return;

	q_look.setup(0,AI::t_None,0,0	);
	q_action.setup(AI::AIC_Action::AttackEnd);
	AI_Path.TravelPath.clear();
	
	bActive = false;
	bEnabled = false;

	if (m_tpEnemyBeingAttacked) {
		Fvector tPosition = Position();
		float fY = ffGetY(*AI_Node,tPosition.x,tPosition.z);
		if (tPosition.y - fY > 0.01f) {
			Fvector tAcceleration;
			tAcceleration.sub(m_tpEnemyBeingAttacked->Position(),Position());
			vfNormalizeSafe(tAcceleration);
			tAcceleration.mul(5);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,0,0.01f,false);
			Movement.GetPosition(vPosition);
		}
	}

	bStopThinking = true;
}

void CAI_Rat::FollowMe()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Follow me");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
		bStopThinking = true;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (Enemy.Enemy) {
			tStateStack.push(eCurrentState);
			eCurrentState = aiRatAttack;
			bStopThinking = true;
			return;
		}
		else {
			// checking if I am under fire
			DWORD dwCurTime = Level().timeServer();
			if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
				tStateStack.push(eCurrentState);
				eCurrentState = aiRatUnderFire;
				bStopThinking = true;
				return;
			}
			else {
				if ((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime)) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiRatSenseSomething;
					bStopThinking = true;
					return;
				}
				else {
					// determining the team
					//Fvector tTempPosition = Position();
					//Msg("%d : %6.2f,%6.2f,%6.2f",this,tTempPosition.x,tTempPosition.y,tTempPosition.z);
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					// determining who is leader
					CEntity* Leader = Squad.Leader;
					// checking if the leader exists
					R_ASSERT (Leader);
					// checking if leader is dead then make myself a leader
					if ((Leader != this) && ((Leader->g_Health() <= 0) || (!(Leader->m_bMobility)))) {
						Leader = this;
						Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
					}
					// I am leader then go to state "Free Hunting"
					if ((Leader == this) || (Leader->g_Health() <= 0) || (!(Leader->m_bMobility))) {
						Leader = this;
						eCurrentState = aiRatFreeHunting;
						bStopThinking = true;
						return;
					}
					else {
						FollowLeader(Leader->Position());
						// setting up a look
						SetDirectionLook();
						// setting up an action
						q_action.setup(AI::AIC_Action::AttackEnd);
						// checking flag to stop processing more states
						m_fCurSpeed = m_fMinSpeed;
						bStopThinking = true;
						return;
					}
				}
			}
		}
	}
}

void CAI_Rat::FreeHunting()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Free hunting");
#endif
	//Fvector tTempPosition = Position();
	//Msg("%d : %6.2f,%6.2f,%6.2f",this,tTempPosition.x,tTempPosition.y,tTempPosition.z);
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		/**/
		if (Enemy.Enemy)		{
			tStateStack.push(eCurrentState);
			eCurrentState = aiRatAttack;
			return;
		}
		else {
			// checking if I am under fire
			DWORD dwCurTime = Level().timeServer();
			if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
				tStateStack.push(eCurrentState);
				eCurrentState = aiRatUnderFire;
				bStopThinking = true;
				return;
			}
			else {
				if ((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime)) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiRatSenseSomething;
					return;
				}
				else {
					// determining the team
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					// determining who is leader
					CEntity* Leader = Squad.Leader;
					// checking if the leader exists
					R_ASSERT (Leader);
					// checking if leader is dead then make myself a leader
					if ((Leader != this) && ((Leader->g_Health() <= 0) || (!(Leader->m_bMobility)))) {
						Leader = this;
						Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
						//Msg("%d",Squad.Groups[g_Group()].m_dwLeaderChangeCount);
					}
					
					if (Leader == this) {
						// setting up watch mode to false
						bool bWatch = false;
						// get pointer to the class of node estimator 
						// for finding the best node in the area
						CRatSelectorFreeHunting S = SelectorFreeHunting;
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
						
						S.m_tMe				= this;
						S.m_tpMyNode		= AI_Node;
						S.m_tMyPosition		= Position();
						
						S.m_tEnemy			= 0;
						S.m_tEnemyPosition.set(0,0,0);
						S.m_tpEnemyNode		= NULL;
						
						//S.taMembers = Squad.Groups[g_Group()].Members;
						// checking if I need to rebuild the path i.e. previous search
						// has found better destination node
						if (AI_Path.bNeedRebuild) {
							/**
							Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
							if (AI_Path.Nodes.size() > 2) {
							// if path is long enough then build travel line
								AI_Path.BuildTravelLine(Position());
								//m_bMobility = true;
							}
							else {
							// if path is too short then clear it (patch for ExecMove)
								AI_Path.TravelPath.clear();
								AI_Path.bNeedRebuild = FALSE;
							}
							//if (AI_Path.Nodes.size() < 2)
							//	m_bMobility = false;
							/**/
							Fvector tLeftDown;
							Fvector tRightUp;
							Fvector tCenter;
							NodeCompressed* Node = Level().AI.Node(AI_Path.DestNode);
							Level().AI.UnpackPosition(tLeftDown,Node->p0);
							Level().AI.UnpackPosition(tRightUp,Node->p1);
							tCenter.add(tLeftDown,tRightUp);
							tCenter.div(2.f);
							FollowLeader(tCenter);
						} 
						else {
							// fill arrays of members and exclude myself
							Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
							// SelectFollow evaluation function in the radius 35 meteres
							float fOldCost;
							Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
							// if search has found new best node then 
							//if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
							if (((AI_Path.DestNode != S.BestNode)) && (S.BestCost < (fOldCost - S.fLaziness))){
								AI_Path.DestNode		= S.BestNode;
								AI_Path.bNeedRebuild	= TRUE;
								//m_bMobility = true;
							} 
							else {
								// search hasn't found a better node we have to look around
								bWatch = true;
								//if (AI_Path.TravelPath.size() < 2)
								//	m_bMobility = false;
							}
							if (AI_Path.TravelPath.size() <= 2)
								AI_Path.bNeedRebuild	= TRUE;
						}
			/**/
					}
					else {
						FollowLeader(Leader->Position());
					}
					// setting up a look
					SetDirectionLook();
					q_action.setup(AI::AIC_Action::AttackEnd);
					// checking flag to stop processing more states
					m_fCurSpeed = m_fMinSpeed;
					bStopThinking = true;
					return;
				}
			}
		}
		/**/
	}
}

void CAI_Rat::GoInThisDirection()
{
}

void CAI_Rat::GoToThisPosition()
{
}

void CAI_Rat::HoldPositionUnderFire()
{
	bStopThinking = true;
}

void CAI_Rat::HoldThisPosition()
{
	bStopThinking = true;
}

void CAI_Rat::Pursuit()
{
	// if no more health then rat is dead
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Pursuit");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do the enemies exist?
		if (Enemy.Enemy) {
			eCurrentState = aiRatAttack;
			return;
		}
		else {
			DWORD dwCurrentTime = Level().timeServer();
			if (dwCurrentTime - dwLostEnemyTime < LOST_ENEMY_REACTION_TIME) {
				// checking if I am under fire
				DWORD dwCurTime = Level().timeServer();
				if ((dwCurTime - dwHitTime < HIT_JUMP_TIME) && (dwHitTime)) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiRatUnderFire;
					bStopThinking = true;
					return;
				}
				else {
					if ((dwCurTime - dwSenseTime < SENSE_JUMP_TIME) && (dwSenseTime)) {
						tStateStack.push(eCurrentState);
						eCurrentState = aiRatSenseSomething;
						return;
					}
					else {
						// determining the team
						CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
						// determining who is leader
						CEntity* Leader = Squad.Leader;
						// checking if the leader exists
						R_ASSERT (Leader);
						// checking if leader is dead then make myself a leader
						if ((Leader != this) && ((Leader->g_Health() <= 0) || (!(Leader->m_bMobility)))) {
							Leader = this;
							Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
							//Msg("%d",Squad.Groups[g_Group()].m_dwLeaderChangeCount);
						}
						// get pointer to the class of node estimator 
						// for finding the best node in the area
						CRatSelectorPursuit S = SelectorPursuit;
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
						
						S.m_tMe				= this;
						S.m_tpMyNode		= AI_Node;
						S.m_tMyPosition		= Position();
						
						S.m_tEnemy			= tSavedEnemy;
						S.m_tEnemyPosition	= tSavedEnemyPosition;
						S.m_tpEnemyNode		= tpSavedEnemyNode;
						
						//S.taMembers = Squad.Groups[g_Group()].Members;
						bool bWatch = false;
						// checking if I need to rebuild the path i.e. previous search
						// has found better destination node
						if (AI_Path.bNeedRebuild) {
							/**
							Level().AI.vfFindTheXestPath(AI_NodeID,AI_Path.DestNode,AI_Path);
							if (AI_Path.Nodes.size() > 2) {
							// if path is long enough then build travel line
								AI_Path.BuildTravelLine(Position());
								m_bMobility = true;
							}
							else {
							// if path is too short then clear it (patch for ExecMove)
								AI_Path.TravelPath.clear();
								AI_Path.bNeedRebuild = FALSE;
								m_bMobility = false;
							}
							/**/
							Fvector tLeftDown;
							Fvector tRightUp;
							Fvector tCenter;
							NodeCompressed* Node = Level().AI.Node(AI_Path.DestNode);
							Level().AI.UnpackPosition(tLeftDown,Node->p0);
							Level().AI.UnpackPosition(tRightUp,Node->p1);
							tCenter.add(tLeftDown,tRightUp);
							tCenter.div(2.f);
							FollowLeader(tCenter);
						} 
						else {
							// fill arrays of members and exclude myself
							Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
							// search for the best node according to the 
							// SelectFollow evaluation function in the radius 35 meteres
							float fOldCost;
							Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
							// if search has found new best node then 
							if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
								AI_Path.DestNode		= S.BestNode;
								AI_Path.bNeedRebuild	= TRUE;
							} 
							else {
								// search hasn't found a better node we have to look around
								bWatch = true;
								m_bMobility = false;
							}
							if (AI_Path.TravelPath.size() < 2)
								AI_Path.bNeedRebuild	= TRUE;
						}
						// setting up a look
						// getting my current node
						NodeCompressed* tNode		= Level().AI.Node(AI_NodeID);
						SetDirectionLook();
						// checking flag to stop processing more states
						q_action.setup(AI::AIC_Action::FireEnd);
						bStopThinking = true;
						m_fCurSpeed = m_fMaxSpeed;
						return;
					}
				}
			}
			else {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
				q_action.setup(AI::AIC_Action::FireEnd);
				m_fCurSpeed = m_fMaxSpeed;
				bStopThinking = true;
				return;
			}
		}
	}
}

void CAI_Rat::Retreat()
{
	bStopThinking = true;
}

void CAI_Rat::SenseSomething()
{
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

void CAI_Rat::UnderFire()
{
#ifdef WRITE_LOG
	Msg("creature : %s, mode : %s",cName(),"Under fire");
#endif
	if (g_Health() <= 0) {
		eCurrentState = aiRatDie;
		return;
	}
	else {
		// selecting enemy if any
		SEnemySelected	Enemy;
		SelectEnemy(Enemy);
		// do I see the enemies?
		if (Enemy.Enemy)		{
			tStateStack.push(eCurrentState);
			eCurrentState = aiRatAttack;
			return;
		}
		else {
			// checking if I am under fire
			DWORD dwCurTime = Level().timeServer();
			if (dwCurTime - dwHitTime > HIT_REACTION_TIME) {
				eCurrentState = tStateStack.top();
				tStateStack.pop();
				return;
			}
			else {
				if (dwCurTime - dwSenseTime < SENSE_JUMP_TIME) {
					tStateStack.push(eCurrentState);
					eCurrentState = aiRatSenseSomething;
					return;
				}
				else {
					// determining the team
					CSquad&	Squad = Level().Teams[g_Team()].Squads[g_Squad()];
					// determining who is leader
					CEntity* Leader = Squad.Leader;
					// checking if the leader exists
					R_ASSERT (Leader);
					// checking if leader is dead then make myself a leader
					if ((Leader != this) && ((Leader->g_Health() <= 0) || (!(Leader->m_bMobility)))) {
						Leader = this;
						Squad.Groups[g_Group()].m_dwLeaderChangeCount++;
						//Msg("%d",Squad.Groups[g_Group()].m_dwLeaderChangeCount);
					}
					// I am leader then go to state "Free Hunting"
					bool bWatch = false;
					// get pointer to the class of node estimator 
					// for finding the best node in the area
					CRatSelectorUnderFire S = SelectorUnderFire;
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
					
					S.m_tMe				= this;
					S.m_tpMyNode		= AI_Node;
					S.m_tMyPosition		= Position();
					
					S.m_tEnemy			= 0;
					S.m_tEnemyPosition.set(0,0,0);
					S.m_tpEnemyNode		= NULL;
					
					//S.taMembers = Squad.Groups[g_Group()].Members;
					// checking if I need to rebuild the path i.e. previous search
					// has found better destination node
					if (AI_Path.bNeedRebuild) {
						/**
						// building a path from and to
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
						/**/
						Fvector tLeftDown;
						Fvector tRightUp;
						Fvector tCenter;
						NodeCompressed* Node = Level().AI.Node(AI_Path.DestNode);
						Level().AI.UnpackPosition(tLeftDown,Node->p0);
						Level().AI.UnpackPosition(tRightUp,Node->p1);
						tCenter.add(tLeftDown,tRightUp);
						tCenter.div(2.f);
						FollowLeader(tCenter);
					} 
					else {
						// fill arrays of members and exclude myself
						Squad.Groups[g_Group()].GetAliveMemberInfo(S.taMemberPositions, S.taMemberNodes, S.taDestMemberPositions, S.taDestMemberNodes, this);
						// search for the best node according to the 
						// SelectFollow evaluation function in the radius 35 meteres
						float fOldCost;
						Level().AI.q_Range(AI_NodeID,Position(),S.fSearchRange,S,fOldCost);
						// if search has found new best node then 
						if (((AI_Path.DestNode != S.BestNode) || (!bfCheckPath(AI_Path))) && (S.BestCost < (fOldCost - S.fLaziness))){
							AI_Path.DestNode		= S.BestNode;
							AI_Path.bNeedRebuild	= TRUE;
						}
						else
							// search hasn't found a better node we have to look around
							bWatch = true;
						if (AI_Path.TravelPath.size() < 2)
							AI_Path.bNeedRebuild	= TRUE;
					}
					// setting up a look
					// getting my current node
					NodeCompressed* tNode = Level().AI.Node(AI_NodeID);
					// if we are going somewhere
					SetDirectionLook();
					if (dwCurTime - dwHitTime < HIT_JUMP_TIME) {
						m_fCurSpeed = m_fMinSpeed;
					}
					else {
						q_action.setup(AI::AIC_Action::FireEnd);
						m_fCurSpeed = m_fMaxSpeed;
					}
					// setting up look speed
					q_look.o_look_speed=_FB_look_speed;
					// checking flag to stop processing more states
					bStopThinking = true;
					return;
				}
			}
		}
	}
}

void CAI_Rat::WaitOnPosition()
{
	bStopThinking = true;
}

void CAI_Rat::Think()
{
	bStopThinking = false;
	do {
		switch(eCurrentState) {
			case aiRatDie : {
				Die();
				break;
			}
			case aiRatUnderFire : {
				UnderFire();
				break;
			}
			case aiRatSenseSomething : {
				SenseSomething();
				break;
			}
			case aiRatGoInThisDirection : {
				GoInThisDirection();
				break;
			}
			case aiRatGoToThisPosition : {
				GoToThisPosition();
				break;
			}
			case aiRatWaitOnPosition : {
				WaitOnPosition();
				break;
			}
			case aiRatHoldThisPosition : {
				HoldThisPosition();
				break;
			}
			case aiRatHoldPositionUnderFire : {
				HoldPositionUnderFire();
				break;
			}
			case aiRatFreeHunting : {
				FreeHunting();
				break;
			}
			case aiRatFollowMe : {
				FollowMe();
				break;
			}
			case aiRatAttack : {
				Attack();
				break;
			}
			case aiRatDefend : {
				Defend();
				break;
			}
			case aiRatPursuit : {
				Pursuit();
				break;
			}
			case aiRatRetreat : {
				Retreat();
				break;
			}
			case aiRatCover : {
				Cover();
				break;
			}
			case aiRatInFlight : {
				InFlight();
				break;
			}
		}
	}
	while (!bStopThinking);
}

void CAI_Rat::net_Export(NET_Packet* P)					// export to server
{
	R_ASSERT				(net_Local);

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P->w_u32				(N.dwTimeStamp);
	P->w_u8					(0);
	P->w_vec3				(N.p_pos);
	P->w_angle8				(N.o_model);
	P->w_angle8				(N.o_torso.yaw);
	P->w_angle8				(N.o_torso.pitch);
}

void CAI_Rat::net_Import(NET_Packet* P)
{
	R_ASSERT				(!net_Local);
	net_update				N;

	u8 flags;
	P->r_u32				(N.dwTimeStamp);
	P->r_u8					(flags);
	P->r_vec3				(N.p_pos);
	P->r_angle8				(N.o_model);
	P->r_angle8				(N.o_torso.yaw);
	P->r_angle8				(N.o_torso.pitch);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	bVisible				= TRUE;
	bEnabled				= TRUE;
}

void CAI_Rat::SelectAnimation(const Fvector& _view, const Fvector& _move, float speed)
{
	//R_ASSERT(fsimilar(_view.magnitude(),1));
	//R_ASSERT(fsimilar(_move.magnitude(),1));

	CMotionDef*	S=0;
	if ((iHealth<=0)){// && ((!m_bAttackStarted) || (m_bAttackFinished))) {
		for (int i=0 ;i<2; i++)
			if (m_tpaDeathAnimations[i] == m_current) {
				S = m_current;
				break;
			}
		if (!S)
			S = m_tpaDeathAnimations[::Random.randI(0,2)];
	}
	else {
		if (m_bAttackStarted) {
			if (m_tpCurrentBlend) {
				if (m_current != m_tpaAttackAnimations[0]) {
					m_current = 0;
					S = m_tpaAttackAnimations[0];
				}
				else
					if (m_tpCurrentBlend->playing)
						S = m_current;
					else {
						m_bAttackFinished = true;
						S = 0;
					}
			}
			else {
				m_current = 0;
				S = m_tpaAttackAnimations[0];
			}
		}
		else {
			if (speed<0.2f)
				S = m_idle;
			else {
				Fvector view = _view; 
				Fvector move = _move; 
				view.y=0; 
				move.y=0; 
				view.normalize_safe();
				move.normalize_safe();
				float	dot  = view.dotproduct(move);
				
				SAnimState* AState = &m_walk;
				
				if (speed > m_fMinSpeed)
					AState = &m_run;
				
				S = AState->fwd;
			}
		}
	}
	if (S != m_current){ 
		m_current = S;
		if (S)
			m_tpCurrentBlend = PKinematics(pVisual)->PlayCycle(S);
		else
			m_tpCurrentBlend = 0;
	}
}

void CAI_Rat::Exec_Action	( float dt )
{
	//*** process action commands
	AI::C_Command* C	= &q_action;
	AI::AIC_Action* L	= (AI::AIC_Action*)C;
	switch (L->Command) {
		case AI::AIC_Action::AttackBegin: {
			
			DWORD dwTime = Level().timeServer();
			
			//if (m_bAttackGravitation)
			//	m_dwAttackStartTime = dwTime;
			
			q_action.Command = AI::AIC_Action::AttackEnd;
			//if (dwTime - m_dwAttackStartTime > m_dwHitInterval) {
				
				//m_dwAttackStartTime = dwTime;
				
				Fvector tDirection;
				tDirection.sub(m_tpEnemyBeingAttacked->Position(),this->Position());
				vfNormalizeSafe(tDirection);
				
				if ((this->Local()) && (m_tpEnemyBeingAttacked) && (m_tpEnemyBeingAttacked->CLS_ID == CLSID_ENTITY))
					if (m_tpEnemyBeingAttacked->g_Health() > 0)
						m_tpEnemyBeingAttacked->Hit(m_fHitPower,tDirection,this);
					else {
						m_bAttackGravitation = false;
						m_bAttackStarted = false;
						m_bAttackFinished = true;
					}
			//}

			break;
		}
		case AI::AIC_Action::AttackEnd: {
			//m_bAttackStarted = false;
			break;
		}
		default:
			break;
	}
	if (Device.dwTimeGlobal>=L->o_timeout)	
		L->setTimeout();
}

void CAI_Rat::Exec_Movement	( float dt )
{
	if (!m_bAttackStarted)
		AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
	else {
		/**/
		if ((m_tpEnemyBeingAttacked) && (!m_bAttackFinished)) {
			UpdateTransform	();
			if (m_bAttackGravitation) {
				//Msg("calc gravity %d",Level().timeServer());
				Fvector tAcceleration, tVelocity;
				tAcceleration.sub(m_tpEnemyBeingAttacked->Position(),Position());
				vfNormalizeSafe(tAcceleration);
				tVelocity = tAcceleration;
				tAcceleration.mul(5);
				tVelocity.mul(m_fCurSpeed);
				Movement.SetPosition(vPosition);
				Movement.SetVelocity(tVelocity);
				tAcceleration.set(0,0,0);
				Movement.Calculate	(tAcceleration,0,7,dt,false);
				Movement.GetPosition(vPosition);
				m_bAttackGravitation = false;
			}
			else {
				Fvector tPosition = Position();
				float fY = ffGetY(*AI_Node,tPosition.x,tPosition.z);
				if (tPosition.y - fY > 0.01f) {
					Fvector tAcceleration;
					tAcceleration.sub(m_tpEnemyBeingAttacked->Position(),Position());
					vfNormalizeSafe(tAcceleration);
					tAcceleration.mul(5);
					Movement.SetPosition(vPosition);
					Movement.Calculate	(tAcceleration,0,0,dt,false);
					Movement.GetPosition(vPosition);
				}
			}
			UpdateTransform	();
		}
		else
		/**/
			AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
	}
}
////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.cpp
//	Created 	: 05.04.2002
//  Modified 	: 12.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\..\\3dsound.h"
//#include "..\\..\\entity.h"
//#include "..\\..\\..\\_matrix.h"
#include "ai_hen.h"
#include "ai_hen_wait_pos.h"
#include "ai_hen_sense_s.h"
#include "ai_hen_sense.h"
#include "ai_hen_attack.h"
#include "ai_hen_cover.h"
#include "ai_hen_defend.h"
#include "ai_hen_die.h"
#include "ai_hen_f_hunting.h"
#include "ai_hen_follow.h"
#include "ai_hen_go_dir.h"
#include "ai_hen_go_pos.h"
#include "ai_hen_h_pos_u_f.h"
#include "ai_hen_hold_pos.h"
#include "ai_hen_pursuit.h"
#include "ai_hen_retreat.h"
#include "ai_hen_u_fire.h"

CAI_Hen::CAI_Hen()
{
	// creating initial state
	State_Push(new AI::_HenFollowMe());
}

CAI_Hen::~CAI_Hen()
{
	// removing all data no more being neded 
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void CAI_Hen::Load(CInifile* ini, const char* section)
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
}

// when someone hit hen
void CAI_Hen::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	State_Get()->Hit(D);

	// Play hit-sound
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->Play3DAtPos(S,vPosition);
}

// when someone hit hen
void CAI_Hen::SenseSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	State_Get()->Sense(D);
}

// when hen is dead
void CAI_Hen::Die		( )
{
	// perform death operations
	inherited::Die			( );
	q_action.setup			(AI::AIC_Action::FireEnd);
	State_Push				(new AI::_HenDie());
	Fvector				dir;
	AI_Path.Direction	(dir);
	SelectAnimation			(clTransform.k,dir,AI_Path.fSpeed);

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

// hen update
void CAI_Hen::Update	(DWORD DT)
{
	inherited::Update	(DT);
}

namespace AI_Space {

	HenState::State(AI_States T) : Type(T) { 
		svTime_Create = Level().timeServer(); 
	};
	
	HenState* HenState::Create(DWORD type)
	{
		EnemySaved = 0;
		hitTime = 0;
		hitDir.set(0,0,1);
		senseTime = 0;
		senseDir.set(0,0,1);
		/**
		switch (type) {
			case aiHenDie : return(new HenDie);
			case aiHenUnderFire : return(new HenUnderFire);
			case aiHenSenseSomething : return(new HenSenseSomething);
			case aiHenGoInThisDirection : return(new HenGoInThisDirection);
			case aiHenGoToThisPosition : return(new HenGoToThisPosition);
			case aiHenWaitOnPosition : return(new HenWaitOnPosition);
			case aiHenHoldThisPosition : return(new HenHoldThisPosition);
			case aiHenHoldPositionUnderFire : return(new HenHoldPositionUnderFire);
			case aiHenFreeHunting : return(new HenFreeHunting);
			case aiHenFollowMe : return(new HenFollowMe);
			case aiHenAttack : return(new HenAttack);
			case aiHenDefend : return(new HenDefend);
			case aiHenPursuit : return(new HenPursuit);
			case aiHenRetreat : return(new HenRetreat);
			case aiHenCover : return(new HenCover);
		}
		/**/
		return NULL;
	}
}

namespace AI {

	virtual void HenState::Hit(Fvector& D)
	{
		hitTime				= Level().timeServer();
		// saving hit direction
		hitDir.set			(D);
	}

	virtual void HenState::Sense(Fvector& D)
	{
		senseTime				= Level().timeServer();
		// saving hit direction
		senseDir.set			(D);
	}

	IC virtual float HenState::EnemyHeuristics(CCustomMonster* Me, CEntity* E)
	{
		if (E->g_Team()  == Me->g_Team())	return flt_max;		// don't attack our team
		int	g_strench						= E->g_Armor()+E->g_Health();
		if (g_strench<=0)					return flt_max;		// don't attack dead enemiyes
		
		float	f1	= Me->Position().distance_to_sqr(E->Position());
		float	f2	= float(g_strench);
		return  f1*f2;
	}

	virtual void HenState::SelectEnemy(EnemySelected& S, CCustomMonster* Me)
	{
		// Initiate process
		objVisible&	Known	= Level().Teams[Me->g_Team()].KnownEnemys;
		S.E					= 0;
		S.Visible			= FALSE;
		S.Cost				= flt_max-1;
		if (Known.size()==0)	return;

		// Get visible list
		objSET		Visible;
		Me->ai_Track.o_get	(Visible);
		std::sort			(Visible.begin(),Visible.end());

		// Iterate on known
		for (DWORD i=0; i<Known.size(); i++)
		{
			CEntity*	E = (CEntity*)Known[i].key;
			float		H = vfEnemyHeuristics(Me,E);
			if (H<S.Cost) {
				// Calculate local visibility
				CObject**	ins	 = lower_bound(Visible.begin(),Visible.end(),(CObject*)E);
				BOOL	bVisible = (ins==Visible.end())?FALSE:((E==*ins)?TRUE:FALSE);
				float	cost	 = H*(bVisible?1:_FB_invisible_hscale);
				if (cost<S.Cost)	{
					S.E			= E;
					S.Visible	= bVisible;
					S.Cost		= cost;
				}
			}
		}
	}
};

virtual	void SelectorBase::Load	(CInifile* ini, const char* section)
{
	sscanf(ini->ReadSTRING(section,Name),
		"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
		fDistanceWeight,
		
		fLightWeight,
		
		fCoverFromLeaderWeight,
		
		fCoverFromMembersWeight,
		
		fCoverFromEnemyWeight,

		fOptimalLeaderDistance,
		fLeaderDistanceWeight,
		
		fOptimalMemberDistance,
		fMemberDistanceWeight,
		
		fOptimalEnemyDistance,
		fEnemyDistanceWeight,
		
		fMaxHeightDistance,
		fMaxHeightDistanceWeight,

		fMemberViewDeviationWeight,
		
		fEnemyViewDeviationWeight,

		fTotalViewVectorWeight,

		fLaziness
	);
}
/**
IC float SelectorBase::h_factor(int s1, int s2, float f1, float f2, BYTE* data)
{
	return	(float(data[s1])*_abs(f1) + float(data[s2])*_abs(f2))/255.f;
}

IC float SelectorBase::h_slerp(Fvector& T, Fvector& S, BYTE* data)
{
	Fvector2	D;
	D.x			= T.x - S.x;
	D.y			= T.z - S.z;
	D.norm		();

	if (D.y>0)	{
		// 0-1-2
		if (D.x>0)	return h_factor(1,2,D.y,D.x,data);	// 1-2
		else		return h_factor(0,1,D.x,D.y,data);	// 0-1
	} else {
		// 2-3-0
		if (D.x>0)	return h_factor(2,3,D.x,D.y,data);	// 2-3
		else		return h_factor(3,0,D.y,D.x,data);	// 3-0
	}
}
/**/

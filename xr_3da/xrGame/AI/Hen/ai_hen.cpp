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

namespace AI {
	CAI_Hen::CAI_Hen()
	{
		// creating initial state
		//State_Push(new AI::CHenFollowMe());
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
		//State_Get()->Hit(D);

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
		//State_Get()->Sense(D);
	}

	// when hen is dead
	void CAI_Hen::Die		( )
	{
		// perform death operations
		inherited::Die			( );
		q_action.setup			(AI::AIC_Action::FireEnd);
		//State_Push				(new CHenDie());
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

	void CHenSelectorBase::Load(CInifile* ini, const char* section)
	{
		sscanf(ini->ReadSTRING(section,Name),
			"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
			fTravelWeight,
			
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
	};
	void CAI_Hen::Think()
	{
	}
};

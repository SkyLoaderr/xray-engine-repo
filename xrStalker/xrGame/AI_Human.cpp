// AI_Human.cpp: implementation of the CAI_Human class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_Human.h"
#include "xr_weapon_list.h"
#include "..\\3dsound.h"

IC float sqr(float a) { return a*a; }

const float	ai_hit_relevant_time	= 5.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Human::CAI_Human()
{
	State_Push(new AI::_FollowBackup());
}

CAI_Human::~CAI_Human()
{
	int i;
	for (i=0; i<SND_HIT_COUNT; i++) pSounds->Delete3D(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete3D(sndDie[i]);
}

void	CAI_Human::Load	(CInifile* ini, const char* section)
{ 
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

void	CAI_Human::HitSignal(int amount, Fvector& vLocalDir, CEntity* who)
{
	// Save event
	Fvector D;
	svTransform.transform_dir(D,vLocalDir);
	State_Get()->Hit		(D);

	// Play hit-sound
	sound3D& S = sndHit[Random.randI(SND_HIT_COUNT)];
	if (S.feedback)			return;
	if (Random.randI(2))	return;
	pSounds->Play3DAtPos(S,vPosition);
}

void	CAI_Human::Die		( )
{
	inherited::Die			( );
	q_action.setup			(AI::AIC_Action::FireEnd);
	State_Push				(new AI::_Die());

	// Play sound
	pSounds->Play3DAtPos(sndDie[Random.randI(SND_DIE_COUNT)],vPosition);
}

void CAI_Human::Update	(DWORD DT)
{
	inherited::Update	(DT);
}

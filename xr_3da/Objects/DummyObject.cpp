// DummyObject.cpp: implementation of the CDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DummyObject.h"
#include "..\ObjectAnimator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDummyObject::CDummyObject	()
{
	animator				= NULL;
	style					= 0;
}

CDummyObject::~CDummyObject	()
{
	_DELETE					(animator);
	pSounds->Delete			(sndDummy);
}

void CDummyObject::Load		(LPCSTR section)
{
	/*
	inherited::Load			(section);

	if (pSettings->LineExists(section,"motions")){
		style			|= esAnimated;
		animator		= new CObjectAnimator();
		animator->Load	(section);
		animator->PlayMotion("idle",true);
		start_position.set(vPosition);
	}
	if (pVisual->Type==MT_SKELETON){
		style			|= esSkeleton;
		PKinematics(pVisual)->PlayCycle	("idle");
	}
	if (pSettings->LineExists(section,"sound"))
	{
		LPCSTR N = pSettings->ReadSTRING(section,"sound");
		pSounds->Create(sndDummy,TRUE,N);
	}

	Sector_Detect();
	*/
}

void CDummyObject::Update	(DWORD dt)
{
	/*
	if (style&esAnimated){
		animator->OnMove();
		mRotate.set		(animator->GetRotate());
		vPosition.set	(animator->GetPosition());
		vPosition.add	(start_position);
		UpdateTransform	();
	}

	Fsphere& S = CFORM()->GetSphere();
	if (sndDummy.feedback)	sndDummy.feedback->SetPosition(S.P);
	*/
}
/*
void CDummyObject::Spawn()

void CDummyObject::PlayDemo		(LPCSTR N)
{
	animator->StopMotion();
	animator->PlayMotion(N,false);
	animator->OnMove	();
	pSounds->PlayAtPos(sndDummy,this,vPosition,true);
	if (sndDummy.feedback) sndDummy.feedback->SetMinMax(1,100);
}
*/

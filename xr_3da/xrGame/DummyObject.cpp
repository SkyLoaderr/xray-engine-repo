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
	animator = 0;
	style = 0;
}

CDummyObject::~CDummyObject	()
{
	_DELETE(animator);
	pSounds->Delete(sndDummy);
}

void CDummyObject::Update(DWORD dt)
{
	if (style&esAnimated){
		animator->OnMove();
		mRotate.set		(animator->GetRotate());
		vPosition.set	(animator->GetPosition());
		vPosition.add	(start_position);
		UpdateTransform	();
	}

	Fsphere& S = CFORM()->GetSphere();
	if (sndDummy.feedback)	sndDummy.feedback->SetPosition(S.P);
}

void CDummyObject::Load	(CInifile* ini, const char* section)
{
	inherited::Load(ini,section);

	if (ini->LineExists(section,"motions")){
		style			|= esAnimated;
		animator		= new CObjectAnimator();
		animator->Load	(ini, section);
		animator->PlayMotion("idle",true);
		start_position.set(vPosition);
	}
	if (pVisual->Type==MT_SKELETON){
		style			|= esSkeleton;
		PKinematics(pVisual)->PlayCycle	("idle");

		/*
		for (int i=0; i<128; i++) {
			PKinematics(pVisual)->Calculate	();
		}
		*/
		
//		if (!ini->LineExists(section,"no_align")){
			
			// Auto-align object
			Collide::ray_query	RQ;
			Fvector		D; D.set(0,-1,0);
			
			for (float r=0; r<3; r+=0.1f) {
				for (float a=0; a<PI_MUL_2; a+=PI/12) {
					float x = cosf(a)*r;
					float z = sinf(a)*r;
					float y = 0.5f;
					Fvector O; O.set(x,y,z);
					Fvector P; P.add(vPosition,O);
					if (pCreator->ObjectSpace.RayPick(P,D,10,RQ))
					{
						vPosition	=	P;
						vPosition.y -=	RQ.range;
						vPosition.y +=	0.01f;
						UpdateTransform	();
						goto _ok;
					}
				}
			}
			;
			_ok:
			;
//		}
	}
	if (ini->LineExists(section,"sound"))
	{
		LPCSTR N = ini->ReadSTRING(section,"sound");
		pSounds->Create(sndDummy,TRUE,N);
	}

	Sector_Detect();
}

void CDummyObject::PlayDemo		(LPCSTR N)
{
	animator->StopMotion();
	animator->PlayMotion(N,false);
	animator->OnMove	();
	pSounds->PlayAtPos(sndDummy,this,vPosition,true);
	if (sndDummy.feedback) sndDummy.feedback->SetMinMax(1,100);
}

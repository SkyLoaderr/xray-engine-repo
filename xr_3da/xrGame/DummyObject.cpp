// DummyObject.cpp: implementation of the CDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\bodyinstance.h"
#include "..\fmesh.h"
#include "..\3dsound.h"

#include "DummyObject.h"
#include "..\ObjectAnimator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDummyObject::CDummyObject()
{
	animator = 0;
	style = 0;
	Device.seqRender.Add(this,REG_PRIORITY_LOW-1111);
}

CDummyObject::~CDummyObject()
{
	Device.seqRender.Remove(this);
	_DELETE(animator);
	pSounds->Delete3D(sndDummy);
}

void CDummyObject::OnRender()
{
	if (bDebug)
		PKinematics(pVisual)->DebugRender(svTransform);
}

void CDummyObject::Update(DWORD dt)
{
	if (style&DUMMY_ANIMATED){
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
		style			|= DUMMY_ANIMATED;
		animator		= new CObjectAnimator();
		animator->Load	(ini, section);
		animator->PlayMotion("idle",true);
		start_position.set(vPosition);
	}
	if (pVisual->Type==MT_SKELETON){
		style			|= DUMMY_SKELETON;
//		PKinematics(pVisual)->PlayCycle	("legs");
//		PKinematics(pVisual)->PlayCycle	("torso");
		PKinematics(pVisual)->PlayCycle	("idle");
//		PKinematics(pVisual)->PlayCycle	("walk_fwd");

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
		pSounds->Create3D(sndDummy,N);
	}

	DetectSector();
}

void CDummyObject::PlayDemo		(LPCSTR N)
{
	animator->StopMotion();
	animator->PlayMotion(N,false);
	animator->OnMove	();
	pSounds->Play3DAtPos(sndDummy,vPosition,true);
	if (sndDummy.feedback) sndDummy.feedback->SetMinMax(1,100);
}

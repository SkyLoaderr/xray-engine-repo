// DummyObject.cpp: implementation of the CDummyObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DummyObject.h"
#include "..\ObjectAnimator.h"
#include "..\psvisual.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDummyObject::CDummyObject	()
{
	style					= 0;
	s_animator				= NULL;
	s_model					= NULL;
	s_particles				= NULL;
	s_sound.feedback		= NULL;
}

CDummyObject::~CDummyObject	()
{
	pSounds->Delete			(s_sound);
	::Render->model_Delete	(s_particles);
	::Render->model_Delete	(s_model);
	_DELETE					(s_animator);
}

void CDummyObject::Load		(LPCSTR section)
{
	inherited::Load			(section);

	/*
	if (pSettings->LineExists(section,"motions")){
		style			|= esAnimated;
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

BOOL CDummyObject::Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	inherited::Spawn		(bLocal,server_id,o_pos,o_angle,P,flags);

	bVisible				= TRUE;

	UpdateTransform			();
	clTransform.set			(svTransform);
	relation.set			(svTransform);

	P.r_u8					(style);
	if (style&esAnimated)		{
		// Load animator
		string256				fn;
		P.r_string				(fn);
		s_animator				= new CObjectAnimator		();
		s_animator->Load		(fn);
		s_animator->PlayMotion	("idle",true);
	}
	if (style&esModel)			{
		// Load model
		string256				fn;
		P.r_string				(fn);
		s_model					= ::Render->model_Create	(fn);
		CKinematics* V			= PKinematics(s_model);
		if (V)					V->PlayCycle("idle");
	}
	if (style&esParticles)		{
		// Load model
		string256				fn;
		P.r_string				(fn);
		s_particles				= ::Render->model_CreatePS	(fn,&s_emitter);
		CPSVisual* V			= dynamic_cast<CPSVisual*>	(s_particles);
		if (V)					{
			s_emitter.m_Position.set	(Position());
			s_emitter.Play				();
		}
	}
	if (style&esSound)			{
		// Load model
		string256				fn;
		P.r_string				(fn);
		pSounds->Create			(s_sound,TRUE,fn);
		pSounds->PlayAtPos		(s_sound,0,Position(),true);
	}
	return TRUE;
}

void CDummyObject::Update		(DWORD dt)
{
	inherited::Update	(dt);

	if (s_particles)	dynamic_cast<CPSVisual*>(s_particles)->Update(dt);
}

void CDummyObject::UpdateCL		()
{
	if (s_animator)
	{
		s_animator->OnMove		();
		mRotate.set				(s_animator->GetRotate());
		vPosition.set			(s_animator->GetPosition());
		UpdateTransform			();
		if (style&esRelativePosition){
			R_ASSERT2(0,"CDummyObject: Relative position error.");
			svTransform.mulB_43	(relation);
		}
	}
	clTransform.set				(svTransform);

	if (s_model)
	{
	}
	if (s_particles)			
	{
		s_emitter.m_Position.set				(Position());
	}
	if (s_sound.feedback)
	{
		s_sound.feedback->SetPosition	(Position());
		s_sound.feedback->SetMinMax		(10,1000);
	}
}

void CDummyObject::OnVisible	()
{
	::Render->set_Transform		(&clTransform);
	if (s_model)		::Render->add_Visual(s_model);
	if (s_particles)	::Render->add_Visual(s_particles);
}

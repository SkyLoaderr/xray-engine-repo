////////////////////////////////////////////////////////////////////////////
//	Module 		: vehicle_helicopter.cpp
//	Created 	: 13.02.2004
//  Modified 	: 13.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Vehicle helicopter class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vehicle_helicopter.h"
#include "../objectanimator.h"

CVehicleHelicopter::CVehicleHelicopter		()
{
	init				();
}

CVehicleHelicopter::~CVehicleHelicopter		()
{
	xr_delete			(m_animator);
}

void CVehicleHelicopter::init				()
{
	m_animator			= xr_new<CObjectAnimator>();
}

void CVehicleHelicopter::Load				(LPCSTR section)
{
	inherited::Load		(section);
}

void CVehicleHelicopter::reinit				()
{
	inherited::reinit	();
}

void CVehicleHelicopter::reload				(LPCSTR section)
{
	inherited::reload	(section);
}

BOOL CVehicleHelicopter::net_Spawn			(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	// assigning m_animator here
	CSE_Abstract		*abstract=(CSE_Abstract*)(DC);
	CSE_ALifeHelicopter	*heli	= dynamic_cast<CSE_ALifeHelicopter*>(abstract);
	VERIFY				(heli);

	R_ASSERT			(Visual()&&PKinematics(Visual()));
	CSkeletonAnimated	*A= PSkeletonAnimated(Visual());
	if (A) {
		A->PlayCycle	(*heli->startup_animation);
		A->Calculate	();
	}
	m_animator->Load	(heli->get_motion());
	m_animator->Play	(true);

	m_engine_sound.create(TRUE,*heli->engine_sound);
	m_engine_sound.play_at_pos(0,XFORM().c,sm_Looped);

	setVisible			(true);
	setEnabled			(true);

	return				(TRUE);
}

void CVehicleHelicopter::net_Destroy		()
{
	inherited::net_Destroy();
}

void CVehicleHelicopter::net_Export			(NET_Packet &P)
{
}

void CVehicleHelicopter::net_Import			(NET_Packet &P)
{
}

void CVehicleHelicopter::renderable_Render	()
{
	inherited::renderable_Render();
}

void CVehicleHelicopter::UpdateCL			()
{
	inherited::UpdateCL	();
	m_animator->OnFrame	();
	XFORM().set			(m_animator->XFORM());
	m_engine_sound.set_position(XFORM().c);
}

void CVehicleHelicopter::shedule_Update		(u32 time_delta)
{
	inherited::shedule_Update(time_delta);

	Fvector				center;
	float				radius;		
	Center				(center);
	radius				= Radius();
	feel_touch_update	(center,radius);
}

void CVehicleHelicopter::feel_touch_new		(CObject *O)
{
}

void CVehicleHelicopter::feel_touch_delete	(CObject *O)
{
}

BOOL CVehicleHelicopter::feel_touch_contact	(CObject *O)
{
	return				(TRUE);
}

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
}

void CVehicleHelicopter::init				()
{
	m_animator			= 0;
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

//	heli

	return				(TRUE);
}

void CVehicleHelicopter::net_Destroy		()
{
	xr_delete			(m_animator);
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
	select_animation	();
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

void CVehicleHelicopter::select_animation	()
{
}
////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_no_alife.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager when no simulation started (for test purposes only!)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_no_alife.h"
#include "state_free_no_alife.h"

CStateManagerNoALife::CStateManagerNoALife	()
{
	Init					();
}

CStateManagerNoALife::~CStateManagerNoALife	()
{
}

void CStateManagerNoALife::Init				()
{
	add						(xr_new<CStateFreeNoAlife>(),	eNoALifeStateFree,		10);
}

void CStateManagerNoALife::Load				(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerNoALife::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object,eNoALifeStateFree);
}

void CStateManagerNoALife::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerNoALife::initialize		()
{
	inherited::initialize	();
}

void CStateManagerNoALife::execute			()
{
	set_dest_vertex_id		(eNoALifeStateFree);
	inherited::execute		();
}

void CStateManagerNoALife::finalize			()
{
	inherited::finalize		();
}

void CStateManagerNoALife::update			(u32 time_delta)
{
	inherited::update		(time_delta);
}

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
}

void CStateManagerNoALife::Load				(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerNoALife::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object);
	add_state				(xr_new<CStateFreeNoAlife>(),	eNoALifeStateFree,		10);
	set_current_state		(eNoALifeStateFree);
	set_dest_state			(eNoALifeStateFree);
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
	set_dest_state			(eNoALifeStateFree);
	inherited::execute		();
}

void CStateManagerNoALife::finalize			()
{
	inherited::finalize		();
}
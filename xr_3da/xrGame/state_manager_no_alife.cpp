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
#include "state_gather_items.h"
#include "ai/stalker/ai_stalker.h"

CStateManagerNoALife::CStateManagerNoALife	(LPCSTR state_name) : inherited(state_name)
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
	add_state				(xr_new<CStateFreeNoAlife>("FreeNoALife"),	eNoALifeStateFree,		0);
	add_state				(xr_new<CStateGatherItems>("GatherItems"),	eNoALifeGatherItems,	0);
	add_transition			(eNoALifeStateFree,eNoALifeGatherItems,1,1);
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
	if (!m_object->item())
		set_dest_state		(eNoALifeStateFree);
	else
		set_dest_state		(eNoALifeGatherItems);

	inherited::execute		();
}

void CStateManagerNoALife::finalize			()
{
	inherited::finalize		();
}
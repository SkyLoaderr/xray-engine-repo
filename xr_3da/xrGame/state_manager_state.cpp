////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_state.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager state
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_state.h"

CStateManagerState::CStateManagerState	()
{
	Init					();
}

CStateManagerState::~CStateManagerState	()
{
}

void CStateManagerState::Init			()
{
}

void CStateManagerState::Load			(LPCSTR section)
{
	CStateBase::Load				(section);
	CSStateManagerAbstract::Load	(section);
}

void CStateManagerState::reinit			(CAI_Stalker *object)
{
	CStateBase::reinit				(object);
	CSStateManagerAbstract::reinit	();
}

void CStateManagerState::reload			(LPCSTR section)
{
	CStateBase::reload				(section);
	CSStateManagerAbstract::reload	(section);
}

void CStateManagerState::initialize		()
{
	CStateBase::initialize			();
}

void CStateManagerState::execute		()
{
	CStateBase::initialize			();
}

void CStateManagerState::finalize		()
{
	CStateBase::finalize			();
}

void CStateManagerState::update			(u32 time_delta)
{
	CSStateManagerAbstract::update	(time_delta);
}

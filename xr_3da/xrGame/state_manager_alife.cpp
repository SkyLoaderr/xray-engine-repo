////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_alife.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager alife
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_alife.h"

CStateManagerALife::CStateManagerALife	()
{
	Init					();
}

CStateManagerALife::~CStateManagerALife	()
{
}

void CStateManagerALife::Init			()
{
}

void CStateManagerALife::Load			(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerALife::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object,u32(-1));
}

void CStateManagerALife::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerALife::initialize		()
{
	inherited::initialize	();
}

void CStateManagerALife::execute		()
{
	inherited::execute		();
}

void CStateManagerALife::finalize		()
{
	inherited::finalize		();
}
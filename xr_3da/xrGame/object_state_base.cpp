////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state.cpp
//	Created 	: 16.01.2004
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "object_state_base.h"
#include "ai/stalker/ai_stalker.h"

CObjectStateBase::CObjectStateBase	()
{
	Init							();
}

CObjectStateBase::~CObjectStateBase	()
{
}

void CObjectStateBase::Init			()
{
}

void CObjectStateBase::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CObjectStateBase::reinit		(CObjectHandler *object)
{
	inherited::reinit				(object);
}

void CObjectStateBase::reload		(LPCSTR section)
{
	inherited::reload				(section);
}

void CObjectStateBase::initialize	()
{
	inherited::initialize			();
}

void CObjectStateBase::execute		()
{
}

void CObjectStateBase::finalize		()
{
	inherited::finalize				();
}

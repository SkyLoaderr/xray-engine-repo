
#include "stdafx.h"
#include "biting_state_eat_approach.h"
#include "ai_biting.h"

CStateBitingEatApproach::CStateBitingEatApproach(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingEatApproach::~CStateBitingEatApproach()
{
}


void CStateBitingEatApproach::initialize()
{
	inherited::initialize();
}

void CStateBitingEatApproach::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;
}

void CStateBitingEatApproach::finalize()
{
	inherited::finalize();
}

bool CStateBitingEatApproach::completed() const
{
	return false;
}
#include "stdafx.h"
#include "biting_state_eat_drag.h"
#include "ai_biting.h"

////////////////////////////////////////////////////////////////////////////////////

CStateBitingEatPrepreDrag::CStateBitingEatPrepreDrag(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingEatPrepreDrag::~CStateBitingEatPrepreDrag()
{
}


void CStateBitingEatPrepreDrag::initialize()
{
	inherited::initialize();
}

void CStateBitingEatPrepreDrag::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;
}

void CStateBitingEatPrepreDrag::finalize()
{
	inherited::finalize();
}

bool CStateBitingEatPrepreDrag::completed() const
{
	return false;
}


////////////////////////////////////////////////////////////////////////////////////

CStateBitingEatDrag::CStateBitingEatDrag(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingEatDrag::~CStateBitingEatDrag()
{
}


void CStateBitingEatDrag::initialize()
{
	inherited::initialize();
}

void CStateBitingEatDrag::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;
}

void CStateBitingEatDrag::finalize()
{
	inherited::finalize();
}

bool CStateBitingEatDrag::completed() const
{
	return false;
}

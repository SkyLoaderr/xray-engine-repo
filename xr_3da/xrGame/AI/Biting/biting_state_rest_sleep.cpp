
#include "stdafx.h"
#include "biting_state_rest_sleep.h"
#include "ai_biting.h"

CStateBitingSleep::CStateBitingSleep(LPCSTR state_name) : inherited(state_name)
{
	Init();
}

CStateBitingSleep::~CStateBitingSleep()
{
}

void CStateBitingSleep::Init()
{
}

void CStateBitingSleep::Load(LPCSTR section)
{
	inherited::Load(section);
}

void CStateBitingSleep::reinit(CAI_Biting *object)
{
	inherited::reinit(object);
}

void CStateBitingSleep::reload(LPCSTR section)
{
	inherited::reload(section);
}

void CStateBitingSleep::initialize()
{
	inherited::initialize();
}

void CStateBitingSleep::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;
}

void CStateBitingSleep::finalize()
{
	inherited::finalize();
}

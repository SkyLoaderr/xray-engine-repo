#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBurerAttackTeleAbstract CStateBurerAttackTele<_Object>

TEMPLATE_SPECIALIZATION
CStateBurerAttackTeleAbstract::CStateBurerAttackTele(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateBurerAttackTeleAbstract::~CStateBurerAttackTele()
{
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackTeleAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackTeleAbstract::execute()
{
//	const CEntityAlive *enemy		= m_object->EnemyMan.get_enemy();
	m_object->MotionMan.m_tAction	= ACT_STAND_IDLE;
	
	//m_object->CTelekineticObject tele_object;
		

}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackTeleAbstract::finalize()
{
	inherited::finalize();
}


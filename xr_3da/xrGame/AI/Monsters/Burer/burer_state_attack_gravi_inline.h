#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBurerAttackGraviStartAbstract CStateBurerAttackGraviStart<_Object>
#define CStateBurerAttackGraviFinishAbstract CStateBurerAttackGraviFinish<_Object>


TEMPLATE_SPECIALIZATION
CStateBurerAttackGraviStartAbstract::CStateBurerAttackGraviStart(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateBurerAttackGraviStartAbstract::~CStateBurerAttackGraviStart()
{
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviStartAbstract::initialize()
{
	inherited::initialize();

	attack_started	= Level().timeServer();
	fade_in			= true;
}

#define GRAVI_FADE_IN_TIME	2000

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviStartAbstract::execute()
{
//	const CEntityAlive *enemy		= m_object->EnemyMan.get_enemy();
//	m_object->MotionMan.m_tAction	= ACT_STAND_IDLE;
//	
//	TTime	cur_time = Level().timeServer();
//	
//	if (attack_started + GRAVI_FADE_IN_TIME < cur_time) fade_in = false;
//	
//	if (fade_in) {
//		// нарастание	
//		m_object->MotionMan.m_tAction	= ACT_WALK_FWD;
//	} else {
//		// атака
//		m_object->MotionMan.m_tAction	= ACT_RUN;
//	}
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviStartAbstract::finalize()
{
	inherited::finalize();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEMPLATE_SPECIALIZATION
CStateBurerAttackGraviFinishAbstract::CStateBurerAttackGraviFinish(LPCSTR state_name) : inherited(state_name)
{
}

TEMPLATE_SPECIALIZATION
CStateBurerAttackGraviFinishAbstract::~CStateBurerAttackGraviFinish()
{
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviFinishAbstract::initialize()
{
	inherited::initialize();

	attack_started	= Level().timeServer();
	fade_in			= true;
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviFinishAbstract::execute()
{
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviFinishAbstract::finalize()
{
	inherited::finalize();
}



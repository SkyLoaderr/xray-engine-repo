#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterPanicAbstract CStateMonsterPanic<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterPanicAbstract::CStateMonsterPanic(LPCSTR state_name, SSubStatePtr state_run) : inherited(state_name)
{
	states[eStateRun]		= state_run;
}

TEMPLATE_SPECIALIZATION
CStateMonsterPanicAbstract::~CStateMonsterPanic()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::Load(LPCSTR section)
{
	add_state				(states[eStateRun],		eStateRun,			1);

	inherited::Load			(section);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::reinit(_Object *object)
{
	inherited::reinit		(object);
	set_current_state		(eStateRun);
	set_dest_state			(eStateRun);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::execute()
{
	Msg("Panic Executed...");
	set_dest_state(eStateRun);
	inherited::execute();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterPanicAbstract::finalize()
{
	inherited::finalize();
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterPanicAbstract::completed()
{
	bool b_completed = !m_object->m_tEnemy.obj;
	Msg("Panic Completed = [%u]",b_completed);

	if (!m_object->m_tEnemy.obj) return true;
	
	return false;
}


#include "stdafx.h"
#include "anim_triple.h"
#include "monster_event_manager.h"
#include "custom_events.h"

char *dbg_states[] = {
	"eStatePrepare", 
	"eStateExecute", 
	"eStateFinalize", 
	"eStateNone"
};


CAnimTriple::CAnimTriple()
{
	m_current_state	= eStateNone;
	m_active		= false;
	m_event_man		= 0;
}
CAnimTriple::~CAnimTriple()
{

}

void CAnimTriple::reinit_external(CMonsterEventManager *man, const MotionID &m_def1, const MotionID &m_def2, const MotionID &m_def3, bool b_execute_once)
{
	pool[0]			= m_def1;
	pool[1]			= m_def2;
	pool[2]			= m_def3;

	m_event_man		= man;
	m_execute_once	= b_execute_once;
}

void CAnimTriple::activate()
{
	if (is_active()) deactivate();

	m_current_state		= eStatePrepare;
	m_previous_state	= eStateNone;
	m_active			= true;
}

void CAnimTriple::deactivate()
{
	m_current_state	= eStateNone;
	m_active		= false;
}

bool CAnimTriple::prepare_animation(MotionID &m)
{
	if (m_current_state == eStateNone) {
		deactivate();
		return false;
	}
	if (!is_active()) return false;

	if ((m_current_state == eStateExecute) && m_execute_once && (m_previous_state == eStateExecute)) return false;
	
	m = pool[m_current_state];
	
	// raise event
	if ((m_current_state != eStateExecute) || ((m_current_state == eStateExecute) && (m_previous_state != eStateExecute))) {
		CEventTAPrepareAnimation	event(m_current_state);
		m_event_man->raise			(eventTAChange, &event);
	}
	
	m_previous_state = m_current_state;
	if (m_current_state != eStateExecute) m_current_state = EStateAnimTriple(m_current_state + 1);
	
	return true;
}


void CAnimTriple::pointbreak()
{
	m_current_state = eStateFinalize;
}




#include "stdafx.h"
#include "anim_triple.h"
#include "control_manager.h"

char *dbg_states[] = {
	"eStatePrepare", 
	"eStateExecute", 
	"eStateFinalize", 
	"eStateNone"
};

void CAnimationTriple::on_capture()
{
	m_current_state	= eStateNone;

	m_man->capture		(this, ControlCom::eControlAnimation);
	//m_man->capture_pure	(this);
	m_man->subscribe	(this, ControlCom::eventAnimationEnd);

	//m_man->path_stop	(this);
	//m_man->move_stop	(this);
	//m_man->dir_stop		(this);
}

void CAnimationTriple::on_release()
{
	//m_man->release_pure	(this);	
	m_man->release		(this, ControlCom::eControlAnimation);
	m_man->unsubscribe	(this, ControlCom::eventAnimationEnd);
}

bool CAnimationTriple::check_start_conditions()
{
	if (is_active())				return false;
	//if (m_man->is_captured_pure())	return false;
	if (m_man->is_captured(ControlCom::eControlAnimation))	return false;

	return true;
}

void CAnimationTriple::activate()
{
	m_current_state		= m_data.skip_prepare ? eStateExecute : eStatePrepare;
	m_previous_state	= m_data.skip_prepare ? eStatePrepare : eStateNone;
	select_next_state	();
}

void CAnimationTriple::on_event(ControlCom::EEventType, ControlCom::IEventData*)
{
	select_next_state();
}

void CAnimationTriple::pointbreak()
{
	m_current_state		= eStateFinalize;
	select_next_state	();
}

void CAnimationTriple::select_next_state()
{
	if (m_current_state == eStateNone) {
		STripleAnimEventData		event(m_current_state);
		m_man->notify				(ControlCom::eventTAChange, &event);
		return;
	}
	
	if ((m_current_state == eStateExecute) && 
		m_data.execute_once && 
		(m_previous_state == eStateExecute)) 
		return;

	play_selected	();

	// raise event
	if ((m_current_state != eStateExecute) || 
		((m_current_state == eStateExecute) && (m_previous_state != eStateExecute))) {
		
		STripleAnimEventData	event	(m_current_state);
		m_man->notify			(ControlCom::eventTAChange, &event);
	}

	m_previous_state = m_current_state;
	if (m_current_state != eStateExecute) 
		m_current_state = EStateAnimTriple(m_current_state + 1);
}

void CAnimationTriple::play_selected()
{
	// start new animation
	SControlAnimationData		*ctrl_data = (SControlAnimationData*)m_man->data(this, ControlCom::eControlAnimation); 
	VERIFY						(ctrl_data);
	
	ctrl_data->motion			= m_data.pool[m_current_state];
	ctrl_data->start_animation	= true;
}

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateAbstract CState<_Object>

TEMPLATE_SPECIALIZATION
CStateAbstract::CState(_Object *obj) 
{
	reset();
	object = obj;
}

TEMPLATE_SPECIALIZATION
CStateAbstract::~CState() 
{
}


TEMPLATE_SPECIALIZATION
void CStateAbstract::initialize() 
{
	time_state_started	= Level().timeServer();

	current_substate	= u32(-1); // means need reselect state
	prev_substate		= u32(-1);
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::execute() 
{ 
	// обновить данные состояния
	update_data();

	// если состояние не выбрано, перевыбрать
	if (current_substate == u32(-1)) {
		reselect_state();
		VERIFY(current_substate != u32(-1));
	}

	// выполнить текущее состояние
	CSState *state = get_state(current_substate);
	state->execute();

	// проверить на завершение текущего состояния
	if (state->check_completion()) {
		state->finalize();
		current_substate = u32(-1);
	} 

	// сохранить текущее состояние
	prev_substate = current_substate;
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::finalize()
{
	reset();
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::critical_finalize()
{
	reset();
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::reset() 
{
	current_substate	= u32(-1);
	prev_substate		= u32(-1);
	time_state_started	= 0;
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::select_state(u32 new_state_id) 
{
	if (current_substate == new_state_id) return;	
	CSState *state;

	// если предыдущее состояние активно, завершить его
	if (current_substate != u32(-1)) {
		state = get_state(current_substate);
		state->critical_finalize();
	}

	// установить новое состояние
	state = get_state(current_substate = new_state_id);
	// инициализировать новое состояние
	state->initialize();
}

TEMPLATE_SPECIALIZATION
CStateAbstract* CStateAbstract::get_state(u32 state_id)
{
	STATE_MAP_IT it = substates.find(state_id);
	VERIFY(it != substates.end());

	return it->second;
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::add_state(u32 state_id, CSState *s) 
{
	substates.insert(mk_pair(state_id, s));
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::free_mem()
{
	for (STATE_MAP_IT it = substates.begin(); it != substates.end(); it++) xr_delete(it->second);
}

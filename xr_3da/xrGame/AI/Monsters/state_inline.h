#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateAbstract CState<_Object>

TEMPLATE_SPECIALIZATION
CStateAbstract::CState(_Object *obj, EStateType s_type, void *data) 
{
	reset		();

	object		= obj;
	_data		= data;
	state_type	= s_type;
}

TEMPLATE_SPECIALIZATION
CStateAbstract::~CState() 
{
	free_mem();
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::reinit()
{
	for (STATE_MAP_IT it = substates.begin(); it != substates.end(); it++) 
		it->second->reinit();
}


TEMPLATE_SPECIALIZATION
void CStateAbstract::initialize() 
{
	time_state_started	= Device.dwTimeGlobal;

	current_substate	= u32(-1); // means need reselect state
	prev_substate		= u32(-1);
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::execute() 
{ 
	// ��������� ������� ������� ��������� ���������
	check_force_state();

	// ���� ��������� �� �������, �����������
	if (current_substate == u32(-1)) {
		reselect_state();
		VERIFY(current_substate != u32(-1));
	}

	// ��������� ������� ���������
	CSState *state = get_state(current_substate);
	state->execute();

	// ��������� ������� ���������
	prev_substate = current_substate;

	// ��������� �� ���������� �������� ���������
	if (state->check_completion()) {
		state->finalize();
		current_substate = u32(-1);
	} 
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::finalize()
{
	reset();
}

TEMPLATE_SPECIALIZATION
void CStateAbstract::critical_finalize()
{
	if (current_substate != u32(-1)) get_state_current()->critical_finalize();
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

	// ���� ���������� ��������� �������, ��������� ���
	if (current_substate != u32(-1)) {
		state = get_state(current_substate);
		state->critical_finalize();
	}

	// ���������� ����� ���������
	state = get_state(current_substate = new_state_id);
	
	// ���������������� ����� ���������
	setup_substates();

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

TEMPLATE_SPECIALIZATION
void CStateAbstract::fill_data_with	(void *ptr_src, u32 size)
{
	VERIFY(ptr_src);
	VERIFY(_data);

	CopyMemory(_data, ptr_src, size);
}

TEMPLATE_SPECIALIZATION
CStateAbstract *CStateAbstract::get_state_current()
{
	if (substates.empty() || (current_substate == u32(-1))) return 0;
	
	STATE_MAP_IT it = substates.find(current_substate);
	VERIFY(it != substates.end());

	return it->second;
}

TEMPLATE_SPECIALIZATION
EStateType CStateAbstract::get_state_type()
{
	// ���� ���� ������������ - ������� ��� �������� ������������
	CStateAbstract *cur_state = get_state_current();
	if (cur_state) return cur_state->get_state_type();
	
	// ����� ������� ���� ���
	return state_type;
}

#undef TEMPLATE_SPECIALIZATION
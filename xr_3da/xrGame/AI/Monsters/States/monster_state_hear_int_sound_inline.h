#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterHearInterestingSoundAbstract CStateMonsterHearInterestingSound<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterHearInterestingSoundAbstract::CStateMonsterHearInterestingSound(_Object *obj) : inherited(obj)
{
	add_state	(eStateMoveToDest,	xr_new<CStateMonsterMoveToPoint<_Object> >(obj));
	add_state	(eStateLookAround,	xr_new<CStateMonsterCustomAction<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHearInterestingSoundAbstract::reselect_state()
{
	if (prev_substate == u32(-1)){
		if (get_state(eStateMoveToDest)->check_start_conditions()) 
			select_state(eStateMoveToDest);
		else 
			select_state(eStateLookAround);
		return;
	}	

	select_state(eStateLookAround);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterHearInterestingSoundAbstract::setup_substates()
{
	state_ptr state = get_state_current();
	
	if (current_substate == eStateMoveToDest) {
		SStateDataMoveToPoint data;
		data.point			= object->SoundMemory.GetSound().position;
		data.vertex			= u32(-1);
		data.action.action	= ACT_WALK_FWD;
		data.accelerated	= true;
		data.braking		= false;
		data.accel_type 	= eAT_Calm;
		data.completion_dist= 2.f;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;
		

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			DBG().object_info(object,object).remove_item	(u32(0));
			DBG().object_info(object,object).add_item		("Interesting Snd :: Move To Sound Source", D3DCOLOR_XRGB(255,0,0), 0);
		}
#endif
		return;
	}

	if (current_substate == eStateLookAround) {
		SStateDataAction data;
		data.action			= ACT_LOOK_AROUND;
		data.sound_type		= MonsterSpace::eMonsterSoundIdle;
		data.sound_delay	= object->get_sd()->m_dwIdleSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataAction));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			DBG().object_info(object,object).remove_item	(u32(0));
			DBG().object_info(object,object).add_item		("Interesting Snd :: Look Around", D3DCOLOR_XRGB(255,0,0), 0);
		}
#endif

		return;
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterHearInterestingSoundAbstract 

#pragma once

#include "state_move_to_point.h"
#include "state_look_point.h"
#include "state_custom_action.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyLookAbstract CStateMonsterFindEnemyLook<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyLookAbstract::CStateMonsterFindEnemyLook(_Object *obj) : inherited(obj)
{
	add_state	(eMoveToPoint,	xr_new<CStateMonsterMoveToPoint<_Object> >	(obj));
	add_state	(eLookAround,	xr_new<CStateMonsterCustomAction<_Object> > (obj));
	add_state	(eTurnToPoint,	xr_new<CStateMonsterLookToPoint<_Object> >	(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyLookAbstract::~CStateMonsterFindEnemyLook()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyLookAbstract::initialize()
{
	inherited::initialize	();

	look_right_side	= ((Random.randI(2)) ? true : false);
	current_stage	= 0;
	target_point	= Fvector().set(0.f,0.f,0.f);
	
	current_dir		= object->Direction();
	start_position	= object->Position();
}


TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyLookAbstract::reselect_state()
{
	if ((current_stage == 1) || (current_stage == 3)) {
		float h,p;
		current_dir.getHP(h,p);

		h += ((look_right_side) ? (-deg(120)) : deg(120));
		current_dir.setHP(angle_normalize(h),p);						
		current_dir.normalize();
		target_point.mad(start_position, current_dir, Random.randF(4.f,5.f));
		select_state((Random.randI(2)) ? eMoveToPoint : eTurnToPoint);

	} else select_state(eLookAround); 

	current_stage++;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterFindEnemyLookAbstract::check_completion()
{	
	if (current_stage < 5) return false;
	return true;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyLookAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eMoveToPoint) {
		
		SStateDataMoveToPoint data;
		data.point			= target_point;
		data.vertex			= u32(-1);
		data.accelerated	= true;
		data.braking		= false;
		data.accel_type 	= eAT_Aggressive;
		data.action.action	= ACT_RUN; 
		data.action.sound_type	= MonsterSpace::eMonsterSoundAttack;
		data.action.sound_delay = object->get_sd()->m_dwAttackSndDelay;


		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Find Enemy :: Move To Point", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif
		return;
	}

	if (current_substate == eLookAround) {
		SStateDataAction data;
		
		data.action		= ACT_LOOK_AROUND;
		data.time_out	= 2000;
		data.sound_type	= MonsterSpace::eMonsterSoundAttack;
		data.sound_delay = object->get_sd()->m_dwAttackSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataAction));

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Find Enemy :: Look around", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif
		return;
	}

	if (current_substate == eTurnToPoint) {
		SStateDataLookToPoint data;

		data.point			= target_point; 
		data.action.action	= ACT_STAND_IDLE;		
		data.action.sound_type	= MonsterSpace::eMonsterSoundAttack;
		data.action.sound_delay = object->get_sd()->m_dwAttackSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataLookToPoint));

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Find Enemy :: Turn to point", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif
		return;
	}
}


#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyLookAbstract CStateMonsterFindEnemyLook<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyLookAbstract::CStateMonsterFindEnemyLook(_Object *obj, state_ptr state_move, state_ptr state_look_around, state_ptr state_turn) : inherited(obj)
{
	add_state	(eMoveToPoint,	state_move);
	add_state	(eLookAround,	state_look_around);
	add_state	(eTurnToPoint,	state_turn);
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyLookAbstract::~CStateMonsterFindEnemyLook()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyLookAbstract::initialize()
{
	look_right_side	= ((Random.randI(2)) ? true : false);
	current_stage	= 0;
	target_point	= Fvector().set(0.f,0.f,0.f);
	
	current_dir		= object->Direction();
	start_position	= object->Position();

	Msg("initialized !!!");
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
		
		object->HDebug->L_Clear();
		object->HDebug->L_AddPoint(target_point, 0.35f, D3DCOLOR_XRGB(0,255,255));

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
		data.action			= ACT_RUN;
		data.accelerated	= true;
		data.braking		= false;
		data.accel_type 	= eAT_Aggressive;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));

		object->HDebug->M_Clear();
		object->HDebug->M_Add(1, "Move To Point", D3DCOLOR_XRGB(255,0,0));

		return;
	}

	if (current_substate == eLookAround) {
		SStateDataCustomAction data;
		
		data.action		= ACT_LOOK_AROUND;
		data.time_out	= 2000;
		
		state->fill_data_with(&data, sizeof(SStateDataCustomAction));

		object->HDebug->M_Clear();
		object->HDebug->M_Add(1, "Look Around", D3DCOLOR_XRGB(255,0,0));

		return;
	}

	if (current_substate == eTurnToPoint) {
		SStateDataLookToPoint data;

		data.point	= target_point; 
		data.action	= ACT_STAND_IDLE;		

		state->fill_data_with(&data, sizeof(SStateDataLookToPoint));

		object->HDebug->M_Clear();
		object->HDebug->M_Add(1, "Turn To Point", D3DCOLOR_XRGB(255,0,0));

		return;
	}
}



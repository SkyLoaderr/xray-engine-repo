#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimeraThreatenAbstract CStateChimeraThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::CStateChimeraThreaten(_Object *obj, state_ptr state_walk, state_ptr state_threaten) : inherited(obj)
{
	add_state(eStateWalk,		state_walk);
	add_state(eStateThreaten,	state_threaten);
}

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::~CStateChimeraThreaten()
{
}


TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::execute()
{
	
}

//////////////////////////////////////////////////////////////////////////

#define CStateChimeraTestAbstract CStateChimeraTest<_Object>

TEMPLATE_SPECIALIZATION
void CStateChimeraTestAbstract::initialize()
{
	inherited::initialize();
	
	object->CMonsterMovement::initialize_movement();
}


TEMPLATE_SPECIALIZATION
void CStateChimeraTestAbstract::execute()
{
	object->MotionMan.m_tAction					= ACT_RUN;
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());		
	Fvector pos		= pA->Position();

	bool bad_target = false;
	if (object->CMonsterMovement::failed()) {
		bad_target = true;
	}

	if (object->CMonsterMovement::path_end()) {
		if (object->Position().distance_to(pA->Position()) > 3.f)
			bad_target = true;
	}

	if (bad_target) {
		Fvector dir, new_pos;
		dir.random_dir();

		new_pos.mad(pos, dir, 20.f);
		if (object->accessible(new_pos)) pos = new_pos;
		else object->accessible_nearest(new_pos, pos);
	}


	object->CMonsterMovement::set_target_point		(pos, u32(-1));
	object->CMonsterMovement::set_rebuild_time		(50000);
	object->CMonsterMovement::set_distance_to_end	(3.f);
	object->CMonsterMovement::set_use_covers		();
	object->CMonsterMovement::set_cover_params		(20.f, 30.f, 1.f, 30.f);
}

#undef TEMPLATE_SPECIALIZATION



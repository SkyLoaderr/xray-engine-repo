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
	u32		node	= u32(-1);
	
	//object->CMonsterMovement::set_target_point			(pos, node);
	object->CMonsterMovement::set_retreat_from_point	(pos);
	object->CMonsterMovement::set_rebuild_time			(5000);
	object->CMonsterMovement::set_distance_to_end		(3.f);
	object->CMonsterMovement::set_use_covers			();
	object->CMonsterMovement::set_cover_params			(5.f, 30.f, 1.f, 30.f);
}

#undef TEMPLATE_SPECIALIZATION



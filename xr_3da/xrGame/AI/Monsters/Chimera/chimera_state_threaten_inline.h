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
	Fvector dir, target;
	CActor *pA = smart_cast<CActor *>(Level().CurrentEntity());		
	//dir.sub(object->Position(), pA->Position());
	//dir.normalize();
	//target.mad(pA->Position(), dir, 3.f);
	object->CMonsterMovement::set_target_point	(pA->Position(), pA->level_vertex_id());
	object->CMonsterMovement::set_rebuild_time	(50000);
	//object->CMonsterMovement::set_use_covers	();
}

#undef TEMPLATE_SPECIALIZATION


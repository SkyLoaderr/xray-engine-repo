#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterMoveToPointAbstract CStateMonsterMoveToPoint<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterMoveToPointAbstract::CStateMonsterMoveToPoint(_Object *obj) : inherited(obj, &data)
{
	data.point.set		(0.f,0.f,0.f);
	data.action			= ACT_STAND_IDLE;
	data.accelerated	= false;
}

TEMPLATE_SPECIALIZATION
CStateMonsterMoveToPointAbstract::~CStateMonsterMoveToPoint()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointAbstract::initialize()
{
	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterMoveToPointAbstract::execute()
{
	if (data.vertex != u32(-1)) object->MoveToTarget(data.point, data.vertex);
	else object->MoveToTarget(data.point);

	object->MotionMan.m_tAction				= data.action;
	
	if (data.accelerated) {
		object->MotionMan.accel_activate	(EAccelType(data.accel_type));
		object->MotionMan.accel_set_braking (data.braking);
	}

	Msg("*MState :: Move to point action Executed :: time = [%u]", Level().timeServer());
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterMoveToPointAbstract::check_completion()
{	
	if (object->Position().distance_to(data.point) > 1.5f) return false;
	return true;
}


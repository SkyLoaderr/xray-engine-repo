#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterLookToPointAbstract CStateMonsterLookToPoint<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterLookToPointAbstract::CStateMonsterLookToPoint(_Object *obj) : inherited(obj, &data)
{
	data.point.set		(0.f,0.f,0.f);
	data.action			= ACT_STAND_IDLE; 
}

TEMPLATE_SPECIALIZATION
CStateMonsterLookToPointAbstract::~CStateMonsterLookToPoint()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterLookToPointAbstract::initialize()
{
		
}

TEMPLATE_SPECIALIZATION
void CStateMonsterLookToPointAbstract::execute()
{

	object->MotionMan.m_tAction				= data.action;
	object->FaceTarget						(data.point);

	Msg("*MState :: Look point action Executed :: time = [%u]", Level().timeServer());
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterLookToPointAbstract::check_completion()
{	
	if (angle_difference(object->m_body.current.yaw, object->m_body.target.yaw) > deg(1)) return false;
	return true;
}


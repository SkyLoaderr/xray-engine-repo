#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterLookActorAbstract CStateMonsterLookActor<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterLookActorAbstract::execute()
{
	object->MotionMan.m_tAction				= ACT_STAND_IDLE;
	object->DirMan.face_target				(Level().CurrentEntity()->Position(), 1200);
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterLookActorAbstract


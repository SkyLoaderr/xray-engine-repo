#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterLookActorAbstract CStateMonsterLookActor<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterLookActorAbstract::execute()
{
	object->set_action			(ACT_STAND_IDLE);
	object->DirMan.face_target	(Level().CurrentEntity()->Position(), 1200);
}



#define CStateMonsterTurnAwayFromActorAbstract CStateMonsterTurnAwayFromActor<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterTurnAwayFromActorAbstract::execute()
{
	Fvector point;
	Fvector dir;
	dir.sub			(object->Position(), Level().CurrentEntity()->Position());
	dir.normalize	();
	point.mad		(object->Position(), dir, 2.f);
	
	object->set_action			(ACT_STAND_IDLE);
	object->DirMan.face_target	(point, 1200);
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterLookActorAbstract
#undef CStateMonsterTurnAwayFromActorAbstract


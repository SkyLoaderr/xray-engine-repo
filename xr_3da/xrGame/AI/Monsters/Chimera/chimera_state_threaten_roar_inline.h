#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateChimeraThreatenRoarAbstract CStateChimeraThreatenRoar<_Object>

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenRoarAbstract::initialize()
{
	inherited::initialize	();

}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenRoarAbstract::execute()
{

	object->set_action				(ACT_STAND_IDLE);
	object->MotionMan.SetSpecParams	(ASP_THREATEN);
	object->set_state_sound			(MonsterSpace::eMonsterSoundThreaten);
	object->DirMan.face_target		(object->EnemyMan.get_enemy(), 1200);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Threaten :: Roar", D3DCOLOR_XRGB(255,0,0));
	}
#endif
}

#define STATE_TIME_OUT	4000

TEMPLATE_SPECIALIZATION
bool CStateChimeraThreatenRoarAbstract::check_completion()
{	
	if (time_state_started + STATE_TIME_OUT < Device.dwTimeGlobal) return true;
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateChimeraThreatenRoarAbstract


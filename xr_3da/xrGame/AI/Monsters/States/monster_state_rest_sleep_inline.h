#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestSleepAbstract CStateMonsterRestSleep<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestSleepAbstract::CStateMonsterRestSleep(_Object *obj) : inherited(obj, ST_Rest)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestSleepAbstract::~CStateMonsterRestSleep	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestSleepAbstract::execute()
{
	object->MotionMan.m_tAction = ACT_SLEEP;

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Rest :: Sleep", D3DCOLOR_XRGB(255,0,0));

	}
#endif

	object->CSoundPlayer::play(MonsterSpace::eMonsterSoundIdle, 0,0,object->get_sd()->m_dwIdleSndDelay);
}


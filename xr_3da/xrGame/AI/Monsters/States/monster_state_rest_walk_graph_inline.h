#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestWalkGraphAbstract CStateMonsterRestWalkGraph<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestWalkGraphAbstract::CStateMonsterRestWalkGraph(_Object *obj) : inherited(obj, ST_WalkGraphPoint)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestWalkGraphAbstract::~CStateMonsterRestWalkGraph	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestWalkGraphAbstract::execute()
{
	object->MotionMan.m_tAction	= ACT_WALK_FWD;
	object->WalkNextGraphPoint	();
	object->CSoundPlayer::play	(MonsterSpace::eMonsterSoundIdle, 0,0,object->get_sd()->m_dwIdleSndDelay);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Rest :: Walk Graph Point", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}


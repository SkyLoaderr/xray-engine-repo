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
	object->detour_graph_points		();
	object->set_action				(ACT_WALK_FWD);
	object->set_state_sound			(MonsterSpace::eMonsterSoundIdle);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Rest :: Walk Graph Point", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}


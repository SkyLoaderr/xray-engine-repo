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
	object->movement().detour_graph_points		();
	object->set_action				(ACT_WALK_FWD);
	object->set_state_sound			(MonsterSpace::eMonsterSoundIdle);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Rest :: Walk Graph Point", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif

}


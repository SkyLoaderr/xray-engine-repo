#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterSquadRestAbstract CStateMonsterSquadRest<_Object>

#define MIN_TIME_IDLE	5000
#define MAX_TIME_IDLE	10000

#define LEADER_RADIUS			20.f
#define FIND_POINT_ATTEMPTS		5


TEMPLATE_SPECIALIZATION
CStateMonsterSquadRestAbstract::CStateMonsterSquadRest(_Object *obj) : inherited(obj)
{
	add_state	(eStateIdle,				xr_new<CStateMonsterCustomAction<_Object> >	(obj));
	add_state	(eStateWalkAroundLeader,	xr_new<CStateMonsterMoveToPoint<_Object> >	(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterSquadRestAbstract::~CStateMonsterSquadRest	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterSquadRestAbstract::reselect_state()
{
	select_state(Random.randI(2) ? eStateIdle : eStateWalkAroundLeader);
}


TEMPLATE_SPECIALIZATION
void CStateMonsterSquadRestAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateIdle) {
		SStateDataAction data;
		data.action			= ACT_REST;
		data.sound_type		= MonsterSpace::eMonsterSoundIdle;
		data.sound_delay	= object->get_sd()->m_dwIdleSndDelay;
		data.time_out		= Random.randI(MIN_TIME_IDLE,MAX_TIME_IDLE);
		
		state->fill_data_with(&data, sizeof(SStateDataAction));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			DBG().object_info(object,object).remove_item	(u32(0));
			DBG().object_info(object,object).add_item		("Squad :: Sleep", D3DCOLOR_XRGB(255,0,0), 0);
		}
#endif

		return;
	}

	if (current_substate == eStateWalkAroundLeader) {
		SStateDataMoveToPoint data;
		CMonsterSquad	*squad = monster_squad().get_squad(object);
		
		if (object->movement().GetNodeInRadius(squad->GetLeader()->ai_location().level_vertex_id(), 8.f, LEADER_RADIUS, FIND_POINT_ATTEMPTS, data.vertex)) {
			data.point			= ai().level_graph().vertex_position(data.vertex);
		} else {
			
			Fvector dest_pos = random_position(squad->GetLeader()->Position(), LEADER_RADIUS);
			if (!object->movement().restrictions().accessible(dest_pos)) {
				data.vertex		= object->movement().restrictions().accessible_nearest(dest_pos, data.point);
			} else {
				data.point		= dest_pos;
				data.vertex		= u32(-1);
			}
		}

		data.action.action	= ACT_WALK_FWD;
		data.accelerated	= true;
		data.braking		= false;
		data.accel_type 	= eAT_Calm;
		data.completion_dist= 2.f;
		data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
		data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;

		state->fill_data_with(&data, sizeof(SStateDataMoveToPoint));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			DBG().object_info(object,object).remove_item	(u32(0));
			DBG().object_info(object,object).add_item		("Squad :: Move Around Leader", D3DCOLOR_XRGB(255,0,0), 0);
		}
#endif

		return;
	}
}



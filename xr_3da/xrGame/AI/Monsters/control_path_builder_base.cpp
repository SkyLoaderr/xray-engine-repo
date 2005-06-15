#include "stdafx.h"
#include "control_path_builder_base.h"
#include "BaseMonster/base_monster.h"
#include "../../phmovementcontrol.h"
#include "../../cover_evaluators.h"

//////////////////////////////////////////////////////////////////////////
// Construction / Destruction
//////////////////////////////////////////////////////////////////////////

CControlPathBuilderBase::CControlPathBuilderBase()
{
	m_cover_approach = 0;
}

CControlPathBuilderBase::~CControlPathBuilderBase()
{
	xr_delete(m_cover_approach);
}

void CControlPathBuilderBase::reinit()
{
	inherited::reinit				();

	if (!m_cover_approach)
		m_cover_approach			= xr_new<CCoverEvaluatorCloseToEnemy>(&m_man->path_builder().restrictions());
	
	reset							();

	m_man->capture					(this, ControlCom::eControlPath);
}

void CControlPathBuilderBase::reset()
{
	m_try_min_time					= false;
	m_use_dest_orient				= false;
	m_enable						= false;
	m_path_type						= MovementManager::ePathTypeLevelPath;
	m_dest_dir.set					(0.f, 0.f, 0.f);
	m_extrapolate					= false;
	m_velocity_mask					= u32(-1);
	m_desirable_mask				= u32(-1);
	
	if (!m_man->path_builder().accessible(m_object->Position())) {
		m_target_found.node		= m_man->path_builder().restrictions().accessible_nearest(m_object->Position(), m_target_found.position);
	} else {
		m_target_found.node			= u32(-1);
		m_target_found.position		= m_object->Position();
	}
	
	prepare_builder					();
}


void CControlPathBuilderBase::on_event(ControlCom::EEventType type, ControlCom::IEventData *data)
{
	switch (type) {
	case ControlCom::eventPathBuilt:			check_failure(); break;
	case ControlCom::eventTravelPointChange:	travel_point_changed(); break;
	}
}

void CControlPathBuilderBase::on_start_control(ControlCom::EContolType type)
{
	switch (type) {
	case ControlCom::eControlPath:	m_man->subscribe	(this, ControlCom::eventPathBuilt);	
									m_man->subscribe	(this, ControlCom::eventTravelPointChange);	
									break;
	}
}

void CControlPathBuilderBase::on_stop_control(ControlCom::EContolType type)
{
	switch (type) {
	case ControlCom::eControlPath:	m_man->unsubscribe	(this, ControlCom::eventPathBuilt);	
									m_man->unsubscribe	(this, ControlCom::eventTravelPointChange);	
									break;
	}
}

void CControlPathBuilderBase::detour_graph_points()
{
	set_game_path_type	();
}

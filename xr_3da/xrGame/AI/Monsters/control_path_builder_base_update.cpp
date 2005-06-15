#include "stdafx.h"
#include "BaseMonster/base_monster.h"
#include "critical_action_info.h"
#include "../../detail_path_manager.h"
#include "../../level_location_selector.h"
#include "../../level_path_manager.h"

//////////////////////////////////////////////////////////////////////////
// Update Movement
void CControlPathBuilderBase::update_frame()
{	
	//Msg("Time [%u] ::ControlPath_Base:: Update Frame", Device.dwTimeGlobal);

	START_PROFILE("AI/Base Monster/Think/Find Target Point");

	// обновить / установить целевую позицию
	update_target_point								();

	// set params
	set_path_builder_params							();	
	
	STOP_PROFILE;
}


//////////////////////////////////////////////////////////////////////////
// update path with new scheme method
void CControlPathBuilderBase::update_target_point() 
{
	if (!m_enable)											return;
	if (m_path_type != MovementManager::ePathTypeLevelPath) return;

	// проверить условия, когда путь строить не нужно
	if (!target_point_need_update())						return; 

	// выбрать ноду и позицию в соответствии с желаемыми нодой и позицией
	find_target			();

	// сохранить текущее время 
	m_last_time_target_set	= Device.dwTimeGlobal;
}

void CControlPathBuilderBase::set_path_builder_params()
{
	SControlPathBuilderData	*ctrl_data = (SControlPathBuilderData *)m_man->data(this, ControlCom::eControlPath);
	if (!ctrl_data) return;

	ctrl_data->use_dest_orientation	= m_use_dest_orient;
	ctrl_data->dest_orientation		= m_dest_dir;
	ctrl_data->target_node			= m_target_found.node;
	ctrl_data->target_position		= m_target_found.position;
	ctrl_data->try_min_time			= m_try_min_time;
	ctrl_data->enable				= m_enable;
	ctrl_data->path_type			= m_path_type;
	ctrl_data->extrapolate			= m_extrapolate;
	ctrl_data->velocity_mask		= m_velocity_mask;
	ctrl_data->desirable_mask		= m_desirable_mask;
}


// обновить информацию о построенном пути (m_failed)
void CControlPathBuilderBase::check_failure()
{
	m_failed		= false;

	//bool new_path = detail().time_path_built() >= m_last_time_target_set;
	if (m_man->path_builder().is_path_end(m_distance_to_path_end) && 
		m_man->path_builder().actual() && 
		!m_wait_path_end && 
		(m_target_selected.position.similar(m_target_set.position))) 
	{
		m_failed	= true;	
	}

	// если level_path_manager failed
	if (m_man->path_builder().level_path().failed()) {
		m_failed	= true;
	}
}

void CControlPathBuilderBase::travel_point_changed()
{
	//if (IsMovingOnPath()) {
	//	u32 cur_point_velocity_index = detail().path()[detail().curr_travel_point_index()].velocity;		
	//	if ((cur_point_velocity_index == eVelocityParameterStand) && !fis_zero(m_velocity_linear.current) && !m_velocity_reset) {
	//		stop_linear			();
	//		m_velocity_reset	= true;
	//	}
	//	if (cur_point_velocity_index != eVelocityParameterStand) m_velocity_reset = false;
	//}

	if (m_man->path_builder().detail().curr_travel_point_index() >= m_man->path_builder().detail().path().size() - 1) {
		if (m_target_selected.position.similar(m_target_set.position)) m_failed	= true;
	}
}


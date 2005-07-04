#include "stdafx.h"
#include "control_path_builder.h"
#include "control_manager.h"
#include "BaseMonster/base_monster.h"
#include "../../path_manager_params_level_evaluator.h"
#include "../../game_location_selector.h"
#include "../../level_location_selector.h"
#include "../../detail_path_manager.h"
#include "../../ai_object_location.h"
#include "../../ai_space.h"
#include "../../movement_manager_space.h"

CControlPathBuilder::CControlPathBuilder(CCustomMonster *monster) : CMovementManager(monster)
{
	m_selector_approach		= xr_new<CVertexEvaluator<aiSearchRange | aiEnemyDistance>  >();
}
CControlPathBuilder::~CControlPathBuilder()
{
	xr_delete(m_selector_approach);
}

void CControlPathBuilder::load(LPCSTR section)
{
	m_selector_approach->Load	(section,"selector_approach");
}

void CControlPathBuilder::reinit()
{
	// todo: remove call twice [CustomMonster reinit && control_manager reinit]
	inherited::reinit			();
	inherited_com::reinit		();
}

void CControlPathBuilder::reset_data() 
{
	// init data
	m_data.use_dest_orientation		= false;
	m_data.dest_orientation.set		(0.f,0.f,0.f);

	m_data.try_min_time				= true;

	m_data.target_position.set		(0.f,0.f,0.f);
	m_data.target_node				= u32(-1);

	m_data.enable					= false;
	m_data.extrapolate				= false;

	m_data.velocity_mask			= u32(-1);
	m_data.desirable_mask			= u32(-1);

	m_data.path_type				= MovementManager::ePathTypeLevelPath;
}


void CControlPathBuilder::update_schedule() 
{
	START_PROFILE("AI/Base Monster/Think/Update Path");
	
	// the one and only reason is because of the restriction-change, so wait until
	// position and node will be in valid state
	if (!accessible(m_data.target_position) || 
		( (m_data.target_node != u32(-1)) && (!accessible(m_data.target_node)) )) {
		return;
	}

	// reset evaluator
	level_selector().set_evaluator			(0);
	use_selector_path						(false);
	
	// set enabled
	enable_movement							(m_data.enable);

	// set params only if enable params
	if (m_data.enable) {
		detail().set_path_type				(eDetailPathTypeSmooth);

		// установить direction
		detail().set_use_dest_orientation	(m_data.use_dest_orientation);
		if (m_data.use_dest_orientation)	detail().set_dest_direction	(m_data.dest_orientation);

		detail().set_try_min_time			(m_data.try_min_time);

		extrapolate_path					(m_data.extrapolate);

		detail().set_velocity_mask			(m_data.velocity_mask);
		detail().set_desirable_mask			(m_data.desirable_mask);

		set_path_type						(m_data.path_type);
		if (m_data.path_type == MovementManager::ePathTypeGamePath)			
			game_selector().set_selection_type	(eSelectionTypeRandomBranching);
		else {	
			// set target
			detail().set_dest_position			(m_data.target_position);
			
			if (m_data.target_node != u32(-1))
				set_level_dest_vertex			(m_data.target_node);

			// set selector
			if (m_data.target_node == u32(-1)) {
				level_selector().set_evaluator		(m_selector_approach);
				level_selector().set_query_interval	(0);
				init_selector						(m_selector_approach, m_data.target_position);
				use_selector_path					(true);
			}
		}
	}

	update_path								();

	STOP_PROFILE;
}

void CControlPathBuilder::init_selector(CAbstractVertexEvaluator *S, Fvector target_pos)
{
	S->m_dwCurTime		= Device.dwTimeGlobal;
	S->m_tMe			= inherited_com::m_object;
	S->m_tpMyNode		= inherited_com::m_object->ai_location().level_vertex();
	S->m_tMyPosition	= inherited_com::m_object->Position();
	S->m_dwStartNode	= inherited_com::m_object->ai_location().level_vertex_id();
	S->m_tStartPosition	= inherited_com::m_object->Position();
	S->m_tEnemyPosition	= target_pos;
	S->m_tEnemy			= 0;
}

void CControlPathBuilder::on_travel_point_change(const u32 &previous_travel_point_index)
{
	m_man->notify	(ControlCom::eventTravelPointChange, 0);
}

void CControlPathBuilder::on_build_path()
{
	m_man->notify	(ControlCom::eventPathBuilt, 0);
}

//////////////////////////////////////////////////////////////////////////
// Special Build Path
//////////////////////////////////////////////////////////////////////////
bool CControlPathBuilder::is_path_built()
{
	return (!path_completed() && (detail().time_path_built() >= Device.dwTimeGlobal));
}

bool CControlPathBuilder::build_special(const Fvector &target, u32 node, u32 vel_mask)
{
	if (!accessible(target)) return false;
	
	if (node == u32(-1)) {
		// нода в прямой видимости?
		restrictions().add_border(object().Position(), target);
		node = ai().level_graph().check_position_in_direction(object().ai_location().level_vertex_id(),object().Position(),target);
		restrictions().remove_border();

		if (!ai().level_graph().valid_vertex_id(node) || !accessible(node)) return false;
	}

	enable_movement						(true);

	detail().set_velocity_mask			(vel_mask);	
	detail().set_desirable_mask			(vel_mask);

	detail().set_try_min_time			(false); 
	detail().set_use_dest_orientation	(false);

	level_selector().set_evaluator		(0);
	detail().set_path_type				(eDetailPathTypeSmooth);
	set_path_type						(MovementManager::ePathTypeLevelPath);

	detail().set_dest_position			(target);
	set_level_dest_vertex				(node);

	set_build_path_at_once				();
	update_path							();	

	if (is_path_built())				return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
// Services
//////////////////////////////////////////////////////////////////////////

bool CControlPathBuilder::is_path_end(float dist_to_end)
{
	if (detail().path().size() < 2) return true;
	if (path_completed())			return true;

	float cur_dist_to_end = 0.f;
	for (u32 i=detail().curr_travel_point_index(); i<detail().path().size()-1; i++) {
		cur_dist_to_end += detail().path()[i].position.distance_to(detail().path()[i+1].position);
		if (cur_dist_to_end > dist_to_end) break;
	}

	if (!is_moving_on_path() || (cur_dist_to_end < dist_to_end)) return true;
	return false;
}

bool CControlPathBuilder::valid_destination(const Fvector &pos, u32 node)
{
	return (
		ai().level_graph().valid_vertex_id(node) &&
		ai().level_graph().valid_vertex_position(pos) && 
		ai().level_graph().inside(node, pos)
	);
}
void CControlPathBuilder::fix_position(const Fvector &pos, u32 node, Fvector &res_pos)
{
	VERIFY(accessible(node));

	if (!accessible(pos)) {
		u32	level_vertex_id = restrictions().accessible_nearest(pos,res_pos);
		VERIFY	(level_vertex_id == node);
	}
}

bool CControlPathBuilder::is_moving_on_path()
{
	return (!detail().completed(inherited_com::m_object->Position()) && enabled());
}

bool CControlPathBuilder::get_node_in_radius(u32 src_node, float min_radius, float max_radius, u32 attempts, u32 &dest_node)
{
	Fvector vertex_position = ai().level_graph().vertex_position(src_node);

	for (u32 i=0; i<attempts; i++) {
		Fvector			dir;
		dir.random_dir	();
		dir.normalize	();

		Fvector			new_pos;
		new_pos.mad		(vertex_position, dir, Random.randF(min_radius, max_radius));

		dest_node		= ai().level_graph().check_position_in_direction(src_node, vertex_position, new_pos);
		if (dest_node != u32(-1) && accessible(dest_node)) return true;
	}
	return false;
}


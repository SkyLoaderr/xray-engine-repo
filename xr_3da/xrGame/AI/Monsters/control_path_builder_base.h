#pragma once

#include "ai_monster_defs.h"
#include "control_combase.h"
#include "../../movement_manager_space.h"

class CMotionStats;
class CCoverEvaluatorCloseToEnemy;

class CControlPathBuilderBase : public CControl_ComBase {
	typedef CControl_ComBase inherited;

	// -----------------------------------------------------------
	// external setup
	bool						m_try_min_time;
	bool						m_enable;
	bool						m_use_dest_orient;
	Fvector						m_dest_dir;
	MovementManager::EPathType	m_path_type;
	bool						m_extrapolate;
	u32							m_velocity_mask;
	u32							m_desirable_mask;

	// -----------------------------------------------------------
    // build path members
	// -----------------------------------------------------------

	struct STarget {
		Fvector		position;
		u32			node;
		void		init		() {
			position.set	(0.f,0.f,0.f);
			node			= u32(-1);
		}

		void		set			(const Fvector &pos, u32 vertex) {
			position.set	(pos);
			node			= vertex;
		}

	} m_target_set, m_target_selected, m_target_found;

	u32			m_time;					// время перестроения пути
	u32			m_last_time_target_set;
	float		m_distance_to_path_end;
	bool		m_failed;
	bool		m_force_rebuild;
	bool		m_wait_path_end;

	bool		m_target_actual;		// устанавливаемый таргет соответствует предыдущему

	struct {
		bool	use_covers;
		float	min_dist;
		float	max_dist;
		float	deviation;
		float	radius;
	} m_cover_info;

	enum {
		eMoveToTarget,
		eRetreatFromTarget,
	} m_target_type;

	CCoverEvaluatorCloseToEnemy	*m_cover_approach;

	// -----------------------------------------------------------

public:
						CControlPathBuilderBase				();
	virtual				~CControlPathBuilderBase			();
	
	// -------------------------------------------------------------------
	// Control Interface
	virtual void		reinit				();
	virtual void		update_frame		();
	virtual void		on_event			(ControlCom::EEventType, ControlCom::IEventData*);	
	virtual void		on_start_control	(ControlCom::EControlType type);
	virtual void		on_stop_control		(ControlCom::EControlType type);

	// -------------------------------------------------------------------

			void		pre_update			();
			

	// -------------------------------------------------------------------
	IC	void	set_try_min_time		(bool new_val) {m_try_min_time		= new_val;}
	IC	void	set_use_dest_orient		(bool new_val) {m_use_dest_orient	= new_val;}
	IC	void	disable_path			() {m_enable = false;}
	IC	void	enable_path				() {m_enable = true;}
	IC	void	set_dest_direction		(const Fvector &dir);
	IC	void	extrapolate_path		(bool val) {m_extrapolate = val;}
	IC	void	set_level_path_type		() {m_path_type = MovementManager::ePathTypeLevelPath;}
	IC	void	set_game_path_type		() {m_path_type = MovementManager::ePathTypeGamePath;}
	IC	void	set_patrol_path_type	() {m_path_type = MovementManager::ePathTypePatrolPath;}
	IC	void	set_velocity_mask		(u32 mask) {m_velocity_mask = mask;}
	IC	void	set_desirable_mask		(u32 mask) {m_desirable_mask = mask;}

	// -------------------------------------------------------------------
	// Properties
		bool	enabled					(){return m_enable;}
	
		float	get_path_angle			() {return 0.f;}


	//////////////////////////////////////////////////////////////////////////
	// special path
		bool		build_special			(const Fvector &target, u32 node, u32 vel_mask, bool linear = true) {return false;}
	
	//////////////////////////////////////////////////////////////////////////
	// 
		void		set_target_point		(const Fvector &position, u32 node = u32(-1));
		void		set_retreat_from_point	(const Fvector &position);

	IC	void		set_rebuild_time		(u32 time);
	IC	void		set_cover_params		(float min, float max, float dev, float radius);
	IC	void		set_use_covers			(bool val = true);
	IC	void		set_distance_to_end		(float dist);

		void		prepare_builder			();
		void		detour_graph_points		();
	IC	void		set_generic_parameters	();

private:
		// functional
		void		update_target_point			();
		void		check_failure				();

		bool		target_point_need_update	();
		void		find_target					();

		void		select_target				();		// выбрать 

		void		set_path_builder_params		();		// set params to control
		// results	
		IC	bool		failed					();

		void		reset						();

		void		travel_point_changed		();
};

#include "control_path_builder_base_inline.h"

#pragma once

#include "../movement_manager.h"
#include "../movement_manager_space.h"
#include "ai_monster_defs.h"
#include "ai_monster_motion_stats.h"

class CMotionStats;
class CAbstractVertexEvaluator;
class CBaseMonster;
class CCoverEvaluatorCloseToEnemy;

class CMonsterMovement : public CMovementManager {
	typedef CMovementManager inherited;
public:
	CBaseMonster		*m_object;
	bool				b_try_min_time;
	bool				b_enable_movement;
	bool				b_use_dest_orient;


	TTime				time_last_approach;

	bool				b_targeted_path;		// ���������������� ���� ���� � ������� �����

	SVelocity			m_velocity_linear;
	float				m_velocity_angular;

	CMotionStats		*MotionStats;

		bool	IsMoveAlongPathFinished	();
		bool	IsMovingOnPath			();
		bool	ObjectNotReachable		(const CEntity *entity);
	IC	CBaseMonster	&object			() const;

	// -------------------------------------------------------------------

		bool	IsPathEnd				(u32 n_points, float dist_to_end);
		bool	IsPathEnd				(float dist_to_end);
			
	// -------------------------------------------------------------------

	IC	void	set_try_min_time		(bool new_val) {b_try_min_time		= new_val;}
	IC	void	set_use_dest_orient		(bool new_val) {b_use_dest_orient	= new_val;}
	IC	void	disable_path			() {b_enable_movement = false;}
	IC	void	enable_path				() {b_enable_movement = true;}

		void	set_dest_direction		(const Fvector &dir);

	IC 	bool	is_path_targeted		() {return b_targeted_path;}
	IC	void	set_path_targeted		(bool val = true) {b_targeted_path = val;}
	// -------------------------------------------------------------------


		void	stop_now				();
	IC	void	stop_linear				() {m_velocity_linear.set		(0.f,0.f);}	// set current && target to zero
	IC	void	stop_linear_accel		() {m_velocity_linear.target	= 0.f;}		// set target only to zero
	IC	void	stop_angular			() {m_velocity_angular			= 0.f;}		// set to zero	

public:
						CMonsterMovement	(CBaseMonster *base_monster);
	virtual				~CMonsterMovement	();
	virtual void		reinit				();
	virtual void		Load				(LPCSTR section);	

				
			void		Update_Initialize	();
			void		Update_Execute		();
			void		Update_Finalize		();


			void		update_velocity		();
//private:
	
		// �������� �� ���������� ����
		bool		IsPathEnd				(u32 n_points);

		void		InitSelector			(CAbstractVertexEvaluator &S, Fvector target_pos);
		void		SetPathParams			(u32 dest_vertex_id, const Fvector &dest_pos);
		void		SetSelectorPathParams	();

		bool		GetNodeInRadius			(u32 src_node, float min_radius, float max_radius, u32 attempts, u32 &dest_node);

//////////////////////////////////////////////////////////////////////////
private:

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

		} m_target_desired, m_target_selected, m_target_required;

		u32			m_time;					// ����� ������������ ����
		u32			m_last_time_target_set;
		float		m_distance_to_path_end;
		bool		m_path_end;
		bool		m_failed;
		bool		m_force_rebuild;

		bool		m_target_actual;		// ��������������� ������ ������������� �����������

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

		CCoverEvaluatorCloseToEnemy			*m_cover_approach;
		CAbstractVertexEvaluator			*m_selector_approach;

public:		
		void		set_target_point		(const Fvector &position, u32 node = u32(-1));
		void		set_retreat_from_point	(const Fvector &position);

	IC	void		set_rebuild_time		(u32 time);
	IC	void		set_cover_params		(float min, float max, float dev, float radius);
	IC	void		set_use_covers			(bool val = true);
	IC	bool		failed					();
	IC	bool		path_end				();
	IC	void		set_distance_to_end		(float dist);

		void		initialize_movement		();
		void		detour_graph_points		();
	IC	void		set_generic_parameters	();

private:
		// functional
		void		update_target_point			();
		void		check_failure				();

		bool		target_point_need_update	();
		void		select_target				();
		void		set_selected_target			();

		void		select_target_desired		();
		
		// utils
		bool		valid_destination			(const Fvector &pos, u32 node);
		void		fix_position				(const Fvector &pos, u32 node, Fvector &res_pos);
//////////////////////////////////////////////////////////////////////////

public:
	//////////////////////////////////////////////////////////////////////////
	// special path
		bool		build_special			(const Fvector &target, u32 node, u32 vel_mask, bool linear = true);
	//////////////////////////////////////////////////////////////////////////
	// service methods
	
		float		get_path_angle			();
		bool		is_path_built			();
		void		set_velocity_from_path	();
};

#include "ai_monster_movement_inline.h"

#pragma once

#include "../movement_manager.h"
#include "ai_monster_defs.h"
#include "ai_monster_motion_stats.h"

class CMotionStats;
class CAbstractVertexEvaluator;
class CAI_Biting;
class CCoverEvaluatorCloseToEnemy;

class CMonsterMovement : virtual public CMovementManager {
	typedef CMovementManager inherited;
public:
	CAI_Biting			*pMonster;
	bool				b_try_min_time;
	bool				b_enable_movement;
	bool				b_use_dest_orient;


	TTime				time_last_approach;

	bool				b_targeted_path;		// ���������������� ���� ���� � ������� �����

	u32					m_dwFrameReinit;
	u32					m_dwFrameLoad;

	SVelocity			m_velocity_linear;
	float				m_velocity_angular;

	CMotionStats		*MotionStats;

	// -------------------------------------------------------------------
		void	MoveToTarget			(const CEntity *entity); 
		void	MoveToTarget			(const Fvector &pos, u32 node_id);
		void	MoveToTarget			(const Fvector &position);
		void	MoveAwayFromTarget		(const Fvector &position, float dist = 10.f);
	// -------------------------------------------------------------------

		bool	IsMoveAlongPathFinished	();
		bool	IsMovingOnPath			();
		bool	ObjectNotReachable		(const CEntity *entity);

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
						CMonsterMovement	();
	virtual				~CMonsterMovement	();
			
			void		InitExternal		(CAI_Biting	*pM);
	virtual void		reinit				();
	virtual void		Load				(LPCSTR section);	

				
			void		Update_Initialize	();
			void		Update_Execute		();
			void		Update_Finalize		();


			void		WalkNextGraphPoint	();

			void		update_velocity		();
//private:
	
		// �������� �� ���������� ����
		bool		IsPathEnd				(u32 n_points);

		void		InitSelector			(CAbstractVertexEvaluator &S, Fvector target_pos);
		void		SetPathParams			(u32 dest_vertex_id, const Fvector &dest_pos);
		void		SetSelectorPathParams	();


//////////////////////////////////////////////////////////////////////////
public:

		struct {
			Fvector		position;
			u32			node;
		} m_target, m_intermediate;

		u32			m_time;					// ����� ������������ ����
		u32			m_last_time_target_set;
		float		m_distance_to_path_end;
		bool		m_path_end;
		bool		m_failed;
		
		struct {
			bool	use_covers;
			float	min_dist;
			float	max_dist;
			float	deviation;
			float	radius;
		} m_cover_info;

		void		set_target_point		(const Fvector &position, u32 node = u32(-1));
	IC	void		set_rebuild_time		(u32 time);
	IC	void		set_use_covers			(float min, float max, float dev, float radius);
	IC	void		force_target_set		();
	IC	bool		failed					();
	IC	bool		path_end				();

		CCoverEvaluatorCloseToEnemy			*m_cover_approach;
		CAbstractVertexEvaluator			*m_selector_approach;

		void		get_intermediate		();
		bool		position_in_direction	(const Fvector &target, u32 &node);
		void		set_parameters			();
		void		update_target_point		();

		void		initialize_movement		();

		
};

#include "ai_monster_movement_inline.h"

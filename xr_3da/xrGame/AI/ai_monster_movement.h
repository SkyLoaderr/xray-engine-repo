#pragma once

#include "../movement_manager.h"
#include "ai_monster_defs.h"
#include "ai_monster_motion_stats.h"

class CMotionStats;
class CAbstractVertexEvaluator;
class CAI_Biting;

class CMonsterMovement : virtual public CMovementManager {
	typedef CMovementManager inherited;

	CAI_Biting			*pMonster;
	bool				b_try_min_time;
	bool				b_enable_movement;
	bool				b_use_dest_orient;


	TTime				time_last_approach;

	bool				b_targeted_path;		// предположительно путь ведёт к целевой точке

	u32					m_dwFrameReinit;

public:

	SVelocity			m_velocity_linear;
	float				m_velocity_angular;

public:

		CAbstractVertexEvaluator	*m_tSelectorApproach;
		CMotionStats				*MotionStats;

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
						~CMonsterMovement	();
			void		InitExternal		(CAI_Biting	*pM);

	virtual void		reinit				();
		

				
			void		Frame_Init			();
			void		Frame_Update		();
			void		Frame_Finalize		();
		

			void		WalkNextGraphPoint	();

			void		update_velocity		();
private:
	
		// проверка на завершение пути
		bool		IsPathEnd				(u32 n_points);

		void		InitSelector			(CAbstractVertexEvaluator &S, Fvector target_pos);
		void		SetPathParams			(u32 dest_vertex_id, const Fvector &dest_pos);
		void		SetSelectorPathParams	();

		
};
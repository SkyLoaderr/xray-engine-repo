#pragma once

#include "../movement_manager.h"
#include "ai_monster_defs.h"
#include "ai_monster_motion_stats.h"

class CMotionStats;
class PathManagers::CAbstractVertexEvaluator;
class CAI_Biting;

class CMonsterMovement : virtual public CMovementManager {
	typedef CMovementManager inherited;

	CAI_Biting			*pMonster;
	bool				b_try_min_time;
	bool				b_enable_movement;

	TTime				time_last_approach;

public:

		PathManagers::CAbstractVertexEvaluator	*m_tSelectorApproach;

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
			
	// -------------------------------------------------------------------

	IC	void	set_try_min_time		(bool new_val) {b_try_min_time = new_val;}
	IC	void	disable_path			() {b_enable_movement = false;}
	IC	void	enable_path				() {b_enable_movement = true;}

	// -------------------------------------------------------------------

public:
				CMonsterMovement		();
				~CMonsterMovement		();

		void		Init				();
				
		void		Frame_Init			();
		void		Frame_Update		();
		void		Frame_Finalize		();
	

		void		WalkNextGraphPoint	();

private:
	
		// проверка на завершение пути
		bool		IsPathEnd				(u32 n_points);
		bool		IsPathEnd				(float dist_to_end);

		void		InitSelector			(PathManagers::CAbstractVertexEvaluator &S, Fvector target_pos);
		void		SetPathParams			(u32 dest_vertex_id, const Fvector &dest_pos);
		void		SetSelectorPathParams	();

};
#pragma once

#include "../movement_manager.h"
#include "ai_monster_defs.h"

class CMotionStats;
class PathManagers::CAbstractVertexEvaluator;
class CAI_Biting;

class CMonsterMovement : public CMovementManager {

public:

	CAI_Biting			*pMonster;

	bool				b_try_min_time;

	PathManagers::CAbstractVertexEvaluator	*m_tSelectorGetAway;
	PathManagers::CAbstractVertexEvaluator	*m_tSelectorApproach;

	CMotionStats		*MotionStats;
	
	
	void MoveToTarget			(const CEntity *entity); 
	void MoveToTarget			(const Fvector &pos, u32 node_id);
	bool IsMoveAlongPathFinished();
	bool IsMovingOnPath			();
	bool ObjectNotReachable		(const CEntity *entity);

	bool	NeedRebuildPath		(u32 n_points);
	bool	NeedRebuildPath		(float dist_to_end);
	bool	NeedRebuildPath		(u32 n_points, float dist_to_end);




public:
			CMonsterMovement		();
			~CMonsterMovement		();

	void	Load					();
	void	Init					();
	void	Reinit					();
			
	void	Frame_Init				();
	void	Frame_Update			();
	void	Frame_Finalize			();
	

public:	
	
	void			InitSelector					(PathManagers::CAbstractVertexEvaluator &S, Fvector target_pos);
	void			Path_GetAwayFromPoint			(Fvector position, float dist);
	void			Path_ApproachPoint				(Fvector position);

	
	
	void			SetPathParams					(u32 dest_vertex_id, const Fvector &dest_pos);
	void			SetSelectorPathParams			();

	void			WalkNextGraphPoint();

};
#pragma once

class CMonsterMovement : virtual public CMovementManager {
	
	CAI_Biting			*pMonster;

	bool				b_try_min_time;

	PathManagers::CAbstractVertexEvaluator	*m_tSelectorGetAway;
	PathManagers::CAbstractVertexEvaluator	*m_tSelectorApproach;


//	CMotionStats	*MotionStats;
//
//	void MoveToTarget			(const CEntity *entity); 
//	void MoveToTarget			(const Fvector &pos, u32 node_id);
//	void FaceTarget				(const CEntity *entity);
//	void FaceTarget				(const Fvector &position);
//	bool IsObjectPositionValid	(const CEntity *entity);
//	bool IsMoveAlongPathFinished();
//	bool IsMovingOnPath			();
//	bool ObjectNotReachable		(const CEntity *entity);
//
//	Fvector	RandomPosInR		(const Fvector &p, float r);
//
//	void	UpdateVelocities	(STravelParams cur_velocity);
//
//	xr_vector<STravelParams> velocities;
//
//	u32		GetNextGameVertex	(float R);
//
//	bool	NeedRebuildPath		(u32 n_points);
//	bool	NeedRebuildPath		(float dist_to_end);
//	bool	NeedRebuildPath		(u32 n_points, float dist_to_end);

	enum EMovementParameters {
		eVelocityParameterStand			= u32(1) <<  4,
		eVelocityParameterWalkNormal	= u32(1) <<  3,
		eVelocityParameterRunNormal		= u32(1) <<  2,

		eVelocityParameterWalkDamaged	= u32(1) <<  5,
		eVelocityParameterRunDamaged	= u32(1) <<  6,
		eVelocityParameterSteal			= u32(1) <<  7,
		eVelocityParameterDrag			= u32(1) <<  8,

		eVelocityParamsWalk				= eVelocityParameterStand | eVelocityParameterWalkNormal,
		eVelocityParamsRun				= eVelocityParameterStand | eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsAttackNorm		= eVelocityParameterStand | eVelocityParameterWalkNormal | eVelocityParameterRunNormal,
		eVelocityParamsAttackDamaged	= eVelocityParameterStand | eVelocityParameterWalkDamaged | eVelocityParameterRunDamaged,
		eVelocityParamsSteal			= eVelocityParameterStand | eVelocityParameterSteal,
	};

public:
			CMonsterMovement		();
			~CMonsterMovement		();

	void	Load					();
	void	Init					();
			
	void	Frame_Init				();
	void	Frame_Update			();
	void	Frame_Finalize			();
	
protected:	
	
	void			InitSelector					(PathManagers::CAbstractVertexEvaluator &S, Fvector target_pos);
	void			Path_GetAwayFromPoint			(Fvector position, float dist);
	void			Path_ApproachPoint				(Fvector position);

	void			SetPathParams					(u32 dest_vertex_id, const Fvector &dest_pos);
	void			SetSelectorPathParams			();

	void			SetVelocity						();
	void			PreprocessAction				();

	bool			IsObstacle						(TTime time);
	void			SetupVelocityMasks				(bool force_real_speed);


};
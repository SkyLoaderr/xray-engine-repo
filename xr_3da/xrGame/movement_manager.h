////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"
#include "path_manager_game_selector.h"
#include "path_manager_level_selector.h"
#include "game_location_selector.h"
#include "level_location_selector.h"
#include "game_path_manager.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "enemy_location_predictor.h"
#include "patrol_path_manager.h"

class CPHMovementControl;

class CMovementManager : 
	public CBaseLocationSelector<CGameGraph,SVertexType<float,u32,u32>,u32>,
	public CBasePathManager		<CGameGraph,SBaseParameters<float,u32,u32>,u32,u32>,
	public CBaseLocationSelector<CLevelGraph,CAbstractVertexEvaluator,u32>,
	public CBasePathManager		<CLevelGraph,SBaseParameters<float,u32,u32>,u32,u32>,
	public CDetailPathManager,
	public CEnemyLocationPredictor,
	public CPatrolPathManager,
	virtual public CGameObject
{
protected:
	typedef CBaseLocationSelector	<CGameGraph,SVertexType<float,u32,u32>,u32>			CGameLocationSelector;
	typedef CBasePathManager		<CGameGraph,SBaseParameters<float,u32,u32>,u32,u32>	CGamePathManager;
	typedef CBaseLocationSelector	<CLevelGraph,CAbstractVertexEvaluator,u32>			CLevelLocationSelector;
	typedef CBasePathManager		<CLevelGraph,SBaseParameters<float,u32,u32>,u32,u32>	CLevelPathManager;

private:
	enum EPathState {
		ePathStateSelectGameVertex = u32(0),
		ePathStateBuildGamePath,
		ePathStateContinueGamePath,
		
		ePathStatePredictEnemyVertices,
		ePathStateSelectEnemyVertex,
		ePathStateContinueEnemySearch,

		ePathStateSelectLevelVertex,
		ePathStateBuildLevelPath,
		ePathStateContinueLevelPath,

		ePathStateSelectPatrolPoint,
		
		ePathStateBuildDetailPath,
		
		ePathStatePathVerification,
		
		ePathStatePathCompleted,
		
		ePathStateDummy = u32(-1),
	};

public:
	enum EPathType {
		ePathTypeGamePath = u32(0),
		ePathTypeLevelPath,
		ePathTypeEnemySearch,
		ePathTypePatrolPath,
		ePathTypeNoPath,
		ePathTypeDummy = u32(-1),
	};

public:
	struct SBoneRotation {
		SRotation		current;
		SRotation		target;
		float			speed;
	};

private:
	EPathState								m_path_state;
	EPathType								m_path_type;
	bool									m_path_actuality;
	bool									m_enabled;
	u64										m_start_time;
	u64										m_time_work;
	CGraphEngine::CBaseParameters			*m_base_game_selector;
	CGraphEngine::CBaseParameters			*m_base_level_selector;
	float									m_speed;
	float									m_old_desirable_speed;
	bool									m_selector_path_usage;
	u32										m_dwCurrentFrame;
	u32										m_refresh_rate;
	u32										m_last_update;

public:
	SBoneRotation							m_body;

private:

	IC		void	time_start				();
	IC		bool	time_over				() const;
			void	process_game_path		();
			void	process_level_path		();
			void	process_enemy_search	();
			void	process_patrol_path		();

	friend class CScriptMonster;
	friend class CGroup;

public:

					CMovementManager		();
	virtual			~CMovementManager		();
			void	Init					();
	virtual void	Load					(LPCSTR caSection);
	virtual void	reinit					();
	virtual void	reload					(LPCSTR caSection);
	IC		bool	actual					() const;
	IC		bool	actual_all				() const;
	IC		void	set_path_type			(EPathType path_type);
	IC		void	set_game_dest_vertex	(const ALife::_GRAPH_ID game_vertex_id);
	IC		void	set_level_dest_vertex	(const u32 level_vertex_id);
	IC		void	enable_movement			(bool enabled);
	IC		EPathType path_type				() const;
	IC		ALife::_GRAPH_ID game_dest_vertex_id	() const;
	IC		u32		level_dest_vertex_id	() const;
	IC		bool	enabled					() const;
	IC		bool	path_completed			() const;
	IC		float	old_desirable_speed		() const;
	IC		void	set_desirable_speed		(float speed);
	IC		void	use_selector_path		(bool selector_path_usage);
	IC		bool	selector_path_used		() const;
	IC		const xr_vector<STravelPathPoint>	&path	() const;
	IC		void	set_body_orientation(const CMovementManager::SBoneRotation &orientation);
	IC		const CMovementManager::SBoneRotation &body_orientation() const;
	IC		CGraphEngine::CBaseParameters	*base_game_selector();
	IC		CGraphEngine::CBaseParameters	*base_level_selector();
	IC		void	set_refresh_rate		(u32 refresh_rate);
	IC		u32		refresh_rate			() const;
			void	update_path				();
			void	move_along_path			(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta);
			float	speed					() const;
};

#include "movement_manager_inline.h"

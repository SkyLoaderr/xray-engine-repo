////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : m_PhysicMovementControl manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "path_manager_game_selector.h"
#include "path_manager_level_selector.h"
#include "abstract_location_selector.h"
#include "abstract_path_manager.h"
#include "detail_path_manager.h"
#include "enemy_location_predictor.h"
#include "gameobject.h"

class CPHMovementControl;

class CMovementManager : 
	public CAbstractLocationSelector<CGameGraph,PathManagers::SVertexType<float,u32,u32>,u32>,
	public CAbstractPathManager		<CGameGraph,PathManagers::SBaseParameters<float,u32,u32>,u32>,
	public CAbstractLocationSelector<CLevelGraph,PathManagers::CAbstractVertexEvaluator,u32>,
	public CAbstractPathManager		<CLevelGraph,PathManagers::SBaseParameters<float,u32,u32>,u32>,
	public CDetailPathManager,
	public CEnemyLocationPredictor,
	virtual public CGameObject
{
private:
	typedef CAbstractLocationSelector	<CGameGraph,PathManagers::SVertexType<float,u32,u32>,u32>		CGameLocationSelector;
	typedef CAbstractPathManager		<CGameGraph,PathManagers::SBaseParameters<float,u32,u32>,u32>	CGamePathManager;
	typedef CAbstractLocationSelector	<CLevelGraph,PathManagers::CAbstractVertexEvaluator,u32>		CLevelLocationSelector;
	typedef CAbstractPathManager		<CLevelGraph,PathManagers::SBaseParameters<float,u32,u32>,u32>	CLevelPathManager;

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
		ePathTypeDummy = u32(-1),
	};

private:
	EPathState								m_path_state;
	EPathType								m_path_type;
	bool									m_path_actuality;
	u64										m_start_time;
	u64										m_time_work;
	CGraphSearchEngine::CBaseParameters		*m_base_game_selector;
	CGraphSearchEngine::CBaseParameters		*m_base_level_selector;

	IC		void	time_start				();
	IC		bool	time_over				() const;
	IC		bool	path_actual				() const;
			void	process_game_path		();
			void	process_level_path		();
			void	process_enemy_search	();

protected:
	u32										m_game_dest_vertex_id;
	u32										m_level_dest_vertex_id;
	Fvector									m_detail_dest_position;
	float									m_speed;

	friend class CScriptMonster;
	friend class CGroup;

public:
					CMovementManager		();
	virtual			~CMovementManager		();
	virtual void	Init					();
			void	move_along_path			(CPHMovementControl *movement_control, float time_delta);
	IC		void	set_path_type			(EPathType path_type);
	IC		void	set_game_dest_node		(const ALife::_GRAPH_ID game_vertex_id);
	IC		void	set_level_dest_node		(const u32 level_vertex_id);
			void	build_path				();
};

#include "movement_manager_inline.h"

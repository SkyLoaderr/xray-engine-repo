////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"
#include "game_location_selector.h"
#include "level_location_selector.h"
#include "game_path_manager.h"
#include "level_path_manager.h"
#include "detail_path_manager.h"
#include "enemy_location_predictor.h"
#include "patrol_path_manager.h"
#include "ai_monster_space.h"
#include "physicsshellholder.h"
#include "restricted_object.h"

class CPHMovementControl;

namespace MovementManager {
	enum EPathType {
		ePathTypeGamePath = u32(0),
		ePathTypeLevelPath,
		ePathTypeEnemySearch,
		ePathTypePatrolPath,
		ePathTypeNoPath,
		ePathTypeDummy = u32(-1),
	};
};

using namespace MovementManager;

class CMovementManager : 
	public CBaseLocationSelector<CGameGraph,SVertexType<float,u32,u32>,u32>,
	public CBasePathManager		<CGameGraph,SBaseParameters<float,u32,u32>,u32,u32>,
	public CBaseLocationSelector<CLevelGraph,CAbstractVertexEvaluator,u32>,
	public CBasePathManager		<CLevelGraph,SBaseParameters<float,u32,u32>,u32,u32>,
	public CDetailPathManager,
	public CEnemyLocationPredictor,
	public CPatrolPathManager,
	public CRestrictedObject,
	virtual public CPhysicsShellHolder
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

private:
	EPathState								m_path_state;
	EPathType								m_path_type;
	bool									m_path_actuality;
	bool									m_enabled;
	u64										m_start_time;
	u64										m_time_work;
	CGraphEngine::CBaseParameters			*m_base_game_selector;
	CGraphEngine::CBaseParameters			*m_base_level_selector;
	float									m_old_desirable_speed;
	bool									m_selector_path_usage;
	u32										m_dwFrameLoad;
	u32										m_dwFrameReinit;
	u32										m_dwFrameReload;
	u32										m_dwFrameNetSpawn;
	u32										m_dwFrameNetDestroy;
	u32										m_refresh_rate;
	u32										m_last_update;

protected:
	float									m_speed;
public:
	MonsterSpace::SBoneRotation				m_body;

private:

	IC		void	time_start				();
	IC		bool	time_over				() const;
			void	process_game_path		();
			void	process_level_path		();
			void	process_enemy_search	();
			void	process_patrol_path		();
			void	verify_detail_path		();

	friend class CScriptMonster;
	friend class CGroup;

public:

					CMovementManager		();
	virtual			~CMovementManager		();
			void	init					();
	virtual void	Load					(LPCSTR caSection);
	virtual void	reinit					();
	virtual void	reload					(LPCSTR caSection);
	virtual BOOL	net_Spawn				(LPVOID data);
	virtual void	net_Destroy				();
	virtual	void	Hit						(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type = ALife::eHitTypeWound);
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
	IC		void	set_body_orientation(const MonsterSpace::SBoneRotation &orientation);
	IC		const MonsterSpace::SBoneRotation &body_orientation() const;
	IC		CGraphEngine::CBaseParameters	*base_game_selector();
	IC		CGraphEngine::CBaseParameters	*base_level_selector();
	IC		void	set_refresh_rate		(u32 refresh_rate);
	IC		u32		refresh_rate			() const;
			void	update_path				();
	virtual	void	move_along_path			(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta);
			float	speed					() const;

	virtual void	on_travel_point_change	() {}

	template <typename T>
	IC		bool	accessible				(T position_or_vertex_id, float radius = EPS_L) const;
};

#include "movement_manager_inline.h"

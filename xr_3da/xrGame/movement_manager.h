////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "physicsshellholder.h"
#include "ai_monster_space.h"
#include "graph_engine_space.h"
#include "game_graph_space.h"

namespace MovementManager {
	enum EPathType;
};

namespace DetailPathManager {
	enum EDetailPathType;
};

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class CBaseLocationSelector;

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type,
	typename _index_type
>
class CBasePathManager;

template <
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>
struct SVertexType;

template <
	typename _dist_type,
	typename _index_type,
	typename _iteration_type
>
struct SBaseParameters;

class CEnemyLocationPredictor;
class CPatrolPathManager;
class CDetailPathManager;
class CPHMovementControl;
class CGameGraph;
class CLevelGraph;
class CAbstractVertexEvaluator;
class CRestrictedObject;
class CSelectorManager;
class CLocationManager;
class CCustomMonster;

namespace DetailPathManager {
	struct STravelPathPoint;
};

class CMovementManager {
	friend class CScriptEntity;
	friend class CGroup;
	friend class CPoltergeist;

protected:
	typedef MonsterSpace::SBoneRotation			CBoneRotation;
	typedef MovementManager::EPathType			EPathType;
	typedef DetailPathManager::STravelPathPoint	CTravelPathPoint;
	typedef GraphEngineSpace::CBaseParameters 	CBaseParameters;
	typedef GraphEngineSpace::CBaseParameters	CBaseParameters;

	typedef CBaseLocationSelector<
				CGameGraph,
				SVertexType<
					float,
					u32,
					u32
				>,
				u32
			>		CGameLocationSelector;
	typedef CBasePathManager<
				CGameGraph,
				SBaseParameters<
					float,
					u32,
					u32
				>,
				u32,
				u32
			>		CGamePathManager;
	typedef CBaseLocationSelector<
				CLevelGraph,
				CAbstractVertexEvaluator,
				u32
			>		CLevelLocationSelector;
	typedef CBasePathManager<
				CLevelGraph,
				SBaseParameters<
					float,
					u32,
					u32
				>,
				u32,
				u32
			>		CLevelPathManager;

private:
	enum EPathState {
		ePathStateSelectGameVertex = u32(0),
		ePathStateBuildGamePath,
		ePathStateContinueGamePath,
		
		ePathStateSelectPatrolPoint,

		ePathStateSelectLevelVertex,
		ePathStateBuildLevelPath,
		ePathStateContinueLevelPath,

		ePathStateBuildDetailPath,
		
		ePathStatePathVerification,
		
		ePathStatePathCompleted,

		ePathStateTeleport,
		
		ePathStateDummy = u32(-1),
	};

protected:
	float					m_speed;

public:
	CBoneRotation			m_body;

private:
	EPathState				m_path_state;
	EPathType				m_path_type;
	bool					m_path_actuality;
	bool					m_enabled;
	u64						m_start_time;
	u64						m_time_work;
	float					m_old_desirable_speed;
	bool					m_selector_path_usage;
	u32						m_refresh_rate;
	u32						m_last_update;
	bool					m_extrapolate_path;
	bool					m_build_at_once;

public:
	CBaseParameters			*m_base_game_selector;
	CBaseParameters			*m_base_level_selector;
	CGameLocationSelector	*m_game_location_selector;
	CGamePathManager		*m_game_path_manager;
	CLevelLocationSelector	*m_level_location_selector;
	CLevelPathManager		*m_level_path_manager;
	CDetailPathManager		*m_detail_path_manager;
	CPatrolPathManager		*m_patrol_path_manager;
	CRestrictedObject		*m_restricted_object;
	CSelectorManager		*m_selector_manager;
	CLocationManager		*m_location_manager;
	CCustomMonster			*m_object;

private:
	IC		void	time_start				();
	IC		bool	time_over				() const;
			void	process_game_path		();
			void	process_level_path		();
			void	process_patrol_path		();
			void	verify_detail_path		();

protected:
	virtual void	teleport				(u32 game_vertex_id);

public:

					CMovementManager		(CCustomMonster *object);
	virtual			~CMovementManager		();
	virtual void	Load					(LPCSTR caSection);
	virtual void	reinit					();
	virtual void	reload					(LPCSTR caSection);
	virtual BOOL	net_Spawn				(CSE_Abstract* data);
	IC		bool	actual					() const;
			bool	actual_all				() const;
	IC		void	set_path_type			(EPathType path_type);
			void	set_game_dest_vertex	(const GameGraph::_GRAPH_ID game_vertex_id);
			void	set_level_dest_vertex	(const u32 level_vertex_id);
	IC		void	set_build_path_at_once	();
	IC		void	enable_movement			(bool enabled);
			EPathType path_type				() const;
			GameGraph::_GRAPH_ID game_dest_vertex_id	() const;
			u32		level_dest_vertex_id	() const;
	IC		bool	enabled					() const;
	IC		bool	path_completed			() const;
	IC		float	old_desirable_speed		() const;
	IC		void	set_desirable_speed		(float speed);
	IC		void	use_selector_path		(bool selector_path_usage);
	IC		bool	selector_path_used		() const;
			const xr_vector<CTravelPathPoint>	&path	() const;
	IC		void	set_body_orientation	(const MonsterSpace::SBoneRotation &orientation);
	IC		const CBoneRotation &body_orientation() const;
	IC		void	set_refresh_rate		(u32 refresh_rate);
	IC		u32		refresh_rate			() const;
			void	update_path				();
	virtual	void	move_along_path			(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta);
	IC		float	speed					() const;
			float	speed					(CPHMovementControl *movement_control) const;

	virtual void	on_travel_point_change	() {}

	template <typename T>
	IC		bool	accessible				(T position_or_vertex_id, float radius = EPS_L) const;

	IC		void	extrapolate_path		(bool value);
	IC		bool	extrapolate_path		() const;

public:
	IC		CBaseParameters			*base_game_params			() const;
	IC		CBaseParameters			*base_level_params			() const;
	IC		CGameLocationSelector	&game_selector				() const;
	IC		CGamePathManager		&game_path					() const;
	IC		CLevelLocationSelector	&level_selector				() const;
	IC		CLevelPathManager		&level_path					() const;
	IC		CDetailPathManager		&detail						() const;
	IC		CPatrolPathManager		&patrol						() const;
	IC		CRestrictedObject		&restrictions				() const;
	IC		CSelectorManager		&selectors					() const;
	IC		CLocationManager		&locations					() const;
	IC		CCustomMonster			&object						() const;
};

#include "movement_manager_inline.h"

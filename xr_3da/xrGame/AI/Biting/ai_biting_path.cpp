////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_path.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Path finding, curve building, position prediction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"
#include "../../graph_engine.h"
#include "../ai_monsters_misc.h"
#include "../../game_graph.h"
#include "../../game_level_cross_table.h"

#define MIN_RANGE_SEARCH_TIME_INTERVAL	 4000		// 2 сек
#define TIME_TO_SEARCH					60000
#define DODGE_AMPLITUDE					.5f
#define MAX_DODGE_DISTANCE				1.5f


/////////////////////////////////////////////////////////////////////////////////////
// Функция InitSelector

void CAI_Biting::vfInitSelector(PathManagers::CAbstractVertexEvaluator &S, bool hear_sound)
{
	S.m_dwCurTime		= m_current_update;
	S.m_tMe				= this;
	S.m_tpMyNode		= level_vertex();
	S.m_tMyPosition		= Position();

	if (!hear_sound && m_tEnemy.obj) {
		S.m_tEnemy			= m_tEnemy.obj;
		S.m_tEnemyPosition	= m_tEnemy.position;
		S.m_dwEnemyNode		= m_tEnemy.node_id;
		S.m_tpEnemyNode		= m_tEnemy.vertex;
	}
	
	INIT_SQUAD_AND_LEADER;

	S.m_taMembers		= &(Squad.Groups[g_Group()].Members);
	S.m_dwStartNode		= level_vertex_id();		// текущий узел
	S.m_tStartPosition	= Position();
}

/////////////////////////////////////////////////////////////////////////////////////
// Функция SearchForBetterPosition
void CAI_Biting::vfSearchForBetterPosition(PathManagers::CAbstractVertexEvaluator &/**tNodeEvaluator/**/, CSquad &/**Squad/**/, CEntity* &/**Leader/**/)
{
//	Device.Statistic.AI_Range.Begin();	// определение времени вып. функции
//
//	if ((!m_previous_query_time) || (CDetailPathManager::m_path.empty()) || (int(CDetailPathManager::curr_travel_point_index()) > int(CDetailPathManager::m_path.size()) - 4) || (speed() < EPS_L) || ((tNodeEvaluator.m_dwCurTime - m_previous_query_time > MIN_RANGE_SEARCH_TIME_INTERVAL))) {
//	//   если запускается в первый раз || нет пути             ||  
//
//		m_previous_query_time = tNodeEvaluator.m_dwCurTime;
//
//		float fOldCost = MAX_NODE_ESTIMATION_COST;
//
//		if (ai().level_graph().valid_vertex_id(m_level_dest_vertex_id)) {
//			tNodeEvaluator.m_tpCurrentNode	= ai().level_graph().vertex(m_level_dest_vertex_id);
//			tNodeEvaluator.m_fDistance		= Position().distance_to(ai().level_graph().vertex_position(m_level_dest_vertex_id));
//			fOldCost						= tNodeEvaluator.ffEvaluateNode();
//		}
//
//		Squad.Groups[g_Group()].GetAliveMemberInfo(tNodeEvaluator.m_taMemberPositions, tNodeEvaluator.m_taMemberNodes, tNodeEvaluator.m_taDestMemberPositions, tNodeEvaluator.m_taDestMemberNodes, this);
//
//		Device.Statistic.AI_Range.End();
//
//		tNodeEvaluator.vfShallowGraphSearch(ai().q_mark_bit);
//
//		Device.Statistic.AI_Range.Begin();
//
//		if ((m_level_dest_vertex_id != tNodeEvaluator.m_dwBestNode) && (tNodeEvaluator.m_fBestCost < fOldCost - 0.f)){
//			m_level_dest_vertex_id		= tNodeEvaluator.m_dwBestNode;
//			if (!m_level_dest_vertex_id) {
//				Msg("! Invalid vertex Evaluator vertex");
//			}
//			m_level_path.clear		();
//			m_tPathState			= PATH_STATE_BUILD_NODE_PATH;
//		} 
//	}
//
//	Device.Statistic.AI_Range.End();
}

/////////////////////////////////////////////////////////////////////////////////////
// Функция BuildPathToDestinationPoint
void CAI_Biting::vfBuildPathToDestinationPoint(PathManagers::CAbstractVertexEvaluator * /**tpNodeEvaluator/**/)
{
//	Device.Statistic.AI_Path.Begin();
//
//	if (m_level_dest_vertex_id == level_vertex_id()) {
//		m_level_path.clear		();
//		CDetailPathManager::m_path.clear();
//		CDetailPathManager::curr_travel_point_index()		= 0;
//		m_fCurSpeed				= 0;
//		m_tPathState			= PATH_STATE_SEARCH_NODE;
//		Device.Statistic.AI_Path.End();
//		return;
//	}
//
//	if (tpNodeEvaluator)
//		ai().m_tpAStar->ffFindOptimalPath(level_vertex_id(),m_level_dest_vertex_id,AI_Path,tpNodeEvaluator->m_dwEnemyNode,tpNodeEvaluator->m_fOptEnemyDistance);
//	else
//		ai().m_tpAStar->ffFindMinimalPath(level_vertex_id(),m_level_dest_vertex_id,AI_Path);
//
//	if (m_level_path.empty()) {
//#ifdef DEBUG
//		Msg("! !!!! node_start %d, node_finish %d",level_vertex_id(),m_level_dest_vertex_id);
//#endif
//		
//		ai().m_tpAStar->ffFindMinimalPath(level_vertex_id(),m_level_dest_vertex_id,AI_Path);
//		
//		if (m_level_path.empty())
//			m_tPathState = PATH_STATE_SEARCH_NODE;
//		else
//			m_tPathState = PATH_STATE_BUILD_TRAVEL_LINE;
//	}
//	else
//		m_tPathState = PATH_STATE_BUILD_TRAVEL_LINE;
//
//	Device.Statistic.AI_Path.End();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Построить TravelLine
void CAI_Biting::vfBuildTravelLine(Fvector * /**tpDestinationPosition/**/)
{
//	Device.Statistic.TEST1.Begin();
//
//	m_tpaPoints.clear			();
//	m_tpaTravelPath.clear		();
//	m_tpaPointNodes.clear		();
//
//	u32							N = (u32)m_level_path.size();
//	if (!N) {
//		Msg("! vertex list is empty!");
//		m_level_path.clear();
//		CDetailPathManager::m_path.clear();
//		m_tPathState = PATH_STATE_SEARCH_NODE;
//		Device.Statistic.TEST1.End();
//		return;
//	}
//	Fvector						tStartPosition = Position();
//
//	u32							dwCurNode = level_vertex_id();
//	m_tpaPoints.push_back		(Position());
//	m_tpaPointNodes.push_back	(dwCurNode);
//
//	for (u32 i=1; i<=N; ++i)
//		if (i<N) {
//			if (!ai().level_graph().check_vertex_in_direction(dwCurNode,tStartPosition,m_level_path[i])) {
//				if (dwCurNode != m_level_path[i - 1])
//					m_tpaPoints.push_back(tStartPosition = ai().level_graph().vertex_position(dwCurNode = m_level_path[--i]));
//				else
//					m_tpaPoints.push_back(tStartPosition = ai().level_graph().vertex_position(dwCurNode = m_level_path[i]));
//				m_tpaPointNodes.push_back(dwCurNode);
//			}
//		}
//		else
//			if (tpDestinationPosition)
//				if (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(dwCurNode,tStartPosition,*tpDestinationPosition))) {
//					//VERIFY(false);
//					if (dwCurNode != m_level_dest_vertex_id)
//						m_tpaPointNodes.push_back(m_level_dest_vertex_id);
//					tpDestinationPosition->y = ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(dwCurNode),tpDestinationPosition->x,tpDestinationPosition->z);
//					m_tpaPoints.push_back(*tpDestinationPosition);
//				}
//				else {
//						tpDestinationPosition->y = ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(m_level_dest_vertex_id),tpDestinationPosition->x,tpDestinationPosition->z);
//						m_tpaPointNodes.push_back(m_level_dest_vertex_id);
//						m_tpaPoints.push_back(*tpDestinationPosition);
//				}
//
//	if (!tpDestinationPosition && (tStartPosition.distance_to(ai().level_graph().vertex_position(m_level_path[N - 1])) > ai().level_graph().header().cell_size())) {
//		m_tpaPoints.push_back(ai().level_graph().vertex_position(m_level_path[N - 1]));
//		m_tpaPointNodes.push_back(m_level_path[N - 1]);
//	}
//
//	N = (u32)m_tpaPoints.size();
//
//	CDetailPathManager::m_path.clear();
//	m_level_path.clear		();
//
//	AI::CTravelNode	T;
//	T.floating		= false;
//	T.P.set			(0,0,0);
//	for (i=1; i<N; ++i) {
//		ai().bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, 1 == i);
//		u32 n = (u32)m_tpaTravelPath.size();
//		for (u32 j= 0; j<n; ++j) {
//			T.P = m_tpaTravelPath[j];
//			CDetailPathManager::m_path.push_back(T);
//			m_level_path.push_back(m_tpaNodes[j]);
//		}
//	}
//	
//	if (m_level_path.size())
//		m_level_path[m_level_path.size() - 1] = m_level_dest_vertex_id;
//	else
//		m_level_path.push_back(m_level_dest_vertex_id);
//	
//	if ((N > 1) && tpDestinationPosition && CDetailPathManager::m_path.size() && CDetailPathManager::m_path[CDetailPathManager::m_path.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L) {
//		if (ai().level_graph().inside(ai().level_graph().vertex(m_level_dest_vertex_id),*tpDestinationPosition) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(m_level_dest_vertex_id,T.P,*tpDestinationPosition))) {
//			T.P = *tpDestinationPosition;
//			CDetailPathManager::m_path.push_back(T);
//		}
//	}
//
//	CDetailPathManager::curr_travel_point_index() = 0;
//	m_tPathState		= PATH_STATE_SEARCH_NODE;
//
//	Device.Statistic.TEST1.End();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Выбор точки, построение пути, построение TravelLine
void CAI_Biting::vfChoosePointAndBuildPath(PathManagers::CAbstractVertexEvaluator * /**tpNodeEvaluator/**/, Fvector * /**tpDestinationPosition/**/, bool /**bSearchForNode/**/, bool /**bSelectorPath/**/, u32 /**TimeToRebuild/**/)
{
//	// если путь был построен - проверить нужно ли перестраивать путь
//	if (PATH_STATE_PATH_BUILT == m_tPathState) {
//		if (m_dwPathBuiltLastTime + TimeToRebuild > m_current_update) return;
//		m_tPathState = PATH_STATE_SEARCH_NODE;
//	}
//
//	INIT_SQUAD_AND_LEADER;
//
//	Fvector tempV;
//	switch (m_tPathState) {
//		case PATH_STATE_SEARCH_NODE: 
//			if (tpNodeEvaluator && bSearchForNode)		// необходимо искать ноду?
//			 	vfSearchForBetterPosition(*tpNodeEvaluator,Squad,Leader);
//			else if (!bSearchForNode || !tpDestinationPosition || !CDetailPathManager::m_path.size() || (CDetailPathManager::m_path[CDetailPathManager::m_path.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
//			 	m_tPathState = PATH_STATE_BUILD_NODE_PATH;	// 
//			break;
//			 						
//		case PATH_STATE_BUILD_NODE_PATH: 
//  			if ((m_level_dest_vertex_id != level_vertex_id()) && (m_level_path.empty() || (m_level_path[m_level_path.size() - 1] != m_level_dest_vertex_id) || CDetailPathManager::m_path.empty() || ((CDetailPathManager::m_path.size() - 1) <= CDetailPathManager::curr_travel_point_index())))
//				vfBuildPathToDestinationPoint(bSelectorPath ? tpNodeEvaluator : 0);
//			else if ((m_level_dest_vertex_id == level_vertex_id()) && tpDestinationPosition && (!CDetailPathManager::m_path.size() || (tpDestinationPosition->distance_to_xz(CDetailPathManager::m_path[CDetailPathManager::m_path.size() - 1].P) > EPS_L))) {
//				m_level_path.clear();
//				m_level_path.push_back(level_vertex_id());
//				m_tPathState = PATH_STATE_BUILD_TRAVEL_LINE;
//			} else if (bSearchForNode && tpNodeEvaluator)
//				m_tPathState = PATH_STATE_SEARCH_NODE;
//			else if (CDetailPathManager::m_path.size() && tpDestinationPosition && (CDetailPathManager::m_path[CDetailPathManager::m_path.size() - 1].P.distance_to(*tpDestinationPosition) > EPS_L))
//				m_tPathState = PATH_STATE_BUILD_TRAVEL_LINE;
//			break;
//									
//		case PATH_STATE_BUILD_TRAVEL_LINE: 
//			tempV = ((tpDestinationPosition) ?  *tpDestinationPosition : ai().level_graph().vertex_position(m_level_path.back()));
//			vfBuildTravelLine(&tempV);
//
//			m_tPathState = PATH_STATE_PATH_BUILT;
//			m_dwPathBuiltLastTime = m_current_update;
//			break;
//	}	
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Выбор следующей точки графа
void CAI_Biting::vfChooseNextGraphPoint()
{
//	_GRAPH_ID			tGraphID		= m_tNextGP;
//	CGameGraph::const_iterator	i,e;
//	ai().game_graph().begin		(tGraphID,i,e);
//	int					iPointCount		= (int)m_tpaTerrain.size();
//	int					iBranches		= 0;
//	for ( ; i != e; ++i)
//		for (int j=0; j<iPointCount; ++j)
//			if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_tCurGP))
//				++iBranches;
//	ai().game_graph().begin		(tGraphID,i,e);
//	if (!iBranches) {
//		for ( ; i != e; ++i) {
//			for (int j=0; j<iPointCount; ++j)
//				if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type())) {
//					m_tCurGP	= m_tNextGP;
//					m_tNextGP	= (*i).vertex_id();
//					m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
//					return;
//				}
//		}
//	}
//	else {
//		int iChosenBranch = ::Random.randI(0,iBranches);
//		iBranches = 0;
//		for ( ; i != e; ++i) {
//			for (int j=0; j<iPointCount; ++j)
//				if (ai().game_graph().mask(m_tpaTerrain[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_tCurGP)) {
//					if (iBranches == iChosenBranch) {
//						m_tCurGP			= m_tNextGP;
//						m_tNextGP			= (*i).vertex_id();
//						m_dwTimeToChange	= Level().timeServer() + ::Random.randI(m_tpaTerrain[j].dwMinTime,m_tpaTerrain[j].dwMaxTime);
//						return;
//					}
//					++iBranches;
//				}
//		}
//	}
}

// Выбор точки графа, в соответствии с выбором лидера
void CAI_Biting::vfUpdateDetourPoint()
{
//	if (!g_Alive())
//		return;
//
//	INIT_SQUAD_AND_LEADER;
//
//	if (this != Leader)	{
//		CAI_Biting *tpLeader			= dynamic_cast<CAI_Biting*>(Leader);
//		if (tpLeader) {
//			m_tNextGraphPoint			= tpLeader->m_tNextGraphPoint;
//			m_tNextGP					= tpLeader->m_tNextGP;
//			return;
//		}
//	}
//	
//	if ((Level().timeServer() >= m_dwTimeToChange) && (ai().cross_table().vertex(level_vertex_id()).game_vertex_id() == m_tNextGP)) {
//		m_tNextGP					= ai().cross_table().vertex(level_vertex_id()).game_vertex_id();
//		vfChooseNextGraphPoint		();
//		m_tNextGraphPoint.set		(ai().game_graph().vertex(m_tNextGP)->level_point());
//		m_dwTimeToChange			= Level().timeServer() + 2000;
//	}
}

// high level 
void CAI_Biting::Path_GetAwayFromPoint(CEntity *pE, Fvector position, float dist, TTime rebuild_time)
{
	if (pE) {
		m_tEnemy.Set(pE,0);									// forse enemy selection
		vfInitSelector(*m_tSelectorGetAway, false);
	} else {
		vfInitSelector(*m_tSelectorGetAway, true);
		m_tSelectorGetAway->m_tEnemyPosition = position;
	}
	
	float dist_to_point = position.distance_to(Position());

	m_tSelectorGetAway->m_fMinEnemyDistance = dist_to_point + 3.f;
	m_tSelectorGetAway->m_fMaxEnemyDistance = m_tSelectorGetAway->m_fMinEnemyDistance + m_tSelectorGetAway->m_fSearchRange + dist;
	m_tSelectorGetAway->m_fOptEnemyDistance = m_tSelectorGetAway->m_fMaxEnemyDistance;

	vfChoosePointAndBuildPath(m_tSelectorGetAway, 0, true, 0, rebuild_time);
}

void CAI_Biting::Path_CoverFromPoint(CEntity *pE, Fvector position, TTime rebuild_time)
{
	if (pE) {
		m_tEnemy.Set(pE,0); 									// forse enemy selection
		vfInitSelector(*m_tSelectorCover, false);
	} else {
		vfInitSelector(*m_tSelectorCover, true);
		m_tSelectorCover->m_tEnemyPosition = position;
	}

	float dist_to_point = position.distance_to(Position());

	m_tSelectorCover->m_fMinEnemyDistance = dist_to_point + 3.f;
	m_tSelectorCover->m_fMaxEnemyDistance = dist_to_point + m_tSelectorCover->m_fSearchRange;
	m_tSelectorCover->m_fOptEnemyDistance = m_tSelectorCover->m_fMaxEnemyDistance;

	vfChoosePointAndBuildPath(m_tSelectorCover, 0, true, 0, rebuild_time);
}

void CAI_Biting::Path_ApproachPoint(CEntity *pE, Fvector position, TTime rebuild_time)
{
	if (pE) {
		m_tEnemy.Set(pE,0); 									// forse enemy selection
		vfInitSelector(*m_tSelectorApproach, false);
	} else {
		vfInitSelector(*m_tSelectorApproach, true);
		m_tSelectorApproach->m_tEnemyPosition = position;
	}

	vfChoosePointAndBuildPath(m_tSelectorApproach,0, true, 0, rebuild_time);
}

// Развернуть объект в направление движения
void CAI_Biting::SetDirectionLook(bool bReversed)
{
//	int i = ps_Size();		// position stack size
//	
//	if (i<2) return;
//
//	CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
//
//	Fvector dir;
//	dir.sub(tCurrentPosition.vPosition,tPreviousPosition.vPosition);
//	
//	if (dir.magnitude() < EPS_L) return;
//	
//	vfNormalizeSafe(dir);
//	
//	float yaw,pitch;
//	dir.getHP(yaw,pitch);
//
//	m_body.target.yaw = -yaw;
//	m_body.target.pitch = 0;
//
//	if (bReversed) m_body.target.yaw = angle_normalize(m_body.target.yaw + PI);
//	else m_body.target.yaw = angle_normalize(m_body.target.yaw);
//	
//	Msg("TARGET YAW CHANGED:: SetDirLook / target_yaw = [%f]", m_body.target.yaw);
//	
//
//	m_head.target = m_body.target;
	
	if (CDetailPathManager::path().empty() || (CDetailPathManager::curr_travel_point_index() >= CDetailPathManager::path().size() - 1)) return;
	
	Fvector vstart,vnext;
	vstart	= CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index()].m_position;
	vnext	= CDetailPathManager::path()[CDetailPathManager::curr_travel_point_index() + 1].m_position;
	
	Fvector dir;
	dir.sub(vnext,vstart);
	
	if (dir.magnitude() < EPS_L) return;
	
	vfNormalizeSafe(dir);
	
	float yaw,pitch;
	dir.getHP(yaw,pitch);

	m_body.target.yaw = -yaw;
	m_body.target.pitch = 0;

	if (bReversed) m_body.target.yaw = angle_normalize(m_body.target.yaw + PI);
	else m_body.target.yaw = angle_normalize(m_body.target.yaw);
	
	m_head.target = m_body.target;
}

// каждый монстр может по-разному реализвать эту функ (e.g. кровосос с поворотом головы и т.п.)
void CAI_Biting::LookPosition(Fvector to_point)
{
	// по-умолчанию просто изменить m_body.target.yaw
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());	
	
	// получаем вектор направления к источнику звука и его мировые углы
	float		yaw,p;
	dir.getHP(yaw,p);

	// установить текущий угол
	m_body.target.yaw = angle_normalize(-yaw);
}

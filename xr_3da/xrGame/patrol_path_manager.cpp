////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path_manager.h"
#include "script_game_object.h"

void CPatrolPathManager::select_point(const Fvector &position, u32 &dest_vertex_id)
{
	VERIFY						(m_path && !m_path->vertices().empty());
	const CPatrolPath::CVertex	*vertex = 0;
	if (!actual() || !m_path->vertex(m_curr_point_index)) {
		switch (m_start_type) {
			case ePatrolStartTypeFirst : {
				vertex		= m_path->vertex(0);
				break;
			}
			case ePatrolStartTypeLast : {
				vertex		= m_path->vertex(m_path->vertices().size() - 1);
				break;
			}
			case ePatrolStartTypeNearest : {
				vertex		= m_path->point(position);
				break;
			}
			case ePatrolStartTypePoint : {
				VERIFY		(m_path->vertex(m_start_point_index));
				vertex		= m_path->vertex(m_start_point_index);
				break;
			}
			case ePatrolStartTypeNext : {
				if (m_prev_point_index != u32(-1))
					vertex		= m_path->vertex(m_prev_point_index+1);
				if (!vertex)
					vertex		= m_path->point(position);

				break;
			}
			default			: NODEFAULT;
		}
		VERIFY				(vertex);

		if (!m_path->vertex(m_prev_point_index))
			m_prev_point_index	= vertex->vertex_id();

		m_curr_point_index	= vertex->vertex_id();
		dest_vertex_id		= vertex->data().level_vertex_id();
		m_dest_position		= vertex->data().position();
		m_actuality			= true;
		m_completed			= false;
		return;
	}
	VERIFY					(m_path->vertex(m_curr_point_index));

	if (m_callback)
		SCRIPT_CALLBACK_EXECUTE_3((*m_callback), dynamic_cast<CGameObject*>(this)->lua_game_object(),u32(CScriptMonster::eActionTypeMovement),m_curr_point_index);

	u32							count = 0;
	float						sum = 0.f;
	vertex						= m_path->vertex(m_curr_point_index);
	CPatrolPath::const_iterator	I = vertex->edges().begin(), E = vertex->edges().end();
	u32							temp = u32(-1);
	for ( ; I != E; ++I) {
		if ((*I).vertex_id() == m_curr_point_index)
			continue;

		if (!count)
			temp				= (*I).vertex_id();
		
		sum						+= (*I).weight();
		++count;
	}

	if (!count) {
		switch (m_route_type) {
			case ePatrolRouteTypeStop : {
				m_completed	= true;
				return;
			}
			case ePatrolRouteTypeContinue : {
				if (vertex->edges().empty()) {
					m_completed	= true;
					return;
				}
				temp			= vertex->edges().begin()->vertex_id();
				break;
			}
			default : NODEFAULT;
		}
	}
	else {
		float			fChoosed = 0.f;
		if (random() && (count > 1))
			fChoosed	= ::Random.randF(sum);
		sum				= 0.f;
		I				= vertex->edges().begin();
		for ( ; I != E; ++I) {
			if ((*I).vertex_id() == m_prev_point_index)
				continue;

			sum			+= (*I).weight();

			if (sum >= fChoosed) {
				temp	= (*I).vertex_id();
				break;
			}
		}
		VERIFY			(m_path->vertex(temp));
	}
	
	m_prev_point_index	= m_curr_point_index;
	m_curr_point_index	= temp;
	dest_vertex_id		= m_path->vertex(m_curr_point_index)->data().level_vertex_id();
	m_dest_position		= m_path->vertex(m_curr_point_index)->data().position();
}
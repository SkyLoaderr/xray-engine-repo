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
#include "script_space.h"
#include "restricted_object.h"

void CPatrolPathManager::reinit				()
{
	CAI_ObjectLocation::reinit();
	m_path					= 0;
	m_start_type			= ePatrolStartTypeDummy;
	m_route_type			= ePatrolRouteTypeDummy;
	m_actuality				= true;
	m_failed				= false;
	m_completed				= true;
	m_curr_point_index		= u32(-1);
	m_prev_point_index		= u32(-1);
	m_start_point_index		= u32(-1);
	m_callback				= 0;
	m_restricted_object		= dynamic_cast<CRestrictedObject*>(this);
	VERIFY					(m_restricted_object);
}

IC	bool CPatrolPathManager::accessible	(const Fvector &position) const
{
	return		(m_restricted_object ? m_restricted_object->accessible(position) : true);
}

IC	bool CPatrolPathManager::accessible	(u32 vertex_id) const
{
	return		(m_restricted_object ? m_restricted_object->accessible(vertex_id) : true);
}

IC	bool CPatrolPathManager::accessible	(const CPatrolPath::CVertex *vertex) const
{
	return		(vertex ? accessible(vertex->data().position()) : true);
}

struct CAccessabilityEvaluator {
	const CPatrolPathManager *m_manager;

	IC	CAccessabilityEvaluator(const CPatrolPathManager *manager)
	{
		m_manager	= manager;
	}

	IC	bool operator()	(const Fvector &position) const
	{
		return		(m_manager->accessible(position));
	}
};

void CPatrolPathManager::select_point(const Fvector &position, u32 &dest_vertex_id)
{
	VERIFY						(m_path && !m_path->vertices().empty());
	const CPatrolPath::CVertex	*vertex = 0;
	if (!actual() || !m_path->vertex(m_curr_point_index)) {
		switch (m_start_type) {
			case ePatrolStartTypeFirst : {
				vertex		= m_path->vertex(0);
				VERIFY		(accessible(vertex));
				break;
			}
			case ePatrolStartTypeLast : {
				vertex		= m_path->vertex(m_path->vertices().size() - 1);
				VERIFY		(accessible(vertex));
				break;
			}
			case ePatrolStartTypeNearest : {
				vertex		= m_path->point(position,CAccessabilityEvaluator(this));
				VERIFY		(accessible(vertex));
				break;
			}
			case ePatrolStartTypePoint : {
				VERIFY		(m_path->vertex(m_start_point_index));
				vertex		= m_path->vertex(m_start_point_index);
				VERIFY		(accessible(vertex));
				break;
			}
			case ePatrolStartTypeNext : {
				if (m_prev_point_index != u32(-1))
					vertex		= m_path->vertex(m_prev_point_index+1);
				if (!vertex)
					vertex		= m_path->point(position);
				
				VERIFY		(accessible(vertex));
				break;
			}
			default			: NODEFAULT;
		}
		VERIFY				(vertex);

		if (!m_path->vertex(m_prev_point_index))
			m_prev_point_index	= vertex->vertex_id();

		m_curr_point_index	= vertex->vertex_id();
		
		// если выбранная нода не соответствует текущей ноде - все ок
		// иначе выбрать следующую вершину патрульного пути
		if (vertex->data().level_vertex_id() != level_vertex_id()) {
			dest_vertex_id		= vertex->data().level_vertex_id();
			m_dest_position		= vertex->data().position();
			VERIFY				(accessible(m_dest_position));
			m_actuality			= true;
			m_completed			= false;
			return;
		} 
	}
	VERIFY					(m_path->vertex(m_curr_point_index));

	if (m_callback)
		SCRIPT_CALLBACK_EXECUTE_3((*m_callback), m_restricted_object->lua_game_object(),u32(ScriptMonster::eActionTypeMovement),m_curr_point_index);

	u32							count = 0;
	float						sum = 0.f;
	vertex						= m_path->vertex(m_curr_point_index);
	CPatrolPath::const_iterator	I = vertex->edges().begin(), E = vertex->edges().end();
	u32							temp = u32(-1);
	for ( ; I != E; ++I) {
		if ((*I).vertex_id() == m_curr_point_index)
			continue;

		if (!accessible(m_path->vertex((*I).vertex_id())))
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
				m_completed	= true;
				return;
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

			if (!accessible(m_path->vertex((*I).vertex_id())))
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
	VERIFY				(accessible(m_dest_position));
	m_actuality			= true;
	m_completed			= false;
}

ref_str	CPatrolPathManager::path_name	() const
{
	if (!m_path) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Path not specified (object %s)!",*m_restricted_object->cName());
		return				("");
	}
	VERIFY					(m_path);
	return					(m_path_name);
}

void CPatrolPathManager::set_previous_point	(int point_index)
{
	if (!m_path) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Path not specified (object %s)!",*m_restricted_object->cName());
		return;
	}
	
	if (!m_path->vertex(point_index)) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Start point violates path bounds %s (object %s)!",*m_path_name,*m_restricted_object->cName());
		return;
	}
	VERIFY					(m_path);
	VERIFY					(m_path->vertex(point_index));
	m_prev_point_index		= point_index;
}

void CPatrolPathManager::set_start_point	(int point_index)
{
	if (!m_path) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Path not specified (object %s)!",*m_restricted_object->cName());
		return;
	}
	if (!m_path->vertex(point_index)) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Start point violates path bounds %s (object %s)!",*m_path_name,*m_restricted_object->cName());
		return;
	}
	VERIFY					(m_path);
	VERIFY					(m_path->vertex(point_index));
	m_start_point_index		= point_index;
}
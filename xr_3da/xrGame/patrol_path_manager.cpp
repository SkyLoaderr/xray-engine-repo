////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameobject.h"
#include "patrol_path_manager.h"
#include "script_game_object.h"
#include "script_space.h"
#include "restricted_object.h"
#include "ai_space.h"
#include "script_engine.h"
#include "ai_object_location.h"
#include "script_entity_space.h"

#ifdef DEBUG
#	include "space_restriction_manager.h"

bool show_restrictions(CRestrictedObject *object)
{
	Msg		("DEFAULT OUT RESTRICTIONS : %s",*Level().space_restriction_manager().default_out_restrictions() ? *Level().space_restriction_manager().default_out_restrictions() : "");
	Msg		("DEFAULT IN RESTRICTIONS  : %s",*Level().space_restriction_manager().default_in_restrictions()  ? *Level().space_restriction_manager().default_in_restrictions()  : "");
	Msg		("OUT RESTRICTIONS         : %s",*object->out_restrictions() ? *object->out_restrictions() : "");
	Msg		("IN RESTRICTIONS          : %s",*object->in_restrictions()  ? *object->in_restrictions()  : "");
	return	(false);
}
#endif

void CPatrolPathManager::reinit				(CRestrictedObject *object)
{
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
	m_object				= object;
	VERIFY					(m_object);
}

IC	bool CPatrolPathManager::accessible	(const Fvector &position) const
{
	return		(m_object ? object().accessible(position) : true);
}

IC	bool CPatrolPathManager::accessible	(u32 vertex_id) const
{
	return		(m_object ? object().accessible(vertex_id) : true);
}

IC	bool CPatrolPathManager::accessible	(const CPatrolPath::CVertex *vertex) const
{
	return		(vertex ? object().accessible(vertex->data().position()) : true);
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
				VERIFY3		(accessible(vertex) || show_restrictions(m_object),*m_path_name,*m_game_object->cName());
				break;
			}
			case ePatrolStartTypeLast : {
				vertex		= m_path->vertex(m_path->vertices().size() - 1);
				VERIFY3		(accessible(vertex) || show_restrictions(m_object),*m_path_name,*m_game_object->cName());
				break;
			}
			case ePatrolStartTypeNearest : {
				vertex		= m_path->point(position,CAccessabilityEvaluator(this));
				VERIFY3		(accessible(vertex) || show_restrictions(m_object),*m_path_name,*m_game_object->cName());
				break;
			}
			case ePatrolStartTypePoint : {
				VERIFY3		(m_path->vertex(m_start_point_index),*m_path_name,*m_game_object->cName());
				vertex		= m_path->vertex(m_start_point_index);
				VERIFY3		(accessible(vertex) || show_restrictions(m_object),*m_path_name,*m_game_object->cName());
				break;
			}
			case ePatrolStartTypeNext : {
				if (m_prev_point_index != u32(-1))
					vertex		= m_path->vertex(m_prev_point_index+1);
				if (!vertex)
					vertex		= m_path->point(position);

				VERIFY3		(accessible(vertex) || show_restrictions(m_object),*m_path_name,*m_game_object->cName());
				break;
			}
			default			: NODEFAULT;
		}
		VERIFY3				(vertex || show_restrictions(m_object),*m_path_name,*m_game_object->cName());

		if (!m_path->vertex(m_prev_point_index))
			m_prev_point_index	= vertex->vertex_id();

		m_curr_point_index	= vertex->vertex_id();

		// ���� ��������� ���� �� ������������� ������� ���� - ��� ��
		// ����� ������� ��������� ������� ����������� ����
		if (vertex->data().level_vertex_id() != m_object->object().ai_location().level_vertex_id()) {
			dest_vertex_id		= vertex->data().level_vertex_id();
			m_dest_position		= vertex->data().position();
			VERIFY				(accessible(m_dest_position) || show_restrictions(m_object));
			m_actuality			= true;
			m_completed			= false;
			return;
		} 
	}
	VERIFY3					(m_path->vertex(m_curr_point_index) || show_restrictions(m_object),*m_path_name,*m_game_object->cName());

	if (m_callback)
		SCRIPT_CALLBACK_EXECUTE_3((*m_callback), m_object->object().lua_game_object(),u32(ScriptEntity::eActionTypeMovement),m_curr_point_index);

	u32							count = 0;		// ���������� ������������
	float						sum = 0.f;		// ����� ����� ������������
	vertex						= m_path->vertex(m_curr_point_index);
	CPatrolPath::const_iterator	I = vertex->edges().begin(), E = vertex->edges().end();
	u32							target = u32(-1);
	
	// ��������� ���������� ������������
	for ( ; I != E; ++I) {
		if ((*I).vertex_id() == m_prev_point_index)
			continue;

		if (!accessible(m_path->vertex((*I).vertex_id())))
			continue;

		if (count == 0)
			target				= (*I).vertex_id();
		
		sum						+= (*I).weight();
		++count;
	}

	if (count == 0) {
		switch (m_route_type) {
			case ePatrolRouteTypeStop : {
				m_completed	= true;
				return;
			}
			case ePatrolRouteTypeContinue : {
				for (I = vertex->edges().begin() ; I != E; ++I) {
					if (!accessible(m_path->vertex((*I).vertex_id())))
						continue;

					target			= (*I).vertex_id();
					break;
				}
				if (target != u32(-1))	break;

				m_completed				= true;
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
				target	= (*I).vertex_id();
				break;
			}
		}
	}

	VERIFY3				(m_path->vertex(target) || show_restrictions(m_object),*m_path_name,*m_game_object->cName());

	m_prev_point_index	= m_curr_point_index;
	m_curr_point_index	= target;
	dest_vertex_id		= m_path->vertex(m_curr_point_index)->data().level_vertex_id();
	m_dest_position		= m_path->vertex(m_curr_point_index)->data().position();
	VERIFY3				(accessible(m_dest_position) || show_restrictions(m_object),*m_path_name,*m_game_object->cName());
	m_actuality			= true;
	m_completed			= false;
}

shared_str	CPatrolPathManager::path_name	() const
{
	if (!m_path) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Path not specified (object %s)!",*m_object->object().cName());
		return				("");
	}
	VERIFY					(m_path);
	return					(m_path_name);
}

void CPatrolPathManager::set_previous_point	(int point_index)
{
	if (!m_path) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Path not specified (object %s)!",*m_object->object().cName());
		return;
	}
	
	if (!m_path->vertex(point_index)) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Start point violates path bounds %s (object %s)!",*m_path_name,*m_object->object().cName());
		return;
	}
	VERIFY					(m_path);
	VERIFY					(m_path->vertex(point_index));
	m_prev_point_index		= point_index;
}

void CPatrolPathManager::set_start_point	(int point_index)
{
	if (!m_path) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Path not specified (object %s)!",*m_object->object().cName());
		return;
	}
	if (!m_path->vertex(point_index)) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Start point violates path bounds %s (object %s)!",*m_path_name,*m_object->object().cName());
		return;
	}
	VERIFY					(m_path);
	VERIFY					(m_path->vertex(point_index));
	m_start_point_index		= point_index;
}

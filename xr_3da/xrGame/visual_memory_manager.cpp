////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "visual_memory_manager.h"
#include "custommonster.h"
#include "ai/stalker/ai_stalker.h"

struct CNotYetVisibleObjectPredicate{
	const CGameObject *m_game_object;

	IC				CNotYetVisibleObjectPredicate(const CGameObject *game_object)
	{
		m_game_object	= game_object;
	}

	IC		bool	operator()	(const CNotYetVisibleObject &object) const
	{
		return		(object.m_object->ID() == m_game_object->ID());
	}
};

CVisualMemoryManager::CVisualMemoryManager		()
{
	init				();
}

CVisualMemoryManager::~CVisualMemoryManager		()
{
}

void CVisualMemoryManager::init					()
{
	m_max_object_count			= 0;
	m_enabled					= true;
}

void CVisualMemoryManager::Load					(LPCSTR section)
{
	CGameObject::Load			(section);
	
	m_max_object_count			= pSettings->r_s32(section,"DynamicObjectsCount");
	m_transparency_threshold	= pSettings->r_float(section,"transparency_threshold");
	m_monster					= smart_cast<CCustomMonster*>(this);
	m_stalker					= smart_cast<CAI_Stalker*>(this);

	if (!m_stalker)
		return;
	
	m_min_view_distance_danger			= pSettings->r_float(section,"min_view_distance_danger");
	m_max_view_distance_danger			= pSettings->r_float(section,"max_view_distance_danger");
	m_min_view_distance_free			= pSettings->r_float(section,"min_view_distance_free");
	m_max_view_distance_free			= pSettings->r_float(section,"max_view_distance_free");
	m_visibility_threshold				= pSettings->r_float(section,"visibility_threshold");
	m_always_visible_distance_danger	= pSettings->r_float(section,"always_visible_distance_danger");
	m_always_visible_distance_free		= pSettings->r_float(section,"always_visible_distance_free");
	m_time_quant						= pSettings->r_float(section,"time_quant");
	m_decrease_value					= pSettings->r_float(section,"decrease_value");
	m_velocity_factor					= pSettings->r_float(section,"velocity_factor");
}

void CVisualMemoryManager::reinit					()
{
	CGameObject::reinit					();
	
	m_objects							= 0;
	
	m_visible_objects.clear				();
	m_visible_objects.reserve			(100);
	
	m_not_yet_visible_objects.clear		();
	m_not_yet_visible_objects.reserve	(100);

	feel_vision_clear					();
}

void CVisualMemoryManager::reload				(LPCSTR section)
{
}

float CVisualMemoryManager::object_visible_distance(const CGameObject *game_object, float &object_distance) const
{
	Fvector								eye_position = Fvector().set(0.f,0.f,0.f), temp, eye_direction;
	Fmatrix								&eye_matrix = PKinematics(m_monster->Visual())->LL_GetTransform(u16(m_stalker->eye_bone));

	eye_matrix.transform_tiny			(temp,eye_position);
	XFORM().transform_tiny				(eye_position,temp);
	eye_direction.setHP					(-m_stalker->m_head.current.yaw, -m_stalker->m_head.current.pitch);
	
	Fvector								object_direction;
	game_object->Center					(object_direction);
	object_distance						= object_direction.distance_to(eye_position);
	object_direction.sub				(eye_position);
	object_direction.normalize_safe		();
	float								fov = deg2rad(m_stalker->eye_fov)*.5f;
	float								cos_alpha = eye_direction.dotproduct(object_direction);
	clamp								(cos_alpha,-.99999f,.99999f);
	float								alpha = acosf(cos_alpha);
	clamp								(alpha,0.f,fov);

	float								max_view_distance = m_stalker->eye_range, min_view_distance = m_stalker->eye_range;
	if (m_stalker->mental_state() == eMentalStateDanger) {
		max_view_distance				*= m_max_view_distance_danger;
		min_view_distance				*= m_min_view_distance_danger;
	}
	else {
		max_view_distance				*= m_max_view_distance_free;
		min_view_distance				*= m_min_view_distance_free;
	}

	float								distance = (1.f - alpha/fov)*(max_view_distance - min_view_distance) + min_view_distance;

	return								(distance);
}

float CVisualMemoryManager::get_object_velocity	(const CGameObject *game_object, const CNotYetVisibleObject &not_yet_visible_object) const
{
	if ((game_object->ps_Size() < 2) || (not_yet_visible_object.m_prev_time == game_object->ps_Element(game_object->ps_Size() - 2).dwTime))
		return							(0.f);

	CObject::SavedPosition	pos0 = game_object->ps_Element	(game_object->ps_Size() - 2);
	CObject::SavedPosition	pos1 = game_object->ps_Element	(game_object->ps_Size() - 1);
	
	return					(
		pos1.vPosition.distance_to(pos0.vPosition)/
		(
			float(pos1.dwTime)/1000.f - 
			float(pos0.dwTime)/1000.f
		)
	);
}

float CVisualMemoryManager::get_visible_value	(float distance, float object_distance, float time_delta, float object_velocity) const
{
	float								always_visible_distance = m_always_visible_distance_free;
	if (m_stalker->mental_state() == eMentalStateDanger)
		always_visible_distance			= m_always_visible_distance_danger;

	if (distance <= always_visible_distance + EPS_L)
		return							(m_visibility_threshold);

	return								(
		time_delta / 
		m_time_quant * 
		(1.f + m_velocity_factor*object_velocity) *
		(distance - object_distance) /
		(distance - always_visible_distance)
	);
}

CNotYetVisibleObject *CVisualMemoryManager::not_yet_visible_object(const CGameObject *game_object)
{
	xr_vector<CNotYetVisibleObject>::iterator	I = std::find_if(
		m_not_yet_visible_objects.begin(),
		m_not_yet_visible_objects.end(),
		CNotYetVisibleObjectPredicate(game_object)
	);
	if (I == m_not_yet_visible_objects.end())
		return							(0);
	return								(&*I);
}

void CVisualMemoryManager::add_not_yet_visible_object	(const CNotYetVisibleObject &not_yet_visible_object)
{
	m_not_yet_visible_objects.push_back	(not_yet_visible_object);
}

u32	 CVisualMemoryManager::get_prev_time				(const CGameObject *game_object) const
{
	if (!game_object->ps_Size())
		return			(0);
	if (game_object->ps_Size() == 1)
		return			(game_object->ps_Element(0).dwTime);
	return				(game_object->ps_Element(game_object->ps_Size() - 2).dwTime);
}

bool CVisualMemoryManager::visible				(const CGameObject *game_object, float time_delta)
{
	VERIFY						(game_object);
	
	if (game_object->getDestroy())
		return					(false);

	if (!m_stalker)
		return					(true);

	float						object_distance, distance = object_visible_distance(game_object,object_distance);

	CNotYetVisibleObject		*object = not_yet_visible_object(game_object);
	
	if (distance < object_distance) {
		if (object) {
			object->m_value		-= m_decrease_value;
			if (object->m_value < 0.f)
				object->m_value	= 0.f;
			else
				object->m_updated = true;
			return				(object->m_value >= m_visibility_threshold);
		}
		return					(false);
	}

	if (!object) {
		CNotYetVisibleObject	new_object;
		new_object.m_object		= game_object;
		new_object.m_prev_time	= 0;
		new_object.m_value		= get_visible_value(distance,object_distance,time_delta,get_object_velocity(game_object,new_object));
		clamp					(new_object.m_value,0.f,m_visibility_threshold + EPS_L);
		new_object.m_updated	= true;
		new_object.m_prev_time	= get_prev_time(game_object);
		add_not_yet_visible_object(new_object);
		return					(new_object.m_value >= m_visibility_threshold);
	}
	
	object->m_updated			= true;
	object->m_value				+= get_visible_value(distance,object_distance,time_delta,get_object_velocity(game_object,*object));
	clamp						(object->m_value,0.f,m_visibility_threshold + EPS_L);
	object->m_prev_time			= get_prev_time(game_object);

	return						(object->m_value >= m_visibility_threshold);
}

void CVisualMemoryManager::add_visible_object	(const CObject *object, float time_delta)
{
	const CGameObject *game_object	= smart_cast<const CGameObject*>(object);
	const CGameObject *self			= this;
	if (!game_object || !visible(game_object,time_delta))
		return;

	xr_vector<CVisibleObject>::iterator	J = std::find(m_objects->begin(),m_objects->end(),object_id(game_object));
	if (m_objects->end() == J) {
		CVisibleObject			visible_object;

		visible_object.fill		(game_object,self);
		visible_object.m_first_level_time	= Level().timeServer();
		visible_object.m_first_game_time	= Level().GetGameTime();

		if (m_max_object_count <= m_objects->size()) {
			xr_vector<CVisibleObject>::iterator	I = std::min_element(m_objects->begin(),m_objects->end(),SLevelTimePredicate<CGameObject>());
			VERIFY				(m_objects->end() != I);
			*I					= visible_object;
		}
		else
			m_objects->push_back	(visible_object);
	}
	else
		(*J).fill				(game_object,self);
}

void CVisualMemoryManager::add_visible_object	(const CVisibleObject visible_object)
{
	xr_vector<CVisibleObject>::iterator			J = std::find(m_objects->begin(),m_objects->end(),object_id(visible_object.m_object));
	if (m_objects->end() != J)
		*J				= visible_object;
	else
		if (m_max_object_count <= m_objects->size()) {
			xr_vector<CVisibleObject>::iterator	I = std::min_element(m_objects->begin(),m_objects->end(),SLevelTimePredicate<CGameObject>());
			VERIFY								(m_objects->end() != I);
			*I									= visible_object;
		}
		else
			m_objects->push_back(visible_object);
}
	
void CVisualMemoryManager::update				(float time_delta)
{
	if (!enabled())
		return;

	VERIFY								(m_objects);
	m_visible_objects.clear				();
	feel_vision_get						(m_visible_objects);

	{
		xr_vector<CVisibleObject>::iterator	I = m_objects->begin();
		xr_vector<CVisibleObject>::iterator	E = m_objects->end();
		for ( ; I != E; ++I)
			(*I).m_visible				= false;
	}

	{
		xr_vector<CNotYetVisibleObject>::iterator	I = m_not_yet_visible_objects.begin();
		xr_vector<CNotYetVisibleObject>::iterator	E = m_not_yet_visible_objects.end();
		for ( ; I != E; ++I)
			(*I).m_updated				= false;
	}

	{
		xr_vector<CObject*>::const_iterator	I = m_visible_objects.begin();
		xr_vector<CObject*>::const_iterator	E = m_visible_objects.end();
		for ( ; I != E; ++I)
			add_visible_object			(*I,time_delta);
	}
	
	{
		xr_vector<CNotYetVisibleObject>::iterator	I = m_not_yet_visible_objects.begin();
		xr_vector<CNotYetVisibleObject>::iterator	E = m_not_yet_visible_objects.end();
		for ( ; I != E; ++I)
			if (!(*I).m_updated)
				(*I).m_value			= 0.f;
	}

	// verifying if object is online
	{
		xr_vector<CVisibleObject>::iterator	J = remove_if(m_objects->begin(),m_objects->end(),SRemoveOfflinePredicate());
		m_objects->erase					(J,m_objects->end());
	}

	// verifying if object is online
	{
		xr_vector<CNotYetVisibleObject>::iterator	J = remove_if(m_not_yet_visible_objects.begin(),m_not_yet_visible_objects.end(),SRemoveOfflinePredicate());
		m_not_yet_visible_objects.erase				(J,m_not_yet_visible_objects.end());
	}
}

bool CVisualMemoryManager::visible(u32 _level_vertex_id, float yaw, float eye_fov) const
{
	Fvector					direction;
	direction.sub			(ai().level_graph().vertex_position(_level_vertex_id),Position());
	direction.normalize_safe();
	float					y, p;
	direction.getHP			(y,p);
	if (angle_difference(yaw,y) <= eye_fov*PI/180.f/2.f)
		return(ai().level_graph().check_vertex_in_direction(level_vertex_id(),Position(),_level_vertex_id));
	else
		return(false);
}

float CVisualMemoryManager::feel_vision_mtl_transp(u32 element)
{
	CDB::TRI* T			= Level().ObjectSpace.GetStaticTris()+element;
	return GMLib.GetMaterialByIdx(T->material)->fVisTransparencyFactor;
}

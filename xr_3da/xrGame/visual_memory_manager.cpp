////////////////////////////////////////////////////////////////////////////
//	Module 		: visual_memory_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Visual memory manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "visual_memory_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "memory_space_impl.h"
#include "../skeletoncustom.h"
#include "clsid_game.h"
#include "ai_object_location.h"
#include "level_graph.h"
#include "stalker_movement_manager.h"
#include "gamemtllib.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "ai_space.h"
#include "profiler.h"
#include "actor.h"
#include "../camerabase.h"
#include "gamepersistent.h"
#include "actor_memory.h"

#ifdef DEBUG
#	include "memory_manager.h"
#endif

struct SRemoveOfflinePredicate {
	bool		operator()						(const CVisibleObject &object) const
	{
		VERIFY	(object.m_object);
		return	(!!object.m_object->getDestroy() || object.m_object->H_Parent());
	}
	
	bool		operator()						(const CNotYetVisibleObject &object) const
	{
		VERIFY	(object.m_object);
		return	(!!object.m_object->getDestroy() || object.m_object->H_Parent());
	}
};

struct CVisibleObjectPredicate {
	u32			m_id;
				CVisibleObjectPredicate			(u32 id) : 
					m_id(id)
	{
	}

	bool		operator()						(const CObject *object) const
	{
		VERIFY	(object);
		return	(object->ID() == m_id);
	}
};

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

CVisualMemoryManager::CVisualMemoryManager		(CCustomMonster *object, CAI_Stalker *stalker, CActor *actor)
{
	VERIFY				(!!object || !!actor);
	VERIFY				(!object || !actor);
	m_object			= object;
	m_stalker			= stalker;
	m_actor				= actor;
	m_max_object_count	= 1;
	m_enabled			= true;
	if (m_actor)
		m_objects		= xr_new<VISIBLES>();
}

CVisualMemoryManager::~CVisualMemoryManager		()
{
	if (m_actor)
		xr_delete		(m_objects);
}

void CVisualMemoryManager::Load					(LPCSTR section)
{
}

void CVisualMemoryManager::reinit					()
{
	if (!m_actor)
		m_objects						= 0;
	else {
		VERIFY							(m_objects);
		m_objects->clear				();
	}

	m_visible_objects.clear				();
//	m_visible_objects.reserve			(100);

	m_not_yet_visible_objects.clear		();
//	m_not_yet_visible_objects.reserve	(100);

	if (m_object)
		m_object->feel_vision_clear		();

	m_last_update_time					= u32(-1);
}

void CVisualMemoryManager::reload				(LPCSTR section)
{
	m_max_object_count			= READ_IF_EXISTS(pSettings,r_s32,section,"DynamicObjectsCount",1);

	if (m_stalker) {
		m_free.Load		(pSettings->r_string(section,"vision_free_section"),true);
		m_danger.Load	(pSettings->r_string(section,"vision_danger_section"),true);
	}
	else
		m_free.Load		(section,!!m_actor);
}

IC	const CVisionParameters &CVisualMemoryManager::current_state() const
{
	return				(!m_stalker || (m_stalker->movement().mental_state() != eMentalStateDanger) ? m_free : m_danger);
}

u32	CVisualMemoryManager::visible_object_time_last_seen	(const CObject *object) const
{
	VISIBLES::iterator	I = std::find(m_objects->begin(),m_objects->end(),object_id(object));
	if (I != m_objects->end()) 
		return (I->m_level_time);	
	else 
		return u32(-1);
}

bool CVisualMemoryManager::visible_right_now	(const CGameObject *game_object) const
{
	VISIBLES::const_iterator		I = std::find(objects().begin(),objects().end(),object_id(game_object));
	if ((objects().end() == I))
		return						(false);

	if (!(*I).visible(mask()))
		return						(false);

	if ((*I).m_level_time < m_last_update_time)
		return						(false);

	return							(true);
}

bool CVisualMemoryManager::visible_now	(const CGameObject *game_object) const
{
	VISIBLES::const_iterator		I = std::find(objects().begin(),objects().end(),object_id(game_object));
	return							((objects().end() != I) && (*I).visible(mask()));
}

void CVisualMemoryManager::enable		(const CObject *object, bool enable)
{
	VISIBLES::iterator	J = std::find(m_objects->begin(),m_objects->end(),object_id(object));
	if (J == m_objects->end())
		return;
	(*J).m_enabled					= enable;
}

float CVisualMemoryManager::object_visible_distance(const CGameObject *game_object, float &object_distance) const
{
	Fvector								eye_position = Fvector().set(0.f,0.f,0.f), eye_direction;
	Fmatrix								eye_matrix;

	if (m_object) {
		eye_matrix						= 
			smart_cast<CKinematics*>(
				m_object->Visual()
			)
			->LL_GetTransform		(
				u16(m_object->eye_bone)
			);

		Fvector							temp;
		eye_matrix.transform_tiny		(temp,eye_position);
		m_object->XFORM().transform_tiny(eye_position,temp);

		if (m_stalker) {
			eye_direction.setHP				(-m_stalker->movement().m_head.current.yaw, -m_stalker->movement().m_head.current.pitch);
		} else { // if its a monster
			const MonsterSpace::SBoneRotation &head_orient = m_object->head_orientation();
			eye_direction.setHP				(-head_orient.current.yaw, -head_orient.current.pitch);
		}
	} 
	else {
		Fvector							temp;
		m_actor->cam_Active()->Get		(eye_position,eye_direction,temp);
	}

	Fvector								object_direction;
	game_object->Center					(object_direction);
	object_distance						= object_direction.distance_to(eye_position);
	object_direction.sub				(eye_position);
	object_direction.normalize_safe		();
	
	float								object_range, object_fov;
	if (m_object)
		m_object->update_range_fov		(object_range,object_fov,m_object->eye_range,deg2rad(m_object->eye_fov));
	else {
		object_fov						= deg2rad(m_actor->cam_Active()->f_fov);
		object_range					= g_pGamePersistent->Environment.CurrentEnv.far_plane;
	}

	float								fov = object_fov*.5f;
	float								cos_alpha = eye_direction.dotproduct(object_direction);
	clamp								(cos_alpha,-.99999f,.99999f);
	float								alpha = acosf(cos_alpha);
	clamp								(alpha,0.f,fov);

	float								max_view_distance = object_range, min_view_distance = object_range;
	max_view_distance					*= current_state().m_max_view_distance;
	min_view_distance					*= current_state().m_min_view_distance;

	float								distance = (1.f - alpha/fov)*(max_view_distance - min_view_distance) + min_view_distance;

	return								(distance);
}

float CVisualMemoryManager::object_luminocity	(const CGameObject *game_object) const
{
	if (game_object->CLS_ID != CLSID_OBJECT_ACTOR)
		return	(1.f);
	float		luminocity = const_cast<CGameObject*>(game_object)->ROS()->get_luminocity();
	float		power = log(luminocity > .001f ? luminocity : .001f)*current_state().m_luminocity_factor;
	return		(exp(power));
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

float CVisualMemoryManager::get_visible_value	(float distance, float object_distance, float time_delta, float object_velocity, float luminocity) const
{
	float								always_visible_distance = current_state().m_always_visible_distance;

	if (distance <= always_visible_distance + EPS_L)
		return							(current_state().m_visibility_threshold);

	return								(
		time_delta / 
		current_state().m_time_quant * 
		luminocity *
		(1.f + current_state().m_velocity_factor*object_velocity) *
		(distance - object_distance) /
		(distance - always_visible_distance)
	);
}

CNotYetVisibleObject *CVisualMemoryManager::not_yet_visible_object(const CGameObject *game_object)
{
	START_PROFILE("Memory Manager/visuals/not_yet_visible_object")
	xr_vector<CNotYetVisibleObject>::iterator	I = std::find_if(
		m_not_yet_visible_objects.begin(),
		m_not_yet_visible_objects.end(),
		CNotYetVisibleObjectPredicate(game_object)
		);
	if (I == m_not_yet_visible_objects.end())
		return							(0);
	return								(&*I);
	STOP_PROFILE
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

#ifndef USE_STALKER_VISION_FOR_MONSTERS
	if (!m_stalker && !m_actor)
		return					(true);
#endif

	float						object_distance, distance = object_visible_distance(game_object,object_distance);

	CNotYetVisibleObject		*object = not_yet_visible_object(game_object);

	if (distance < object_distance) {
		if (object) {
			object->m_value		-= current_state().m_decrease_value;
			if (object->m_value < 0.f)
				object->m_value	= 0.f;
			else
				object->m_update_time	= Device.dwTimeGlobal;
			return				(object->m_value >= current_state().m_visibility_threshold);
		}
		return					(false);
	}

	if (!object) {
		CNotYetVisibleObject		new_object;
		new_object.m_object			= game_object;
		new_object.m_prev_time		= 0;
		new_object.m_value			= get_visible_value(distance,object_distance,time_delta,get_object_velocity(game_object,new_object),object_luminocity(game_object));
		clamp						(new_object.m_value,0.f,current_state().m_visibility_threshold + EPS_L);
		new_object.m_update_time	= Device.dwTimeGlobal;
		new_object.m_prev_time		= get_prev_time(game_object);
		add_not_yet_visible_object	(new_object);
		return						(new_object.m_value >= current_state().m_visibility_threshold);
	}

	object->m_update_time		= Device.dwTimeGlobal;
	object->m_value				+= get_visible_value(distance,object_distance,time_delta,get_object_velocity(game_object,*object),object_luminocity(game_object));
	clamp						(object->m_value,0.f,current_state().m_visibility_threshold + EPS_L);
	object->m_prev_time			= get_prev_time(game_object);

	return						(object->m_value >= current_state().m_visibility_threshold);
}

void CVisualMemoryManager::add_visible_object	(const CObject *object, float time_delta, bool fictitious)
{
	xr_vector<CVisibleObject>::iterator	J;
	const CGameObject *game_object;
	const CGameObject *self;

//	START_PROFILE("Memory Manager/visuals/update/add_visibles/visible")
	game_object					= smart_cast<const CGameObject*>(object);
	if (!game_object || (!fictitious && !visible(game_object,time_delta)))
		return;
//	STOP_PROFILE

//	START_PROFILE("Memory Manager/visuals/update/add_visibles/find_object_by_id")
	self						= m_object;
	J							= std::find(m_objects->begin(),m_objects->end(),object_id(game_object));
//	STOP_PROFILE

//	START_PROFILE("Memory Manager/visuals/update/add_visibles/fill")
	if (m_objects->end() == J) {
		CVisibleObject			visible_object;

		visible_object.fill		(game_object,self,mask());
#ifdef USE_FIRST_GAME_TIME
		visible_object.m_first_game_time	= Level().GetGameTime();
#endif
#ifdef USE_FIRST_LEVEL_TIME
		visible_object.m_first_level_time	= Device.dwTimeGlobal;
#endif

		if (m_max_object_count <= m_objects->size()) {
			xr_vector<CVisibleObject>::iterator	I = std::min_element(m_objects->begin(),m_objects->end(),SLevelTimePredicate<CGameObject>());
			VERIFY				(m_objects->end() != I);
			*I					= visible_object;
		}
		else
			m_objects->push_back	(visible_object);
	}
	else
		(*J).fill				(game_object,self,(*J).m_squad_mask.get() | mask());
//	STOP_PROFILE
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

#ifdef DEBUG
void CVisualMemoryManager::check_visibles	() const
{
	squad_mask_type						mask = this->mask();
	xr_vector<CVisibleObject>::iterator	I = m_objects->begin();
	xr_vector<CVisibleObject>::iterator	E = m_objects->end();
	for ( ; I != E; ++I) {
		if (!(*I).visible(mask))
			continue;
		
		xr_vector<Feel::Vision::feel_visible_Item>::iterator	i = m_object->feel_visible.begin();
		xr_vector<Feel::Vision::feel_visible_Item>::iterator	e = m_object->feel_visible.end();
		for (; i!=e; ++i)
			if (i->O->ID() == (*I).m_object->ID()) {
				VERIFY						(i->fuzzy > 0.f);
				break;
			}
	}
}
#endif

bool CVisualMemoryManager::visible(u32 _level_vertex_id, float yaw, float eye_fov) const
{
	Fvector					direction;
	direction.sub			(ai().level_graph().vertex_position(_level_vertex_id),m_object->Position());
	direction.normalize_safe();
	float					y, p;
	direction.getHP			(y,p);
	if (angle_difference(yaw,y) <= eye_fov*PI/180.f/2.f)
		return(ai().level_graph().check_vertex_in_direction(m_object->ai_location().level_vertex_id(),m_object->Position(),_level_vertex_id));
	else
		return(false);
}

float CVisualMemoryManager::feel_vision_mtl_transp(CObject* O, u32 element)
{
	float vis				= 1.f;
	if (O){
		CKinematics* V		= smart_cast<CKinematics*>(O->Visual());
		if (0!=V){
			CBoneData& B	= V->LL_GetData((u16)element);
			vis				= GMLib.GetMaterialByIdx(B.game_mtl_idx)->fVisTransparencyFactor;
		}
	}else{
		CDB::TRI* T			= Level().ObjectSpace.GetStaticTris()+element;
		vis					= GMLib.GetMaterialByIdx(T->material)->fVisTransparencyFactor;
	}
	return vis;
}

struct CVisibleObjectPredicateEx {
	const CObject *m_object;

				CVisibleObjectPredicateEx	(const CObject *object) :
	m_object		(object)
	{
	}

	bool		operator()			(const MemorySpace::CVisibleObject &visible_object) const
	{
		if (!m_object)
			return			(!visible_object.m_object);
		if (!visible_object.m_object)
			return			(false);
		return				(m_object->ID() == visible_object.m_object->ID());
	}

	bool		operator()			(const MemorySpace::CNotYetVisibleObject &not_yet_visible_object) const
	{
		if (!m_object)
			return			(!not_yet_visible_object.m_object);
		if (!not_yet_visible_object.m_object)
			return			(false);
		return				(m_object->ID() == not_yet_visible_object.m_object->ID());
	}
};

void CVisualMemoryManager::remove_links	(CObject *object)
{
	{
		VERIFY						(m_objects);
		VISIBLES::iterator			I = std::find_if(m_objects->begin(),m_objects->end(),CVisibleObjectPredicateEx(object));
		if (I != m_objects->end())
			m_objects->erase		(I);
	}
	{
		NOT_YET_VISIBLES::iterator	I = std::find_if(m_not_yet_visible_objects.begin(),m_not_yet_visible_objects.end(),CVisibleObjectPredicateEx(object));
		if (I != m_not_yet_visible_objects.end())
			m_not_yet_visible_objects.erase	(I);
	}
}

CVisibleObject *CVisualMemoryManager::visible_object	(const CGameObject *game_object)
{
	VISIBLES::iterator			I = std::find_if(m_objects->begin(),m_objects->end(),CVisibleObjectPredicateEx(game_object));
	if (I == m_objects->end())
		return					(0);
	return						(&*I);
}

IC	squad_mask_type CVisualMemoryManager::mask			() const
{
	if (!m_stalker)
		return					(squad_mask_type(-1));
	return						(m_stalker->agent_manager().member().mask(m_stalker));
}

void CVisualMemoryManager::update				(float time_delta)
{
	START_PROFILE("Memory Manager/visuals/update")

	if (!enabled())
		return;

	m_last_update_time					= Device.dwTimeGlobal;

	squad_mask_type						mask = this->mask();
	VERIFY								(m_objects);
	m_visible_objects.clear				();

	START_PROFILE("Memory Manager/visuals/update/feel_vision_get")
	if (m_object)
		m_object->feel_vision_get		(m_visible_objects);
	else
		m_actor->memory().feel_vision_get(m_visible_objects);
	STOP_PROFILE

	START_PROFILE("Memory Manager/visuals/update/make_invisible")
	{
		xr_vector<CVisibleObject>::iterator	I = m_objects->begin();
		xr_vector<CVisibleObject>::iterator	E = m_objects->end();
		for ( ; I != E; ++I)
			if ((*I).m_level_time + current_state().m_still_visible_time < Device.dwTimeGlobal)
				(*I).visible			(mask,false);
	}
	STOP_PROFILE

	START_PROFILE("Memory Manager/visuals/update/add_visibles")
	{
		xr_vector<CObject*>::const_iterator	I = m_visible_objects.begin();
		xr_vector<CObject*>::const_iterator	E = m_visible_objects.end();
		for ( ; I != E; ++I)
			add_visible_object			(*I,time_delta);
	}
	STOP_PROFILE

	START_PROFILE("Memory Manager/visuals/update/make_not_yet_visible")
	{
		xr_vector<CNotYetVisibleObject>::iterator	I = m_not_yet_visible_objects.begin();
		xr_vector<CNotYetVisibleObject>::iterator	E = m_not_yet_visible_objects.end();
		for ( ; I != E; ++I)
			if ((*I).m_update_time < Device.dwTimeGlobal)
				(*I).m_value			= 0.f;
	}
	STOP_PROFILE

	START_PROFILE("Memory Manager/visuals/update/removing_offline")
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
	STOP_PROFILE

#if 0//def DEBUG
	if (m_stalker) {
		CAgentMemberManager::MEMBER_STORAGE::const_iterator	I = m_stalker->agent_manager().member().members().begin();
		CAgentMemberManager::MEMBER_STORAGE::const_iterator	E = m_stalker->agent_manager().member().members().end();
		for ( ; I != E; ++I)
			(*I)->object().memory().visual().check_visibles();
	}
#endif

	if (m_object) {
		xr_vector<CNotYetVisibleObject>::iterator	I = std::find_if(
			m_not_yet_visible_objects.begin(),
			m_not_yet_visible_objects.end(),
			CNotYetVisibleObjectPredicate(Actor())
		);
		if (I != m_not_yet_visible_objects.end()) {
			Actor()->SetActorVisibility				(
				m_object->ID(),
				clampr(
					(*I).m_value/visibility_threshold(),
					0.f,
					1.f
				)
			);
		}
		else
			Actor()->SetActorVisibility				(m_object->ID(),0.f);
	}

	STOP_PROFILE
}

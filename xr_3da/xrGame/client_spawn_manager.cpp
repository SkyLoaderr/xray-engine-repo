////////////////////////////////////////////////////////////////////////////
//	Module 		: client_spawn_manager.cpp
//	Created 	: 08.10.2004
//  Modified 	: 08.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Seniority hierarchy holder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h" 
#include "client_spawn_manager.h" 
#include "ai_space.h" 
#include "script_engine.h" 
#include "level.h"
#include "gameobject.h"
#include "script_space.h"
#include "script_game_object.h"

CClientSpawnManager::~CClientSpawnManager	()
{
}

void CClientSpawnManager::add				(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, const luabind::object &lua_object, LPCSTR method)
{
	CScriptCallback					callback;
	callback.set					(lua_object,method);
	add								(requesting_id,requested_id,callback);
}

void CClientSpawnManager::add				(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, const luabind::functor<void> &lua_function)
{
	CScriptCallback					callback;
	callback.set					(lua_function);
	add								(requesting_id,requested_id,callback);
}

void CClientSpawnManager::add				(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, CScriptCallback &script_callback)
{
	CObject							*object = Level().Objects.net_Find(requesting_id);
	if (object) {
		callback					(script_callback,object);
		return;
	}

	REQUEST_REGISTRY::iterator		I = m_registry.find(requesting_id);
	if (I == m_registry.end()) {
		REQUESTED_REGISTRY			registry;
		registry.insert				(std::make_pair(requested_id,script_callback));
		m_registry.insert			(std::make_pair(requesting_id,registry));
		return;
	}
	
	REQUESTED_REGISTRY::iterator	J = (*I).second.find(requested_id);
	if (J == (*I).second.end()) {
		(*I).second.insert			(std::make_pair(requested_id,script_callback));
		return;
	}
	
	ai().script_engine().script_log	(eLuaMessageTypeError,"Callback on object with id %d by object with id %d has already been setup!",requesting_id,requested_id);
}

void CClientSpawnManager::remove			(REQUESTED_REGISTRY &registry, ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id, bool no_warning)
{
	REQUESTED_REGISTRY::iterator	I = registry.find(requested_id);
	if (I != registry.end())
		registry.erase				(I);
	else
		ai().script_engine().script_log	(eLuaMessageTypeError,"There is no spawn callback on object with id %d from object with id %d!",requesting_id,requested_id);
}

void CClientSpawnManager::remove			(ALife::_OBJECT_ID requesting_id, ALife::_OBJECT_ID requested_id)
{
	REQUEST_REGISTRY::iterator		I = m_registry.find(requesting_id);
	if (I != m_registry.end())
		remove						((*I).second,requesting_id,requested_id);
	else
		ai().script_engine().script_log	(eLuaMessageTypeError,"There is no spawn callback on object with id %d from object with id %d!",requesting_id,requested_id);
}

void CClientSpawnManager::clear				(ALife::_OBJECT_ID requested_id)
{
	REQUEST_REGISTRY::iterator		I = m_registry.begin();
	REQUEST_REGISTRY::iterator		E = m_registry.end();
	for ( ; I != E; ++I)
		remove						((*I).second,(*I).first,requested_id,true);
}

void CClientSpawnManager::callback			(CScriptCallback &script_callback, CObject *object)
{
	CGameObject						*game_object = smart_cast<CGameObject*>(object);
	CScriptGameObject				*script_game_object = !game_object ? 0 : game_object->lua_game_object();
	SCRIPT_CALLBACK_EXECUTE_2		(script_callback,object->ID(),script_game_object);
}

void CClientSpawnManager::callback			(CObject *object)
{
	REQUEST_REGISTRY::iterator		I = m_registry.find(object->ID());
	if (I == m_registry.end())
		return;

	REQUESTED_REGISTRY::iterator	i = (*I).second.begin();
	REQUESTED_REGISTRY::iterator	e = (*I).second.end();
	for ( ; i != e; ++i)
		callback					((*i).second,object);

	(*I).second.clear				();
}

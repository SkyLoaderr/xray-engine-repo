////////////////////////////////////////////////////////////////////////////
//	Module 		: level_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Level script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level.h"
#include "script_space.h"
#include "actor.h"
#include "script_game_object.h"
#include "patrol_path_storage.h"
#include "xrServer.h"

using namespace luabind;

CScriptGameObject *tpfGetActor()
{
	CActor *l_tpActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (l_tpActor)
		return	(dynamic_cast<CGameObject*>(l_tpActor)->lua_game_object());
	else
		return	(0);
}

CScriptGameObject *get_object_by_name(LPCSTR caObjectName)
{
	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
	if (l_tpGameObject)
		return		(l_tpGameObject->lua_game_object());
	else
		return		(0);
}

//CScriptGameObject *get_object_by_id(ALife::_OBJECT_ID id)
//{
//	CGameObject		*l_tpGameObject	= dynamic_cast<CGameObject*>(Level().Objects.FindObjectByID(id));
//	if (l_tpGameObject)
//		return		(l_tpGameObject->lua_game_object());
//	else
//		return		(0);
//}

LPCSTR get_weather	()
{
	return			(*g_pGamePersistent->Environment.GetWeather());
}

void set_weather	(LPCSTR weather_name)
{
	VERIFY3			(g_pGamePersistent->Environment.Weathers.find(weather_name) != g_pGamePersistent->Environment.Weathers.end(),"There is no weather with the specified name ",weather_name);
	return			(g_pGamePersistent->Environment.SetWeather(weather_name));
}

void set_time_factor(float time_factor)
{
	if (!OnServer())
		return;

	Level().Server->game->SetGameTimeFactor(time_factor);
}

float get_time_factor()
{
	return			(Level().GetGameTimeFactor());
}

float cover_in_direction(u32 level_vertex_id, const Fvector &direction)
{
	float			y,p;
	direction.getHP	(y,p);
	return			(ai().level_graph().cover_in_direction(y,level_vertex_id));
}

float rain_factor()
{
	return			(g_pGamePersistent->Environment.CurrentEnv.rain_density);
}

u32	vertex_in_direction(u32 level_vertex_id, Fvector direction, float max_distance)
{
	direction.normalize_safe();
	direction.mul	(max_distance);
	Fvector			start_position = ai().level_graph().vertex_position(level_vertex_id);
	Fvector			finish_position = Fvector(start_position).add(direction);
	u32				result = u32(-1);
	ai().level_graph().farthest_vertex_in_direction(level_vertex_id,start_position,finish_position,result,0);
	return			(ai().level_graph().valid_vertex_id(result) ? result : level_vertex_id);
}

Fvector vertex_position(u32 level_vertex_id)
{
	return			(ai().level_graph().vertex_position(level_vertex_id));
}

void map_add_object_icon(CScriptGameObject* lua_object, u16 text_id)
{
	Level().AddObjectMapLocation(lua_object->object());
}
void map_remove_object_icon(CScriptGameObject* lua_object)
{
	Level().RemoveMapLocationByID(lua_object->object()->ID());
}

bool patrol_path_exists(LPCSTR patrol_path)
{
	return		(!!Level().patrol_paths().path(patrol_path,true));
}

LPCSTR name()
{
	return		(Level().Name());
}

void CLevel::script_register(lua_State *L)
{
	module(L,"level")
	[
		// declarations
		def("object",							get_object_by_name),
//		def("object",							get_object_by_id),
		def("actor",							tpfGetActor),
		//def("set_artifact_merge",				SetArtefactMergeFunctor),
		def("get_weather",						get_weather),
		def("set_weather",						set_weather),
		def("set_time_factor",					set_time_factor),
		def("get_time_factor",					get_time_factor),
		def("cover_in_direction",				cover_in_direction),
		def("vertex_in_direction",				vertex_in_direction),
		def("rain_factor",						rain_factor),
		def("patrol_path_exists",				patrol_path_exists),
		def("vertex_position",					vertex_position),
		def("name",								name),

		def("map_add_object_icon",				map_add_object_icon),
		def("map_remove_object_icon",			map_remove_object_icon)
	];
}

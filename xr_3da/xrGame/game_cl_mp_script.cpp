#include "stdafx.h"

#include "game_cl_mp_script.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"
#include "UIGameCustom.h"
#include "level.h"
#include "GameObject.h"
#include "script_game_object.h"
#include "xrmessages.h"

#include <luabind/adopt_policy.hpp>

using namespace luabind;

void game_cl_mp::script_register(lua_State *L)
{
	module(L)
		[
			luabind::class_<game_cl_GameState,game_GameState>("game_cl_GameState")
			.def_readwrite("local_svdpnid",		&game_cl_GameState::local_svdpnid)
			.def_readwrite("local_player",		&game_cl_GameState::local_player),
			luabind::class_<game_cl_mp,game_cl_GameState>("game_cl_mp")
		];
}

template <typename T>
struct CWrapperBase : public T, public luabind::wrap_base {
	typedef T inherited;
	typedef CWrapperBase<T>	self_type;

	DEFINE_LUA_WRAPPER_METHOD_0(CanBeReady,bool)
	DEFINE_LUA_WRAPPER_METHOD_V0(Init)
	DEFINE_LUA_WRAPPER_METHOD_R2P2_V2(TranslateGameMessage, u32, NET_Packet)
	DEFINE_LUA_WRAPPER_METHOD_1(OnKeyboardPress, bool, int)
	DEFINE_LUA_WRAPPER_METHOD_1(OnKeyboardRelease, bool, int)
	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1(net_import_state, NET_Packet)
	DEFINE_LUA_WRAPPER_METHOD_0(createGameUI,CUIGameCustom*)
	DEFINE_LUA_WRAPPER_METHOD_V1(shedule_Update, u32)
	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1(GetMapEntities, xr_vector<SZoneMapEntityData>)

	virtual game_PlayerState* createPlayerState()
	{ return call_member<game_PlayerState*>(this,"createPlayerState")[adopt(result)];}
	static game_PlayerState* createPlayerState_static(inherited* ptr)
	{ return ptr->self_type::inherited::createPlayerState();}



};

void game_cl_mp_script::EventGen	( NET_Packet* P, u16 type, u16 dest)
{ u_EventGen(*P,type,dest); }

void game_cl_mp_script::GameEventGen	( NET_Packet* P, u16 dest)
{ u_EventGen(*P,u16(GE_GAME_EVENT&0xffff),dest); }

void game_cl_mp_script::EventSend	( NET_Packet* P)
{ u_EventSend(*P); }

void game_cl_mp_script::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}

game_cl_mp_script::game_cl_mp_script()
:inherited()
{
};

CScriptGameObject*	game_cl_mp_script::GetObjectByGameID (u32 id)
{
	CObject* pObject = Level().Objects.net_Find(id);
	CGameObject* pGameObject = smart_cast<CGameObject*>(pObject);
	if(!pGameObject)
		return NULL;

	return pGameObject->lua_game_object();
}



void game_cl_mp_script::script_register(lua_State *L)
{
	typedef CWrapperBase<game_cl_mp_script> WrapType;
	typedef game_cl_mp_script BaseType;
	module(L)
		[
			luabind::class_< game_cl_mp_script, WrapType, game_cl_mp >("game_cl_mp_script")
			.def(	constructor<>())
			.def("CommonMessageOut",	&BaseType::CommonMessageOut)
			.def("GetPlayersCount",		&BaseType::GetPlayersCount)
			.def("GetObjectByGameID",	&BaseType::GetObjectByGameID)
			.def("GetPlayerByOrderID",	&BaseType::GetPlayerByOrderID)
			.def("GetClientIDByOrderID"	,&BaseType::GetClientIDByOrderID)
			.def("GetLocalPlayer",		&BaseType::GetLocalPlayer)
			.def("EventGen",			&BaseType::EventGen)
			.def("GameEventGen",		&BaseType::GameEventGen)
			.def("EventSend",			&BaseType::EventSend)
			.def("StartStopMenu",		&BaseType::StartStopMenu)
			.def("StartMenu",			&BaseType::StartMenu)
			.def("StopMenu",			&BaseType::StopMenu)

			.def("CanBeReady",			&BaseType::CanBeReady, &WrapType::CanBeReady_static)
			.def("Init",				&BaseType::Init, &WrapType::Init_static)
			.def("OnKeyboardPress",		&BaseType::OnKeyboardPress, &WrapType::OnKeyboardPress_static)
			.def("OnKeyboardRelease",	&BaseType::OnKeyboardRelease, &WrapType::OnKeyboardRelease_static)
			.def("net_import_state",	&BaseType::net_import_state, &WrapType::net_import_state_static)
			.def("shedule_Update",		&BaseType::shedule_Update, &WrapType::shedule_Update_static)
			.def("FillMapEntities",		&BaseType::GetMapEntities, &WrapType::GetMapEntities_static)
			.def("TranslateGameMessage",&BaseType::TranslateGameMessage, &WrapType::TranslateGameMessage_static)

			.def("createPlayerState",	&BaseType::createPlayerState, &WrapType::createPlayerState_static, adopt(result) )
			.def("createGameUI",		&BaseType::createGameUI, &WrapType::createGameUI_static, adopt(result) )
			
		];

}

////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script game object script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_game_object.h"

using namespace luabind;

extern class_<CScriptGameObject> &script_register_game_object1(class_<CScriptGameObject> &);
extern class_<CScriptGameObject> &script_register_game_object2(class_<CScriptGameObject> &);

void CScriptGameObject::script_register(lua_State *L)
{
	class_<CScriptGameObject>	instance("game_object");

	module(L)
	[
		script_register_game_object2(
			script_register_game_object1(instance)
		),

		class_<enum_exporter<GameObject::ECallbackType> >("callback")
		.enum_("callback_types")
		[
			value("trade_start",			int(GameObject::eTradeStart)),
			value("trade_stop",				int(GameObject::eTradeStop)),
			value("trade_sell_buy_item",	int(GameObject::eTradeSellBuyItem)),
			value("trade_perform_operation",int(GameObject::eTradePerformTradeOperation)),
			value("zone_enter",				int(GameObject::eZoneEnter)),
			value("zone_exit",				int(GameObject::eZoneExit)),
			value("death",					int(GameObject::eDeath)),
			value("patrol_path_in_point",	int(GameObject::ePatrolPathInPoint)),
			value("inventory_pda",			int(GameObject::eInventoryPda)),
			value("inventory_info",			int(GameObject::eInventoryInfo)),
			value("use_object",				int(GameObject::eUseObject)),
			value("hit",					int(GameObject::eHit)),
			value("sound",					int(GameObject::eSound)),
			value("action_movement",		int(GameObject::eActionTypeMovement)),
			value("action_watch",			int(GameObject::eActionTypeWatch)),
			value("action_animation",		int(GameObject::eActionTypeAnimation)),
			value("action_sound",			int(GameObject::eActionTypeSound)),
			value("action_particle",		int(GameObject::eActionTypeParticle)),
			value("action_object",			int(GameObject::eActionTypeObject))
		]

	];
}
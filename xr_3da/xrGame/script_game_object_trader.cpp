////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.сpp :	функции для торговли и торговцев
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "ai/trader/ai_trader.h"
#include "ai/trader/trader_animation.h"

void CScriptGameObject::set_trader_global_anim(LPCSTR anim)
{
	CAI_Trader *trader = smart_cast<CAI_Trader *>(&object());
	if (!trader) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot cast sctipt game object to trader!");
		return;
	}
	trader->animation().set_animation(anim);

}
void CScriptGameObject::set_trader_head_anim(LPCSTR anim)
{
	CAI_Trader *trader = smart_cast<CAI_Trader *>(&object());
	if (!trader) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot cast sctipt game object to trader!");
		return;
	}
	trader->animation().set_head_animation(anim);
}

void CScriptGameObject::set_trader_sound(LPCSTR sound, LPCSTR anim)
{
	CAI_Trader *trader = smart_cast<CAI_Trader *>(&object());
	if (!trader) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"Cannot cast sctipt game object to trader!");
		return;
	}
	trader->animation().set_sound(sound, anim);
}


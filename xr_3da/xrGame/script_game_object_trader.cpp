////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.сpp :	функции для торговли и торговцев
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"

#include "script_task.h"
#include "script_zone.h"
#include "ai/trader/ai_trader.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_task_registry.h"
#include "alife_trader_registry.h"


void CScriptGameObject::SetCallback(const luabind::functor<void> &tpZoneCallback, bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
	else if (l_tpScriptZone) l_tpScriptZone->set_callback(tpZoneCallback,bOnEnter);
	else l_tpTrader->set_callback(tpZoneCallback,bOnEnter);
}

void CScriptGameObject::SetCallback(const luabind::object &object, LPCSTR method, bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
	else if (l_tpScriptZone) l_tpScriptZone->set_callback(object,method,bOnEnter);
	else l_tpTrader->set_callback(object,method,bOnEnter);
}

void CScriptGameObject::ClearCallback(bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = smart_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone : cannot access class member set_callback!");
	else if (l_tpScriptZone)l_tpScriptZone->clear_callback(bOnEnter);
	else l_tpTrader->clear_callback(bOnEnter);
}



void CScriptGameObject::SetTradeCallback(const luabind::functor<void> &tpTradeCallback) {
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
	else l_tpTrader->set_trade_callback(tpTradeCallback);
}

void CScriptGameObject::SetTradeCallback(const luabind::object &object, LPCSTR method) {
	CAI_Trader	*l_tpTrader	= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
	else l_tpTrader->set_trade_callback(object, method);
}


void CScriptGameObject::ClearTradeCallback() {
	CAI_Trader	*l_tpTrader		= smart_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member clear_trade_callback!");
	else l_tpTrader->clear_trade_callback();
}
////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.сpp :	функции для торговли и торговцев
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_game_object.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_task_registry.h"
#include "script_task.h"
#include "script_zone.h"
#include "ai/trader/ai_trader.h"


void CScriptGameObject::SetCallback(const luabind::functor<void> &tpZoneCallback, bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = dynamic_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
	else if (l_tpScriptZone) l_tpScriptZone->set_callback(tpZoneCallback,bOnEnter);
	else l_tpTrader->set_callback(tpZoneCallback,bOnEnter);
}

void CScriptGameObject::SetCallback(const luabind::object &object, LPCSTR method, bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = dynamic_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone or CTrader: cannot access class member set_callback!");
	else if (l_tpScriptZone) l_tpScriptZone->set_callback(object,method,bOnEnter);
	else l_tpTrader->set_callback(object,method,bOnEnter);
}

void CScriptGameObject::ClearCallback(bool bOnEnter)
{
	CScriptZone	*l_tpScriptZone = dynamic_cast<CScriptZone*>(m_tpGameObject);
	CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpScriptZone && !l_tpTrader)
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CScriptZone : cannot access class member set_callback!");
	else if (l_tpScriptZone)l_tpScriptZone->clear_callback(bOnEnter);
	else l_tpTrader->clear_callback(bOnEnter);
}



void CScriptGameObject::SetTradeCallback(const luabind::functor<void> &tpTradeCallback) {
	CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
	else l_tpTrader->set_trade_callback(tpTradeCallback);
}

void CScriptGameObject::SetTradeCallback(const luabind::object &object, LPCSTR method) {
	CAI_Trader	*l_tpTrader	= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member set_trade_callback!");
	else l_tpTrader->set_trade_callback(object, method);
}


void CScriptGameObject::ClearTradeCallback() {
	CAI_Trader	*l_tpTrader		= dynamic_cast<CAI_Trader*>	(m_tpGameObject);

	if (!l_tpTrader) 
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CAI_Trader : cannot access class member clear_trade_callback!");
	else l_tpTrader->clear_trade_callback();
}

const ALIFE_TASK_VECTOR& CScriptGameObject::TraderArtefactTask ()
{
	CAI_Trader	*pTrader = dynamic_cast<CAI_Trader*>	(m_tpGameObject);
	
	static ALIFE_TASK_VECTOR dummy_vector;
	dummy_vector.clear();

	if (!pTrader) 
	{
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"the object does not belong to a CAI_Trader class!");
		return dummy_vector;
	}

	pTrader->alife_tasks.clear();
	
	//пройтись по всем заданиям, выбрать те, которые выдал этот торговец
	ALife::OBJECT_TASK_MAP task_id_map = ai().alife().tasks().cross();
	ALife::TASK_MAP task_prt_map = ai().alife().tasks().tasks();
    
	ALife::OBJECT_TASK_PAIR_IT it = task_id_map.find(pTrader->ID());
	if(task_id_map.end() != it)
	{
		ALife::TASK_SET& task_set = (*it).second;
		
		for(ALife::TASK_SET_IT set_it = task_set.begin();
			                      task_set.end() != set_it; set_it++)
		{
			R_ASSERT(task_prt_map.find(*set_it)!=task_prt_map.end());
			CScriptTask script_task(task_prt_map[*set_it]);
			pTrader->alife_tasks.push_back(script_task);
		}
	}

	return pTrader->alife_tasks;
}
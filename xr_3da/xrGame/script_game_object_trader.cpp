////////////////////////////////////////////////////////////////////////////
// script_game_object_trader.�pp :	������� ��� �������� � ���������
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
/*	
	//�������� �� ���� ��������, ������� ��, ������� ����� ���� ��������
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
*/

	ALife::ARTEFACT_TRADER_ORDER_MAP::const_iterator        i = pTrader->m_tpOrderedArtefacts.begin();
	ALife::ARTEFACT_TRADER_ORDER_MAP::const_iterator        e = pTrader->m_tpOrderedArtefacts.end();
	for ( ; i != e; ++i) {
		//			Msg     ("Artefact : section[%s] total_count[%d]",(*i).second->m_caSection,(*i).second->m_dwTotalCount);
		//			Msg     ("Orders : ");
		CScriptTask script_task;
		script_task.m_sName = pSettings->r_string((*i).second->m_caSection, "inv_name");

		ALife::ARTEFACT_ORDER_VECTOR::const_iterator    II = (*i).second->m_tpOrders.begin();
		ALife::ARTEFACT_ORDER_VECTOR::const_iterator    EE = (*i).second->m_tpOrders.end();
		for ( ; II != EE; ++II)
		{
			script_task.m_iPrice = (*II).m_price; 
			script_task.m_iQuantity = (*II).m_count;
			script_task.m_sOrganization = pSettings->r_string((*II).m_section, "name");
			pTrader->alife_tasks.push_back(script_task);
			//Msg("order : section[%s], count[%d], price[%d]",(*II).m_section,(*II).m_count,(*II).m_price);
		}
	}

	return pTrader->alife_tasks;
}
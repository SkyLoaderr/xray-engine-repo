#include "stdafx.h"
#include "game_sv_single.h"
#include "xrserver_objects_alife_monsters.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "alife_graph_registry.h"
#include "alife_time_manager.h"
#include "level.h"

game_sv_Single::game_sv_Single()
{
	m_alife_simulator			= NULL;
	type						= GAME_SINGLE;
};

game_sv_Single::~game_sv_Single			()
{
	xr_delete							(m_alife_simulator);
}

void	game_sv_Single::Create			(ref_str& options)
{
	inherited::Create					(options);
	if (strstr(*options,"/alife"))
		m_alife_simulator				= xr_new<CALifeSimulator>(&server(),&options);

	switch_Phase		(GAME_PHASE_INPROGRESS);
}

CSE_Abstract*		game_sv_Single::get_entity_from_eid		(u16 id)
{
	if (ai().get_alife()) {
		CSE_Abstract	*object = ai().alife().objects().object(id,true);
		if (object)
			return(object);
		else
			return(inherited::get_entity_from_eid(id));
	}
	else
		return(inherited::get_entity_from_eid(id));
}

void	game_sv_Single::OnCreate		(u16 id_who)
{
	if (!ai().get_alife())
		return;

	CSE_Abstract			*e_who			= get_entity_from_eid(id_who);
	VERIFY					(e_who);
	if (!e_who->m_bALifeControl)
		return;

	CSE_ALifeObject			*alife_object	= dynamic_cast<CSE_ALifeObject*>(e_who);
	if (!alife_object)
		return;

	alife_object->m_bOnline	= true;

	if (alife_object->ID_Parent != 0xffff) {
		CSE_ALifeDynamicObject			*parent = ai().alife().objects().object(alife_object->ID_Parent,true);
		if (parent) {
			CSE_ALifeTraderAbstract		*trader = dynamic_cast<CSE_ALifeTraderAbstract*>(parent);
			if (trader)
				alife().create			(alife_object);
			else
				alife_object->m_bALifeControl	= false;
		}
		else
			alife_object->m_bALifeControl		= false;
	}
	else
		alife().create					(alife_object);
}

BOOL	game_sv_Single::OnTouch			(u16 eid_who, u16 eid_what)
{
	CSE_Abstract*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

	if (ai().get_alife()) {
		CSE_ALifeTraderAbstract	*l_tpTraderParams		= dynamic_cast<CSE_ALifeTraderAbstract*>(e_who);
		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem	= dynamic_cast<CSE_ALifeInventoryItem*>	(e_what);
		CSE_ALifeDynamicObject	*l_tpDynamicObject		= dynamic_cast<CSE_ALifeDynamicObject*>	(e_who);
		
		if	(
				l_tpTraderParams && 
				l_tpALifeInventoryItem && 
				l_tpDynamicObject && 
				ai().alife().graph().level().object(l_tpALifeInventoryItem->base()->ID,true) &&
				ai().alife().objects().object(e_who->ID,true) &&
				ai().alife().objects().object(e_what->ID,true)
			)
			alife().graph().attach	(*e_who,l_tpALifeInventoryItem,l_tpDynamicObject->m_tGraphID,false);
#ifdef DEBUG
		else
			if (psAI_Flags.test(aiALife)) {
				Msg				("Cannot attach object [%s][%s][%d] to object [%s][%s][%d]",e_what->s_name_replace,e_what->s_name,e_what->ID,e_who->s_name_replace,e_who->s_name,e_who->ID);
			}
#endif
	}
	return TRUE;
}

BOOL	game_sv_Single::OnDetach		(u16 eid_who, u16 eid_what)
{
	if (ai().get_alife()) {
		CSE_Abstract*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
		CSE_Abstract*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

		CSE_ALifeInventoryItem *l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(e_what);
		if (!l_tpALifeInventoryItem)
			return TRUE;

		CSE_ALifeDynamicObject *l_tpDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(e_who);
		if (!l_tpDynamicObject)
			return TRUE;
		
		if	(
				ai().alife().objects().object(e_who->ID,true) && 
				!ai().alife().graph().level().object(l_tpALifeInventoryItem->base()->ID,true) && 
				ai().alife().objects().object(e_what->ID,true)
			)
			alife().graph().detach(*e_who,l_tpALifeInventoryItem,l_tpDynamicObject->m_tGraphID,false);
		else {
			if (!ai().alife().objects().object(e_what->ID,true)) {
				u16				id = l_tpALifeInventoryItem->base()->ID_Parent;
				l_tpALifeInventoryItem->base()->ID_Parent	= 0xffff;
				
				CSE_ALifeDynamicObject *dynamic_object = dynamic_cast<CSE_ALifeDynamicObject*>(e_what);
				VERIFY			(dynamic_object);
				dynamic_object->m_tNodeID		= l_tpDynamicObject->m_tNodeID;
				dynamic_object->m_tGraphID		= l_tpDynamicObject->m_tGraphID;
				dynamic_object->m_bALifeControl	= true;
				dynamic_object->m_bOnline		= true;
				alife().create	(dynamic_object);
				l_tpALifeInventoryItem->base()->ID_Parent	= id;
			}
#ifdef DEBUG
			else
				if (psAI_Flags.test(aiALife)) {
					Msg			("Cannot detach object [%s][%s][%d] from object [%s][%s][%d]",l_tpALifeInventoryItem->base()->s_name_replace,l_tpALifeInventoryItem->base()->s_name,l_tpALifeInventoryItem->base()->ID,l_tpDynamicObject->base()->s_name_replace,l_tpDynamicObject->base()->s_name,l_tpDynamicObject->ID);
				}
#endif
		}
	}
	return					(TRUE);
}


void	game_sv_Single::Update			()
{
	inherited::Update	();
/*	switch(phase) 	{
		case GAME_PHASE_PENDING : {
			OnRoundStart();
			switch_Phase(GAME_PHASE_INPROGRESS);
			break;
		}
	}*/
}


ALife::_TIME_ID game_sv_Single::GetGameTime		()
{
	if (ai().get_alife() && ai().alife().initialized())
		return(ai().alife().time_manager().game_time());
	else
		return(inherited::GetGameTime());
}

float game_sv_Single::GetGameTimeFactor		()
{
	if (ai().get_alife() && ai().alife().initialized())
		return(ai().alife().time_manager().time_factor());
	else
		return(inherited::GetGameTimeFactor());
}

void game_sv_Single::SetGameTimeFactor		(const float fTimeFactor)
{
	if (ai().get_alife() && ai().alife().initialized())
		return(alife().time_manager().set_time_factor(fTimeFactor));
	else
		return(inherited::SetGameTimeFactor(fTimeFactor));
}

bool game_sv_Single::change_level			(NET_Packet &net_packet, ClientID sender)
{
	if (ai().get_alife())
		return					(alife().change_level(net_packet));
	else
		return					(true);
}

void game_sv_Single::save_game				(NET_Packet &net_packet, ClientID sender)
{
	if (!ai().get_alife())
		return;
	
	Level().ClientSave			();

	ref_str						game_name;
	net_packet.r_stringZ		(game_name);
	alife().save				(*game_name);
}

bool game_sv_Single::load_game				(NET_Packet &net_packet, ClientID sender)
{
	if (!ai().get_alife())
		return					(inherited::load_game(net_packet,sender));
	ref_str						game_name;
	net_packet.r_stringZ		(game_name);
	return						(alife().load_game(*game_name,true));
}

void game_sv_Single::reload_game			(NET_Packet &net_packet, ClientID sender)
{
}

void game_sv_Single::switch_distance		(NET_Packet &net_packet, ClientID sender)
{
	if (!ai().get_alife())
		return;

	alife().set_switch_distance	(net_packet.r_float());
}

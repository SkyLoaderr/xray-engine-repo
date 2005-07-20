////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_base2.cpp
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator base class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_simulator_base.h"
#include "relation_registry.h"
#include "alife_registry_wrappers.h"
#include "xrServer_Objects_ALife_Items.h"
#include "alife_graph_registry.h"
#include "alife_object_registry.h"
#include "alife_story_registry.h"
#include "alife_schedule_registry.h"
#include "alife_trader_registry.h"

using namespace ALife;

void CALifeSimulatorBase::register_object	(CSE_ALifeDynamicObject *object, bool add_object)
{
	if (add_object)
		objects().add					(object);
	
	graph().update						(object);
	traders().add						(object);
	scheduled().add						(object);
	story_objects().add					(object->m_story_id,object);

	setup_simulator						(object);
	
	CSE_ALifeInventoryItem				*item = smart_cast<CSE_ALifeInventoryItem*>(object);
	if (item && item->attached()) {
		CSE_ALifeDynamicObject			*II = objects().object(item->base()->ID_Parent);
#ifdef DEBUG
		if (std::find(II->children.begin(),II->children.end(),item->base()->ID) != II->children.end()) {
//			if (psAI_Flags.test(aiALife)) {
				Msg						("[LSS] Specified item [%s][%d] is already attached to the specified object [%s][%d]",item->base()->name_replace(),item->base()->ID,II->name_replace(),II->ID);
//			}
			Debug.fatal					("[LSS] Cannot recover from the previous error!");
		}
#endif
		II->children.push_back			(item->base()->ID);
		CSE_ALifeTraderAbstract			*trader_abstract = smart_cast<CSE_ALifeTraderAbstract*>(II);
		if (trader_abstract)
			trader_abstract->attach		(item,true,false);
	}
}

void CALifeSimulatorBase::unregister_object	(CSE_ALifeDynamicObject *object, bool alife_query)
{
	CSE_ALifeInventoryItem			*item = smart_cast<CSE_ALifeInventoryItem*>(object);
	if (item && item->attached())
		graph().detach				(*objects().object(item->base()->ID_Parent),item,objects().object(item->base()->ID_Parent)->m_tGraphID,alife_query);

	objects().remove				(object->ID);
	story_objects().remove			(object->m_story_id);

	if (!object->m_bOnline) {
		graph().remove				(object,object->m_tGraphID);
		scheduled().remove			(object);
	}
	else
		if (object->ID_Parent == 0xffff)
			graph().level().remove	(object);

	//	RELATION_REGISTRY().ClearRelations(object->ID);
}


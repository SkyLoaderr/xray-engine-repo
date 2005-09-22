#include "stdafx.h"
#include "psy_dog.h"
#include "../../../level_navigation_graph.h"
#include "../../../ai_space.h"
#include "../../../alife_simulator.h"
#include "../../../xrServer_Object_Base.h"
#include "../../../xrserver.h"
#include "../../../ai_object_location.h"
#include "../../../level.h"

CPsyDog::CPsyDog()
{
}
CPsyDog::~CPsyDog()
{
}

void CPsyDog::Load(LPCSTR section)
{
	inherited::Load(section);
}

BOOL CPsyDog::net_Spawn(CSE_Abstract *dc)
{
	return inherited::net_Spawn(dc);

}
void CPsyDog::reinit()
{
	inherited::reinit();

	spawn_phantom();
}
void CPsyDog::reload(LPCSTR section)
{
	inherited::reload(section);
}

void CPsyDog::register_phantom(CPsyDogPhantom *phantom)
{
	m_storage.push_back(phantom);
}

bool CPsyDog::spawn_phantom()
{
	u32 node;
	if (!control().path_builder().get_node_in_radius(ai_location().level_vertex_id(), 5,10,5,node)) return false;
	
	Level().spawn_item("psy_dog_phantom", ai().level_graph().vertex_position(node), node, ID());
	return true;
}

void CPsyDog::delete_phantom(CPsyDogPhantom *phantom)
{
	if (ai().get_alife()) {
		CALifeSimulator *p_sim = const_cast<CALifeSimulator *>(ai().get_alife());
		CSE_Abstract* e_entity	= Level().Server->game->get_entity_from_eid	(phantom->ID()); 
		VERIFY(e_entity);
		p_sim->release(e_entity);
	} else {
		NET_Packet		P;
		u_EventGen		(P,GE_DESTROY,phantom->ID());
		u_EventSend		(P);
	}
}


//////////////////////////////////////////////////////////////////////////
// Phantom Psy Dog
//////////////////////////////////////////////////////////////////////////
CPsyDogPhantom::CPsyDogPhantom()
{
}
CPsyDogPhantom::~CPsyDogPhantom()
{
}
BOOL CPsyDogPhantom::net_Spawn(CSE_Abstract *dc)
{
	VERIFY(dc->ID_Parent != 0xffff);
		
	CObject	*O = Level().Objects.net_Find(dc->ID_Parent);
	VERIFY(O);

	CPsyDog *dog = smart_cast<CPsyDog *>(O);
	VERIFY(dog);
	
	dog->register_phantom(this);
	
	//dog->created = this;
	
	return inherited::net_Spawn(dc);
}



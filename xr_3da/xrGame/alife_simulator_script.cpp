////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_script.cpp
//	Created 	: 25.12.2002
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_simulator.h"
#include "script_space.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "alife_story_registry.h"
#include "script_engine.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "restriction_space.h"

using namespace luabind;

typedef xr_vector<std::pair<shared_str,int> >	STORY_PAIRS;
extern											LPCSTR GAME_CONFIG;
LPCSTR											_INVALID_STORY_ID	= "INVALID_STORY_ID";
STORY_PAIRS										story_ids;

CALifeSimulator *alife			()
{
	return			(const_cast<CALifeSimulator*>(ai().get_alife()));
}

CSE_ALifeDynamicObject *alife_object		(const CALifeSimulator *self, ALife::_OBJECT_ID id)
{
	VERIFY			(self);
	return			(self->objects().object(id,true));
}

CSE_ALifeDynamicObject *alife_object		(const CALifeSimulator *self, LPCSTR name)
{
	VERIFY			(self);
	
	for (CALifeObjectRegistry::OBJECT_REGISTRY::const_iterator it = self->objects().objects().begin(); it != self->objects().objects().end(); it++) {
		CSE_ALifeDynamicObject *obj = it->second;
		if (xr_strcmp(obj->name_replace(),name) == 0)
			return it->second;
	}
	
	return			(0);
}


CSE_ALifeDynamicObject *alife_object		(const CALifeSimulator *self, ALife::_OBJECT_ID id, bool no_assert)
{
	VERIFY			(self);
	return			(self->objects().object(id,no_assert));
}

CSE_ALifeDynamicObject *alife_story_object	(const CALifeSimulator *self, ALife::_STORY_ID id)
{
	return			(self->story_objects().object(id,true));
}

void generate_story_ids	(STORY_PAIRS &result)
{
	result.clear			();

    CInifile				*Ini = 0;
    string_path				game_ltx;
    FS.update_path			(game_ltx,"$game_config$",GAME_CONFIG);
    R_ASSERT3				(FS.exist(game_ltx),"Couldn't find file",game_ltx);
    Ini						= xr_new<CInifile>(game_ltx);
    
    LPCSTR					N,V;
	u32 					k;
	shared_str				temp;
    LPCSTR					section = "story_ids";
    R_ASSERT				(Ini->section_exist(section));

	for (k = 0; Ini->r_line(section,k,&N,&V); ++k) {
		temp				= Ini->r_string_wb(section,N);
		
		R_ASSERT3			(!strchr(*temp,' '),"Invalid story id description (contains spaces)!",*temp);
		R_ASSERT2			(xr_strcmp(*temp,_INVALID_STORY_ID),"INVALID_STORY_ID redifinition!");
		
		STORY_PAIRS::const_iterator	I = result.begin();
		STORY_PAIRS::const_iterator	E = result.end();
		for ( ; I != E; ++I)
			R_ASSERT3		((*I).first != temp,"Duplicated story id description!",*temp);
		
		result.push_back	(std::make_pair(*temp,atoi(N)));
	}

	result.push_back		(std::make_pair(_INVALID_STORY_ID,INVALID_STORY_ID));

    xr_delete				(Ini);
}

void kill_entity0			(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, const GameGraph::_GRAPH_ID &game_vertex_id)
{
	alife->kill_entity		(monster,game_vertex_id,0);
}

void kill_entity1			(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster)
{
	alife->kill_entity		(monster,monster->m_tGraphID,0);
}

void add_in_restriction		(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, ALife::_OBJECT_ID id)
{
	alife->add_restriction	(monster->ID,id,RestrictionSpace::eRestrictorTypeIn);
}

void add_out_restriction	(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, ALife::_OBJECT_ID id)
{
	alife->add_restriction	(monster->ID,id,RestrictionSpace::eRestrictorTypeOut);
}

void remove_in_restriction	(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, ALife::_OBJECT_ID id)
{
	alife->remove_restriction	(monster->ID,id,RestrictionSpace::eRestrictorTypeIn);
}

void remove_out_restriction	(CALifeSimulator *alife, CSE_ALifeMonsterAbstract *monster, ALife::_OBJECT_ID id)
{
	alife->remove_restriction	(monster->ID,id,RestrictionSpace::eRestrictorTypeOut);
}

void CALifeSimulator::script_register(lua_State *L)
{
	module(L)
	[
		class_<CALifeSimulator>("alife_simulator")
			.def("object",					(CSE_ALifeDynamicObject *(*) (const CALifeSimulator *,ALife::_OBJECT_ID))(alife_object))
			.def("object",					(CSE_ALifeDynamicObject *(*) (const CALifeSimulator *,LPCSTR))(alife_object))
			.def("object",					(CSE_ALifeDynamicObject *(*) (const CALifeSimulator *,ALife::_OBJECT_ID, bool))(alife_object))
			.def("story_object",			(CSE_ALifeDynamicObject *(*) (const CALifeSimulator *,ALife::_STORY_ID))(alife_story_object))
			.def("set_switch_online",		(void (CALifeSimulator::*) (ALife::_OBJECT_ID,bool))(CALifeSimulator::set_switch_online))
			.def("set_switch_offline",		(void (CALifeSimulator::*) (ALife::_OBJECT_ID,bool))(CALifeSimulator::set_switch_offline))
			.def("set_interactive",			(void (CALifeSimulator::*) (ALife::_OBJECT_ID,bool))(CALifeSimulator::set_interactive))
			.def("kill_entity",				&CALifeSimulator::kill_entity)
			.def("kill_entity",				&kill_entity0)
			.def("kill_entity",				&kill_entity1)
			.def("add_in_restriction",		&add_in_restriction)
			.def("add_out_restriction",		&add_out_restriction)
			.def("remove_in_restriction",	&remove_in_restriction)
			.def("remove_out_restriction",	&remove_out_restriction)
			.def("remove_all_restrictions",	&CALifeSimulator::remove_all_restrictions),

		def("alife",						&alife)
	];

	VERIFY						(story_ids.empty());
	generate_story_ids			(story_ids);

	luabind::class_<class_exporter<CALifeSimulator> >	instance("story_ids");

	STORY_PAIRS::const_iterator	I = story_ids.begin();
	STORY_PAIRS::const_iterator	E = story_ids.end();
	for ( ; I != E; ++I)
		instance.enum_			("_story_ids")[luabind::value(*(*I).first,(*I).second)];

	luabind::module				(L)[instance];
}

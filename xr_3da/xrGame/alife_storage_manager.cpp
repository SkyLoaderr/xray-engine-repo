////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_storage_manager.cpp
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator storage manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_storage_manager.h"
#include "alife_simulator_header.h"
#include "alife_time_manager.h"
#include "alife_spawn_registry.h"
#include "alife_object_registry.h"
#include "alife_graph_registry.h"
#include "alife_group_registry.h"
#include "alife_registry_container.h"
#include "xrserver.h"
#include "level.h"
#include "../x_ray.h"

using namespace ALife;

class CALifeUpdatePredicate {
private:
	CALifeStorageManager			*m_manager;

public:
	IC								CALifeUpdatePredicate	(CALifeStorageManager *manager)
	{
		m_manager					= manager;
	}

	IC		void					operator()				(CSE_ALifeDynamicObject *object) const
	{
		ALife::_OBJECT_ID			id = object->ID;
		object->ID					= m_manager->server().PerformIDgen(id);
		VERIFY						(id == object->ID);
		m_manager->register_object	(object);
	}
};

CALifeStorageManager::~CALifeStorageManager	()
{
}

void CALifeStorageManager::save	(LPCSTR save_name, bool update_name)
{
	string256					save;
	strcpy						(save,m_save_name);
	if (save_name) {
		strconcat				(m_save_name,save_name,SAVE_EXTENSION);
	}
	else {
		if (!xr_strlen(m_save_name)) {
			Log					("There is no file name specified!");
			return;
		}
	}

	CMemoryWriter				stream;
	header().save				(stream);
	time_manager().save			(stream);
	spawns().save				(stream);
	objects().save				(stream);
	registry().save				(stream);

	string256					temp;
	FS.update_path				(temp,"$game_saves$",m_save_name);
	stream.save_to				(temp);
	Msg							("* Game %s is successfully saved to file '%s' (%d bytes)",m_save_name,temp,stream.size());
	if (!update_name)
		strcpy					(m_save_name,save);
}

bool CALifeStorageManager::load	(LPCSTR save_name)
{
	CTimer						timer;
	timer.Start					();
	string256					save;
	strcpy						(save,m_save_name);
	if (!save_name) {
		if (!xr_strlen(m_save_name))
			R_ASSERT2			(false,"There is no file name specified!");
	}
	else
		strconcat				(m_save_name,save_name,SAVE_EXTENSION);
	string256					file_name;
	FS.update_path				(file_name,"$game_saves$",m_save_name);

	IReader						*stream;
	stream						= FS.r_open(file_name);
	if (!stream) {
		Msg						("* Cannot find saved game %s",file_name);
		strcpy					(m_save_name,save);
		return					(false);
	}

	{
		string512				temp;
		pApp->LoadTitle			(strconcat(temp,"Loading saved game \"",save_name,SAVE_EXTENSION,"\"..."));
	}

	unload						();
	reload						(m_section);

	header().load				(*stream);
	time_manager().load			(*stream);
	spawns().load				(*stream,file_name);
	objects().load				(*stream,CALifeUpdatePredicate(this));
	registry().load				(*stream);

	FS.r_close					(stream);

	groups().on_after_game_load	();

	VERIFY						(graph().actor());
	
	Msg							("* Game %s is successfully loaded from file '%s' (%.3fs)",save_name, file_name,timer.GetElapsed_sec());
	return						(true);
}

void CALifeStorageManager::save	(NET_Packet &net_packet)
{
	prepare_objects_for_save	();

	shared_str					game_name;
	net_packet.r_stringZ		(game_name);
	save						(*game_name,!!net_packet.r_u8());
}

void CALifeStorageManager::prepare_objects_for_save	()
{
	Level().ClientSend			();
	Level().ClientSave			();
}

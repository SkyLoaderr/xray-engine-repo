////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_object_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife object registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_object_registry.h"
#include "net_utils.h"

CALifeObjectRegistry::CALifeObjectRegistry	(LPCSTR section)
{
}

CALifeObjectRegistry::~CALifeObjectRegistry	()
{
	OBJECT_REGISTRY::iterator	I = m_objects.begin();
	OBJECT_REGISTRY::iterator	E = m_objects.end();
	for ( ; I != E; ++I)	
		xr_delete				((*I).second);
}

void CALifeObjectRegistry::save				(IWriter &memory_stream)
{
	Msg							("* Saving objects...");
	memory_stream.open_chunk	(OBJECT_CHUNK_DATA);
	u32							object_count = 0;
	OBJECT_REGISTRY::iterator	I = m_objects.begin();
	OBJECT_REGISTRY::iterator	E = m_objects.end();
	for ( ; I != E; ++I)
		if (!(*I).second->can_save())
			++object_count;

	VERIFY						(object_count <= objects().size());

	memory_stream.w_u32			(u32(objects().size() - object_count));
	I							= m_objects.begin();
	for ( ; I != E; ++I) {
		if (!(*I).second->can_save())
			continue;

		NET_Packet				tNetPacket;
		// Spawn
		(*I).second->Spawn_Write(tNetPacket,TRUE);
		memory_stream.w_u16		(u16(tNetPacket.B.count));
		memory_stream.w			(tNetPacket.B.data,tNetPacket.B.count);

		// Update
		tNetPacket.w_begin		(M_UPDATE);
		(*I).second->UPDATE_Write(tNetPacket);

		memory_stream.w_u16		(u16(tNetPacket.B.count));
		memory_stream.w			(tNetPacket.B.data,tNetPacket.B.count);
	}
	memory_stream.close_chunk	();
	
	Msg							("%d objects are successfully saved",u32(objects().size() - object_count));
}

CSE_ALifeDynamicObject *CALifeObjectRegistry::get_object		(IReader &file_stream)
{
	NET_Packet				tNetPacket;
	u16						u_id;
	// Spawn
	tNetPacket.B.count		= file_stream.r_u16();
	file_stream.r			(tNetPacket.B.data,tNetPacket.B.count);
	tNetPacket.r_begin		(u_id);
	R_ASSERT2				(M_SPAWN==u_id,"Invalid packet ID (!= M_SPAWN)");

	string64				s_name;
	tNetPacket.r_stringZ	(s_name);
	if (psAI_Flags.test(aiALife)) {
		Msg					("Loading object %s",s_name);
	}
	// create entity
	CSE_Abstract			*tpSE_Abstract = F_entity_Create	(s_name);
	R_ASSERT2				(tpSE_Abstract,"Can't create entity.");
	CSE_ALifeDynamicObject	*tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
	R_ASSERT2				(tpALifeDynamicObject,"Non-ALife object in the saved game!");
	tpALifeDynamicObject->Spawn_Read(tNetPacket);

	// Update
	tNetPacket.B.count		= file_stream.r_u16();
	file_stream.r			(tNetPacket.B.data,tNetPacket.B.count);
	tNetPacket.r_begin		(u_id);
	R_ASSERT2				(M_UPDATE==u_id,"Invalid packet ID (!= M_UPDATE)");
	tpALifeDynamicObject->UPDATE_Read(tNetPacket);

	return					(tpALifeDynamicObject);
}

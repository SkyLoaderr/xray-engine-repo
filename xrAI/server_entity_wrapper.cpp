////////////////////////////////////////////////////////////////////////////
//	Module 		: server_entity_wrapper.cpp
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Server entity wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "server_entity_wrapper.h"
#include "xrServer_Object_Base.h"
#include "xrmessages.h"
#include "xrSE_Factory_import_export.h"

struct ISE_Abstract;

CServerEntityWrapper::~CServerEntityWrapper	()
{
	F_entity_Destroy		(m_object);
}

void CServerEntityWrapper::save				(IWriter &stream)
{
	NET_Packet				net_packet;

	stream.open_chunk		(m_object->m_spawn_id);

	// Spawn
	m_object->Spawn_Write	(net_packet,TRUE);
	stream.w_u16			(u16(net_packet.B.count));
	stream.w				(net_packet.B.data,net_packet.B.count);

	// Update
	net_packet.w_begin		(M_UPDATE);
	m_object->UPDATE_Write	(net_packet);
	stream.w_u16			(u16(net_packet.B.count));
	stream.w				(net_packet.B.data,net_packet.B.count);

	stream.close_chunk		();
}

void CServerEntityWrapper::load				(IReader &stream)
{
}

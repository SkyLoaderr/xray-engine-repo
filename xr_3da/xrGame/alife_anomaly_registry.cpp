////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_anomaly_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife anomaly registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_anomaly_registry.h"
#include "object_broker.h"

CALifeAnomalyRegistry::~CALifeAnomalyRegistry	()
{
	delete_data					(m_anomalies);
}

void CALifeAnomalyRegistry::save				(IWriter &memory_stream)
{
	Msg							("* Saving anomalies...");
	memory_stream.open_chunk	(ANOMALY_CHUNK_DATA);
	save_data					(m_anomalies,memory_stream);
	memory_stream.close_chunk	();
}

void CALifeAnomalyRegistry::load				(IReader &file_stream)
{ 
	Msg							("* Loading anomalies...");
	R_ASSERT2					(file_stream.find_chunk(ANOMALY_CHUNK_DATA),"Can't find chunk ANOMALY_CHUNK_DATA!");
	load_data					(m_anomalies,file_stream);
}

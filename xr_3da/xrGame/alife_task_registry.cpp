////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_task_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife task registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_task_registry.h"
#include "object_broker.h"

IC const ALife::_TASK_ID tfChooseTaskKeyPredicate	(const CSE_ALifeTask *T)
{
	return						(T->m_tTaskID);
};

CALifeTaskRegistry::~CALifeTaskRegistry	()
{
	clear						();
}

void CALifeTaskRegistry::save			(IWriter &memory_stream)
{
	Msg							("* Saving tasks...");
	memory_stream.open_chunk	(TASK_CHUNK_DATA);
	memory_stream.w				(&m_id,sizeof(m_id));
	save_data					(m_tasks,memory_stream);
	memory_stream.close_chunk	();
}

void CALifeTaskRegistry::load			(IReader &file_stream)
{
	Msg							("* Loading tasks...");
	R_ASSERT2					(file_stream.find_chunk(TASK_CHUNK_DATA),"Can't find chunk TASK_CHUNK_DATA");
	file_stream.r				(&m_id,sizeof(m_id));
	load_data					(m_tasks,file_stream,tfChooseTaskKeyPredicate);
	ALife::TASK_PAIR_IT			I = m_tasks.begin();
	ALife::TASK_PAIR_IT			E = m_tasks.end();
	for ( ; I != E; ++I)
		update					((*I).second);
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_task.h
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife task class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_interfaces.h"
#include "alife_space.h"

class CALifeTask : public IPureALifeLSObject {
public:
	ALife::_TASK_ID				m_tTaskID;
	ALife::_TIME_ID				m_tTimeID;
	ALife::_OBJECT_ID			m_tCustomerID;
	float						m_fCost;
	ALife::ETaskType			m_tTaskType;
	u32							m_dwTryCount;
	union {
		string32				m_caSection;
		ALife::_OBJECT_ID		m_tObjectID;
	};
	union {
		ALife::_LOCATION_ID		m_tLocationID[LOCATION_TYPE_COUNT];
		ALife::_GRAPH_ID		m_tGraphID;
	};

public:
	virtual						~CALifeTask		();
	virtual void				load			(IReader &stream);
	virtual void				save			(IWriter &stream);
};

#include "alife_task_inline.h"
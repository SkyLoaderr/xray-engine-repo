////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_task_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife task registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Objects_ALife.h"
#include "object_broker.h"

class CALifeTaskRegistry {
protected:
	ALife::_TASK_ID							m_id;
	ALife::TASK_MAP							m_tasks;
	ALife::OBJECT_TASK_MAP					m_cross;

public:
	IC										CALifeTaskRegistry	(LPCSTR section);
	virtual									~CALifeTaskRegistry	();
	virtual	void							save				(IWriter &tMemoryStream);
	virtual	void							load				(IReader &tFileStream);
	IC		void							update				(CSE_ALifeTask *tpTask);
	IC		void							add					(CSE_ALifeTask *tpTask);
	IC		void							remove				(const ALife::_TASK_ID &id, bool no_assert = false);
	IC		CSE_ALifeTask					*task				(const ALife::_TASK_ID &id, bool no_assert = false) const;
	IC		const ALife::TASK_MAP			&tasks				() const;
	IC		const ALife::OBJECT_TASK_MAP	&cross				() const;
	IC		void							clear				();
};

#include "alife_task_registry_inline.h"
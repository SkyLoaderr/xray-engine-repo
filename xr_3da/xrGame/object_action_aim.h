////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_show.h
//	Created 	: 14.03.2004
//  Modified 	: 14.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object show command
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_action_base.h"

class CInventoryItem;

class CObjectActionAim : public CObjectActionBase<CInventoryItem> {
protected:
	typedef CObjectActionBase<CInventoryItem> inherited;

protected:
	bool				*m_condition;
	u32					m_start_time;

public:
						CObjectActionAim	(CInventoryItem *item, CAI_Stalker *owner, bool *condition, LPCSTR action_name = "");
	virtual void		initialize			();
	virtual void		execute				();
};
////////////////////////////////////////////////////////////////////////////
//	Module 		: object_action_base.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base action
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "action_base.h"

class CAI_Stalker;

class CObjectActionBase : public CActionBase<CAI_Stalker> {
protected:
	typedef CActionBase<CAI_Stalker> inherited;
	CGameObject			*m_item;

public:
	IC					CObjectActionBase(CGameObject *item, CAI_Stalker *owner, LPCSTR action_name = "");
};

#include "object_action_base_inline.h"
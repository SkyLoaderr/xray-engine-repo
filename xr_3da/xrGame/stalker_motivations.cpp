////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_motivations.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivations classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_motivations.h"
#include "motivation_action_manager_stalker.h"
#include "ai/stalker/ai_stalker.h"

//////////////////////////////////////////////////////////////////////////
// CMotivationGlobal
//////////////////////////////////////////////////////////////////////////

float CMotivationGlobal::evaluate	(u32 sub_motivation_id)
{
	VERIFY				(m_object);
	if (CMotivationActionManagerStalker::eMotivationAlive == sub_motivation_id)
		if (m_object->g_Alive())
			return		(1.f);
		else
			return		(0.f);
	else
		if (CMotivationActionManagerStalker::eMotivationDead == sub_motivation_id)
			if (!m_object->g_Alive())
				return	(1.f);
			else
				return	(0.f);

	NODEFAULT;
#ifdef DEBUG
	return				(0.f);
#endif
}
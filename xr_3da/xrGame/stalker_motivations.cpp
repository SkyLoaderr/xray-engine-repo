////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_motivations.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker motivations classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_motivations.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_decision_space.h"

using namespace StalkerDecisionSpace;

//////////////////////////////////////////////////////////////////////////
// CMotivationGlobal
//////////////////////////////////////////////////////////////////////////

float CMotivationGlobal::evaluate	(u32 sub_motivation_id)
{
	VERIFY				(m_object);
	if (eMotivationAlive == sub_motivation_id)
		if (m_object->g_Alive())
			return		(1.f);
		else
			return		(0.f);
	else
		if (eMotivationDead == sub_motivation_id)
			if (!m_object->g_Alive())
				return	(1.f);
			else
				return	(0.f);

	NODEFAULT;
#ifdef DEBUG
	return				(0.f);
#endif
}
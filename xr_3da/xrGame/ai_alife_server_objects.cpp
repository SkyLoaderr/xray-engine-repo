////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_server_objects.cpp
//	Created 	: 13.02.2003
//  Modified 	: 13.02.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_server_objects.h"

void CALifeMonsterParams::Init(LPCSTR caSection)
{
	m_iHealth					= pSettings->ReadINT(caSection, "health");
};

void CALifeTraderParams::Init(LPCSTR caSection)
{
	m_fCumulativeItemMass		= 0.0f;
	m_dwMoney					= 0;
	if (pSettings->LineExists(caSection, "money"))
		m_dwMoney = pSettings->ReadINT(caSection, "money");
	m_tRank						= EStalkerRank(pSettings->ReadINT(caSection, "rank"));
	m_tpItemIDs.clear			();
};

void CALifeTraderAbstract::Init(LPCSTR caSection)
{
	m_tpEvents.clear			();
	m_tpTaskIDs.clear			();
	m_fMaxItemMass				= pSettings->ReadFLOAT(caSection, "max_item_mass");
};

void CALifeZone::Init(LPCSTR caSection)
{
	m_tAnomalousZone			= EAnomalousZoneType(pSettings->ReadINT(caSection, "anomaly_type"));
};

void CALifeItem::Init(LPCSTR caSection)
{
	inherited::Init				(caSection);
	m_fMass						= pSettings->ReadFLOAT(caSection, "ph_mass");
	m_dwCost					= pSettings->ReadINT(caSection, "cost");
	if (pSettings->LineExists	(caSection, "health_value"))
		m_iHealthValue			= pSettings->ReadINT(caSection, "health_value");
	else
		m_iHealthValue			= 0;
	m_bAttached					= false;
};

void CALifeMonsterAbstract::Init(LPCSTR caSection)
{
	inherited::Init				(caSection);
	m_tNextGraphID				= m_tGraphID;
	m_tPrevGraphID				= m_tGraphID;
	m_fGoingSpeed				= pSettings->ReadFLOAT	(caSection, "going_speed");
	m_fCurSpeed					= 0.0f;
	m_fDistanceFromPoint		= 0.0f;
	m_fDistanceToPoint			= 0.0f;
};

void CALifeHumanAbstract::Init(LPCSTR caSection)
{
	CALifeMonsterAbstract::Init	(caSection);
	CALifeTraderAbstract::Init	(caSection);
	m_tpaVertices.clear			();
	m_baVisitedVertices.clear	();
	m_tpTasks.clear				();
	m_dwCurTask					= u32(-1);
	m_tTaskState				= eTaskStateNoTask;
	m_dwCurNode					= u32(-1);
	m_dwCurTaskLocation			= u32(-1);
	m_fSearchSpeed				= pSettings->ReadFLOAT				(caSection, "search_speed");
};

////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_process.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation processing
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "a_star.h"

//#define WRITE_TO_LOG

void CAI_ALife::Update(u32 dt)
{
	inherited::Update(dt);
	if (!m_bLoaded)
		return;
#ifdef WRITE_TO_LOG
	Msg("* %7.2fs",Level().timeServer()/1000.f);
#endif
	u64	qwStartTime = CPU::GetCycleCount();
	if (m_tpScheduledObjects.size()) {
		int i=0;
		do {
			i++;
			m_dwObjectsBeingProcessed = ((m_dwObjectsBeingProcessed + 1) % m_tpScheduledObjects.size());
			vfProcessNPC(m_tpScheduledObjects[m_dwObjectsBeingProcessed]);
		}
		while (((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i < m_qwMaxProcessTime) && (i < (int)m_tpScheduledObjects.size()));
	}
#ifdef WRITE_TO_LOG
	u64 t2x = CPU::GetCycleCount() - qwStartTime;
	Msg("* %.3f microseconds",CPU::cycles2microsec*t2x);
#endif
}

void CAI_ALife::vfProcessNPC(CALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	CALifeHumanAbstract *tpALifeHumanAbstract = dynamic_cast<CALifeHumanAbstract *>(tpALifeMonsterAbstract);
	if (tpALifeHumanAbstract) {
		CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeMonsterAbstract);
		if (tpALifeHuman)
			if (tpALifeHuman->m_bIsTrader)
				vfUpdateTrader(tpALifeHuman);
			else
				vfUpdateHuman(tpALifeHuman);
		else
			vfUpdateHumanGroup(dynamic_cast<CALifeHumanGroup *>(tpALifeMonsterAbstract));
	}
	else
		vfUpdateMonster(tpALifeMonsterAbstract);
	tpALifeMonsterAbstract->m_tTimeID = Level().timeServer();
}

void CAI_ALife::vfUpdateMonster(CALifeMonsterAbstract *tpALifeMonsterAbstract)
{
	vfChooseNextRoutePoint	(tpALifeMonsterAbstract);
	vfCheckForTheBattle		(tpALifeMonsterAbstract);
}

void CAI_ALife::vfUpdateTrader(CALifeHuman *tpALifeHuman)
{
}

void CAI_ALife::vfUpdateHumanGroup(CALifeHumanGroup *tpALifeHumanGroup)
{
}

void CAI_ALife::vfUpdateHuman(CALifeHuman *tpALifeHuman)
{
	if (tpALifeHuman->m_dwCurTask == u32(-1)) {
		CALifeHuman *tpTrader = tpfGetNearestSuitableTrader(tpALifeHuman);
		Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,tpTrader->m_tGraphID,tpALifeHuman->m_tpaVertices);
	}
	else
		switch (tpALifeHuman->m_tTaskState) {
			case eTaskStateNone : {
				break;
			}
		};
	vfChooseNextRoutePoint	(tpALifeHuman);
	vfCheckForTheBattle		(tpALifeHuman);
	vfCheckForItems			(tpALifeHuman);
	vfCheckForDeletedEvents	(tpALifeHuman);
}
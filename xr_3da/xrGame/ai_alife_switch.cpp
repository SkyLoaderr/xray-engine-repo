////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_switch.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life Online/Offline switches
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "GameObject.h"

void CAI_ALife::vfCreateObject(CALifeDynamicObject *tpALifeObject)
{
	NET_Packet						P;
	m_tpServer->Process_spawn		(P,0,FALSE,tpALifeObject);
}

void CAI_ALife::vfReleaseObject(CALifeDynamicObject *tpALifeObject)
{
	NET_Packet						P;
	m_tpServer->game->u_EventGen	(P,GE_DESTROY,tpALifeObject->ID);
	m_tpServer->game->u_EventSend	(P);
}

void CAI_ALife::vfSwitchObjectOnline(CALifeDynamicObject *tpALifeObject)
{
	VERIFY							(!tpALifeObject->m_bOnline);
	vfCreateObject					(tpALifeObject);
	tpALifeObject->m_bOnline		= true;
	Msg								("- SERVER: Going online [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeObject->s_name_replace, tpALifeObject->g_team(), tpALifeObject->g_squad(), tpALifeObject->g_group(), tpALifeObject->ID, "*SERVER*");
}

void CAI_ALife::vfSwitchObjectOffline(CALifeDynamicObject *tpALifeObject)
{
	VERIFY							(tpALifeObject->m_bOnline);
	vfReleaseObject					(tpALifeObject);
	tpALifeObject->m_bOnline		= false;
	Msg								("- SERVER: Going offline [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeObject->s_name_replace, tpALifeObject->g_team(), tpALifeObject->g_squad(), tpALifeObject->g_group(), tpALifeObject->ID, "*SERVER*");
}

void CAI_ALife::ProcessOnlineOfflineSwitches(CALifeDynamicObject *I)
{
	if (I->m_bOnline) {
		if (I->ID_Parent == 0xffff) {
			if (m_tpActor->o_Position.distance_to(I->o_Position) > m_fOnlineDistance)
				vfSwitchObjectOffline(I);
		}
		else {
			if (I->ID_Parent != 0xfffe) {
				OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
				R_ASSERT			(J != m_tObjectRegistry.end());
				if (!(*J).second->m_bOnline)
					vfSwitchObjectOffline(I);
			}
		}
	}
	else {
		if (I->ID_Parent == 0xffff) {
			if (m_tpActor->o_Position.distance_to(I->o_Position) <= m_fOnlineDistance)
				vfSwitchObjectOnline(I);
		}
		else {
			if (I->ID_Parent != 0xfffe) {
				OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
				R_ASSERT			(J != m_tObjectRegistry.end());
				if ((*J).second->m_bOnline)
					vfSwitchObjectOnline(I);
			}
		}
	}
}
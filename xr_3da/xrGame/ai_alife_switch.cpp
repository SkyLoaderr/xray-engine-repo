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

void CAI_ALife::vfCreateObject(CALifeDynamicObject *tpALifeDynamicObject)
{
	NET_Packet						tNetPacket;
//	tpALifeDynamicObject->s_flags.set(M_SPAWN_UPDATE);
	m_tpServer->Process_spawn		(tNetPacket,0,FALSE,tpALifeDynamicObject);
//	tpALifeDynamicObject->s_flags.clear(M_SPAWN_UPDATE);
//	xrClientData* CL				= m_tpServer->ID_to_client(0);
//	if (CL) {
//		tNetPacket.w_begin			(M_UPDATE);
//		tNetPacket.w_u16			(tpALifeDynamicObject->ID);
//		tpALifeDynamicObject->UPDATE_Write(tNetPacket);
//		m_tpServer->SendTo			(CL->ID,tNetPacket,net_flags(TRUE,TRUE));
//		m_tpServer->SendBroadcast	(CL->ID,tNetPacket,net_flags(TRUE,TRUE));
//	}
}

void CAI_ALife::vfReleaseObject(CALifeDynamicObject *tpALifeDynamicObject)
{
	m_tpServer->Perform_destroy		(tpALifeDynamicObject,net_flags(TRUE,TRUE));
}

void CAI_ALife::vfSwitchObjectOnline(CALifeDynamicObject *tpALifeDynamicObject)
{
	VERIFY							(!tpALifeDynamicObject->m_bOnline);
	vfCreateObject					(tpALifeDynamicObject);
	tpALifeDynamicObject->m_dwLastSwitchTime = 0;
	tpALifeDynamicObject->m_bOnline	= true;
	Msg								("- SERVER: Going online [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->g_team(), tpALifeDynamicObject->g_squad(), tpALifeDynamicObject->g_group(), tpALifeDynamicObject->ID, "*SERVER*");
}

void CAI_ALife::vfSwitchObjectOffline(CALifeDynamicObject *tpALifeDynamicObject)
{
	VERIFY							(tpALifeDynamicObject->m_bOnline);
	vfReleaseObject					(tpALifeDynamicObject);
	tpALifeDynamicObject->m_dwLastSwitchTime = 0;
	tpALifeDynamicObject->m_bOnline	= false;
	Msg								("- SERVER: Going offline [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->g_team(), tpALifeDynamicObject->g_squad(), tpALifeDynamicObject->g_group(), tpALifeDynamicObject->ID, "*SERVER*");
}

void CAI_ALife::ProcessOnlineOfflineSwitches(CALifeDynamicObject *I)
{
	if (I->m_bOnline)
		if (I->ID_Parent == 0xffff) {
			if (I->m_dwLastSwitchTime) {
				if (m_tpActor->o_Position.distance_to(I->o_Position) > m_fOnlineDistance) {
					if (Device.dwTimeGlobal -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
						vfSwitchObjectOffline(I);
				}
				else
					I->m_dwLastSwitchTime = 0;
			}
			else
				I->m_dwLastSwitchTime = Device.dwTimeGlobal;
		}
		else {
			OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
			VERIFY				(J != m_tObjectRegistry.end());
			if (!(*J).second->m_bOnline)
				vfSwitchObjectOffline(I);
		}
	else
		if (I->ID_Parent == 0xffff) {
			if (I->m_dwLastSwitchTime) {
				if (m_tpActor->o_Position.distance_to(I->o_Position) <= m_fOnlineDistance) {
					if (Device.dwTimeGlobal -  I->m_dwLastSwitchTime > m_dwSwitchDelay)
						vfSwitchObjectOnline(I);
				}
				else
					I->m_dwLastSwitchTime = 0;
			}
			else
				I->m_dwLastSwitchTime = Device.dwTimeGlobal;
		}
		else {
			OBJECT_PAIR_IT		J = m_tObjectRegistry.find(I->ID_Parent);
			VERIFY				(J != m_tObjectRegistry.end());
			if ((*J).second->m_bOnline)
				vfSwitchObjectOnline(I);
		}
}
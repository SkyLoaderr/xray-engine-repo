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
	m_tpServer->Process_spawn		(tNetPacket,0,FALSE,tpALifeDynamicObject);
	xrClientData* CL				= m_tpServer->ID_to_client(0);
	
	if (CL) {
		tNetPacket.w_begin			(M_UPDATE);
		tNetPacket.w_u16			(tpALifeDynamicObject->ID);
		tpALifeDynamicObject->UPDATE_Write(tNetPacket);
		m_tpServer->SendTo			(CL->ID,tNetPacket,net_flags(TRUE,TRUE));
		m_tpServer->SendBroadcast	(CL->ID,tNetPacket,net_flags(TRUE,TRUE));
	}
}

void CAI_ALife::vfReleaseObject(CALifeDynamicObject *tpALifeDynamicObject)
{
	NET_Packet						P;
	m_tpServer->game->u_EventGen	(P,GE_DESTROY,tpALifeDynamicObject->ID);
	m_tpServer->game->u_EventSend	(P);
}

void CAI_ALife::vfSwitchObjectOnline(CALifeDynamicObject *tpALifeDynamicObject)
{
	VERIFY							(!tpALifeDynamicObject->m_bOnline);
	vfCreateObject					(tpALifeDynamicObject);
	tpALifeDynamicObject->m_bOnline	= true;
	Msg								("- SERVER: Going online [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->g_team(), tpALifeDynamicObject->g_squad(), tpALifeDynamicObject->g_group(), tpALifeDynamicObject->ID, "*SERVER*");
}

void CAI_ALife::vfSwitchObjectOffline(CALifeDynamicObject *tpALifeDynamicObject)
{
	VERIFY							(tpALifeDynamicObject->m_bOnline);
	vfReleaseObject					(tpALifeDynamicObject);
	tpALifeDynamicObject->m_bOnline	= false;
	Msg								("- SERVER: Going offline [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Device.dwTimeGlobal,tpALifeDynamicObject->s_name_replace, tpALifeDynamicObject->g_team(), tpALifeDynamicObject->g_squad(), tpALifeDynamicObject->g_group(), tpALifeDynamicObject->ID, "*SERVER*");
}

void CAI_ALife::ProcessOnlineOfflineSwitches(CALifeDynamicObject *I)
{
	if (I->m_bOnline) {
		if (I->ID_Parent == 0xffff) {
			float fDistance = m_tpActor->o_Position.distance_to(I->o_Position);
			if (fDistance > m_fOnlineDistance) {
				Msg("%7.2f [%7.2f][%7.2f][%7.2f]",fDistance,I->o_Position.x,I->o_Position.y,I->o_Position.z);
				vfSwitchObjectOffline(I);
			}
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
			float fDistance = m_tpActor->o_Position.distance_to(I->o_Position);
			if (fDistance < m_fOnlineDistance) {
				Msg("%7.2f [%7.2f][%7.2f][%7.2f]",fDistance,I->o_Position.x,I->o_Position.y,I->o_Position.z);
				vfSwitchObjectOnline(I);
			}
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
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

void CAI_ALife::vfCreateObject(CALifeObject *tpALifeObject)
{
	NET_Packet						P;
	u16								ID;
	tpALifeObject->ID				= 0xffff;
	Level().Server->Process_spawn	(P,0,FALSE,tpALifeObject);
	
	P.w_begin						(M_UPDATE);
	tpALifeObject->UPDATE_Write		(P);
	P.r_begin						(ID);
	R_ASSERT						(M_UPDATE==ID);
	Level().Server->Process_update	(P,0);
	tpALifeObject->m_tObjectID		= tpALifeObject->ID;
}

void CAI_ALife::vfReleaseObject(CALifeObject *tpALifeObject)
{
	NET_Packet						P;
	CObject							*tpObject = Level().Objects.net_Find(tpALifeObject->ID);
	if (!tpObject)
		return;
	CGameObject						*tpGameObject = dynamic_cast<CGameObject*>(tpObject);
	R_ASSERT						(tpGameObject);
	tpGameObject->u_EventGen		(P,GE_DESTROY,tpGameObject->ID());
	tpGameObject->u_EventSend		(P);
}

void CAI_ALife::vfSwitchObjectOnline(CALifeObject *tpALifeObject)
{
	R_ASSERT						(!tpALifeObject->m_bOnline);
	CObject							*tpObject = Level().Objects.net_Find(tpALifeObject->ID);
	if (!tpObject) {
		vfCreateObject				(tpALifeObject);
		return;
	}
	tpObject->setVisible			(true);
	tpObject->setEnabled			(true);
	tpObject->setActive				(true);
	
	tpALifeObject->m_bOnline		= true;
	Msg								("- SERVER: Going online [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Level().timeServer(),tpALifeObject->s_name_replace, tpALifeObject->g_team(), tpALifeObject->g_squad(), tpALifeObject->g_group(), tpALifeObject->ID, "*SERVER*");
}

void CAI_ALife::vfSwitchObjectOffline(CALifeObject *tpALifeObject)
{
	R_ASSERT						(tpALifeObject->m_bOnline);
	CObject							*tpObject = Level().Objects.net_Find(tpALifeObject->ID);
	if (!tpObject)
		return;
	tpObject->setVisible			(false);
	tpObject->setEnabled			(false);
	tpObject->setActive				(false);
	
#ifdef DYNAMIC_ALLOCATION
	vfReleaseObject					(tpALifeObject);
#endif
	
	tpALifeObject->m_bOnline		= false;
	Msg								("- SERVER: Going offline [%d] '%s'(%d,%d,%d) as #%d, on '%s'",Level().timeServer(),tpALifeObject->s_name_replace, tpALifeObject->g_team(), tpALifeObject->g_squad(), tpALifeObject->g_group(), tpALifeObject->ID, "*SERVER*");
}

void CAI_ALife::vfUpdateOfflineObject(CALifeObject *tpALifeObject)
{
	NET_Packet						P;
	u16								ID;
	CObject							*tpObject = Level().Objects.net_Find(tpALifeObject->ID);
	if (!tpObject)
		return;
	P.w_begin						(M_UPDATE);
	tpObject->net_Export			(P);
	P.r_begin						(ID);
	tpALifeObject->UPDATE_Read		(P);
}

void CAI_ALife::vfUpdateOnlineObject(CALifeObject *tpALifeObject)
{
	NET_Packet						P;
	u16								ID;
	P.w_begin						(M_UPDATE);
	tpALifeObject->UPDATE_Write		(P);
	CObject							*tpObject = tpObject = Level().Objects.net_Find(tpALifeObject->ID);
	if (!tpObject)
		return;
	P.r_begin						(ID);
	tpObject->net_Import			(P);
}

void CAI_ALife::ProcessOnlineOfflineSwitches(CObject *tpObject, CALifeObject *I)
{
	float						fDistance;
	if (I->m_bOnline) {
		vfUpdateOfflineObject	(I);
		if (I->ID_Parent == 0xffff) {
			fDistance			= tpObject->Position().distance_to(I->o_Position);
			if (fDistance > m_fOnlineDistance)
				vfSwitchObjectOffline(I);
		}
		else {
			xrServerEntity		*tpServerEntity = Level().Server->ID_to_entity(I->ID_Parent);
			R_ASSERT			(tpServerEntity);
			CALifeObject		*tpALifeObject  = dynamic_cast<CALifeObject*>(tpServerEntity);
			R_ASSERT			(tpALifeObject);
			if (!tpALifeObject->m_bOnline)
				vfSwitchObjectOffline(I);
		}
	}
	else {
		vfUpdateOfflineObject	(I);
		if (I->ID_Parent == 0xffff) {
			fDistance			= tpObject->Position().distance_to(I->o_Position);
			if (fDistance <= m_fOnlineDistance) {
				vfUpdateOnlineObject(I);
				vfSwitchObjectOnline(I);
			}
		}
		else {
			xrServerEntity		*tpServerEntity = Level().Server->ID_to_entity(I->ID_Parent);
			R_ASSERT			(tpServerEntity);
			CALifeObject		*tpALifeObject  = dynamic_cast<CALifeObject*>(tpServerEntity);
			R_ASSERT			(tpALifeObject);
			if (tpALifeObject->m_bOnline) {
				vfUpdateOnlineObject(I);
				vfSwitchObjectOnline(I);
			}
		}
	}
}
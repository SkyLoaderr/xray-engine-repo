////////////////////////////////////////////////////////////////////////////
// script_game_object_inventory_owner.�pp :	������� ��� inventory owner
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_game_object.h"
#include "InventoryOwner.h"
#include "Pda.h"
#include "xrMessages.h"
#include "character_info.h"
#include "ai_phrasedialogmanager.h"

bool CScriptGameObject::GiveInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::IdToIndex(info_id), true);
	return			true;
}

bool CScriptGameObject::DisableInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::IdToIndex(info_id), false);
	return true;
}

bool CScriptGameObject::GiveInfoPortionViaPda(LPCSTR info_id, CScriptGameObject* pFromWho)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pFromWhoInvOwner = dynamic_cast<CInventoryOwner*>(pFromWho->m_tpGameObject);
	if(!pFromWhoInvOwner) return false;
	if(!pFromWhoInvOwner->GetPDA()) return false;

	//���������� �� ������ PDA ����� ���������� � �������
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_PDA,pInventoryOwner->GetPDA()->ID());
	P.w_u16			(u16(pFromWhoInvOwner->GetPDA()->ID()));		//�����������
	P.w_s16			(ePdaMsgInfo);									
	P.w_s32			(CInfoPortion::IdToIndex(info_id));
	m_tpGameObject->u_EventSend(P);
	return			true;
}


bool  CScriptGameObject::HasInfo				(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;
	return pInventoryOwner->HasInfo(CInfoPortion::IdToIndex(info_id));

}
bool  CScriptGameObject::DontHasInfo			(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return true;
	if(!pInventoryOwner->GetPDA()) return true;
	return !pInventoryOwner->HasInfo(CInfoPortion::IdToIndex(info_id));
}


bool CScriptGameObject::SendPdaMessage(EPdaMsg pda_msg, CScriptGameObject* pForWho)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pForWhoInvOwner = dynamic_cast<CInventoryOwner*>(pForWho->m_tpGameObject);
	if(!pForWhoInvOwner) return false;
	if(!pForWhoInvOwner->GetPDA()) return false;

	//���������� �� ������ PDA ����� ���������� � �������
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_PDA,pForWhoInvOwner->GetPDA()->ID());
	P.w_u16			(u16(pInventoryOwner->GetPDA()->ID()));		//�����������
	P.w_s16			((u16)pda_msg);
	P.w_s32			(-1);
	m_tpGameObject->u_EventSend(P);
	return			true;
}


bool CScriptGameObject::IsTalking()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	return			pInventoryOwner->IsTalking();
}

void CScriptGameObject::StopTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->StopTalk();
}
void CScriptGameObject::EnableTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->EnableTalk();
}
void CScriptGameObject::DisableTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->DisableTalk();
}

bool CScriptGameObject::IsTalkEnabled()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	return pInventoryOwner->IsTalkEnabled();
}


//�������� ���� �� ������ ��������� � ��������� ��������
void CScriptGameObject::TransferItem(CScriptGameObject* pItem, CScriptGameObject* pForWho)
{
	R_ASSERT(pItem);
	R_ASSERT(pForWho);

	CInventoryItem* pIItem = dynamic_cast<CInventoryItem*>(pItem->m_tpGameObject);
	VERIFY(pIItem);

	// ��������� � ���� 
	NET_Packet						P;
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_REJECT, m_tpGameObject->ID());
	P.w_u16							(pIItem->ID());
	CGameObject::u_EventSend		(P);

	// ������ ��������
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_TAKE, pForWho->m_tpGameObject->ID());
	P.w_u16							(pIItem->ID());
	CGameObject::u_EventSend		(P);
}

u32 CScriptGameObject::Money	()
{
	CInventoryOwner* pOurOwner		= dynamic_cast<CInventoryOwner*>(m_tpGameObject); VERIFY(pOurOwner);
	return pOurOwner->m_dwMoney;
}

void CScriptGameObject::TransferMoney(int money, CScriptGameObject* pForWho)
{
	R_ASSERT(pForWho);
	CInventoryOwner* pOurOwner		= dynamic_cast<CInventoryOwner*>(m_tpGameObject); VERIFY(pOurOwner);
	CInventoryOwner* pOtherOwner	= dynamic_cast<CInventoryOwner*>(pForWho->m_tpGameObject); VERIFY(pOtherOwner);

	R_ASSERT3(pOurOwner->m_dwMoney-money>=0, "Character does not have enought money", pOurOwner->CharacterInfo().Name());

	pOurOwner->m_dwMoney	-= money;
	pOtherOwner->m_dwMoney	+= money;
}


void CScriptGameObject::SetGoodwill(int goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().SetGoodwill(pWhoToSet->m_tpGameObject->ID(), goodwill);
}

void CScriptGameObject::SetRelation(ALife::ERelationType relation, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().SetRelationType(pWhoToSet->m_tpGameObject->ID(), relation);
}
void CScriptGameObject::SetStartDialog(LPCSTR dialog_id)
{
	CAI_PhraseDialogManager* pDialogManager = dynamic_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->SetStartDialog(dialog_id);
}

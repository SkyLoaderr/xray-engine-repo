/////////////////////////////////////////////////////
// ��� ����������, ������� ���������
// InventoryOwner.cpp
//////////////////////////////////////////////////////


#include "stdafx.h"
#include "InventoryOwner.h"
#include "entity_alive.h"
#include "pda.h"
#include "actor.h"
#include "trade.h"
#include "inventory.h"
#include "xrserver_objects_alife_items.h"
#include "character_info.h"
#include "script_game_object.h"
#include "script_engine.h"
#include "script_callback.h"
#include "AI_PhraseDialogManager.h"
#include "level.h"

//////////////////////////////////////////////////////////////////////////
// CInventoryOwner class 
//////////////////////////////////////////////////////////////////////////
CInventoryOwner::CInventoryOwner			()
{
	Init						();
	m_pPdaCallback				= xr_new<CScriptCallback>();
	m_pInfoCallback				= xr_new<CScriptCallback>();
}

CInventoryOwner::~CInventoryOwner			() 
{
	xr_delete					(m_inventory);
	xr_delete					(m_pTrade);
	xr_delete					(m_pCharacterInfo);
	xr_delete					(m_pPdaCallback);
	xr_delete					(m_pInfoCallback);
}

void CInventoryOwner::Init					()
{
	m_pCharacterInfo = NULL;
	m_pTrade = NULL;
	
	m_inventory					= xr_new<CInventory>();
	m_pCharacterInfo			= xr_new<CCharacterInfo>();
	
	EnableTalk();
}

void CInventoryOwner::Load					(LPCSTR section)
{
}

void CInventoryOwner::reload				(LPCSTR section)
{
	CAttachmentOwner::reload	(section);
}

void CInventoryOwner::reinit				()
{
	CAttachmentOwner::reinit	();

	inventory().Clear			();
	inventory().m_pOwner		= this;
	inventory().SetSlotsUseful (true);

	m_dwMoney					= 0;
	m_tRank						= ALife::eStalkerRankNone;
	m_bTalking					= false;
	m_pTalkPartner				= NULL;

	m_pPdaCallback->clear		();
	m_pInfoCallback->clear		();

#ifdef _DEBUG
	m_KnowInfoWithoutAlife.clear();
#endif	

}

BOOL CInventoryOwner::net_Spawn		(LPVOID DC)
{
	//�������� ��������� �� ������, InventoryOwner
	CGameObject			*pThis = dynamic_cast<CGameObject*>(this);
	if(!pThis) return FALSE;

	
	bool init_default_profile = true;
	if (pThis->spawn_ini() && pThis->spawn_ini()->section_exist("game_info"))
	{
		LPCSTR name_id = pThis->spawn_ini()->r_string("game_info", "name_id");
		init_default_profile = !CharacterInfo().Load(name_id);
		
		CAI_PhraseDialogManager* dialog_manager = dynamic_cast<CAI_PhraseDialogManager*>(this);
		if(dialog_manager && *CharacterInfo().m_sStartDialog != NULL)
			dialog_manager->SetStartDialog(*CharacterInfo().m_sStartDialog);

	}
	
	if(init_default_profile)
	{
		CharacterInfo().m_sGameName = pThis->cName();
		CEntity*		 pEntity = dynamic_cast<CEntity*>(pThis);
		VERIFY(pEntity);
		CharacterInfo().m_iIconX = pEntity->GetTradeIconX();
		CharacterInfo().m_iIconY = pEntity->GetTradeIconY();

		CharacterInfo().m_iMapIconX = pEntity->GetMapIconX();
		CharacterInfo().m_iMapIconY = pEntity->GetMapIconY();

	}


	
	if(!pThis->Local())  return TRUE;

#ifdef DEBUG
	CSE_Abstract			*E	= (CSE_Abstract*)(DC);
	CSE_ALifeDynamicObject	*dynamic_object = dynamic_cast<CSE_ALifeDynamicObject*>(E);
	VERIFY					(dynamic_object);
#endif

	return TRUE;
}

void CInventoryOwner::net_Destroy()
{
	CAttachmentOwner::net_Destroy();
	m_inventory->SetActiveSlot(NO_ACTIVE_SLOT);
	m_pPdaCallback->clear();
	m_pInfoCallback->clear();
}


void CInventoryOwner::UpdateInventoryOwner(u32 deltaT)
{
	inventory().Update();
	if(m_pTrade) m_pTrade->UpdateTrade();

	if(IsTalking())
	{
		//���� ��� ���������� �������� �������� � ����,
		//�� � ��� ������ �����.
		if(!m_pTalkPartner->IsTalking())
		{
			StopTalk();
		}

		//���� �� ������, �� ���� �� ��������
		CEntityAlive* pOurEntityAlive = dynamic_cast<CEntityAlive*>(this);
		R_ASSERT(pOurEntityAlive);
		if(!pOurEntityAlive->g_Alive()) StopTalk();
	}
}


//������� PDA �� ������������ ����� ���������
CPda* CInventoryOwner::GetPDA() const
{
//	CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(this);
	
//	if(!pEntityAlive || !pEntityAlive->g_Alive()) return NULL; 

	/*R_ASSERT2(inventory().m_slots[PDA_SLOT].m_pIItem, 
			"PDA for character does not init yet");*/
	
	return static_cast<CPda*>(inventory().m_slots[PDA_SLOT].m_pIItem);
}

bool CInventoryOwner::IsActivePDA() const
{
	if(GetPDA() && GetPDA()->IsActive())
		return true;
	else
		return false;
}

//����������� ������� ��������� ���������
//who - id PDA �� �������� ������ ���������
void CInventoryOwner::ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index)
{
	if(msg == ePdaMsgInfo)
	{
		//��������� ���� �� ���������� ����������
		TransferInfo(info_index, true);
	}


	//��������� ���������� callback
	CGameObject* pThisGameObject = dynamic_cast<CGameObject*>(this);
	VERIFY(pThisGameObject);
	CPda* pWhoPda = dynamic_cast<CPda*>(Level().Objects.net_Find(who));
	VERIFY(pWhoPda);
	CGameObject* pWho = dynamic_cast<CGameObject*>(pWhoPda->H_Parent());


	/*Msg("who pda %d, who parent %d, we pda %d, we parent %d", who, 
		pWho->ID(),
		GetPDA()->ID(),
		GetPDA()->GetOriginalOwnerID());*/


 	SCRIPT_CALLBACK_EXECUTE_4(*m_pPdaCallback, 
							  pThisGameObject->lua_game_object(),
							  pWho->lua_game_object(), 
							  (int)msg,
							  info_index);
}



//who - id PDA �������� ���������� ���������
void CInventoryOwner::SendPdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index)
{
	if(!GetPDA() || !GetPDA()->IsActive()) return;

	GetPDA()->SendMessageID(who, msg, info_index);
}

void CInventoryOwner::InitTrade()
{
	m_pTrade = xr_new<CTrade>(this);
}
CTrade* CInventoryOwner::GetTrade() 
{
	R_ASSERT2(m_pTrade, "trade for object does not init yet");
	return m_pTrade;
}


//��������� �������

//��� ���������� ����������,
//��������� ���� ��������� 
//� ���� �� ���� �������� ��������
bool CInventoryOwner::OfferTalk(CInventoryOwner* talk_partner)
{
	if(!IsTalkEnabled()) return false;

	//��������� ��������� � �����������
	CEntityAlive* pOurEntityAlive = dynamic_cast<CEntityAlive*>(this);
	R_ASSERT(pOurEntityAlive);

	CEntityAlive* pPartnerEntityAlive = dynamic_cast<CEntityAlive*>(talk_partner);
	R_ASSERT(pPartnerEntityAlive);
	
	ALife::ERelationType relation = pOurEntityAlive->tfGetRelationType(pPartnerEntityAlive);

	if(relation == ALife::eRelationTypeEnemy) return false;

	if(!pOurEntityAlive->g_Alive() || !pPartnerEntityAlive->g_Alive()) return false;

	StartTalk(talk_partner);

	return true;
}


void CInventoryOwner::StartTalk(CInventoryOwner* talk_partner)
{
	m_bTalking = true;
	m_pTalkPartner = talk_partner;

	//��� �� �������� ��������
	GetTrade()->StartTrade(talk_partner);
}

void CInventoryOwner::StopTalk()
{
	m_pTalkPartner = NULL;
	m_bTalking = false;

	//��������� ��������
	GetTrade()->StopTrade();
}
bool CInventoryOwner::IsTalking()
{
	return m_bTalking;
}

void CInventoryOwner::renderable_Render		()
{
	if (inventory().ActiveItem())
		inventory().ActiveItem()->renderable_Render();

	CAttachmentOwner::renderable_Render();
}

void CInventoryOwner::OnItemTake			(CInventoryItem *inventory_item)
{
	attach		(inventory_item);
}

void CInventoryOwner::OnItemDrop			(CInventoryItem *inventory_item)
{
	detach		(inventory_item);
}

//���������� ������� ������� �������� � ������ �������� (� ��������)
float CInventoryOwner::GetWeaponAccuracy	() const
{
	return 0.f;
}

//������������ ���������� ���
float  CInventoryOwner::MaxCarryWeight () const
{
	return inventory().GetMaxWeight();
}

void CInventoryOwner::spawn_supplies		()
{
	CGameObject								*game_object = dynamic_cast<CGameObject*>(this);
	VERIFY									(game_object);

	if (use_bolts())
		Level().spawn_item					("bolt",game_object->Position(),game_object->level_vertex_id(),game_object->ID());

	if (!ai().get_alife()) {
		CSE_Abstract						*abstract = Level().spawn_item("device_pda",game_object->Position(),game_object->level_vertex_id(),game_object->ID(),true);
		CSE_ALifeItemPDA					*pda = dynamic_cast<CSE_ALifeItemPDA*>(abstract);
		R_ASSERT							(pda);
		pda->m_original_owner				= (u16)game_object->ID();
		NET_Packet							P;
		abstract->Spawn_Write				(P,TRUE);
		Level().Send						(P,net_flags(TRUE));
		F_entity_Destroy					(abstract);
	}
}
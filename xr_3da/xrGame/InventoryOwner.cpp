/////////////////////////////////////////////////////
// Для персонажей, имеющих инвентарь
// InventoryOwner.cpp
//////////////////////////////////////////////////////


#include "stdafx.h"
#include "InventoryOwner.h"
#include "gameobject.h"
#include "entity_alive.h"
#include "pda.h"
#include "actor.h"
#include "trade.h"
#include "inventory.h"
#include "xrserver_objects_alife_items.h"
#include "character_info.h"

//////////////////////////////////////////////////////////////////////////
// CInventoryOwner class 
//////////////////////////////////////////////////////////////////////////
CInventoryOwner::CInventoryOwner			()
{
	Init						();
}

CInventoryOwner::~CInventoryOwner			() 
{
	xr_delete					(m_inventory);
	xr_delete					(m_trade_storage);
	xr_delete					(m_pTrade);
	xr_delete					(m_pCharacterInfo);
}

void CInventoryOwner::Init					()
{
	m_pCharacterInfo = NULL;
	m_pTrade = NULL;
	
	m_inventory					= xr_new<CInventory>();
	m_trade_storage				= xr_new<CInventory>();
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
	m_trade_storage->m_pOwner	= this;

	m_dwMoney					= 0;
	m_tRank						= ALife::eStalkerRankNone;
	m_bTalking					= false;
	m_pTalkPartner				= NULL;

	m_pPdaCallback.clear		();
	m_pInfoCallback.clear		();
}

BOOL CInventoryOwner::net_Spawn		(LPVOID DC)
{
	//получить указатель на объект, InventoryOwner
	CGameObject			*pThis = dynamic_cast<CGameObject*>(this);
	if(!pThis) return FALSE;

	
	bool init_default_profile = true;
	if (pThis->spawn_ini() && pThis->spawn_ini()->section_exist("game_info"))
	{
		LPCSTR name_id = pThis->spawn_ini()->r_string("game_info", "name_id");
		init_default_profile = !CharacterInfo().Load(name_id);
	}
	
	if(init_default_profile)
	{
		CharacterInfo().m_sGameName = pThis->cName();
		CEntity* pEntity = dynamic_cast<CEntity*>(pThis);
		VERIFY(pEntity);
		CharacterInfo().m_iIconX = pEntity->GetTradeIconX();
		CharacterInfo().m_iIconY = pEntity->GetTradeIconY();
	}


	
	if(!pThis->Local())  return TRUE;
    
	CSE_Abstract			*E	= (CSE_Abstract*)(DC);
	CSE_ALifeDynamicObject	*dynamic_object = dynamic_cast<CSE_ALifeDynamicObject*>(E);
	VERIFY					(dynamic_object);

	return TRUE;
}

void CInventoryOwner::net_Destroy()
{
	CAttachmentOwner::net_Destroy();
	m_inventory->SetActiveSlot(NO_ACTIVE_SLOT);
}

void CInventoryOwner::UpdateInventoryOwner(u32 deltaT)
{
	inventory().Update();
	if(m_pTrade) m_pTrade->UpdateTrade();

	if(IsTalking())
	{
		//если наш собеседник перестал говорить с нами,
		//то и нам нечего ждать.
		if(!m_pTalkPartner->IsTalking())
		{
			StopTalk();
		}

		//если мы умерли, то тоже не говорить
		CEntityAlive* pOurEntityAlive = dynamic_cast<CEntityAlive*>(this);
		R_ASSERT(pOurEntityAlive);
		if(!pOurEntityAlive->g_Alive()) StopTalk();
	}
}


//достать PDA из специального слота инвентаря
CPda* CInventoryOwner::GetPDA()
{
//	CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(this);
	
//	if(!pEntityAlive || !pEntityAlive->g_Alive()) return NULL; 

	/*R_ASSERT2(inventory().m_slots[PDA_SLOT].m_pIItem, 
			"PDA for character does not init yet");*/
	
	return (CPda*)inventory().m_slots[PDA_SLOT].m_pIItem;
}

bool CInventoryOwner::IsActivePDA()
{
	if(GetPDA() && GetPDA()->IsActive())
		return true;
	else
		return false;
}

//виртуальная функция обработки сообщений
//who - id PDA от которого пришло сообщение
void CInventoryOwner::ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index)
{
	if(msg == ePdaMsgInfo)
	{
		//переслать себе же полученную информацию
		GetPDA()->TransferInfoToID(GetPDA()->ID(), info_index);
	}



	//Запустить скриптовый callback
	CGameObject* pThisGameObject = dynamic_cast<CGameObject*>(this);
	VERIFY(pThisGameObject);
	CPda* pWhoPda = dynamic_cast<CPda*>(Level().Objects.net_Find(who));
	VERIFY(pWhoPda);
	CGameObject* pWho = dynamic_cast<CGameObject*>(pWhoPda->H_Parent());


	/*Msg("who pda %d, who parent %d, we pda %d, we parent %d", who, 
		pWho->ID(),
		GetPDA()->ID(),
		GetPDA()->GetOriginalOwnerID());*/


 	SCRIPT_CALLBACK_EXECUTE_4(m_pPdaCallback, 
							  pThisGameObject->lua_game_object(),
							  pWho->lua_game_object(), 
							  (int)msg,
							  info_index);
}


void CInventoryOwner::OnReceiveInfo(int info_index)
{
	//Запустить скриптовый callback
	CGameObject* pThisGameObject = dynamic_cast<CGameObject*>(this);
	VERIFY(pThisGameObject);

	SCRIPT_CALLBACK_EXECUTE_2(m_pInfoCallback, pThisGameObject->lua_game_object(), info_index);
}


//who - id PDA которому отправляем сообщение
void CInventoryOwner::SendPdaMessage(u16 who, EPdaMsg msg, int info_index)
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


//состояние диалога

//нам предлагают поговорить,
//проверяем наше отношение 
//и если не враг начинаем разговор
bool CInventoryOwner::OfferTalk(CInventoryOwner* talk_partner)
{
	if(!IsTalkEnabled()) return false;

	//проверить отношение к собеседнику
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

	//тут же включаем торговлю
	GetTrade()->StartTrade(talk_partner);
}

void CInventoryOwner::StopTalk()
{
	m_pTalkPartner = NULL;
	m_bTalking = false;

	//выключаем торговлю
	GetTrade()->StopTrade();
}
bool CInventoryOwner::IsTalking()
{
	return m_bTalking;
}

bool CInventoryOwner::AskQuestion(SInfoQuestion& question, INFO_INDEX_LIST& index_list)
{
	R_ASSERT2(GetPDA(), "PDA for character does not init yet");

	bool result_answer = false;

	index_list.clear();

	INFO_INDEX_LIST_it it;
	for(it = question.IndexList.begin();
		question.IndexList.end() != it;
		++it)
	{
		int info_index = *it;
		if(GetPDA()->IsKnowAbout(info_index))
		{
			//переслать информацию PDA 
			u32 partner_pda_id = m_pTalkPartner->GetPDA()->ID();
			GetPDA()->TransferInfoToID(partner_pda_id, info_index);

			index_list.push_back(info_index);

			result_answer = true;
		}
	}

	return result_answer;
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

//возвращает текуший разброс стрельбы с учетом движения (в радианах)
float CInventoryOwner::GetWeaponAccuracy	() const
{
	return 0.f;
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

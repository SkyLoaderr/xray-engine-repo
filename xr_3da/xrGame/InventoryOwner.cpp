/////////////////////////////////////////////////////
// Для персонажей, имеющих инвентарь
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
#include "script_space.h"
#include "PhraseDialog.h"

#include "xrServer_Objects_ALife_Monsters.h"


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
	inventory().Clear			();
	inventory().m_pOwner		= this;
	inventory().SetSlotsUseful (true);

	m_dwMoney					= 0;
	m_tRank						= ALife::eStalkerRankNone;
	m_bTalking					= false;
	m_pTalkPartner				= NULL;

	m_pPdaCallback->clear		();
	m_pInfoCallback->clear		();


	CAttachmentOwner::reload	(section);
}

void CInventoryOwner::reinit				()
{
	CAttachmentOwner::reinit	();
}

//call this after CGameObject::net_Spawn
BOOL CInventoryOwner::net_Spawn		(LPVOID DC)
{
	//получить указатель на объект, InventoryOwner
	CGameObject			*pThis = smart_cast<CGameObject*>(this);
	if(!pThis) return FALSE;

	//-------------------------------------
	known_info_registry.init(pThis->ID());
	CharacterInfo().relation_registry.init(pThis->ID());
	//-------------------------------------



	CSE_Abstract* E	= (CSE_Abstract*)(DC);
	CSE_ALifeTraderAbstract* pTrader = NULL;
	if(E) pTrader = smart_cast<CSE_ALifeTraderAbstract*>(E);
	if(!pTrader) return FALSE;

	CharacterInfo().m_iSpecificCharacterIndex = pTrader->specific_character(); 
	
	if(NO_PROFILE != pTrader->character_profile())
	{
		CharacterInfo().Load(pTrader->character_profile());

		CAI_PhraseDialogManager* dialog_manager = smart_cast<CAI_PhraseDialogManager*>(this);
		if(dialog_manager && CharacterInfo().StartDialog() != NO_PHRASE_DIALOG)
		{
			dialog_manager->SetStartDialog(CPhraseDialog::IndexToId(CharacterInfo().StartDialog()));
			dialog_manager->SetDefaultStartDialog(CPhraseDialog::IndexToId(CharacterInfo().StartDialog()));
		}
	}
	else
	{
		pTrader->set_character_profile(DEFAULT_PROFILE);
		CharacterInfo().Load(DEFAULT_PROFILE);
		/*CharacterInfo().data()->m_sGameName = pThis->cName();
		CEntity* pEntity = smart_cast<CEntity*>(pThis); VERIFY(pEntity);
		CharacterInfo().data()->m_iIconX = pEntity->GetTradeIconX();
		CharacterInfo().data()->m_iIconY = pEntity->GetTradeIconY();

		CharacterInfo().data()->m_iMapIconX = pEntity->GetMapIconX();
		CharacterInfo().data()->m_iMapIconY = pEntity->GetMapIconY();*/
	}
	pTrader->set_specific_character(CharacterInfo().m_iSpecificCharacterIndex); 
	
	if(!pThis->Local())  return TRUE;

#ifdef DEBUG
	//CSE_Abstract			*E	= (CSE_Abstract*)(DC);
	CSE_ALifeDynamicObject	*dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(E);
	VERIFY					(dynamic_object);
#endif

	return TRUE;
}

void CInventoryOwner::net_Destroy()
{
	CAttachmentOwner::net_Destroy();
	
	inventory().Clear();
	inventory().SetActiveSlot(NO_ACTIVE_SLOT);
	m_pPdaCallback->clear();
	m_pInfoCallback->clear();
}


void	CInventoryOwner::save	(NET_Packet &output_packet)
{
	if(inventory().GetActiveSlot() == NO_ACTIVE_SLOT)
		output_packet.w_u8((u8)(-1));
	else
		output_packet.w_u8((u8)inventory().GetActiveSlot());

	CharacterInfo().save(output_packet);
}
void	CInventoryOwner::load	(IReader &input_packet)
{
	u8 active_slot = input_packet.r_u8();
	if(active_slot == u8(-1))
		inventory().SetActiveSlot(NO_ACTIVE_SLOT);
	else
		inventory().SetActiveSlot(active_slot);

	CharacterInfo().load(input_packet);
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
		CEntityAlive* pOurEntityAlive = smart_cast<CEntityAlive*>(this);
		R_ASSERT(pOurEntityAlive);
		if(!pOurEntityAlive->g_Alive()) StopTalk();
	}
}


//достать PDA из специального слота инвентаря
CPda* CInventoryOwner::GetPDA() const
{
//	CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(this);
	
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

//виртуальная функция обработки сообщений
//who - id PDA от которого пришло сообщение
void CInventoryOwner::ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_INDEX info_index)
{
	if(msg == ePdaMsgInfo)
	{
		//переслать себе же полученную информацию
		TransferInfo(info_index, true);
	}


	//Запустить скриптовый callback
	CGameObject* pThisGameObject = smart_cast<CGameObject*>(this);
	VERIFY(pThisGameObject);
	CPda* pWhoPda = smart_cast<CPda*>(Level().Objects.net_Find(who));
	VERIFY(pWhoPda);
	CGameObject* pWho = smart_cast<CGameObject*>(pWhoPda->H_Parent());


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



//who - id PDA которому отправляем сообщение
void CInventoryOwner::SendPdaMessage(u16 who, EPdaMsg msg, INFO_INDEX info_index)
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
	CEntityAlive* pOurEntityAlive = smart_cast<CEntityAlive*>(this);
	R_ASSERT(pOurEntityAlive);

	CEntityAlive* pPartnerEntityAlive = smart_cast<CEntityAlive*>(talk_partner);
	R_ASSERT(pPartnerEntityAlive);
	
	ALife::ERelationType relation = pOurEntityAlive->tfGetRelationType(pPartnerEntityAlive);

	if(relation == ALife::eRelationTypeEnemy) return false;

	if(!pOurEntityAlive->g_Alive() || !pPartnerEntityAlive->g_Alive()) return false;

	StartTalk(talk_partner);

	return true;
}


void CInventoryOwner::StartTalk(CInventoryOwner* talk_partner, bool start_trade)
{
	m_bTalking = true;
	m_pTalkPartner = talk_partner;

	//тут же включаем торговлю
	if(start_trade)
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

//максимальный переносимы вес
float  CInventoryOwner::MaxCarryWeight () const
{
	return inventory().GetMaxWeight();
}

void CInventoryOwner::spawn_supplies		()
{
	CGameObject								*game_object = smart_cast<CGameObject*>(this);
	VERIFY									(game_object);

	if (use_bolts())
		Level().spawn_item					("bolt",game_object->Position(),game_object->level_vertex_id(),game_object->ID());

	if (!ai().get_alife()) {
		CSE_Abstract						*abstract = Level().spawn_item("device_pda",game_object->Position(),game_object->level_vertex_id(),game_object->ID(),true);
		CSE_ALifeItemPDA					*pda = smart_cast<CSE_ALifeItemPDA*>(abstract);
		R_ASSERT							(pda);
		pda->m_original_owner				= (u16)game_object->ID();
		NET_Packet							P;
		abstract->Spawn_Write				(P,TRUE);
		Level().Send						(P,net_flags(TRUE));
		F_entity_Destroy					(abstract);
	}
}

//игровое имя 
LPCSTR	CInventoryOwner::Name () const
{
	return CharacterInfo().Name();
}



//для детекторов
void CInventoryOwner::FoundZone	(CCustomZone*)
{
}
void CInventoryOwner::LostZone	(CCustomZone*)
{
}


void CInventoryOwner::NewPdaContact		(CInventoryOwner* pInvOwner)
{
}
void CInventoryOwner::LostPdaContact	(CInventoryOwner* pInvOwner)
{
}
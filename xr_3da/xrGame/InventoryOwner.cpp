/////////////////////////////////////////////////////
// Для персонажей, имеющих инвентарь
// InventoryOwner.cpp
//////////////////////////////////////////////////////


#include "stdafx.h"
#include "InventoryOwner.h"
#include "gameobject.h"
#include "entity.h"
#include "pda.h"
#include "actor.h"

#include "trade.h"


//////////////////////////////////////////////////////////////////////////
// CInventoryOwner class 
//////////////////////////////////////////////////////////////////////////
CInventoryOwner::CInventoryOwner()
 {
	m_inventory.m_pOwner		= this;
	m_trade_storage.m_pOwner	= this;
	m_pTrade					= NULL;

	m_dwMoney					= 0;
	m_tRank						= eStalkerRankNone;

	m_bTalking					= false;
	m_pTalkPartner				= NULL;
}
CInventoryOwner::~CInventoryOwner() 
{
	if(m_pTrade) xr_delete(m_pTrade);
}

BOOL CInventoryOwner::net_Spawn		(LPVOID DC)
{
	//получить указатель на объект, InventoryOwner
	CGameObject* pThis = dynamic_cast<CGameObject*>(this);
	if(!pThis) return FALSE;

	
	if(!pThis->Local())  return TRUE;
    
	//////////////////////////////////////////////
	//проспавнить PDA каждому inventory owner
	//////////////////////////////////////////////
	CSE_Abstract *E	= (CSE_Abstract*)(DC);

	// Create
	CSE_Abstract* D = F_entity_Create("device_pda");
	//CSE_Abstract* D = F_entity_Create("detector_simple");
	R_ASSERT(D);
	CSE_ALifeDynamicObject *l_tpALifeDynamicObject = 
							dynamic_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT(l_tpALifeDynamicObject);
	
		
	// Fill
	strcpy				(D->s_name,"device_pda");
	strcpy				(D->s_name_replace,"");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	E->ID;
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	pThis->Position();
	D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);


	return TRUE;
}

/*
	GetPDA()->OnReceiveInfo(1);
	GetPDA()->OnReceiveInfo(2);
	GetPDA()->OnReceiveInfo(3);
	GetPDA()->OnReceiveInfo(4);
*/

void CInventoryOwner::UpdateInventoryOwner(u32 deltaT)
{
	
	if(dynamic_cast<CActor*>(this))
	{
		if(m_inventory.m_slots[PDA_SLOT].m_pIItem)
			GetPDA()->OnReceiveInfo(1);
	}
	else
	{
		if(m_inventory.m_slots[PDA_SLOT].m_pIItem)
		{

		GetPDA()->OnReceiveInfo(1);
		GetPDA()->OnReceiveInfo(2);
		GetPDA()->OnReceiveInfo(3);
		GetPDA()->OnReceiveInfo(4);
		}
	}

	
	
	
	m_inventory.Update(deltaT);
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

	R_ASSERT2(m_inventory.m_slots[PDA_SLOT].m_pIItem, 
			"PDA for character does not init yet");
	
	return (CPda*)m_inventory.m_slots[PDA_SLOT].m_pIItem;
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
void CInventoryOwner::ReceivePdaMessage(u16 who, EPdaMsg msg, EPdaMsgAnger anger)
{
}

//who - id PDA которому отправляем сообщение
void CInventoryOwner::SendPdaMessage(u16 who, EPdaMsg msg, EPdaMsgAnger anger)
{
	if(!GetPDA() || !GetPDA()->IsActive()) return;

	GetPDA()->SendMessageID(who, msg, anger);
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
	//проверить отношение к собеседнику
	CEntityAlive* pOurEntityAlive = dynamic_cast<CEntityAlive*>(this);
	R_ASSERT(pOurEntityAlive);

	CEntityAlive* pPartnerEntityAlive = dynamic_cast<CEntityAlive*>(talk_partner);
	R_ASSERT(pPartnerEntityAlive);
	
	ALife::ERelationType relation = pOurEntityAlive->tfGetRelationType(pPartnerEntityAlive);

	if(relation == eRelationTypeEnemy) return false;

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
	bool result_answer = false;

	index_list.clear();

	INFO_INDEX_LIST_it it;
	for(it = question.IndexList.begin();
		it != question.IndexList.end();
		it++)
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
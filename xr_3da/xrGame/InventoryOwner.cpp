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


//////////////////////////////////////////////////////////////////////////
// CInventoryOwner class 
//////////////////////////////////////////////////////////////////////////
CInventoryOwner::CInventoryOwner			()
{
	Init						();
}
CInventoryOwner::~CInventoryOwner			() 
{
	if (m_pTrade)
		xr_delete				(m_pTrade);
}

void CInventoryOwner::Init					()
{
	m_torch_angle_offset		= Fvector().set(0,0,0);
	m_torch_position_offset		= Fvector().set(0,0,0);
	m_torch_bone_name			= "";
}

void CInventoryOwner::Load					(LPCSTR section)
{
	if (use_torch()) {
		m_torch_angle_offset	= pSettings->r_fvector3	(section,"torch_angle_offset");
		m_torch_position_offset	= pSettings->r_fvector3	(section,"torch_position_offset");
		m_torch_bone_name		= pSettings->r_string	(section,"torch_bone_name");
	}
}

void CInventoryOwner::reinit				()
{
	m_inventory.m_pOwner		= this;
	m_trade_storage.m_pOwner	= this;
	m_pTrade					= NULL;

	m_dwMoney					= 0;
	m_tRank						= eStalkerRankNone;

	m_bTalking					= false;
	m_pTalkPartner				= NULL;
	m_inventory.Clear			();
}

void CInventoryOwner::reload				(LPCSTR section)
{
}

void __stdcall VisualCallback(CKinematics *tpKinematics);

BOOL CInventoryOwner::net_Spawn		(LPVOID DC)
{
	//получить указатель на объект, InventoryOwner
	CGameObject			*pThis = dynamic_cast<CGameObject*>(this);
	if(!pThis) return FALSE;

	
	if(!pThis->Local())  return TRUE;
    
	if (use_torch()) {
		VERIFY			(pThis->Visual());
		PKinematics		(pThis->Visual())->Callback(VisualCallback,this);
	}
	//////////////////////////////////////////////
	//проспавнить PDA каждому inventory owner
	//////////////////////////////////////////////
	if(ai().get_alife())
		return			TRUE;

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


void CInventoryOwner::UpdateInventoryOwner(u32 deltaT)
{

	
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

	/*R_ASSERT2(m_inventory.m_slots[PDA_SLOT].m_pIItem, 
			"PDA for character does not init yet");*/
	
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
void CInventoryOwner::ReceivePdaMessage(u16 who, EPdaMsg msg, int info_index)
{
	if(msg == ePdaMsgInfo)
	{
		//переслать себе же полученную информацию
		GetPDA()->TransferInfoToID(GetPDA()->ID(), info_index);
	}
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

LPCSTR CInventoryOwner::GetGameName()
{
	CObject* pObject = dynamic_cast<CObject*>(this);
	
	
	if(pObject)
		return pObject->cName();
	else
		return NULL;
}
LPCSTR CInventoryOwner::GetGameRank()
{
	return "Char Rank";
}
LPCSTR CInventoryOwner::GetGameCommunity()
{
	return "Char Community";
}

void __stdcall VisualCallback(CKinematics *tpKinematics)
{
	CInventoryOwner		*inventory_owner = dynamic_cast<CInventoryOwner*>(static_cast<CObject*>(tpKinematics->Update_Callback_Param));
	VERIFY				(inventory_owner);

	CInventoryItem		*torch = inventory_owner->m_inventory.Get(CLSID_DEVICE_TORCH,false);
	if (!torch) {
		torch			= inventory_owner->m_inventory.Get(CLSID_DEVICE_TORCH,true);
		if (!torch)
			return;
	}

	CGameObject			*game_object = dynamic_cast<CGameObject*>(inventory_owner);
	if (!game_object)
		return;

	Fmatrix				matrix;

	matrix.setHPB		(VPUSH(inventory_owner->torch_angle_offset()));
	matrix.c			= inventory_owner->torch_position_offset();

	if (xr_strlen(inventory_owner->torch_bone_name())) {
		CBoneInstance	&l_tBoneInstance = PKinematics(game_object->Visual())->LL_GetBoneInstance(PKinematics(game_object->Visual())->LL_BoneID(inventory_owner->torch_bone_name()));
		matrix.mulA		(l_tBoneInstance.mTransform);
		matrix.mulA		(game_object->XFORM());
	}

	torch->XFORM()		= matrix;
}

void CInventoryOwner::renderable_Render		()
{
	if (m_inventory.ActiveItem())
		m_inventory.ActiveItem()->renderable_Render();

	CInventoryItem		*torch = m_inventory.Get(CLSID_DEVICE_TORCH,false);
	if (!torch)
		torch			= m_inventory.Get(CLSID_DEVICE_TORCH,true);

	if (torch)
		torch->renderable_Render();
}

bool CInventoryOwner::use_torch				() const
{
	return				(true);
}

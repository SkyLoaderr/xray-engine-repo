/////////////////////////////////////////////////////
// Для персонажей, имеющих инвентарь
// InventoryOwner.cpp
//////////////////////////////////////////////////////


#include "stdafx.h"
#include "InventoryOwner.h"
#include "gameobject.h"
#include "entity.h"
#include "pda.h"


//////////////////////////////////////////////////////////////////////////
// CInventoryOwner class 
//////////////////////////////////////////////////////////////////////////
CInventoryOwner::CInventoryOwner()
 {
	m_inventory.m_pOwner		= this;
	m_trade_storage.m_pOwner	= this;
	m_trade						= NULL;

	m_dwMoney					= 0;
	m_tRank						= eStalkerRankNone;
}

CInventoryOwner::~CInventoryOwner() 
{
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

//достать PDA из специального слота инвентаря
CPda* CInventoryOwner::GetPDA()
{
//	CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(this);
	
//	if(!pEntityAlive || !pEntityAlive->g_Alive()) return NULL; 
	
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
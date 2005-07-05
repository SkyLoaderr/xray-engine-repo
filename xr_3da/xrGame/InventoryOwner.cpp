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
#include "AI_PhraseDialogManager.h"
#include "level.h"
#include "game_base_space.h"
#include "script_space.h"
#include "PhraseDialog.h"
#include "xrserver.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_registry_wrappers.h"
#include "relation_registry.h"
#include "ai_object_location.h"
#include "script_callback_ex.h"
#include "game_object_space.h"

//////////////////////////////////////////////////////////////////////////
// CInventoryOwner class 
//////////////////////////////////////////////////////////////////////////
CInventoryOwner::CInventoryOwner			()
{
	m_pCharacterInfo			= NULL;
	m_pTrade					= NULL;
	
	m_inventory					= xr_new<CInventory>();
	m_pCharacterInfo			= xr_new<CCharacterInfo>();
	
	EnableTalk();
	EnableTrade();
	
	m_known_info_registry		= xr_new<CInfoPortionWrapper>();
}

DLL_Pure *CInventoryOwner::_construct		()
{
	m_pTrade					= xr_new<CTrade>(this);
	return						(0);
}

CInventoryOwner::~CInventoryOwner			() 
{
	xr_delete					(m_inventory);
	xr_delete					(m_pTrade);
	xr_delete					(m_pCharacterInfo);
	xr_delete					(m_known_info_registry);
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
	m_bTalking					= false;
	m_pTalkPartner				= NULL;

	CAttachmentOwner::reload	(section);
}

void CInventoryOwner::reinit				()
{
	CAttachmentOwner::reinit	();
	m_item_to_spawn				= shared_str();
	m_ammo_in_box_to_spawn		= 0;
}

//call this after CGameObject::net_Spawn
BOOL CInventoryOwner::net_Spawn		(CSE_Abstract* DC)
{
	//�������� ��������� �� ������, InventoryOwner
	m_inventory->setSlotsBlocked(false);
	CGameObject			*pThis = smart_cast<CGameObject*>(this);
	if(!pThis) return FALSE;
	CSE_Abstract* E	= (CSE_Abstract*)(DC);
	if (GameID() == GAME_SINGLE)
	{
		CSE_ALifeTraderAbstract* pTrader = NULL;
		if(E) pTrader = smart_cast<CSE_ALifeTraderAbstract*>(E);
		if(!pTrader) return FALSE;

//		R_ASSERT(NO_PROFILE != pTrader->character_profile());
		R_ASSERT( pTrader->character_profile().size() );

		//�������������� ��������� ��������� � ��������� ��������
		CharacterInfo().m_CurrentCommunity.set(pTrader->m_community_index);
		CharacterInfo().m_CurrentRank.set(pTrader->m_rank);
		CharacterInfo().m_CurrentReputation.set(pTrader->m_reputation);


		CharacterInfo().Load(pTrader->character_profile());
		CharacterInfo().InitSpecificCharacter (pTrader->specific_character());

		//-------------------------------------
		m_known_info_registry->registry().init(E->ID);
		//-------------------------------------


		CAI_PhraseDialogManager* dialog_manager = smart_cast<CAI_PhraseDialogManager*>(this);
		if( dialog_manager && !dialog_manager->GetStartDialog().size() )
		{
//			PHRASE_DIALOG_ID dialog_id = CPhraseDialog::IndexToId(CharacterInfo().StartDialog());
			dialog_manager->SetStartDialog(CharacterInfo().StartDialog());
			dialog_manager->SetDefaultStartDialog(CharacterInfo().StartDialog());
		}
	}
	else
	{
		CharacterInfo().m_SpecificCharacter.Load(DEFAULT_PROFILE);
		CharacterInfo().InitSpecificCharacter (DEFAULT_PROFILE);
		CharacterInfo().m_SpecificCharacter.data()->m_sGameName = (E->name_replace()[0]) ? E->name_replace() : *pThis->cName();
		CEntity* pEntity = smart_cast<CEntity*>(pThis); VERIFY(pEntity);
		CharacterInfo().m_SpecificCharacter.data()->m_iIconX = pEntity->GetTradeIconX();
		CharacterInfo().m_SpecificCharacter.data()->m_iIconY = pEntity->GetTradeIconY();

//		CharacterInfo().m_SpecificCharacter.data()->m_iMapIconX = pEntity->GetMapIconX();
//		CharacterInfo().m_SpecificCharacter.data()->m_iMapIconY = pEntity->GetMapIconY();
	}
	
	if(!pThis->Local())  return TRUE;


	return TRUE;
}

void CInventoryOwner::net_Destroy()
{
	CAttachmentOwner::net_Destroy();
	
	inventory().Clear();
	inventory().SetActiveSlot(NO_ACTIVE_SLOT);
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
		//���� ��� ���������� �������� �������� � ����,
		//�� � ��� ������ �����.
		if(!m_pTalkPartner->IsTalking())
		{
			StopTalk();
		}

		//���� �� ������, �� ���� �� ��������
		CEntityAlive* pOurEntityAlive = smart_cast<CEntityAlive*>(this);
		R_ASSERT(pOurEntityAlive);
		if(!pOurEntityAlive->g_Alive()) StopTalk();
	}
}


//������� PDA �� ������������ ����� ���������
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

//����������� ������� ��������� ���������
//who - id PDA �� �������� ������ ���������
void CInventoryOwner::ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_ID info_id)
{
	if(msg == ePdaMsgInfo)
	{
		//��������� ���� �� ���������� ����������
		TransferInfo(info_id, true);
	}


	//��������� ���������� callback
	CGameObject* pThisGameObject = cast_game_object();
	VERIFY(pThisGameObject);
	CPda* pWhoPda = smart_cast<CPda*>(Level().Objects.net_Find(who));
	VERIFY(pWhoPda);
	CGameObject* pWho = smart_cast<CGameObject*>(pWhoPda->H_Parent());


	/*Msg("who pda %d, who parent %d, we pda %d, we parent %d", who, 
		pWho->ID(),
		GetPDA()->ID(),
		GetPDA()->GetOriginalOwnerID());*/

	pThisGameObject->callback(GameObject::eInventoryPda)(
		pThisGameObject->lua_game_object(),
		pWho->lua_game_object(), 
		(int)msg,
		info_id
	);

}



//who - id PDA �������� ���������� ���������
void CInventoryOwner::SendPdaMessage(u16 who, EPdaMsg msg, INFO_ID info_id)
{
	if(!GetPDA() || !GetPDA()->IsActive()) return;

	GetPDA()->SendMessageID(who, msg, info_id);
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
	CEntityAlive* pOurEntityAlive = smart_cast<CEntityAlive*>(this);
	R_ASSERT(pOurEntityAlive);

	CEntityAlive* pPartnerEntityAlive = smart_cast<CEntityAlive*>(talk_partner);
	R_ASSERT(pPartnerEntityAlive);
	
//	ALife::ERelationType relation = RELATION_REGISTRY().GetRelationType(this, talk_partner);
//	if(relation == ALife::eRelationTypeEnemy) return false;

	if(!pOurEntityAlive->g_Alive() || !pPartnerEntityAlive->g_Alive()) return false;

	StartTalk(talk_partner);

	return true;
}


void CInventoryOwner::StartTalk(CInventoryOwner* talk_partner, bool start_trade)
{
	m_bTalking = true;
	m_pTalkPartner = talk_partner;

	//��� �� �������� ��������
	if(start_trade)
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
	CGameObject	*object = smart_cast<CGameObject*>(this);
	VERIFY		(object);
	object->callback(GameObject::eOnItemTake)(inventory_item->object().lua_game_object());

	attach		(inventory_item);
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
	CGameObject								*game_object = smart_cast<CGameObject*>(this);
	VERIFY									(game_object);

	if (use_bolts())
		Level().spawn_item					("bolt",game_object->Position(),game_object->ai_location().level_vertex_id(),game_object->ID());

	if (!ai().get_alife() && GameID()==GAME_SINGLE) {
		CSE_Abstract						*abstract = Level().spawn_item("device_pda",game_object->Position(),game_object->ai_location().level_vertex_id(),game_object->ID(),true);
		CSE_ALifeItemPDA					*pda = smart_cast<CSE_ALifeItemPDA*>(abstract);
		R_ASSERT							(pda);
		pda->m_original_owner				= (u16)game_object->ID();
		NET_Packet							P;
		abstract->Spawn_Write				(P,TRUE);
		Level().Send						(P,net_flags(TRUE));
		F_entity_Destroy					(abstract);
	}
}

//������� ��� 
LPCSTR	CInventoryOwner::Name () const
{
	return CharacterInfo().Name();
}



//��� ����������
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

//////////////////////////////////////////////////////////////////////////
//��� ������ � relation system
u16 CInventoryOwner::object_id	()  const
{
	return smart_cast<const CGameObject*>(this)->ID();
}


//////////////////////////////////////////////////////////////////////////
//��������� ����������� �� ���������� � ��������� ������

void CInventoryOwner::SetCommunity	(CHARACTER_COMMUNITY_INDEX new_community)
{
	CEntityAlive* EA					= smart_cast<CEntityAlive*>(this); VERIFY(EA);
	CSE_Abstract* e_entity				= Level().Server->game->get_entity_from_eid	(EA->ID()); VERIFY(e_entity);
	CSE_ALifeTraderAbstract* trader		= smart_cast<CSE_ALifeTraderAbstract*>(e_entity);
	if(!trader) return;

	CharacterInfo().m_CurrentCommunity.set(new_community);
//	EA->id_Team = CharacterInfo().m_CurrentCommunity.team();
	EA->ChangeTeam(CharacterInfo().m_CurrentCommunity.team(), EA->g_Squad(), EA->g_Group());
	trader->m_community_index  = new_community;
}

void CInventoryOwner::SetRank			(CHARACTER_RANK_VALUE rank)
{
	CEntityAlive* EA					= smart_cast<CEntityAlive*>(this); VERIFY(EA);
	CSE_Abstract* e_entity				= Level().Server->game->get_entity_from_eid	(EA->ID()); VERIFY(e_entity);
	CSE_ALifeTraderAbstract* trader		= smart_cast<CSE_ALifeTraderAbstract*>(e_entity);
	if(!trader) return;

	CharacterInfo().m_CurrentRank.set(rank);
	trader->m_rank  = rank;
}

void CInventoryOwner::ChangeRank			(CHARACTER_RANK_VALUE delta)
{
	SetRank(Rank()+delta);
}
void CInventoryOwner::SetReputation		(CHARACTER_REPUTATION_VALUE reputation)
{
	CEntityAlive* EA					= smart_cast<CEntityAlive*>(this); VERIFY(EA);
	CSE_Abstract* e_entity				= Level().Server->game->get_entity_from_eid	(EA->ID()); VERIFY(e_entity);
	CSE_ALifeTraderAbstract* trader		= smart_cast<CSE_ALifeTraderAbstract*>(e_entity);
	if(!trader) return;

	CharacterInfo().m_CurrentReputation.set(reputation);
	trader->m_reputation  = reputation;
}

void CInventoryOwner::ChangeReputation	(CHARACTER_REPUTATION_VALUE delta)
{
	SetReputation(Reputation() + delta);
}



CHARACTER_COMMUNITY_INDEX  CInventoryOwner::Community	() const
{
	return CharacterInfo().Community().index();
}

CHARACTER_REPUTATION_VALUE	CInventoryOwner::Rank		() const
{
	return CharacterInfo().Rank().value();
}

CHARACTER_REPUTATION_VALUE	CInventoryOwner::Reputation	() const
{
	return CharacterInfo().Reputation().value();
}

const CSpecificCharacter&	CInventoryOwner::SpecificCharacter	() const
{
	return CharacterInfo().m_SpecificCharacter;
}

void CInventoryOwner::OnItemDrop			(CInventoryItem *inventory_item)
{
	CGameObject	*object = smart_cast<CGameObject*>(this);
	VERIFY		(object);
	object->callback(GameObject::eOnItemDrop)(inventory_item->object().lua_game_object());

	detach		(inventory_item);
}

void CInventoryOwner::OnItemDropUpdate ()
{
}

void CInventoryOwner::OnItemBelt	(CInventoryItem *inventory_item, EItemPlace previous_place)
{
	attach		(inventory_item);
}
void CInventoryOwner::OnItemRuck	(CInventoryItem *inventory_item, EItemPlace previous_place)
{
	detach		(inventory_item);
}
void CInventoryOwner::OnItemSlot	(CInventoryItem *inventory_item, EItemPlace previous_place)
{
}

CInventoryItem* CInventoryOwner::GetCurrentOutfit() const
{
    return inventory().m_slots[OUTFIT_SLOT].m_pIItem;
}
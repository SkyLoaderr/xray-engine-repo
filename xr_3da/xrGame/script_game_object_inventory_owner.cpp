////////////////////////////////////////////////////////////////////////////
// script_game_object_inventory_owner.сpp :	функции для inventory owner
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_game_object.h"
#include "script_game_object_impl.h"
#include "InventoryOwner.h"
#include "Pda.h"
#include "xrMessages.h"
#include "character_info.h"
#include "gametask.h"
#include "actor.h"
#include "level.h"
#include "date_time.h"
#include "uigamesp.h"
#include "hudmanager.h"
#include "restricted_object.h"
#include "script_engine.h"
#include "attachable_item.h"
#include "script_entity.h"
#include "string_table.h"
#include "alife_registry_wrappers.h"
#include "relation_registry.h"
#include "custommonster.h"
#include "movement_manager.h"
#include "actorcondition.h"

bool CScriptGameObject::GiveInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::IdToIndex(info_id), true);
	return			true;
}

bool CScriptGameObject::DisableInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::IdToIndex(info_id), false);
	return true;
}


void  CScriptGameObject::SetNewsShowTime	(LPCSTR news, int show_time)
{
	if(HUD().GetUI())
		HUD().GetUI()->UIMainIngameWnd.SetDelayForPdaMessage(CStringTable().IndexById(news), show_time);
}
bool  CScriptGameObject::GiveGameNews		(LPCSTR news, LPCSTR texture_name, int x1, int y1, int x2, int y2)
{
	CActor* pActor = smart_cast<CActor*>(&object());
	if(!pActor) return false;

	GAME_NEWS_DATA news_data;
	news_data.news_text = CStringTable().IndexById(news);
	news_data.show_time = DEFAULT_NEWS_SHOW_TIME;
	
	if(xr_strlen(texture_name)>0)
	{
		news_data.texture_name = texture_name;
		news_data.x1 = x1;
		news_data.x2 = x2;
		news_data.y1 = y1;
		news_data.y2 = y2;
	}
	else
	{
		news_data.texture_name = NULL;
	}

	R_ASSERT3(news_data.news_text != NO_STRING, "no entrance found for news in string table", news);
	pActor->AddGameNews(news_data);

	return true;
}
bool CScriptGameObject::GiveInfoPortionViaPda(LPCSTR info_id, CScriptGameObject* pFromWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pFromWhoInvOwner = smart_cast<CInventoryOwner*>(&pFromWho->object());
	if(!pFromWhoInvOwner) return false;
	if(!pFromWhoInvOwner->GetPDA()) return false;

	//отправляем от нашему PDA пакет информации с номером
	NET_Packet		P;
	object().u_EventGen(P,GE_PDA,pInventoryOwner->GetPDA()->ID());
	P.w_u16			(u16(pFromWhoInvOwner->GetPDA()->ID()));		//отправитель
	P.w_s16			(ePdaMsgInfo);									
	P.w_s32			(CInfoPortion::IdToIndex(info_id));
	object().u_EventSend(P);
	return			true;
}


bool  CScriptGameObject::HasInfo				(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;
	return pInventoryOwner->HasInfo(CInfoPortion::IdToIndex(info_id));

}
bool  CScriptGameObject::DontHasInfo			(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return true;
	if(!pInventoryOwner->GetPDA()) return true;
	return !pInventoryOwner->HasInfo(CInfoPortion::IdToIndex(info_id));
}

xrTime CScriptGameObject::GetInfoTime			(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return xrTime(0);

	INFO_DATA info_data;
	if(pInventoryOwner->GetInfo(CInfoPortion::IdToIndex(info_id), info_data))
		return xrTime(info_data.receive_time);
	else
		return xrTime(0);
}


bool CScriptGameObject::SendPdaMessage(EPdaMsg pda_msg, CScriptGameObject* pForWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pForWhoInvOwner = smart_cast<CInventoryOwner*>(&pForWho->object());
	if(!pForWhoInvOwner) return false;
	if(!pForWhoInvOwner->GetPDA()) return false;

	//отправляем от нашему PDA пакет информации с номером
	NET_Packet		P;
	object().u_EventGen(P,GE_PDA,pForWhoInvOwner->GetPDA()->ID());
	P.w_u16			(u16(pInventoryOwner->GetPDA()->ID()));		//отправитель
	P.w_s16			((u16)pda_msg);
	P.w_s32			(-1);
	object().u_EventSend(P);
	return			true;
}


bool CScriptGameObject::IsTalking()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	return			pInventoryOwner->IsTalking();
}

void CScriptGameObject::StopTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return;
	pInventoryOwner->StopTalk();
}
void CScriptGameObject::EnableTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return;
	pInventoryOwner->EnableTalk();
}
void CScriptGameObject::DisableTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return;
	pInventoryOwner->DisableTalk();
}

bool CScriptGameObject::IsTalkEnabled()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	if(!pInventoryOwner) return false;
	return pInventoryOwner->IsTalkEnabled();
}


//передаче вещи из своего инвентаря в инвентарь партнера
void CScriptGameObject::TransferItem(CScriptGameObject* pItem, CScriptGameObject* pForWho)
{
	R_ASSERT(pItem);
	R_ASSERT(pForWho);

	CInventoryItem* pIItem = smart_cast<CInventoryItem*>(&pItem->object());
	VERIFY(pIItem);

	// выбросить у себя 
	NET_Packet						P;
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_REJECT, object().ID());
	P.w_u16							(pIItem->object().ID());
	CGameObject::u_EventSend		(P);

	// отдать партнеру
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_TAKE, pForWho->object().ID());
	P.w_u16							(pIItem->object().ID());
	CGameObject::u_EventSend		(P);
}

u32 CScriptGameObject::Money	()
{
	CInventoryOwner* pOurOwner		= smart_cast<CInventoryOwner*>(&object()); VERIFY(pOurOwner);
	return pOurOwner->m_dwMoney;
}

void CScriptGameObject::TransferMoney(int money, CScriptGameObject* pForWho)
{
	R_ASSERT(pForWho);
	CInventoryOwner* pOurOwner		= smart_cast<CInventoryOwner*>(&object()); VERIFY(pOurOwner);
	CInventoryOwner* pOtherOwner	= smart_cast<CInventoryOwner*>(&pForWho->object()); VERIFY(pOtherOwner);

	R_ASSERT3(pOurOwner->m_dwMoney-money>=0, "Character does not have enought money", pOurOwner->CharacterInfo().Name());

	pOurOwner->m_dwMoney	-= money;
	pOtherOwner->m_dwMoney	+= money;
}
//////////////////////////////////////////////////////////////////////////

int	CScriptGameObject::GetGoodwill(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return RELATION_REGISTRY().GetGoodwill(pInventoryOwner->object_id(), pToWho->object().ID());
}

void CScriptGameObject::SetGoodwill(int goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return RELATION_REGISTRY().SetGoodwill(pInventoryOwner->object_id(), pWhoToSet->object().ID(), goodwill);
}

void CScriptGameObject::ChangeGoodwill(int delta_goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	RELATION_REGISTRY().ChangeGoodwill(pInventoryOwner->object_id(), pWhoToSet->object().ID(), delta_goodwill);
}

//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::SetCommunityGoodwill(int goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	RELATION_REGISTRY().SetCommunityGoodwill(pInventoryOwner->object_id(), pWhoToSet->object().ID(), goodwill);
}

int	CScriptGameObject::GetCommunityGoodwill(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return RELATION_REGISTRY().GetCommunityGoodwill(pInventoryOwner->object_id(), pToWho->object().ID());
}

void CScriptGameObject::ChangeCommunityGoodwill(int delta_goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	RELATION_REGISTRY().ChangeCommunityGoodwill(pInventoryOwner->object_id(), pWhoToSet->object().ID(), delta_goodwill);
}

//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::SetRelation(ALife::ERelationType relation, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	CInventoryOwner* pOthersInventoryOwner = smart_cast<CInventoryOwner*>(&pWhoToSet->object());
	VERIFY(pOthersInventoryOwner);
	RELATION_REGISTRY().SetRelationType(pInventoryOwner, pOthersInventoryOwner, relation);
}

//////////////////////////////////////////////////////////////////////////

int	CScriptGameObject::GetAttitude			(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());VERIFY(pInventoryOwner);
	CInventoryOwner* pOthersInventoryOwner = smart_cast<CInventoryOwner*>(&pToWho->object());VERIFY(pOthersInventoryOwner);
	return RELATION_REGISTRY().GetAttitude(pInventoryOwner, pOthersInventoryOwner);
}


//////////////////////////////////////////////////////////////////////////

LPCSTR CScriptGameObject::ProfileName			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	
	PROFILE_INDEX profile_index =  pInventoryOwner->CharacterInfo().Profile();
	if(NO_PROFILE == profile_index)
		return NULL;
	else
		return *CCharacterInfo::IndexToId(profile_index);
}


LPCSTR CScriptGameObject::CharacterName			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().Name();
}
int CScriptGameObject::CharacterRank			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return pInventoryOwner->Rank();
}
void CScriptGameObject::SetCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return pInventoryOwner->SetRank(char_rank);
}

void CScriptGameObject::ChangeCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return pInventoryOwner->ChangeRank(char_rank);
}

int CScriptGameObject::CharacterReputation			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return pInventoryOwner->Reputation();
}

void CScriptGameObject::SetCharacterReputation		(int char_rep)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	pInventoryOwner->SetReputation(char_rep);
}

void CScriptGameObject::ChangeCharacterReputation		(int char_rep)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	pInventoryOwner->ChangeReputation(char_rep);
}

LPCSTR CScriptGameObject::CharacterCommunity	()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return *pInventoryOwner->CharacterInfo().Community().id();
}

void CScriptGameObject::SetCharacterCommunity	(LPCSTR comm)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	CHARACTER_COMMUNITY	community;
	community.set(comm);
	return pInventoryOwner->SetCommunity(community.index());
}

LPCSTR CScriptGameObject::snd_character_profile_sect () const
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(&object());
	VERIFY(pInventoryOwner);
	return pInventoryOwner->SpecificCharacter().SndConfigSect();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ETaskState CScriptGameObject::GetGameTaskState	(LPCSTR task_id, int objective_num)
{
	CActor* pActor = smart_cast<CActor*>(&object());
	VERIFY(pActor);

	TASK_INDEX task_index = CGameTask::IdToIndex(task_id);
	R_ASSERT3(task_index != NO_TASK, "wrong task id", task_id);
	
	const GAME_TASK_VECTOR* tasks =  pActor->game_task_registry->registry().objects_ptr();
	if(!tasks) 
		return eTaskStateDummy;

	for(GAME_TASK_VECTOR::const_iterator it = tasks->begin();
			tasks->end() != it; it++)
	{
		if((*it).index == task_index) 
			break;
	}
	
	if(tasks->end() == it) 
		return eTaskStateDummy;

	R_ASSERT3((std::size_t)objective_num < (*it).states.size(), "wrong objective num", task_id);
	return (*it).states[objective_num];
}

void CScriptGameObject::SetGameTaskState	(ETaskState state, LPCSTR task_id, int objective_num)
{
	CActor* pActor = smart_cast<CActor*>(&object());
	VERIFY(pActor);

	TASK_INDEX task_index = CGameTask::IdToIndex(task_id);
	R_ASSERT3(task_index != NO_TASK, "wrong task id", task_id);
	
	GAME_TASK_VECTOR& tasks =  pActor->game_task_registry->registry().objects();

	for(GAME_TASK_VECTOR::iterator it = tasks.begin();
			tasks.end() != it; it++)
	{
		if((*it).index == task_index) 
			break;
	}

	R_ASSERT3(tasks.end() != it, "actor does not has task", task_id);
	R_ASSERT3((std::size_t)objective_num < (*it).states.size(), "wrong objective num", task_id);
	(*it).states[objective_num] = state;

	//если мы устанавливаем финальное состояние для основного задания, то
	//запомнить время выполнения
	if(0 == objective_num && eTaskStateCompleted == state || eTaskStateFail == state)
	{
		(*it).finish_time = Level().GetGameTime();
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void  CScriptGameObject::SwitchToTrade		()
{
	CActor* pActor = smart_cast<CActor*>(&object());	if(!pActor) return;

	//только если находимся в режиме single
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu.IsShown())
	{
		pGameSP->TalkMenu.SwitchToTrade();
	}
}
void  CScriptGameObject::SwitchToTalk		()
{
	CActor* pActor = smart_cast<CActor*>(&object());	if(!pActor) return;

	//только если находимся в режиме single
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TradeMenu.IsShown())
	{
		pGameSP->TradeMenu.SwitchToTalk();
	}
}

void  CScriptGameObject::RunTalkDialog			(CScriptGameObject* pToWho)
{
	CActor* pActor = smart_cast<CActor*>(&object());	
	R_ASSERT2(pActor, "RunTalkDialog applicable only for actor");
	CInventoryOwner* pPartner = smart_cast<CInventoryOwner*>(&pToWho->object());	VERIFY(pPartner);
	pActor->RunTalkDialog(pPartner);
}

void  CScriptGameObject::ActorSleep			(int hours, int minutes)
{
	CActor* pActor = smart_cast<CActor*>(&object());	if(!pActor) return;
	pActor->conditions().GoSleep(generate_time(1,1,1,hours, minutes, 0, 0), true);
}

//////////////////////////////////////////////////////////////////////////

void construct_restriction_vector(shared_str restrictions, xr_vector<ALife::_OBJECT_ID> &result)
{
	result.clear();
	string64	temp;
	u32			n = _GetItemCount(*restrictions);
	for (u32 i=0; i<n; ++i) {
		CObject	*object = Level().Objects.FindObjectByName(_GetItem(*restrictions,i,temp));
		if (!object)
			continue;
		result.push_back(object->ID());
	}
}

void CScriptGameObject::add_restrictions		(LPCSTR out, LPCSTR in)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member add_restrictions!");
		return;
	}
	
	xr_vector<ALife::_OBJECT_ID>			temp0;
	xr_vector<ALife::_OBJECT_ID>			temp1;

	construct_restriction_vector			(out,temp0);
	construct_restriction_vector			(in,temp1);
	
	monster->movement().restrictions().add_restrictions(temp0,temp1);
}

void CScriptGameObject::remove_restrictions		(LPCSTR out, LPCSTR in)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_restrictions!");
		return;
	}

	xr_vector<ALife::_OBJECT_ID>			temp0;
	xr_vector<ALife::_OBJECT_ID>			temp1;

	construct_restriction_vector			(out,temp0);
	construct_restriction_vector			(in,temp1);

	monster->movement().restrictions().remove_restrictions	(temp0,temp1);
}

void CScriptGameObject::remove_all_restrictions	()
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_all_restrictions!");
		return;
	}
	monster->movement().restrictions().remove_all_restrictions	();
}

LPCSTR CScriptGameObject::in_restrictions	()
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member in_restrictions!");
		return								("");
	}
	return									(*monster->movement().restrictions().in_restrictions());
}

LPCSTR CScriptGameObject::out_restrictions	()
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member out_restrictions!");
		return								("");
	}
	return									(*monster->movement().restrictions().out_restrictions());
}

bool CScriptGameObject::accessible_position	(const Fvector &position)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(false);
	}
	return									(monster->movement().restrictions().accessible(position));
}

bool CScriptGameObject::accessible_vertex_id(u32 level_vertex_id)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(false);
	}
	return									(monster->movement().restrictions().accessible(level_vertex_id));
}

u32	 CScriptGameObject::accessible_nearest	(const Fvector &position, Fvector &result)
{
	CCustomMonster	*monster = smart_cast<CCustomMonster*>(&object());
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(u32(-1));
	}
	return									(monster->movement().restrictions().accessible_nearest(position,result));
}

bool CScriptGameObject::limping				() const
{
	CEntityAlive							*entity_alive = smart_cast<CEntityAlive*>(&object());
	if (!entity_alive) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member limping!");
		return								(false);
	}
	return									(entity_alive->conditions().IsLimping());
}

void CScriptGameObject::enable_attachable_item	(bool value)
{
	CAttachableItem							*attachable_item = smart_cast<CAttachableItem*>(&object());
	if (!attachable_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAttachableItem : cannot access class member enable_attachable_item!");
		return;
	}
	attachable_item->enable					(value);
}

bool CScriptGameObject::attachable_item_enabled	() const
{
	CAttachableItem							*attachable_item = smart_cast<CAttachableItem*>(&object());
	if (!attachable_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAttachableItem : cannot access class member attachable_item_enabled!");
		return								(false);
	}
	return									(attachable_item->enabled());
}



void  CScriptGameObject::RestoreWeapon		()
{
	CActor* pActor = smart_cast<CActor*>(&object());	VERIFY(pActor);
	pActor->RestoreHidedWeapon(GEG_PLAYER_INVENTORYMENU_CLOSE);
}
void  CScriptGameObject::HideWeapon			()
{
	CActor* pActor = smart_cast<CActor*>(&object());	VERIFY(pActor);
	pActor->HideCurrentWeapon(GEG_PLAYER_INVENTORYMENU_OPEN);
}
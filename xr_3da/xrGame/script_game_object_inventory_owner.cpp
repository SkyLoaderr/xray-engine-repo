////////////////////////////////////////////////////////////////////////////
// script_game_object_inventory_owner.сpp :	функции для inventory owner
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_game_object.h"
#include "InventoryOwner.h"
#include "Pda.h"
#include "xrMessages.h"
#include "character_info.h"
#include "ai_phrasedialogmanager.h"

#include "gametask.h"
#include "actor.h"
#include "level.h"

#include "date_time.h"

#include "uigamesp.h"
#include "hudmanager.h"

#include "restricted_object.h"
#include "script_engine.h"
#include "visual_memory_manager.h"
#include "sound_memory_manager.h"
#include "attachable_item.h"
#include "ai/script/ai_script_monster.h"
#include "string_table.h"

bool CScriptGameObject::GiveInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::IdToIndex(info_id), true);
	return			true;
}

bool CScriptGameObject::DisableInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::IdToIndex(info_id), false);
	return true;
}


bool  CScriptGameObject::GiveGameNews		(LPCSTR news, LPCSTR texture_name, int x1, int y1, int x2, int y2)
{
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);
	if(!pActor) return false;

	GAME_NEWS_DATA news_data;
	news_data.news_text = CStringTable().IndexById(news);

	R_ASSERT3(news_data.news_text != NO_STRING, "no entrance found for news in string table", news);
	pActor->AddGameNews(news_data);

	return true;
}
bool CScriptGameObject::GiveInfoPortionViaPda(LPCSTR info_id, CScriptGameObject* pFromWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pFromWhoInvOwner = smart_cast<CInventoryOwner*>(pFromWho->m_tpGameObject);
	if(!pFromWhoInvOwner) return false;
	if(!pFromWhoInvOwner->GetPDA()) return false;

	//отправляем от нашему PDA пакет информации с номером
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_PDA,pInventoryOwner->GetPDA()->ID());
	P.w_u16			(u16(pFromWhoInvOwner->GetPDA()->ID()));		//отправитель
	P.w_s16			(ePdaMsgInfo);									
	P.w_s32			(CInfoPortion::IdToIndex(info_id));
	m_tpGameObject->u_EventSend(P);
	return			true;
}


bool  CScriptGameObject::HasInfo				(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;
	return pInventoryOwner->HasInfo(CInfoPortion::IdToIndex(info_id));

}
bool  CScriptGameObject::DontHasInfo			(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return true;
	if(!pInventoryOwner->GetPDA()) return true;
	return !pInventoryOwner->HasInfo(CInfoPortion::IdToIndex(info_id));
}


bool CScriptGameObject::SendPdaMessage(EPdaMsg pda_msg, CScriptGameObject* pForWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pForWhoInvOwner = smart_cast<CInventoryOwner*>(pForWho->m_tpGameObject);
	if(!pForWhoInvOwner) return false;
	if(!pForWhoInvOwner->GetPDA()) return false;

	//отправляем от нашему PDA пакет информации с номером
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_PDA,pForWhoInvOwner->GetPDA()->ID());
	P.w_u16			(u16(pInventoryOwner->GetPDA()->ID()));		//отправитель
	P.w_s16			((u16)pda_msg);
	P.w_s32			(-1);
	m_tpGameObject->u_EventSend(P);
	return			true;
}


bool CScriptGameObject::IsTalking()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	return			pInventoryOwner->IsTalking();
}

void CScriptGameObject::StopTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->StopTalk();
}
void CScriptGameObject::EnableTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->EnableTalk();
}
void CScriptGameObject::DisableTalk()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->DisableTalk();
}

bool CScriptGameObject::IsTalkEnabled()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	return pInventoryOwner->IsTalkEnabled();
}


//передаче вещи из своего инвентаря в инвентарь партнера
void CScriptGameObject::TransferItem(CScriptGameObject* pItem, CScriptGameObject* pForWho)
{
	R_ASSERT(pItem);
	R_ASSERT(pForWho);

	CInventoryItem* pIItem = smart_cast<CInventoryItem*>(pItem->m_tpGameObject);
	VERIFY(pIItem);

	// выбросить у себя 
	NET_Packet						P;
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_REJECT, m_tpGameObject->ID());
	P.w_u16							(pIItem->ID());
	CGameObject::u_EventSend		(P);

	// отдать партнеру
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_TAKE, pForWho->m_tpGameObject->ID());
	P.w_u16							(pIItem->ID());
	CGameObject::u_EventSend		(P);
}

u32 CScriptGameObject::Money	()
{
	CInventoryOwner* pOurOwner		= smart_cast<CInventoryOwner*>(m_tpGameObject); VERIFY(pOurOwner);
	return pOurOwner->m_dwMoney;
}

void CScriptGameObject::TransferMoney(int money, CScriptGameObject* pForWho)
{
	R_ASSERT(pForWho);
	CInventoryOwner* pOurOwner		= smart_cast<CInventoryOwner*>(m_tpGameObject); VERIFY(pOurOwner);
	CInventoryOwner* pOtherOwner	= smart_cast<CInventoryOwner*>(pForWho->m_tpGameObject); VERIFY(pOtherOwner);

	R_ASSERT3(pOurOwner->m_dwMoney-money>=0, "Character does not have enought money", pOurOwner->CharacterInfo().Name());

	pOurOwner->m_dwMoney	-= money;
	pOtherOwner->m_dwMoney	+= money;
}


int	CScriptGameObject::GetGoodwill(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().Relations().GetGoodwill(pToWho->m_tpGameObject->ID());
}

void CScriptGameObject::SetGoodwill(int goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().Relations().SetGoodwill(pWhoToSet->m_tpGameObject->ID(), goodwill);
}

void CScriptGameObject::SetRelation(ALife::ERelationType relation, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().Relations().SetRelationType(pWhoToSet->m_tpGameObject->ID(), relation);
}
bool CScriptGameObject::NeedToAnswerPda		()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return false;
	return pDialogManager->NeedAnswerOnPDA();
}
void CScriptGameObject::AnswerPda			()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->AnswerOnPDA();
}



LPCSTR CScriptGameObject::CharacterName			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().Name();
}
int CScriptGameObject::CharacterRank			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().Rank();
}
void CScriptGameObject::SetCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().SetRank(char_rank);
}

void CScriptGameObject::ChangeCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().SetRank(pInventoryOwner->CharacterInfo().Rank() + char_rank);
}

int CScriptGameObject::CharacterReputation			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().Reputation();
}

void CScriptGameObject::SetCharacterReputation		(int char_rep)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().SetReputation(char_rep);
}

void CScriptGameObject::ChangeCharacterReputation		(int char_rep)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().SetReputation(pInventoryOwner->CharacterInfo().Reputation() + char_rep);
}

LPCSTR CScriptGameObject::CharacterCommunity	()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return *pInventoryOwner->CharacterInfo().Community().id();
}

void CScriptGameObject::SetCharacterCommunity	(LPCSTR comm)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	CHARACTER_COMMUNITY	community;
	community.set(comm);
	return pInventoryOwner->CharacterInfo().SetCommunity(community);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ETaskState CScriptGameObject::GetGameTaskState	(LPCSTR task_id, int objective_num)
{
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);
	VERIFY(pActor);

	TASK_INDEX task_index = CGameTask::IdToIndex(task_id);
	R_ASSERT3(task_index != NO_TASK, "wrong task id", task_id);
	
	const GAME_TASK_VECTOR* tasks =  pActor->game_task_registry.objects_ptr();
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
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);
	VERIFY(pActor);

	TASK_INDEX task_index = CGameTask::IdToIndex(task_id);
	R_ASSERT3(task_index != NO_TASK, "wrong task id", task_id);
	
	GAME_TASK_VECTOR& tasks =  pActor->game_task_registry.objects();

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

void CScriptGameObject::SetStartDialog(LPCSTR dialog_id)
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->SetStartDialog(dialog_id);
}

void CScriptGameObject::GetStartDialog		()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->GetStartDialog();
}
void CScriptGameObject::RestoreDefaultStartDialog()
{
	CAI_PhraseDialogManager* pDialogManager = smart_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->RestoreDefaultStartDialog();
}

void  CScriptGameObject::SwitchToTrade		()
{
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);	if(!pActor) return;

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
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);	if(!pActor) return;

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
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);	
	R_ASSERT2(pActor, "RunTalkDialog applicable only for actor");
	CInventoryOwner* pPartner = smart_cast<CInventoryOwner*>(pToWho->m_tpGameObject);	VERIFY(pPartner);
	pActor->RunTalkDialog(pPartner);
}

void  CScriptGameObject::ActorSleep			(int hours, int minutes)
{
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);	if(!pActor) return;
	pActor->GoSleep(generate_time(1,1,1,hours, minutes, 0, 0), true);
}

//////////////////////////////////////////////////////////////////////////


void CScriptGameObject::add_restrictions		(LPCSTR in, LPCSTR out)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member add_restrictions!");
		return;
	}
	restricted_object->add_restrictions		(in,out);
}

void CScriptGameObject::remove_restrictions		(LPCSTR in, LPCSTR out)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_restrictions!");
		return;
	}
	restricted_object->remove_restrictions	(in,out);
}

void CScriptGameObject::remove_all_restrictions	()
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_all_restrictions!");
		return;
	}
	restricted_object->remove_all_restrictions	();
}

LPCSTR CScriptGameObject::in_restrictions	()
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member in_restrictions!");
		return								("");
	}
	return									(*restricted_object->in_restrictions());
}

LPCSTR CScriptGameObject::out_restrictions	()
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member out_restrictions!");
		return								("");
	}
	return									(*restricted_object->out_restrictions());
}

bool CScriptGameObject::accessible_position	(const Fvector &position)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(false);
	}
	return									(restricted_object->accessible(position));
}

bool CScriptGameObject::accessible_vertex_id(u32 level_vertex_id)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(false);
	}
	return									(restricted_object->accessible(level_vertex_id));
}

u32	 CScriptGameObject::accessible_nearest	(const Fvector &position, Fvector &result)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member accessible!");
		return								(u32(-1));
	}
	return									(restricted_object->accessible_nearest(position,result));
}

bool CScriptGameObject::limping				() const
{
	CEntityCondition						*entity_condition = smart_cast<CEntityCondition*>(m_tpGameObject);
	if (!entity_condition) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member limping!");
		return								(false);
	}
	return									(entity_condition->IsLimping());
}

void CScriptGameObject::enable_vision			(bool value)
{
	CVisualMemoryManager					*visual_memory_manager = smart_cast<CVisualMemoryManager*>(m_tpGameObject);
	if (!visual_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CVisualMemoryManager : cannot access class member enable_vision!");
		return;
	}
	visual_memory_manager->enable			(value);
}

bool CScriptGameObject::vision_enabled			() const
{
	CVisualMemoryManager					*visual_memory_manager = smart_cast<CVisualMemoryManager*>(m_tpGameObject);
	if (!visual_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CVisualMemoryManager : cannot access class member vision_enabled!");
		return								(false);
	}
	return									(visual_memory_manager->enabled());
}

void CScriptGameObject::set_sound_threshold		(float value)
{
	CSoundMemoryManager						*sound_memory_manager = smart_cast<CSoundMemoryManager*>(m_tpGameObject);
	if (!sound_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSoundMemoryManager : cannot access class member set_sound_threshold!");
		return;
	}
	sound_memory_manager->set_threshold		(value);
}

void CScriptGameObject::restore_sound_threshold	()
{
	CSoundMemoryManager						*sound_memory_manager = smart_cast<CSoundMemoryManager*>(m_tpGameObject);
	if (!sound_memory_manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CSoundMemoryManager : cannot access class member restore_sound_threshold!");
		return;
	}
	sound_memory_manager->restore_threshold	();
}

void CScriptGameObject::enable_attachable_item	(bool value)
{
	CAttachableItem							*attachable_item = smart_cast<CAttachableItem*>(m_tpGameObject);
	if (!attachable_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAttachableItem : cannot access class member enable_attachable_item!");
		return;
	}
	attachable_item->enable					(value);
}

bool CScriptGameObject::attachable_item_enabled	() const
{
	CAttachableItem							*attachable_item = smart_cast<CAttachableItem*>(m_tpGameObject);
	if (!attachable_item) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAttachableItem : cannot access class member attachable_item_enabled!");
		return								(false);
	}
	return									(attachable_item->enabled());
}

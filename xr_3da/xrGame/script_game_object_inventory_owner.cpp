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
#include "alife_registry_wrappers.h"
#include "relation_registry.h"
#include "ai/stalker/ai_stalker.h"

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

xrTime CScriptGameObject::GetInfoTime			(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return xrTime(0);

	INFO_DATA info_data;
	if(pInventoryOwner->GetInfo(CInfoPortion::IdToIndex(info_id), info_data))
		return xrTime(info_data.receive_time);
	else
		return xrTime(0);
}


bool CScriptGameObject::SendPdaMessage(EPdaMsg pda_msg, CScriptGameObject* pForWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pForWhoInvOwner = smart_cast<CInventoryOwner*>(pForWho->m_tpGameObject);
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


//�������� ���� �� ������ ��������� � ��������� ��������
void CScriptGameObject::TransferItem(CScriptGameObject* pItem, CScriptGameObject* pForWho)
{
	R_ASSERT(pItem);
	R_ASSERT(pForWho);

	CInventoryItem* pIItem = smart_cast<CInventoryItem*>(pItem->m_tpGameObject);
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
//////////////////////////////////////////////////////////////////////////

int	CScriptGameObject::GetGoodwill(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return RELATION_REGISTRY().GetGoodwill(pInventoryOwner->object_id(), pToWho->m_tpGameObject->ID());
}

void CScriptGameObject::SetGoodwill(int goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return RELATION_REGISTRY().SetGoodwill(pInventoryOwner->object_id(), pWhoToSet->m_tpGameObject->ID(), goodwill);
}

void CScriptGameObject::ChangeGoodwill(int delta_goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	RELATION_REGISTRY().ChangeGoodwill(pInventoryOwner->object_id(), pWhoToSet->m_tpGameObject->ID(), delta_goodwill);
}

//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::SetCommunityGoodwill(int goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	RELATION_REGISTRY().SetCommunityGoodwill(pInventoryOwner->object_id(), pWhoToSet->m_tpGameObject->ID(), goodwill);
}

int	CScriptGameObject::GetCommunityGoodwill(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return RELATION_REGISTRY().GetCommunityGoodwill(pInventoryOwner->object_id(), pToWho->m_tpGameObject->ID());
}

void CScriptGameObject::ChangeCommunityGoodwill(int delta_goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	RELATION_REGISTRY().ChangeCommunityGoodwill(pInventoryOwner->object_id(), pWhoToSet->m_tpGameObject->ID(), delta_goodwill);
}

//////////////////////////////////////////////////////////////////////////

void CScriptGameObject::SetRelation(ALife::ERelationType relation, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	CInventoryOwner* pOthersInventoryOwner = smart_cast<CInventoryOwner*>(pWhoToSet->m_tpGameObject);
	VERIFY(pOthersInventoryOwner);
	RELATION_REGISTRY().SetRelationType(pInventoryOwner, pOthersInventoryOwner, relation);
}

//////////////////////////////////////////////////////////////////////////

int	CScriptGameObject::GetAttitude			(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);VERIFY(pInventoryOwner);
	CInventoryOwner* pOthersInventoryOwner = smart_cast<CInventoryOwner*>(pToWho->m_tpGameObject);VERIFY(pOthersInventoryOwner);
	return RELATION_REGISTRY().GetAttitude(pInventoryOwner, pOthersInventoryOwner);
}


//////////////////////////////////////////////////////////////////////////

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


LPCSTR CScriptGameObject::ProfileName			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	
	PROFILE_INDEX profile_index =  pInventoryOwner->CharacterInfo().Profile();
	if(NO_PROFILE == profile_index)
		return NULL;
	else
		return *CCharacterInfo::IndexToId(profile_index);
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
	return pInventoryOwner->Rank();
}
void CScriptGameObject::SetCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->SetRank(char_rank);
}

void CScriptGameObject::ChangeCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->ChangeRank(char_rank);
}

int CScriptGameObject::CharacterReputation			()
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->Reputation();
}

void CScriptGameObject::SetCharacterReputation		(int char_rep)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->SetReputation(char_rep);
}

void CScriptGameObject::ChangeCharacterReputation		(int char_rep)
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->ChangeReputation(char_rep);
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
	return pInventoryOwner->SetCommunity(community.index());
}

LPCSTR CScriptGameObject::snd_character_profile_sect () const
{
	CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->SpecificCharacter().SndConfigSect();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ETaskState CScriptGameObject::GetGameTaskState	(LPCSTR task_id, int objective_num)
{
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);
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
	CActor* pActor = smart_cast<CActor*>(m_tpGameObject);
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

	//���� �� ������������� ��������� ��������� ��� ��������� �������, ��
	//��������� ����� ����������
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

	//������ ���� ��������� � ������ single
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

	//������ ���� ��������� � ������ single
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
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member add_restrictions!");
		return;
	}
	
	xr_vector<ALife::_OBJECT_ID>			temp0;
	xr_vector<ALife::_OBJECT_ID>			temp1;

	construct_restriction_vector			(out,temp0);
	construct_restriction_vector			(in,temp1);
	
	restricted_object->add_restrictions		(temp0,temp1);
}

void CScriptGameObject::remove_restrictions		(LPCSTR out, LPCSTR in)
{
	CRestrictedObject	*restricted_object = smart_cast<CRestrictedObject*>(m_tpGameObject);
	if (!restricted_object) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CRestrictedObject : cannot access class member remove_restrictions!");
		return;
	}

	xr_vector<ALife::_OBJECT_ID>			temp0;
	xr_vector<ALife::_OBJECT_ID>			temp1;

	construct_restriction_vector			(out,temp0);
	construct_restriction_vector			(in,temp1);

	restricted_object->remove_restrictions	(temp0,temp1);
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


Flags32 CScriptGameObject::get_actor_relation_flags () const
{
	CAI_Stalker* stalker = smart_cast<CAI_Stalker*>(m_tpGameObject);
	VERIFY(stalker);

	return stalker->m_actor_relation_flags;
}
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

bool CScriptGameObject::GiveInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::IdToIndex(info_id), true);
	return			true;
}

bool CScriptGameObject::DisableInfoPortion(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::IdToIndex(info_id), false);
	return true;
}

bool CScriptGameObject::GiveInfoPortionViaPda(LPCSTR info_id, CScriptGameObject* pFromWho)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pFromWhoInvOwner = dynamic_cast<CInventoryOwner*>(pFromWho->m_tpGameObject);
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
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;
	return pInventoryOwner->HasInfo(CInfoPortion::IdToIndex(info_id));

}
bool  CScriptGameObject::DontHasInfo			(LPCSTR info_id)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return true;
	if(!pInventoryOwner->GetPDA()) return true;
	return !pInventoryOwner->HasInfo(CInfoPortion::IdToIndex(info_id));
}


bool CScriptGameObject::SendPdaMessage(EPdaMsg pda_msg, CScriptGameObject* pForWho)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pForWhoInvOwner = dynamic_cast<CInventoryOwner*>(pForWho->m_tpGameObject);
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
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	return			pInventoryOwner->IsTalking();
}

void CScriptGameObject::StopTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->StopTalk();
}
void CScriptGameObject::EnableTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->EnableTalk();
}
void CScriptGameObject::DisableTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->DisableTalk();
}

bool CScriptGameObject::IsTalkEnabled()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	return pInventoryOwner->IsTalkEnabled();
}


//передаче вещи из своего инвентаря в инвентарь партнера
void CScriptGameObject::TransferItem(CScriptGameObject* pItem, CScriptGameObject* pForWho)
{
	R_ASSERT(pItem);
	R_ASSERT(pForWho);

	CInventoryItem* pIItem = dynamic_cast<CInventoryItem*>(pItem->m_tpGameObject);
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
	CInventoryOwner* pOurOwner		= dynamic_cast<CInventoryOwner*>(m_tpGameObject); VERIFY(pOurOwner);
	return pOurOwner->m_dwMoney;
}

void CScriptGameObject::TransferMoney(int money, CScriptGameObject* pForWho)
{
	R_ASSERT(pForWho);
	CInventoryOwner* pOurOwner		= dynamic_cast<CInventoryOwner*>(m_tpGameObject); VERIFY(pOurOwner);
	CInventoryOwner* pOtherOwner	= dynamic_cast<CInventoryOwner*>(pForWho->m_tpGameObject); VERIFY(pOtherOwner);

	R_ASSERT3(pOurOwner->m_dwMoney-money>=0, "Character does not have enought money", pOurOwner->CharacterInfo().Name());

	pOurOwner->m_dwMoney	-= money;
	pOtherOwner->m_dwMoney	+= money;
}


int	CScriptGameObject::GetGoodwill(CScriptGameObject* pToWho)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().GetGoodwill(pToWho->m_tpGameObject->ID());
}

void CScriptGameObject::SetGoodwill(int goodwill, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().SetGoodwill(pWhoToSet->m_tpGameObject->ID(), goodwill);
}

void CScriptGameObject::SetRelation(ALife::ERelationType relation, CScriptGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().SetRelationType(pWhoToSet->m_tpGameObject->ID(), relation);
}
bool CScriptGameObject::NeedToAnswerPda		()
{
	CAI_PhraseDialogManager* pDialogManager = dynamic_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return false;
	return pDialogManager->NeedAnswerOnPDA();
}
void CScriptGameObject::AnswerPda			()
{
	CAI_PhraseDialogManager* pDialogManager = dynamic_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->AnswerOnPDA();
}



LPCSTR CScriptGameObject::CharacterName			()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().Name();
}
int CScriptGameObject::CharacterRank			()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().Rank();
}
void CScriptGameObject::SetCharacterRank			(int char_rank)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().SetRank(char_rank);
}

int CScriptGameObject::CharacterReputation			()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().Reputation();
}

void CScriptGameObject::SetCharacterReputation		(int char_rep)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().SetReputation(char_rep);
}

LPCSTR CScriptGameObject::CharacterCommunity	()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return *pInventoryOwner->CharacterInfo().Community();
}

void CScriptGameObject::SetCharacterCommunity	(LPCSTR comm)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	return pInventoryOwner->CharacterInfo().SetCommunity(comm);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ETaskState CScriptGameObject::GetGameTaskState	(LPCSTR task_id, int objective_num)
{
	CActor* pActor = dynamic_cast<CActor*>(m_tpGameObject);
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
	CActor* pActor = dynamic_cast<CActor*>(m_tpGameObject);
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
	CAI_PhraseDialogManager* pDialogManager = dynamic_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->SetStartDialog(dialog_id);
}

void CScriptGameObject::GetStartDialog		()
{
	CAI_PhraseDialogManager* pDialogManager = dynamic_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->GetStartDialog();
}
void CScriptGameObject::RestoreDefaultStartDialog()
{
	CAI_PhraseDialogManager* pDialogManager = dynamic_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->RestoreDefaultStartDialog();
}

void  CScriptGameObject::SwitchToTrade		()
{
	CActor* pActor = dynamic_cast<CActor*>(m_tpGameObject);	if(pActor) return;

	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu.IsShown())
	{
		pGameSP->TalkMenu.SwitchToTrade();
	}
}
void  CScriptGameObject::SwitchToTalk		()
{
	CActor* pActor = dynamic_cast<CActor*>(m_tpGameObject);	if(pActor) return;

	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TradeMenu.IsShown())
	{
		pGameSP->TradeMenu.SwitchToTalk();
	}
}

void  CScriptGameObject::RunTalkDialog			(CScriptGameObject* pToWho)
{
	CActor* pActor = dynamic_cast<CActor*>(m_tpGameObject);	
	R_ASSERT2(pActor, "RunTalkDialog applicable only for actor");
	CInventoryOwner* pPartner = dynamic_cast<CInventoryOwner*>(pToWho->m_tpGameObject);	VERIFY(pPartner);
	pActor->RunTalkDialog(pPartner);
}

void  CScriptGameObject::ActorSleep			(int hours, int minutes)
{
	CActor* pActor = dynamic_cast<CActor*>(m_tpGameObject);	if(pActor) return;
	pActor->GoSleep(generate_time(0,0,0,hours, minutes, 0, 0), true);
}
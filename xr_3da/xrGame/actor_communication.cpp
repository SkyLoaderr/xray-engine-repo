// actor_communication.cpp:	 связь по PDA и диалоги
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "actor.h"

#include "UIGameSP.h"
#include "UI.h"
#include "PDA.h"
#include "HUDManager.h"
#include "level.h"
#include "string_table.h"
#include "PhraseDialog.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_registry_container.h"
#include "script_game_object.h"



void CActor::AddMapLocationsFromInfo(const CInfoPortion* info_portion)
{
	VERIFY(info_portion);
	//добавить отметки на карте
	for(u32 i=0; i<info_portion->MapLocations().size(); i++)
	{
		const SMapLocation& map_location = info_portion->MapLocations()[i];
		if(xr_strlen(*map_location.level_name) > 0)
		{
			if(map_location.level_name == Level().Name())
				Level().AddMapLocation(map_location);
		}
		else
			Level().AddMapLocation(map_location);
	}
}

void CActor::AddEncyclopediaArticle	 (const CInfoPortion* info_portion)
{
	VERIFY(info_portion);
	ARTICLE_VECTOR& article_vector = encyclopedia_registry.objects();

	ARTICLE_VECTOR::iterator last_end = article_vector.end();
	ARTICLE_VECTOR::iterator B = article_vector.begin();
	ARTICLE_VECTOR::iterator E = last_end;

	for(ARTICLE_VECTOR::const_iterator it = info_portion->ArticlesDisable().begin();
									it != info_portion->ArticlesDisable().end(); it++)
	{
		last_end = std::remove(B, last_end, *it);
	}
	article_vector.erase(last_end, E);

	
	for(ARTICLE_VECTOR::const_iterator it = info_portion->Articles().begin();
									it != info_portion->Articles().end(); it++)
	{
		article_vector.push_back(*it);
	}
}

//information receive
void CActor::OnReceiveInfo(INFO_ID info_id)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	CInfoPortion info_portion;
	info_portion.Load(info_id);

	AddMapLocationsFromInfo(&info_portion);
	AddEncyclopediaArticle(&info_portion);
	
	if(pGameSP->TalkMenu.IsShown())
	{
		if(pGameSP->TalkMenu.IsShown())
		{
			pGameSP->TalkMenu.UpdateQuestions();
		}
		//если пришло сообщение по PDA когда сам PDA не был запущен
/*		else if(!pGameSP->m_pUserMenu->IsShown())
		{
		}*/
	}

	CInventoryOwner::OnReceiveInfo(info_id);
}


void CActor::OnDisableInfo(INFO_ID info_id)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(pGameSP && pGameSP->TalkMenu.IsShown()) pGameSP->TalkMenu.UpdateQuestions();

	Level().RemoveMapLocationByInfo(info_id);
	CInventoryOwner::OnDisableInfo(info_id);
}



void CActor::ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_ID info_index)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	//визуализация в интерфейсе
	CObject* pPdaObject =  Level().Objects.net_Find(who);
	VERIFY(pPdaObject);
	CPda* pPda = dynamic_cast<CPda*>(pPdaObject);
	VERIFY(pPda);
	HUD().GetUI()->UIMainIngameWnd.ReceivePdaMessage(pPda->GetOriginalOwner(), msg, info_index);

	SPdaMessage last_pda_message;
	//bool prev_msg = 
	GetPDA()->GetLastMessageFromLog(who, last_pda_message);

	//обновить информацию о контакте
	UpdateContact(pPda->GetOriginalOwnerID());


/*	CStringTable string_table;
	ref_str text1 = string_table("test1");
	ref_str text2 = string_table("test2");
	ref_str text3 = string_table("item test2");
	ref_str text4 = string_table("item test1");
	LPCSTR str = *text4;
	text3 = string_table("item test2");
	str = *text3;
*/	
	CInventoryOwner::ReceivePdaMessage(who, msg, info_index);
}

void  CActor::ReceivePhrase		(DIALOG_SHARED_PTR& phrase_dialog)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu.IsShown())
	{
		pGameSP->TalkMenu.UpdateQuestions();
	}

	CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}

void   CActor::UpdateAvailableDialogs	(CPhraseDialogManager* partner)
{
	m_AvailableDialogs.clear();

	if(CInventoryOwner::known_info_registry.objects_ptr())
	{
		for(KNOWN_INFO_VECTOR::const_iterator it = CInventoryOwner::known_info_registry.objects_ptr()->begin();
			CInventoryOwner::known_info_registry.objects_ptr()->end() != it; ++it)
		{
			//подгрузить кусочек информации с которым мы работаем
			CInfoPortion info_portion;
			info_portion.Load((*it).id);

			for(u32 i = 0; i<info_portion.DialogNames().size(); i++)
				AddAvailableDialog(*info_portion.DialogNames()[i], partner);
		}
	}
	CPhraseDialogManager::UpdateAvailableDialogs(partner);
}

void CActor::TryToTalk()
{
	VERIFY(m_pPersonWeLookingAt);

	if(!IsTalking())
	{
		//предложить поговорить с нами
		if(m_pPersonWeLookingAt->OfferTalk(this))
		{	
			StartTalk(m_pPersonWeLookingAt);
			//только если находимся в режиме single
			CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
			if(pGameSP)pGameSP->StartTalk();
		}
	}
}

void CActor::StartTalk (CInventoryOwner* talk_partner)
{
	CGameObject* GO = dynamic_cast<CGameObject*>(talk_partner); VERIFY(GO);
	//обновить информацию о контакте
	UpdateContact(GO->ID());

	CInventoryOwner::StartTalk(talk_partner);
}

void CActor::UpdateContact		(u16 contact_id)
{
	if(ID() == contact_id) return;

	TALK_CONTACT_VECTOR& contacts = contacts_registry.objects();
	for(TALK_CONTACT_VECTOR_IT it = contacts.begin(); contacts.end() != it; ++it)
		if((*it).id == contact_id) break;

	if(contacts.end() == it)
	{
		TALK_CONTACT_DATA contact_data(contact_id, Level().GetGameTime());
		contacts.push_back(contact_data);
	}
	else
	{
		(*it).time = Level().GetGameTime();
	}
}
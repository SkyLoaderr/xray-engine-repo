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
#include "character_info.h"
#include "relation_registry.h"


#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_registry_container.h"
#include "alife_news_registry.h"

#include "script_game_object.h"

#include "game_cl_base.h"

#include "xrServer.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_registry_wrappers.h"

#include "map_manager.h"

#include "ui/UIMainIngameWnd.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "encyclopedia_article.h"
#include "GameTaskManager.h"
#include "GameTaskdefs.h"

class RemoveByIDPred
{
public:
	RemoveByIDPred(ARTICLE_ID id){object_id = id;}
	bool operator() (const ARTICLE_DATA& item)
	{
		if(item.article_id == object_id)
			return true;
		else
			return false;
	}
private:
	ARTICLE_ID object_id;
};

void CActor::AddEncyclopediaArticle	 (const CInfoPortion* info_portion) const
{
	VERIFY(info_portion);
	ARTICLE_VECTOR& article_vector = encyclopedia_registry->registry().objects();

	ARTICLE_VECTOR::iterator last_end = article_vector.end();
	ARTICLE_VECTOR::iterator B = article_vector.begin();
	ARTICLE_VECTOR::iterator E = last_end;

	for(ARTICLE_ID_VECTOR::const_iterator it = info_portion->ArticlesDisable().begin();
									it != info_portion->ArticlesDisable().end(); it++)
	{
		RemoveByIDPred pred(*it);
		last_end = std::remove_if(B, last_end, pred);
	}
	article_vector.erase(last_end, E);

	bool actor_diary_article = false;
	bool encyclopedia_article = false;

	for(ARTICLE_ID_VECTOR::const_iterator it = info_portion->Articles().begin();
									it != info_portion->Articles().end(); it++)
	{
		CEncyclopediaArticle article;

		article.Load(*it);
		if(article.data()->articleType == ARTICLE_DATA::eEncyclopediaArticle)
			encyclopedia_article = true;
		else if(article.data()->articleType == ARTICLE_DATA::eDiaryArticle)
			actor_diary_article = true;

		article_vector.push_back(ARTICLE_DATA(*it, Level().GetGameTime(), article.data()->articleType));
		LPCSTR g,n;
		g = *(article.data()->group);
		n = *(article.data()->name);
		callback(GameObject::eArticleInfo)(lua_game_object(), g, n);

	}

	if( HUD().GetUI() ){

		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP) 
			pGameSP->PdaMenu.OnNewArticleAdded();
	}
}


void CActor::AddGameTask			 (const CInfoPortion* info_portion) const
{
	VERIFY(info_portion);

	if(info_portion->GameTasks().empty()) return;
	for(TASK_ID_VECTOR::const_iterator it = info_portion->GameTasks().begin();
		it != info_portion->GameTasks().end(); it++)
	{
		GameTaskManager().GiveGameTaskToActor(*it);
	}
/*
	GAME_TASK_VECTOR& task_vector = game_task_registry->registry().objects();

	std::size_t old_size = task_vector.size();

	for(TASK_ID_VECTOR::const_iterator it = info_portion->GameTasks().begin();
		it != info_portion->GameTasks().end(); it++)
	{
		for(GAME_TASK_VECTOR::const_iterator it1 = task_vector.begin();
			it1 != task_vector.end(); it1++)
		{
			if(*it == (*it1).task_id)
				break;
		}

		if(it1 == task_vector.end()){
			task_vector.push_back(TASK_DATA(*it, Level().GetGameTime()));
		
			CGameTask task;
			task.Init(task_vector.back());
			SGameTaskObjective	*obj = NULL;
			for (u32 i = 0; i < task.ObjectivesNum(); ++i){
				obj = &task.data()->m_Objectives[i];
				if(obj->object_id!=u16(-1) && obj->map_location.size())
					Level().MapManager().AddMapLocation(obj->map_location, obj->object_id);
			}
		}
	}

	//установить флажок необходимости прочтения тасков в PDA
	if(old_size != task_vector.size())
		if(HUD().GetUI())
			HUD().GetUI()->UIMainIngameWnd->SetFlashIconState(CUIMainIngameWnd::efiPdaTask, true);

	if( HUD().GetUI() ){
		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP) 
		{
			if(pGameSP->PdaMenu.UIDiaryWnd->IsShown() &&
				pGameSP->PdaMenu.UIDiaryWnd->UIJobsWnd.IsShown())
			{
				pGameSP->PdaMenu.UIDiaryWnd->UIJobsWnd.ReloadJobs();
			}
		}
	}
*/
}


void  CActor::AddGameNews			 (GAME_NEWS_DATA& news_data)
{
	if(news_data.news_id != NOT_SIMULATION_NEWS)
	{
		const CALifeNews* pNewsItem  = ai().alife().news().news(news_data.news_id); VERIFY(pNewsItem);
		//добавляем в архив только новости
		if(ALife::eNewsTypeKill == pNewsItem->m_news_type/* || ALife::eNewsTypeRetreat == pNewsItem->m_news_type*/)
		{
			CSE_Abstract* E = Level().Server->game->get_entity_from_eid(pNewsItem->m_object_id[1]);
			CSE_ALifeTraderAbstract	 *pTA	= smart_cast<CSE_ALifeTraderAbstract*>(E); 
			if(!pTA) return;
		}
		else return;
	}
		

	GAME_NEWS_VECTOR& news_vector = game_news_registry->registry().objects();
	news_data.receive_time = Level().GetGameTime();
	news_vector.push_back(news_data);

	if(HUD().GetUI())
		HUD().GetUI()->UIMainIngameWnd->OnNewsReceived(news_data);

	if( HUD().GetUI() ){
		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP) 
		{
			if(pGameSP->PdaMenu.UIDiaryWnd->IsShown() &&
				pGameSP->PdaMenu.UIDiaryWnd->UINewsWnd.IsShown())
			{
				pGameSP->PdaMenu.UIDiaryWnd->UINewsWnd.AddNews();
				pGameSP->PdaMenu.UIDiaryWnd->MarkNewsAsRead(true);
			}
			else
				pGameSP->PdaMenu.UIDiaryWnd->MarkNewsAsRead(false);
		}
	}
}


bool CActor::OnReceiveInfo(INFO_ID info_id) const
{
	if(!CInventoryOwner::OnReceiveInfo(info_id))
		return false;

	CInfoPortion info_portion;
	info_portion.Load(info_id);

//	AddMapLocationsFromInfo	(&info_portion);
	AddEncyclopediaArticle	(&info_portion);
	AddGameTask				(&info_portion);

	callback(GameObject::eInventoryInfo)(lua_game_object(), *info_id);

	if(!HUD().GetUI())
		return false;
	//только если находимся в режиме single
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return false;

	if(pGameSP->TalkMenu.IsShown())
	{
		pGameSP->TalkMenu.NeedUpdateQuestions();
	}


	return true;
}


void CActor::OnDisableInfo(INFO_ID info_id) const
{
//	Level().RemoveMapLocationByInfo(info_index);
	CInventoryOwner::OnDisableInfo(info_id);

	if(!HUD().GetUI())
		return;

	//только если находимся в режиме single
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

/* 
	//обновить отмеки на карте, если мы прямо в карте и находимся
	if(pGameSP->PdaMenu.UIMapWnd.IsShown())
	{
		pGameSP->PdaMenu.UIMapWnd.InitGlobalMapObjectives	();
		pGameSP->PdaMenu.UIMapWnd.InitLocalMapObjectives	();
	}
*/

	if(pGameSP->TalkMenu.IsShown())
	{
		pGameSP->TalkMenu.NeedUpdateQuestions();
	}
//	else if(pGameSP->PdaMenu.UIPdaCommunication.IsShown())
//	{
//		pGameSP->PdaMenu.UIPdaCommunication.NeedUpdateQuestions();
//	}
}



void CActor::ReceivePdaMessage(u16 who, EPdaMsg msg, INFO_ID info_id)
{
	//только если находимся в режиме single
	if(!HUD().GetUI())
		return;
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	//визуализация в интерфейсе
	CObject* pPdaObject =  Level().Objects.net_Find(who);
	VERIFY(pPdaObject);
	CPda* pPda = smart_cast<CPda*>(pPdaObject);
	VERIFY(pPda);
	HUD().GetUI()->UIMainIngameWnd->ReceivePdaMessage(pPda->GetOriginalOwner(), msg, info_id);


	SPdaMessage last_pda_message;
	//bool prev_msg = 
	GetPDA()->GetLastMessageFromLog(who, last_pda_message);

	//обновить информацию о контакте
	UpdateContact(pPda->GetOriginalOwnerID());
	CInventoryOwner::ReceivePdaMessage(who, msg, info_id);
}

void  CActor::ReceivePhrase		(DIALOG_SHARED_PTR& phrase_dialog)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	if(pGameSP->TalkMenu.IsShown())
	{
		pGameSP->TalkMenu.NeedUpdateQuestions();
	}
//	else if(pGameSP->PdaMenu.UIPdaCommunication.IsShown())
//	{
//		pGameSP->PdaMenu.UIPdaCommunication.NeedUpdateQuestions();
//	}


	CPhraseDialogManager::ReceivePhrase(phrase_dialog);
}

void   CActor::UpdateAvailableDialogs	(CPhraseDialogManager* partner)
{
	m_AvailableDialogs.clear();
	m_CheckedDialogs.clear();

	if(CInventoryOwner::m_known_info_registry->registry().objects_ptr())
	{
		for(KNOWN_INFO_VECTOR::const_iterator it = CInventoryOwner::m_known_info_registry->registry().objects_ptr()->begin();
			CInventoryOwner::m_known_info_registry->registry().objects_ptr()->end() != it; ++it)
		{
			//подгрузить кусочек информации с которым мы работаем
			CInfoPortion info_portion;
			info_portion.Load((*it).info_id);

			for(u32 i = 0; i<info_portion.DialogNames().size(); i++)
				AddAvailableDialog(*info_portion.DialogNames()[i], partner);
		}
	}

	//добавить актерский диалог собеседника
	CInventoryOwner* pInvOwnerPartner = smart_cast<CInventoryOwner*>(partner); VERIFY(pInvOwnerPartner);
	
	for(u32 i = 0; i<pInvOwnerPartner->CharacterInfo().ActorDialogs().size(); i++)
		AddAvailableDialog(pInvOwnerPartner->CharacterInfo().ActorDialogs()[i], partner);

	//добавить актерские диалоги собеседника из info portions
	if(pInvOwnerPartner->m_known_info_registry->registry().objects_ptr())
	{
		for(KNOWN_INFO_VECTOR::const_iterator it = pInvOwnerPartner->m_known_info_registry->registry().objects_ptr()->begin();
			pInvOwnerPartner->m_known_info_registry->registry().objects_ptr()->end() != it; ++it)
		{
			//подгрузить кусочек информации с которым мы работаем
			CInfoPortion info_portion;
			info_portion.Load((*it).info_id);
			for(u32 i = 0; i<info_portion.ActorDialogNames().size(); i++)
				AddAvailableDialog(*info_portion.ActorDialogNames()[i], partner);
		}
	}

	CPhraseDialogManager::UpdateAvailableDialogs(partner);
}

void CActor::TryToTalk()
{
	VERIFY(m_pPersonWeLookingAt);

	if(!IsTalking())
	{
		RunTalkDialog(m_pPersonWeLookingAt);
	}
}

void CActor::RunTalkDialog(CInventoryOwner* talk_partner)
{
	//предложить поговорить с нами
	if(talk_partner->OfferTalk(this))
	{	
		StartTalk(talk_partner);
		//только если находимся в режиме single
		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP)
		{
			if(pGameSP->MainInputReceiver())
				Game().StartStopMenu(pGameSP->MainInputReceiver(),true);
			pGameSP->StartTalk();
		}
	}
}

void CActor::StartTalk (CInventoryOwner* talk_partner)
{
	CGameObject* GO = smart_cast<CGameObject*>(talk_partner); VERIFY(GO);
	//обновить информацию о контакте
	UpdateContact(GO->ID());

	CInventoryOwner::StartTalk(talk_partner);
}

void CActor::UpdateContact		(u16 contact_id)
{
	if(ID() == contact_id) return;

	TALK_CONTACT_VECTOR& contacts = contacts_registry->registry().objects();
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
void CActor::NewPdaContact		(CInventoryOwner* pInvOwner)
{	
	if(Game().Type() != GAME_SINGLE) return;
		
	HUD().GetUI()->UIMainIngameWnd->AnimateContacts();

	Level().MapManager().AddRelationLocation( pInvOwner );
}

void CActor::LostPdaContact		(CInventoryOwner* pInvOwner)
{

	CGameObject* GO = smart_cast<CGameObject*>(pInvOwner);
	if (GO){

		for(int t = ALife::eRelationTypeFriend; t<ALife::eRelationTypeLast; ++t){
			ALife::ERelationType tt = (ALife::ERelationType)t;
			Level().MapManager().RemoveMapLocation(RELATION_REGISTRY().GetSpotName(tt),	GO->ID());
		}
	};

}

void CActor::AddGameNews_deffered	 (GAME_NEWS_DATA& news_data, u32 delay)
{
	GAME_NEWS_DATA * d = xr_new<GAME_NEWS_DATA>(news_data);
	//*d = news_data;
	m_defferedMessages.push_back( SDefNewsMsg() );
	m_defferedMessages.back().news_data = d;
	m_defferedMessages.back().time = Device.dwTimeGlobal+delay;
	std::sort(m_defferedMessages.begin(), m_defferedMessages.end() );
}
void CActor::UpdateDefferedMessages()
{
	while( m_defferedMessages.size() ){
		SDefNewsMsg& M = m_defferedMessages.back();
		if(M.time <=Device.dwTimeGlobal){
			AddGameNews(*M.news_data);		
			xr_delete(M.news_data);
			m_defferedMessages.pop_back();
		}else
			break;
	}
}

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



//information receive
void CActor::OnReceiveInfo(INFO_ID info_index)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(!pGameSP) return;

	CInfoPortion info_portion;
	info_portion.Load(info_index);

	//добавить отметки на карте
	for(u32 i=0; i<info_portion.MapLocations().size(); i++)
		Level().AddMapLocation(info_portion.MapLocations()[i]);


	if(pGameSP->TalkMenu.IsShown())
	{
		if(pGameSP->TalkMenu.IsShown())
		{
			pGameSP->TalkMenu.UpdateQuestions();
		}
		//если пришло сообщение по PDA когда сам PDA не был запущен
		else if(!pGameSP->m_pUserMenu->IsShown())
		{
		}
	}

	CInventoryOwner::OnReceiveInfo(info_index);
}


void CActor::OnDisableInfo(INFO_ID info_index)
{
	//только если находимся в режиме single
	CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(pGameSP && pGameSP->TalkMenu.IsShown()) pGameSP->TalkMenu.UpdateQuestions();

	Level().RemoveMapLocationByInfo(info_index);
	CInventoryOwner::OnDisableInfo(info_index);
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

	for(KNOWN_INFO_VECTOR::const_iterator it = CInventoryOwner::KnownInfo().begin();
								CInventoryOwner::KnownInfo().end() != it; ++it)
	{
		//подгрузить кусочек информации с которым мы работаем
		CInfoPortion info_portion;
		info_portion.Load((*it).id);
	
		for(u32 i = 0; i<info_portion.DialogNames().size(); i++)
			AddAvailableDialog(*info_portion.DialogNames()[i], partner);
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

	TALK_CONTACT_VECTOR& contacts = Contacts();
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

//возвращает существующий вектор из реестра, или добавляет новый
TALK_CONTACT_VECTOR& CActor::Contacts		()
{
#ifdef _DEBUG
	if(NULL == ai().get_alife()) 
		return m_ContactsWithoutAlife;
#endif	

	TALK_CONTACT_VECTOR* talk_contact_vector = ai().alife().registry(known_contacts).object(ID(), true);

	if(!talk_contact_vector)	
	{
		TALK_CONTACT_VECTOR new_contact_vector;
		ai().alife().registry(known_contacts).add(ID(), new_contact_vector, false);
		talk_contact_vector = ai().alife().registry(known_contacts).object(ID(), true);
		VERIFY(talk_contact_vector);
	}
	return *talk_contact_vector;
}

//возвращает NULL, если вектора с контактами не добавлено
const TALK_CONTACT_VECTOR*	CActor::ContactsPtr		() const
{
#ifdef _DEBUG
	if(NULL == ai().get_alife()) 
		return &m_ContactsWithoutAlife;
#endif	

	TALK_CONTACT_VECTOR* talk_contact_vector = ai().alife().registry(known_contacts).object(ID(), true);
	return talk_contact_vector;
}

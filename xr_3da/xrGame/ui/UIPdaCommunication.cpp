//-----------------------------------------------------------------------------/
//  Коммуникационный поддиалог PDA
//-----------------------------------------------------------------------------/

#include "stdafx.h"
#include "UIPdaCommunication.h"
#include "../Pda.h"
#include "UIPdaWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../HUDManager.h"
#include "../actor.h"
#include "../level.h"
#include "../character_info.h"

#include "../PhraseDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPdaCommunication::CUIPdaCommunication()
{
	Show(false);

	m_pInvOwner = NULL;
	m_pPda = NULL;

	SetFont(HUD().pFontMedium);

	m_pOurDialogManager = m_pOthersDialogManager = NULL;

	ToTopicMode();
}

CUIPdaCommunication::~CUIPdaCommunication()
{
}

void CUIPdaCommunication::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$",PDA_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

//	AttachChild(&UIStaticTop);
//	UIStaticTop.Init("ui\\ui_top_background", 0,0,1024,128);
//	AttachChild(&UIStaticBottom);
//	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

//	AttachChild(&UIMainPdaFrame);
//	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainPdaFrame);
//
//	AttachChild(&UIPDAHeader);
//	xml_init.InitStatic(uiXml, "static", 0, &UIPDAHeader);

	//окно разговора по PDA
	AttachChild(&UIPdaDialogWnd);
	UIPdaDialogWnd.Init(0, 0, GetWidth(), GetHeight());
	UIPdaDialogWnd.Show(false);

	//список контактов
	AttachChild(&UIPdaContactsWnd);
	UIPdaContactsWnd.Init(0,0, GetWidth(), GetHeight());
	UIPdaContactsWnd.Show(true);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);
}

void CUIPdaCommunication::InitPDACommunication()
{
	m_pInvOwner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) return;

	m_pPda = m_pInvOwner->GetPDA();

	if(!m_pPda)	return;

	InitPdaContacts();

	UIPdaDialogWnd.Show(false);
	UIPdaContactsWnd.Show(true);
}


void CUIPdaCommunication::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UIPdaContactsWnd)
	{
		if(msg == PDA_CONTACTS_WND_CONTACT_SELECTED)
		{
			UIPdaContactsWnd.Show(false);
			UIPdaDialogWnd.ContactRestore();

			InitPdaDialog();
			UIPdaDialogWnd.Show(true);
		}
	}
	else if(pWnd == &UIPdaDialogWnd)
	{
		/*if(msg == CUIPdaDialogWnd::BACK_BUTTON_CLICKED)
		{
			UIPdaContactsWnd.Show(true);
			UIPdaDialogWnd.Show(false);
		}
		else 
		*/
		if(msg == PDA_DIALOG_WND_MESSAGE_BUTTON_CLICKED)
		{
			EPdaMsg pda_msg = ePdaMsgDialog;
			u32 id_pda_contact = m_pContactPda->ID();
			/*
			if(m_pPda->NeedToAnswer(id_pda_contact))
			{
				switch(UIPdaDialogWnd.m_iMsgNum)
				{
				case 0:
					pda_msg = ePdaMsgAccept;
					break;
				case 1:
					pda_msg = ePdaMsgDecline;
					break;
				case 2:
					pda_msg = ePdaMsgDeclineRude;
					break;
				}
			}
			else
			{
				switch(UIPdaDialogWnd.m_iMsgNum)
				{
				case 0:
					pda_msg = ePdaMsgTrade;
					break;
				case 1:
					pda_msg = ePdaMsgGoAway;					
					break;
				case 2:
					pda_msg = ePdaMsgNeedHelp;
					break;
				}
			}*/
			m_pPda->SendMessageID(id_pda_contact, pda_msg, NO_INFO_INDEX);
			
			AskQuestion();
			UpdateMessageLog();
		}
	}
	inherited::SendMessage(pWnd, msg, pData);
}


void CUIPdaCommunication::Update()
{
	if(!m_pPda) return;

	//обновлять список контактов вне зависимости
	//от того показано окно контактов или нет
	UpdatePdaContacts();

	/*
	if(UIPdaDialogWnd.IsShown() && 
		m_pPda->IsNewMessage())
	{
		UpdateMessageLog();
		UpdateMsgButtons();

		m_pPda->NoNewMessage();
	}*/

	inherited::Update();
}

void CUIPdaCommunication::Draw()
{
	inherited::Draw();
}

void CUIPdaCommunication::Show(bool status)
{
	inherited::Show(status);
	if (status)
		InitPDACommunication();
}


/////////////////////////////////////////
// Функции работы с котактами
////////////////////////////////////////
void CUIPdaCommunication::InitPdaContacts()
{
	UIPdaContactsWnd.RemoveAll();

	PDA_LIST_it it;
	for(it = m_pPda->m_PDAList.begin(); m_pPda->m_PDAList.end() != it; ++it)
	{
		UIPdaContactsWnd.AddContact(*it);
	}
}

//обновление списка активных контактов PDA
void CUIPdaCommunication::UpdatePdaContacts()
{
	PDA_LIST_it it;	

	//удалить из списка все PDA ушедшие из зоны досягаемости
	for(it = m_pPda->m_DeletedPDAList.begin(); m_pPda->m_DeletedPDAList.end() != it; ++it)
	{	
		UIPdaContactsWnd.RemoveContact(*it);

		//текущий контак вышел из зоны досягаемости!
		if(m_pContactPda == *it)
		{
			UIPdaDialogWnd.ContactLoss();
		}
	}

	
	//добавить новые
	for(it = m_pPda->m_NewPDAList.begin(); m_pPda->m_NewPDAList.end() != it; ++it)
	{	
		//только если объекта еще нет в списке
		if(UIPdaContactsWnd.IsInList(*it)==false)
			UIPdaContactsWnd.AddContact(*it);

		//текущий контак снова вошел в зону досягаемости
		if(m_pContactPda == *it)
		{
			UIPdaDialogWnd.ContactRestore();
		}
	}
}

/////////////////////////////////////////
// Функции работы для поддержания диалога
////////////////////////////////////////

void CUIPdaCommunication::InitPdaDialog()
{
	m_idContact = UIPdaContactsWnd.GetContactID();

	m_pContactObject =  Level().Objects.net_Find(m_idContact);

	R_ASSERT2(m_pContactObject, "wrong ID");
	m_pOthersInvOwner = m_pContactInvOwner = smart_cast<CInventoryOwner*>(m_pContactObject);
	R_ASSERT2(m_pContactInvOwner, "can't cast to inventory owner");
	m_pContactPda = m_pContactInvOwner->GetPDA();
	VERIFY(m_pContactPda);

	m_pOurInvOwner = m_pInvOwner  = smart_cast<CInventoryOwner*>(Level().CurrentEntity());;
	R_ASSERT2(m_pInvOwner, "wrong inventory owner");

	//инициализировать окошко с информацией о собеседнике
	UIPdaDialogWnd.UICharacterInfo.InitCharacter(m_pContactInvOwner);

	m_pActor  = smart_cast<CActor*>(Level().CurrentEntity());;
	if(m_pActor)
	{
		CEntityAlive* ContactEA = smart_cast<CEntityAlive*>(m_pContactInvOwner);
		UIPdaDialogWnd.UICharacterInfo.SetRelation(ContactEA->tfGetRelationType(smart_cast<CEntityAlive*>(m_pActor)));
	}

	m_pOurDialogManager = smart_cast<CPhraseDialogManager*>(m_pOurInvOwner);
	m_pOthersDialogManager = smart_cast<CPhraseDialogManager*>(m_pOthersInvOwner);

	ToTopicMode			();
	UpdateMessageLog	();
	UpdateMsgButtons	();

	std::string		buf;
	string128		buf2;
	buf = UIPdaDialogWnd.UICharacterInfo.UIName.GetText();
	UIPdaDialogWnd.UICharIconHeader.UITitleText.SetText(buf.c_str());
	strconcat(buf2, ALL_PDA_HEADER_PREFIX, PDA_CONTACTS_HEADER_SUFFIX, "/", buf.c_str());
	UIPdaDialogWnd.UIMsglogHeader.UITitleText.SetText(buf2);
}

//заполнить окно логами сообщений с текущим контактом
void CUIPdaCommunication::UpdateMessageLog()
{
	//сказать фразу
	CUIString speaker_name;

	//добавить ответ собеседника в список, если он что-то сказал
	if(m_pCurrentDialog && m_pCurrentDialog->GetLastPhraseID() !=  m_iLastPhraseID)
	{
		m_iLastPhraseID = m_pCurrentDialog->GetLastPhraseID();
		speaker_name.SetText(m_pOthersInvOwner->CharacterInfo().Name());
		AddAnswer(m_pCurrentDialog->GetLastPhraseText(), speaker_name);
	}

/*	UIPdaDialogWnd.UILogListWnd.RemoveAll();

	u32 id_pda_contact;

		id_pda_contact = m_pContactPda->ID();

	//котактов еще не было
	if(m_pPda->m_mapPdaLog.find(id_pda_contact) == m_pPda->m_mapPdaLog.end()) return;

	//	for(CPda::PDA_MESSAGE_LIST_it it = m_pPda->m_mapPdaLog[id_pda_contact].begin();
	//							      m_pPda->m_mapPdaLog[id_pda_contact].end() != it;
	//							      ++it)
	for(CPda::PDA_MESSAGE_LIST::reverse_iterator it = m_pPda->m_mapPdaLog[id_pda_contact].rbegin();
		m_pPda->m_mapPdaLog[id_pda_contact].rend() != it;
		++it)
	{
		if((*it).receive)
			UIPdaDialogWnd.AddOthersMessageToLog((*it).msg, m_pContactInvOwner);
		else
			UIPdaDialogWnd.AddOurMessageToLog((*it).msg, m_pInvOwner);
	}
*/
	UIPdaDialogWnd.UILogListWnd.ScrollToEnd();
}


//оновления состояния диалога с контактом:
//ожидание ответа, задавание вопроса,
//ответ на вопрос
//и обновление кнопок ответа
void CUIPdaCommunication::UpdateMsgButtons()
{
/*	u32 id_pda_contact = 0xffff;

	id_pda_contact = m_pContactPda->ID();

	if(m_pPda->WaitForReply(id_pda_contact))
	{
		UIPdaDialogWnd.ContactWaitForReply();
	}
	else
	{
		UIPdaDialogWnd.ContactRestore();
	}*/
	UpdateQuestions();
}



void CUIPdaCommunication::UpdateDisplay()
{ 
	UIPdaDialogWnd.ContactRestore();
	UpdateMessageLog();
	UpdateMsgButtons();
}

const int MessageShift = 30;

// Функции добавления строк в листы вопросов и ответов
void CUIPdaCommunication::AddQuestion(CUIString str, void* pData, int value)
{
	CUIStatic::PreprocessText(str.m_str, UIPdaDialogWnd.UIPhrasesListWnd.GetWidth() - MessageShift - 25, 
		UIPdaDialogWnd.UIPhrasesListWnd.GetFont());
	UIPdaDialogWnd.UIPhrasesListWnd.AddParsedItem<CUIListItem>(str, 0, 
		UIPdaDialogWnd.UIPhrasesListWnd.GetTextColor(), 
		UIPdaDialogWnd.UIPhrasesListWnd.GetFont(), pData, value);
}

void CUIPdaCommunication::AddAnswer(CUIString str, const CUIString &SpeakerName)
{
		//для пустой фразы вообще ничего не выводим
	if(xr_strlen((LPCSTR)str) == 0) return;

	// Делаем препроцессинг строки
	CUIStatic::PreprocessText(str.m_str, UIPdaDialogWnd.UILogListWnd.GetWidth() - MessageShift - 25, // 20 means approximate scrollbar width value
							  UIPdaDialogWnd.UILogListWnd.GetFont());

	u32 cl = UIPdaDialogWnd.UILogListWnd.GetTextColor();
	if (0 == xr_strcmp(SpeakerName.GetBuf(), m_pOurInvOwner->CharacterInfo().Name())) 
		cl = UIPdaDialogWnd.GetOurReplicsColor();

	UIPdaDialogWnd.UILogListWnd.AddParsedItem<CUIListItem>(SpeakerName, 0, 
		UIPdaDialogWnd.GetHeaderColor(), UIPdaDialogWnd.GetHeaderFont());
	UIPdaDialogWnd.UILogListWnd.AddParsedItem<CUIListItem>(str, MessageShift, cl);

	CUIString Local;
	Local.SetText(" ");
	UIPdaDialogWnd.UILogListWnd.AddParsedItem<CUIListItem>(Local, 0, 
		UIPdaDialogWnd.UILogListWnd.GetTextColor());
	UIPdaDialogWnd.UILogListWnd.ScrollToEnd();
}

//////////////////////////////////////////////////////////////////////////

bool  CUIPdaCommunication::TopicMode			() 
{
	return NULL == m_pCurrentDialog.get();
}

//////////////////////////////////////////////////////////////////////////

void  CUIPdaCommunication::ToTopicMode		() 
{
	m_pCurrentDialog = DIALOG_SHARED_PTR((CPhraseDialog*)NULL);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaCommunication::AskQuestion()
{
	//очистить лог сообщений
	PHRASE_ID phrase_id;

	//игрок выбрал тему разговора
	if(TopicMode())
	{
		m_pCurrentDialog = m_pOurDialogManager->AvailableDialogs()[UIPdaDialogWnd.m_iClickedQuestion];
		
		m_pOurDialogManager->InitDialog(m_pOthersDialogManager, m_pCurrentDialog);
		phrase_id = START_PHRASE;
	}
	else
	{
		phrase_id = (PHRASE_ID)UIPdaDialogWnd.m_iClickedQuestion;
	}

	SayPhrase(phrase_id);
	UpdateQuestions();
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaCommunication::SayPhrase(PHRASE_ID phrase_id)
{
	//сказать фразу
	CUIString speaker_name;
	speaker_name.SetText(m_pOurInvOwner->CharacterInfo().Name());

	AddAnswer(m_pCurrentDialog->GetPhraseText(phrase_id), speaker_name);
	m_pOurDialogManager->SayPhrase(m_pCurrentDialog, phrase_id);

	m_iLastPhraseID = phrase_id;

	//если диалог завершился, перейти в режим выбора темы
	if(m_pCurrentDialog->IsFinished()) 
		ToTopicMode();
	else
		//добавить табличку, что мы ждем ответа
		UIPdaDialogWnd.ContactWaitForReply();
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaCommunication::UpdateQuestions()
{
	UIPdaDialogWnd.UIPhrasesListWnd.RemoveAll();
	R_ASSERT2(m_pOurInvOwner->GetPDA(), "PDA for character does not init yet");

	//если нет активного диалога, то
	//режима выбора темы
	if(!m_pCurrentDialog)
	{
		m_pOurDialogManager->UpdateAvailableDialogs(m_pOthersDialogManager);
		for(u32 i=0; i< m_pOurDialogManager->AvailableDialogs().size(); i++)
		{
			const DIALOG_SHARED_PTR& phrase_dialog = m_pOurDialogManager->AvailableDialogs()[i];
			if(phrase_dialog->GetDialogType() == eDialogTypePDA)
				AddQuestion(phrase_dialog->DialogCaption(), NULL, (int)i);
		}
	}
	else
	{
		if(m_pCurrentDialog->IsWeSpeaking(m_pOurDialogManager))
		{
			//если в списке допустимых фраз только одна фраза пустышка, то просто
			//сказать (игрок сам не производит никаких действий)
			if(m_pCurrentDialog->PhraseList().size() == 1)
			{
				CPhrase* phrase = m_pCurrentDialog->PhraseList().front();
				if(phrase->IsDummy()) SayPhrase(phrase->GetIndex());
			}

			//выбор доступных фраз из активного диалога
			if(m_pCurrentDialog)
			{			
				for(PHRASE_VECTOR::const_iterator   it = m_pCurrentDialog->PhraseList().begin();
					it != m_pCurrentDialog->PhraseList().end();
					it++)
				{
					CPhrase* phrase = *it;
					AddQuestion(phrase->GetText(), NULL, (PHRASE_ID)phrase->GetIndex());
				}
			}
			else
				UpdateQuestions();
		}
	}
}
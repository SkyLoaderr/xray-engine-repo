//-----------------------------------------------------------------------------/
//  Коммуникационный поддиалог PDA
//-----------------------------------------------------------------------------/

#include "stdafx.h"
#include "UIPdaCommunication.h"
#include "../Pda.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../HUDManager.h"
#include "../actor.h"
#include "../level.h"

#define PDA_XML "pda.xml"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPdaCommunication::CUIPdaCommunication()
{
	Hide();

	m_pInvOwner = NULL;
	m_pPda = NULL;

	SetFont(HUD().pFontMedium);
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
	UIPdaDialogWnd.Hide();

	//список контактов
	AttachChild(&UIPdaContactsWnd);
	UIPdaContactsWnd.Init(0,0, GetWidth(), GetHeight());
	UIPdaContactsWnd.Show();

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);
}

void CUIPdaCommunication::InitPDA()
{
	m_pInvOwner = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!m_pInvOwner) return;

	m_pPda = m_pInvOwner->GetPDA();

	if(!m_pPda)	return;

	InitPdaContacts();

	UIPdaDialogWnd.Hide();
	UIPdaContactsWnd.Show();
}


void CUIPdaCommunication::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UIPdaContactsWnd)
	{
		if(msg == CUIPdaContactsWnd::CONTACT_SELECTED)
		{
			UIPdaContactsWnd.Hide();
			UIPdaDialogWnd.ContactRestore();

			InitPdaDialog();
			UIPdaDialogWnd.Show();
		}
	}
	else if(pWnd == &UIPdaDialogWnd)
	{
		if(msg == CUIPdaDialogWnd::BACK_BUTTON_CLICKED)
		{
			UIPdaContactsWnd.Show();
			UIPdaDialogWnd.Hide();
		}
		else if(msg == CUIPdaDialogWnd::MESSAGE_BUTTON_CLICKED)
		{
			EPdaMsg pda_msg = ePdaMsgAccept;
			u32 id_pda_contact = m_pContactPda->ID();

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
			}
			m_pPda->SendMessageID(id_pda_contact, pda_msg, NO_INFO_INDEX);
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


	if(UIPdaDialogWnd.IsShown() && 
		m_pPda->IsNewMessage())
	{
		UpdateMessageLog();
		UpdateMsgButtons();

		m_pPda->NoNewMessage();
	}

	inherited::Update();
}

void CUIPdaCommunication::Draw()
{
	inherited::Draw();
}

void CUIPdaCommunication::Show()
{
	inherited::Show();
	InitPDA();
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
	m_pContactInvOwner = dynamic_cast<CInventoryOwner*>(m_pContactObject);
	R_ASSERT2(m_pContactInvOwner, "can't cast to inventory owner");
	m_pContactPda = m_pContactInvOwner->GetPDA();
	VERIFY(m_pContactPda);

	//что за глюк?! херит память если выводить m_pContactObject->cName()

	//	sprintf(UIPdaDialogWnd.m_sContactName, "%s", *m_pContactObject->cName());
	//	UIPdaDialogWnd.UIStaticContactName.SetText(UIPdaDialogWnd.m_sContactName);

	m_pInvOwner  = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());;
	R_ASSERT2(m_pInvOwner, "wrong inventory owner");

	//инициализировать окошко с информацие о собеседнике
	UIPdaDialogWnd.UICharacterInfo.InitCharacter(m_pContactInvOwner);

	CActor* pActor  = dynamic_cast<CActor*>(Level().CurrentEntity());;
	if(pActor)
	{
		CEntityAlive* ContactEA = dynamic_cast<CEntityAlive*>(m_pContactInvOwner);
		UIPdaDialogWnd.UICharacterInfo.SetRelation(ContactEA->tfGetRelationType(dynamic_cast<CEntityAlive*>(pActor)));
	}

	UpdateMessageLog();
	UpdateMsgButtons();
}

//заполнить окно логами сообщений с текущим контактом
void CUIPdaCommunication::UpdateMessageLog()
{
	UIPdaDialogWnd.UILogListWnd.RemoveAll();

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

	UIPdaDialogWnd.UILogListWnd.ScrollToEnd();
}


//оновления состояния диалога с контактом:
//ожидание ответа, задавание вопроса,
//ответ на вопрос
//и обновление кнопок ответа
void CUIPdaCommunication::UpdateMsgButtons()
{
	u32 id_pda_contact = 0xffff;

	id_pda_contact = m_pContactPda->ID();


	if(m_pPda->WaitForReply(id_pda_contact))
	{
		UIPdaDialogWnd.ContactWaitForReply();
	}
	else
	{
		UIPdaDialogWnd.ContactRestore();
	}


	if(m_pPda->NeedToAnswer(id_pda_contact))
	{
		UIPdaDialogWnd.PhrasesAnswer();
	}
	else
	{
		UIPdaDialogWnd.PhrasesAsk();
	}
}
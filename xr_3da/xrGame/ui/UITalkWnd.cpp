// UITalkWnd.cpp:  ������ ��� ������� ����������
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UITalkWnd.h"

#include "../actor.h"
#include "../HUDManager.h"
#include "../UIGameSP.h"
#include "../PDA.h"
#include "../character_info.h"
#include "../level.h"

#include "../PhraseDialog.h"
#include "../PhraseDialogManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const u32 NameTextColor = 0xff00ff00;
const u32 MsgTextColor  = 0xffffffff;
const char MessageShift = 2;




CUITalkWnd::CUITalkWnd()
{
	m_pActor = NULL;

	m_pOurInvOwner = NULL;
	m_pOthersInvOwner = NULL;

	m_pOurDialogManager = NULL;
	m_pOthersDialogManager = NULL;

	ToTopicMode();

	Init();
	Hide();
	SetFont(HUD().pFontHeaderRussian);
}

CUITalkWnd::~CUITalkWnd()
{
}

void CUITalkWnd::Init()
{
	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	/////////////////////////
	//���� ���������
	AttachChild(&UITalkDialogWnd);
	UITalkDialogWnd.Init(0,0, Device.dwWidth, Device.dwHeight);

	/////////////////////////
	//���� ��������
	AttachChild(&UITradeWnd);
	UITradeWnd.Hide();
}



void CUITalkWnd::InitTalkDialog()
{
	m_pActor = dynamic_cast<CActor *>(Level().CurrentEntity());
	if (m_pActor && !m_pActor->IsTalking()) return;

	m_pOurInvOwner = dynamic_cast<CInventoryOwner*>(m_pActor);
	m_pOthersInvOwner = m_pActor->GetTalkPartner();

	m_pOurDialogManager = dynamic_cast<CPhraseDialogManager*>(m_pOurInvOwner);
	m_pOthersDialogManager = dynamic_cast<CPhraseDialogManager*>(m_pOthersInvOwner);

	//����� ������������
	UITalkDialogWnd.UICharacterInfoLeft.InitCharacter(m_pOurInvOwner);
	UITalkDialogWnd.UICharacterInfoRight.InitCharacter(m_pOthersInvOwner);
	UITalkDialogWnd.UICharacterName.SetText(m_pOurInvOwner->CharacterInfo().Name());
	
	CEntityAlive* ContactEA = dynamic_cast<CEntityAlive*>(m_pOthersInvOwner);
	UITalkDialogWnd.UICharacterInfoRight.SetRelation(ContactEA->tfGetRelationType(m_pActor));

	//�������� ��� ���������
	UITalkDialogWnd.UIAnswersList.RemoveAll();

	InitOthersStartDialog();
	UpdateQuestions();

	UITalkDialogWnd.Show();
	UITradeWnd.Hide();
}

void CUITalkWnd::InitOthersStartDialog()
{
	m_pOthersDialogManager->UpdateAvailableDialogs(m_pOurDialogManager);
	if(!m_pOthersDialogManager->AvailableDialogs().empty())
	{
		m_pCurrentDialog = m_pOthersDialogManager->AvailableDialogs().front();
		m_pOthersDialogManager->InitDialog(m_pOurDialogManager, m_pCurrentDialog);
		
		//������� �����
		CUIString speaker_name;
		speaker_name.SetText(m_pOthersInvOwner->CharacterInfo().Name());
		AddAnswer(m_pCurrentDialog->GetPhraseText(START_PHRASE), speaker_name);
		m_pOthersDialogManager->SayPhrase(m_pCurrentDialog, START_PHRASE);

		//���� ������ ����������, ������� � ����� ������ ����
		if(m_pCurrentDialog->IsFinished()) ToTopicMode();
	}
}

void CUITalkWnd::UpdateQuestions()
{
	UITalkDialogWnd.UIQuestionsList.RemoveAll();
	R_ASSERT2(m_pOurInvOwner->GetPDA(), "PDA for character does not init yet");

	//���� ��� ��������� �������, ��
	//������ ������ ����
	if(!m_pCurrentDialog)
	{
		m_pOurDialogManager->UpdateAvailableDialogs(m_pOthersDialogManager);
		for(u32 i=0; i< m_pOurDialogManager->AvailableDialogs().size(); i++)
		{
			const DIALOG_SHARED_PTR& phrase_dialog = m_pOurDialogManager->AvailableDialogs()[i];
			AddQuestion(phrase_dialog->DialogCaption(), NULL, (int)i);
		}
	}
	else
	{
		if(m_pCurrentDialog->IsWeSpeaking(m_pOurDialogManager))
		{
			//���� � ������ ���������� ���� ������ ���� ����� ��������, �� ������
			//������� (����� ��� �� ���������� ������� ��������)
			if(m_pCurrentDialog->PhraseList().size() == 1)
			{
				CPhrase* phrase = m_pCurrentDialog->PhraseList().front();
				if(xr_strlen(phrase->GetText()) == 0) SayPhrase(phrase->GetIndex());
			}

			//����� ��������� ���� �� ��������� �������
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

void CUITalkWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UITalkDialogWnd && msg == CUITalkDialogWnd::TRADE_BUTTON_CLICKED)
	{
		UITalkDialogWnd.Hide();

		UITradeWnd.InitTrade(m_pOurInvOwner, m_pOthersInvOwner);
		UITradeWnd.Show();
		UITradeWnd.BringAllToTop();
	}
	else if(pWnd == &UITalkDialogWnd && msg == CUITalkDialogWnd::QUESTION_CLICKED)
	{
		AskQuestion();
	}
	else if(pWnd == &UITradeWnd && msg == CUITradeWnd::TRADE_WND_CLOSED)
	{
		UITalkDialogWnd.Show();
		UITradeWnd.Hide();
	}

	inherited::SendMessage(pWnd, msg, pData);
}


void CUITalkWnd::Update()
{
	//���������� ��������, ���� �����
	if (m_pActor && !m_pActor->IsTalking())
	{
		CUIGameSP* pGameSP = dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP) pGameSP->StartStopMenu(this);
	}

	inherited::Update();
}

void CUITalkWnd::Draw()
{
	inherited::Draw();
}

void CUITalkWnd::Show()
{
	InitTalkDialog();
	inherited::Show();
}

void CUITalkWnd::Hide()
{
	inherited::Hide();

	if(!m_pActor) return;
	
	ToTopicMode();

	if (m_pActor->IsTalking()) m_pActor->StopTalk();
	m_pActor = NULL;
}


bool  CUITalkWnd::TopicMode			() 
{
	return NULL == m_pCurrentDialog.get();
}
void  CUITalkWnd::ToTopicMode		() 
{
	m_pCurrentDialog = DIALOG_SHARED_PTR((CPhraseDialog*)NULL);
}


void CUITalkWnd::AskQuestion()
{

	//�������� ��� ���������
	//UITalkDialogWnd.UIAnswersList.RemoveAll();
	PHRASE_ID phrase_id;

	//����� ������ ���� ���������
	if(TopicMode())
	{
		m_pCurrentDialog = m_pOurDialogManager->AvailableDialogs()[UITalkDialogWnd.m_iClickedQuestion];
		
		m_pOurDialogManager->InitDialog(m_pOthersDialogManager, m_pCurrentDialog);
		phrase_id = START_PHRASE;
	}
	else
	{
		phrase_id = (PHRASE_ID)UITalkDialogWnd.m_iClickedQuestion;
	}

	SayPhrase(phrase_id);
	UpdateQuestions();
}


void CUITalkWnd::SayPhrase(PHRASE_ID phrase_id)
{
	//������� �����
	CUIString speaker_name;
	speaker_name.SetText(m_pOurInvOwner->CharacterInfo().Name());

	AddAnswer(m_pCurrentDialog->GetPhraseText(phrase_id), speaker_name);
	m_pOurDialogManager->SayPhrase(m_pCurrentDialog, phrase_id);

	//�������� ����� ����������� � ������, ���� �� ���-�� ������
	if(m_pCurrentDialog->GetLastPhraseID() !=  phrase_id)
	{
		speaker_name.SetText(m_pOthersInvOwner->CharacterInfo().Name());
		AddAnswer(m_pCurrentDialog->GetLastPhraseText(), speaker_name);
	}

	//���� ������ ����������, ������� � ����� ������ ����
	if(m_pCurrentDialog->IsFinished()) ToTopicMode();
}


void CUITalkWnd::AddQuestion(const CUIString &str, void* pData , int value)
{
	UITalkDialogWnd.UIQuestionsList.AddParsedItem(str, 0, UITalkDialogWnd.UIQuestionsList.GetTextColor(), 
						UITalkDialogWnd.UIQuestionsList.GetFont(), pData, value);
}

void CUITalkWnd::AddAnswer(const CUIString &str, const CUIString &SpeakerName)
{
	//��� ������ ����� ������ ������ �� �������
	if(xr_strlen((LPCSTR)str) == 0) return;

	UITalkDialogWnd.UIAnswersList.AddParsedItem(SpeakerName, 0, UITalkDialogWnd.GetHeaderColor(), UITalkDialogWnd.GetHeaderFont());
	UITalkDialogWnd.UIAnswersList.AddParsedItem(str, MessageShift, UITalkDialogWnd.UIAnswersList.GetTextColor());

	CUIString Local;
	Local.SetText("----------------");
	UITalkDialogWnd.UIAnswersList.AddParsedItem(Local, 0, UITalkDialogWnd.UIAnswersList.GetTextColor());
	UITalkDialogWnd.UIAnswersList.ScrollToEnd();
}
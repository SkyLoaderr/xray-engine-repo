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
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const u32 NameTextColor = 0xff00ff00;
const u32 MsgTextColor  = 0xffffffff;
const char MessageShift = 2;

CUITalkWnd::CUITalkWnd()
{
	Init();

	Hide();
	
	SetFont(HUD().pFontHeaderRussian);

	m_pOurInvOwner = NULL;
	m_pOthersInvOwner = NULL;
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
	CActor *pActor = dynamic_cast<CActor *>(Level().CurrentEntity());

	if (pActor && !pActor->IsTalking()) return;


	m_pOurInvOwner = dynamic_cast<CInventoryOwner*>(pActor);
	m_pOthersInvOwner = pActor->GetTalkPartner();


	//����� ������������
	UITalkDialogWnd.UICharacterInfoLeft.InitCharacter(m_pOurInvOwner);
	UITalkDialogWnd.UICharacterInfoRight.InitCharacter(m_pOthersInvOwner);
	UITalkDialogWnd.UICharacterName.SetText(m_pOurInvOwner->CharacterInfo().Name());
	
	CEntityAlive* ContactEA = dynamic_cast<CEntityAlive*>(m_pOthersInvOwner);
	UITalkDialogWnd.UICharacterInfoRight.SetRelation(ContactEA->tfGetRelationType(pActor));

	
	UpdateQuestions();

	//�������� ��� ���������
	UITalkDialogWnd.UIAnswersList.RemoveAll();


	UITalkDialogWnd.Show();
	UITradeWnd.Hide();
}

void CUITalkWnd::UpdateQuestions()
{
	UITalkDialogWnd.UIQuestionsList.RemoveAll();

	R_ASSERT2(m_pOurInvOwner->GetPDA(), "PDA for character does not init yet");

	//�������� ��������� ������� � ��������� ��� ������
	m_pOurInvOwner->GetPDA()->UpdateQuestions();
	
	for(INFO_QUESTIONS_LIST_it it = m_pOurInvOwner->GetPDA()->m_ActiveQuestionsList.begin();
		m_pOurInvOwner->GetPDA()->m_ActiveQuestionsList.end() != it;
		++it)
	{
		SInfoQuestion* pQuestion = &(*it);
//		CUIString CharName;
//		CharName.SetText(m_pOurInvOwner->GetGameName());
//		UITalkDialogWnd.AddMessageToLog();
//		UITalkDialogWnd.UIQuestionsList.AddItem(pQuestion->text, pQuestion);
		AddQuestion(pQuestion->text, pQuestion);
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
	CActor *pActor = dynamic_cast<CActor *>(Level().CurrentEntity());

	if (pActor && !pActor->IsTalking())
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

	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;


	if (pActor->IsTalking())
	{
		pActor->StopTalk();
	}
}

void CUITalkWnd::AskQuestion()
{

	INFO_INDEX_LIST index_list;
	bool result = m_pOthersInvOwner->AskQuestion(*UITalkDialogWnd.m_pClickedQuestion,
												 index_list);
	CUIString str, SpeakerName;

	//�������� ��� ���������
	UITalkDialogWnd.UIAnswersList.RemoveAll();

	SpeakerName.SetText(m_pOurInvOwner->CharacterInfo().Name());
	AddAnswer(UITalkDialogWnd.m_pClickedQuestion->text, SpeakerName);
//	UITalkDialogWnd.AddMessageToLog(SpeakerName, UITalkDialogWnd.m_pClickedQuestion->text, &UITalkDialogWnd.UIAnswersList);
	SpeakerName.SetText(m_pOthersInvOwner->CharacterInfo().Name());

	if(!result)
	{
		//UITalkDialogWnd.UIAnswer.SetText(UITalkDialogWnd.m_pClickedQuestion->
		//										negative_answer_text.GetBuf());
		AddAnswer(UITalkDialogWnd.m_pClickedQuestion->negative_answer_text, SpeakerName);
//		UITalkDialogWnd.AddMessageToLog(SpeakerName, UITalkDialogWnd.m_pClickedQuestion->negative_answer_text, &UITalkDialogWnd.UIAnswersList);
	}
	else
	{
		CInfoPortion info_portion; 	

		for(INFO_INDEX_LIST_it it = index_list.begin(); 
							   index_list.end() != it; ++it)
		{
			info_portion.Load(*it);
			str.AppendText(info_portion.GetText());
			str.AppendText("\\n");
		}

		//UITalkDialogWnd.AddMessageToLog(SpeakerName, str, &UITalkDialogWnd.UIAnswersList);
		AddAnswer(str, SpeakerName);
		//UITalkDialogWnd.UIAnswer.SetText(str);

		//UpdateQuestions();
	}
}

void CUITalkWnd::AddQuestion(const CUIString &str, void* pData)
{
	UITalkDialogWnd.UIQuestionsList.AddParsedItem(str, 0, UITalkDialogWnd.UIQuestionsList.GetTextColor(), 
		UITalkDialogWnd.UIQuestionsList.GetFont(), pData);
}

void CUITalkWnd::AddAnswer(const CUIString &str, const CUIString &SpeakerName)
{
	UITalkDialogWnd.UIAnswersList.AddParsedItem(SpeakerName, 0, UITalkDialogWnd.GetHeaderColor(), UITalkDialogWnd.GetHeaderFont());
	UITalkDialogWnd.UIAnswersList.AddParsedItem(str, MessageShift, UITalkDialogWnd.UIAnswersList.GetTextColor());

	CUIString Local;
	Local.SetText("----------------");
	UITalkDialogWnd.UIAnswersList.AddParsedItem(Local, 0, UITalkDialogWnd.UIAnswersList.GetTextColor());
	UITalkDialogWnd.UIAnswersList.ScrollToEnd();
}
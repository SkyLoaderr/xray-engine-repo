// UITalkWnd.cpp:  окошко для общения персонажей
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UITalkWnd.h"

#include "../actor.h"
#include "../HUDManager.h"
#include "../UIGameSP.h"
#include "../PDA.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
	//Меню разговора
	AttachChild(&UITalkDialogWnd);
	UITalkDialogWnd.Init(0,0, Device.dwWidth, Device.dwHeight);

	/////////////////////////
	//Меню торговли
	AttachChild(&UITradeWnd);
	UITradeWnd.Hide();
}


void CUITalkWnd::InitTalkDialog()
{
	CActor *pActor = dynamic_cast<CActor *>(Level().CurrentEntity());

	if (pActor && !pActor->IsTalking()) return;


	m_pOurInvOwner = dynamic_cast<CInventoryOwner*>(pActor);
	m_pOthersInvOwner = dynamic_cast<CInventoryOwner*>(pActor->GetTalkPartner());

	//имена собеседников
	UITalkDialogWnd.UICharacterInfoLeft.InitCharacter(m_pOurInvOwner);
	UITalkDialogWnd.UICharacterInfoRight.InitCharacter(m_pOthersInvOwner);

	UITalkDialogWnd.UIAnswer.SetText("...");
	
	UpdateQuestions();

	UITalkDialogWnd.Show();
	UITradeWnd.Hide();
}

void CUITalkWnd::UpdateQuestions()
{
	UITalkDialogWnd.UIQuestionsList.RemoveAll();


	R_ASSERT2(m_pOurInvOwner->GetPDA(), "PDA for character does not init yet");

	//получить возможные вопросы и заполнить ими список
	m_pOurInvOwner->GetPDA()->UpdateQuestions();
	
	for(INFO_QUESTIONS_LIST_it it = m_pOurInvOwner->GetPDA()->m_ActiveQuestionsList.begin();
		m_pOurInvOwner->GetPDA()->m_ActiveQuestionsList.end() != it;
		++it)
	{
		SInfoQuestion* pQuestion = &(*it); 
		UITalkDialogWnd.UIQuestionsList.AddItem((*it).text.GetBuf(), pQuestion);
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
	//остановить разговор, если нужно
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

	if(!result)
	{
		UITalkDialogWnd.UIAnswer.SetText(UITalkDialogWnd.m_pClickedQuestion->
												negative_answer_text.GetBuf());
	}
	else
	{
		CUIString str;
		CInfoPortion info_portion; 	

		for(INFO_INDEX_LIST_it it = index_list.begin(); 
							   index_list.end() != it; ++it)
		{
			info_portion.Load(*it);		
			str.AppendText(info_portion.GetText());
			str.AppendText("\\n");
		}

		UITalkDialogWnd.UIAnswer.SetText(str);

		//UpdateQuestions();
	}
}
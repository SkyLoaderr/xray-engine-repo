// UITalkWnd.cpp:  окошко дл€ общени€ персонажей
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UITalkWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "..\\actor.h"
#include "..\\HUDManager.h"
#include "..\\UIGameSP.h"
#include "..\\PDA.h"


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


	CUIXml uiXml;
	uiXml.Init("$game_data$","talk.xml");
	CUIXmlInit xml_init;

	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);



	/////////////////////////
	//ћеню разговора

	AttachChild(&UITalkDialogWnd);
	UITalkDialogWnd.Init(0,0, Device.dwWidth, Device.dwHeight);
	
	
	UITalkDialogWnd.AttachChild(&UITalkDialogWnd.UIStaticBottom);
	UITalkDialogWnd.UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);

	//¬опросы
	UITalkDialogWnd.AttachChild(&UITalkDialogWnd.UIQuestionFrame);
	UITalkDialogWnd.UIQuestionFrame.Init("ui\\ui_frame", 200, 20, 600, 300);
	UITalkDialogWnd.AttachChild(&UITalkDialogWnd.UIQuestionsList);
	UITalkDialogWnd.UIQuestionsList.Init(200, 20, 600, 300);

	//ќтветы
	UITalkDialogWnd.AttachChild(&UITalkDialogWnd.UIAnswerFrame);
	UITalkDialogWnd.UIAnswerFrame.Init("ui\\ui_frame", 200, 350,  600, 370);

	UITalkDialogWnd.AttachChild(&UITalkDialogWnd.UIAnswer);
	UITalkDialogWnd.UIAnswer.Init(200, 350,  600, 370);
	
	

	UITalkDialogWnd.AttachChild(&UITalkDialogWnd.UIOurName);
	UITalkDialogWnd.UIOurName.Init(10,10, 300, 50);
	UITalkDialogWnd.UIOurName.SetText("Our Name");

	UITalkDialogWnd.AttachChild(&UITalkDialogWnd.UIPartnerName);
	UITalkDialogWnd.UIPartnerName.Init(800, 10, 300, 50);
	UITalkDialogWnd.UIPartnerName.SetText("Partner Name");


	
	

	//кнопка перехода в режим торговли
	UITalkDialogWnd.AttachChild(&UITalkDialogWnd.UIToTradeButton);
	xml_init.InitButton(uiXml, "button", 0, &UITalkDialogWnd.UIToTradeButton);



	/////////////////////////
	//ћеню торговли
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
	UITalkDialogWnd.UIOurName.SetText((LPSTR)pActor->cName());
	UITalkDialogWnd.UIPartnerName.SetText((LPSTR)dynamic_cast<CObject*>(pActor->GetTalkPartner())->cName());

	UITalkDialogWnd.UIAnswer.SetText("...");
	
	UpdateQuestions();

	UITalkDialogWnd.Show();
	UITradeWnd.Hide();
}

void CUITalkWnd::UpdateQuestions()
{
	UITalkDialogWnd.UIQuestionsList.RemoveAll();

	//получить возможные вопросы и заполнить ими список
	m_pOurInvOwner->GetPDA()->UpdateQuestions();
	
	for(INFO_QUESTIONS_LIST_it it = m_pOurInvOwner->GetPDA()->m_ActiveQuestionsList.begin();
		it != m_pOurInvOwner->GetPDA()->m_ActiveQuestionsList.end();
		it++)
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
							   it!= index_list.end(); it++)
		{
			info_portion.Load(*it);		
			str.AppendText(info_portion.GetText());
			str.AppendText("\\n");
		}

		UITalkDialogWnd.UIAnswer.SetText(str);

		//UpdateQuestions();
	}
}
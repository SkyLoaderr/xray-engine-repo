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

#include "../game_cl_base.h"
#include "../string_table.h"
#include "../xr_level_controller.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int MessageShift	= 30;

//////////////////////////////////////////////////////////////////////////

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
	SetFont(HUD().Font().pFontHeaderRussian);

	m_bNeedToUpdateQuestions = false;
}

//////////////////////////////////////////////////////////////////////////

CUITalkWnd::~CUITalkWnd()
{
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Init()
{
	inherited::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	/////////////////////////
	//���� ���������
	AttachChild(&UITalkDialogWnd);
	UITalkDialogWnd.Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	/////////////////////////
	//���� ��������
	AttachChild(&UITradeWnd);
	UITradeWnd.Hide();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::InitTalkDialog()
{
	m_pActor = smart_cast<CActor *>(Level().CurrentEntity());
	if (m_pActor && !m_pActor->IsTalking()) return;

	m_pOurInvOwner = smart_cast<CInventoryOwner*>(m_pActor);
	m_pOthersInvOwner = m_pActor->GetTalkPartner();

	m_pOurDialogManager = smart_cast<CPhraseDialogManager*>(m_pOurInvOwner);
	m_pOthersDialogManager = smart_cast<CPhraseDialogManager*>(m_pOthersInvOwner);

	//����� ������������
	UITalkDialogWnd.UICharacterInfoLeft.InitCharacter(m_pOurInvOwner);
	UITalkDialogWnd.UICharacterInfoRight.InitCharacter(m_pOthersInvOwner);
	UITalkDialogWnd.UIDialogFrame.UITitleText.SetText(m_pOthersInvOwner->CharacterInfo().Name());
	UITalkDialogWnd.UIOurPhrasesFrame.UITitleText.SetText(m_pOurInvOwner->CharacterInfo().Name());
	
//	CEntityAlive* ContactEA = smart_cast<CEntityAlive*>(m_pOthersInvOwner);
//	UITalkDialogWnd.UICharacterInfoRight.SetRelation(ContactEA->tfGetRelationType(m_pActor));

	//�������� ��� ���������
	UITalkDialogWnd.UIAnswersList.RemoveAll();

	InitOthersStartDialog();
	UpdateQuestions();

	UITalkDialogWnd.Show();
	UITradeWnd.Hide();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::InitOthersStartDialog()
{
	m_pOthersDialogManager->UpdateAvailableDialogs(m_pOurDialogManager);
	if(!m_pOthersDialogManager->AvailableDialogs().empty())
	{
		m_pCurrentDialog = m_pOthersDialogManager->AvailableDialogs().front();
		m_pOthersDialogManager->InitDialog(m_pOurDialogManager, m_pCurrentDialog);
		
		//������� �����
		CUIString speaker_name;
		CStringTable stbl;
		speaker_name.SetText(m_pOthersInvOwner->CharacterInfo().Name());
		AddAnswer(m_pCurrentDialog->GetPhraseText(START_PHRASE), speaker_name);
		m_pOthersDialogManager->SayPhrase(m_pCurrentDialog, START_PHRASE);

		//���� ������ ����������, ������� � ����� ������ ����
		if(!m_pCurrentDialog || m_pCurrentDialog->IsFinished()) ToTopicMode();
	}
}

//////////////////////////////////////////////////////////////////////////
void CUITalkWnd::NeedUpdateQuestions()
{
	m_bNeedToUpdateQuestions = true;
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
			if(phrase_dialog->GetDialogType(eDialogTypeActor))
				AddQuestion(phrase_dialog->DialogCaption(), NULL, (int)i);
		}
	}
	else
	{
		if(m_pCurrentDialog->IsWeSpeaking(m_pOurDialogManager))
		{
			//���� � ������ ���������� ���� ������ ���� ����� ��������, �� ������
			//������� (����� ��� �� ���������� ������� ��������)
			if( m_pCurrentDialog->allIsDummy() ){
				CPhrase* phrase = m_pCurrentDialog->PhraseList()[Random.randI(m_pCurrentDialog->PhraseList().size())];
				SayPhrase(phrase->GetIndex());
			};
/*			if(m_pCurrentDialog->PhraseList().size() == 1)
			{
				CPhrase* phrase = m_pCurrentDialog->PhraseList().front();
				if(phrase->IsDummy()) SayPhrase(phrase->GetIndex());
			}*/

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

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UITalkDialogWnd && msg == TALK_DIALOG_TRADE_BUTTON_CLICKED)
	{
//		UITalkDialogWnd.Hide();
//
//		UITradeWnd.InitTrade(m_pOurInvOwner, m_pOthersInvOwner);
//		UITradeWnd.Show();
//		UITradeWnd.BringAllToTop();
		SwitchToTrade();
	}
	else if(pWnd == &UITalkDialogWnd && msg == TALK_DIALOG_QUESTION_CLICKED)
	{
		AskQuestion();
	}
	else if(pWnd == &UITradeWnd && msg == TRADE_WND_CLOSED)
	{
		UITalkDialogWnd.Show();
		UITradeWnd.Hide();
	}

	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Update()
{
	//���������� ��������, ���� �����
	if (m_pActor && !m_pActor->IsTalking())
	{
//		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
//		if(pGameSP) pGameSP->StartStopMenu(this);
		Game().StartStopMenu(this,true);
	}

	if(m_bNeedToUpdateQuestions)
	{
		m_bNeedToUpdateQuestions = false;
		UpdateQuestions();
	}
	inherited::Update();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Draw()
{
	inherited::Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Show()
{
	InitTalkDialog();
	inherited::Show();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::Hide()
{
	inherited::Hide();
	UITradeWnd.Hide();
	if(!m_pActor) return;
	
	ToTopicMode();

	if (m_pActor->IsTalking()) m_pActor->StopTalk();
	m_pActor = NULL;
}

//////////////////////////////////////////////////////////////////////////

bool  CUITalkWnd::TopicMode			() 
{
	return NULL == m_pCurrentDialog.get();
}

//////////////////////////////////////////////////////////////////////////

void  CUITalkWnd::ToTopicMode		() 
{
	m_pCurrentDialog = DIALOG_SHARED_PTR((CPhraseDialog*)NULL);
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::AskQuestion()
{

	//�������� ��� ���������
	//UITalkDialogWnd.UIAnswersList.RemoveAll();
	PHRASE_ID phrase_id;

	//����� ������ ���� ���������
	if(TopicMode())
	{
		if ( (UITalkDialogWnd.m_iClickedQuestion < 0) ||
			(UITalkDialogWnd.m_iClickedQuestion >= (int)m_pOurDialogManager->AvailableDialogs().size()) ) {

			string128	s;
			sprintf		(s,"ID = [%i] of selected question is out of range of available dialogs ",UITalkDialogWnd.m_iClickedQuestion);
			VERIFY2(FALSE, s);
		}

		m_pCurrentDialog = m_pOurDialogManager->AvailableDialogs()[UITalkDialogWnd.m_iClickedQuestion];
		
		m_pOurDialogManager->InitDialog(m_pOthersDialogManager, m_pCurrentDialog);
		phrase_id = START_PHRASE;
	}
	else
	{
		phrase_id = (PHRASE_ID)UITalkDialogWnd.m_iClickedQuestion;
	}

	SayPhrase(phrase_id);
	NeedUpdateQuestions();
}

//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::AddQuestion(LPCSTR text, void* pData , int value)
{
	CUIString str(*CStringTable()(text));

//	CUIStatic::PreprocessText(str.m_str, UITalkDialogWnd.UIQuestionsList.GetWidth() - MessageShift - 25,
//							  UITalkDialogWnd.UIQuestionsList.GetFont());
	UITalkDialogWnd.UIQuestionsList.AddParsedItem<CUIListItem>(str, 0, UITalkDialogWnd.UIQuestionsList.GetTextColor(), 
						UITalkDialogWnd.UIQuestionsList.GetFont(), pData, value);
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::AddAnswer(LPCSTR text, const CUIString &SpeakerName)
{
	//��� ������ ����� ������ ������ �� �������
	if(xr_strlen(text) == 0) return;

	CUIString str(*CStringTable()(text));

	// ������ ������������� ������
//	CUIStatic::PreprocessText(str.m_str, UITalkDialogWnd.UIAnswersList.GetWidth() - MessageShift - 25, // 20 means approximate scrollbar width value
//							  UITalkDialogWnd.UIAnswersList.GetFont());

	u32 cl = UITalkDialogWnd.UIAnswersList.GetTextColor();
	if (0 == xr_strcmp(SpeakerName.GetBuf(), m_pOurInvOwner->CharacterInfo().Name())) cl = UITalkDialogWnd.GetOurReplicsColor();

	UITalkDialogWnd.UIAnswersList.AddParsedItem<CUIListItem>(SpeakerName, 0, UITalkDialogWnd.GetHeaderColor(), UITalkDialogWnd.GetHeaderFont());
	UITalkDialogWnd.UIAnswersList.AddParsedItem<CUIListItem>(str, MessageShift, cl);

	CUIString Local;
	Local.SetText(" ");
	UITalkDialogWnd.UIAnswersList.AddParsedItem<CUIListItem>(Local, 0, UITalkDialogWnd.UIAnswersList.GetTextColor());
	UITalkDialogWnd.UIAnswersList.ScrollToEnd();
}

//////////////////////////////////////////////////////////////////////////

void CUITalkWnd::SwitchToTrade()
{
	if(m_pOurInvOwner->IsTradeEnabled() && m_pOthersInvOwner->IsTradeEnabled() ){

		UITalkDialogWnd.Hide();

		UITradeWnd.InitTrade(m_pOurInvOwner, m_pOthersInvOwner);
		UITradeWnd.Show();
		UITradeWnd.StartTrade();
		UITradeWnd.BringAllToTop();
	}
}

bool CUITalkWnd::IR_OnKeyboardPress(int dik)
{
	int cmd = key_binding[dik];
	if(cmd==kUSE)
	{
		GetHolder()->StartStopMenu(this, true);
		return true;
	}
	return inherited::IR_OnKeyboardPress(dik);
}

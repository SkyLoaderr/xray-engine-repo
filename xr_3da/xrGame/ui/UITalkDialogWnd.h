////////////////////////////////////////////////
// UITalkDialogWnd.h
// меню диалога со сталкером в режиме разговора
////////////////////////////////////////////////

#pragma once


#include "UIStatic.h"
#include "UIButton.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"

#include "..\\InfoPortion.h"

class CUITalkDialogWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUITalkDialogWnd();
	virtual ~CUITalkDialogWnd();
	
	typedef enum{TRADE_BUTTON_CLICKED,
				 QUESTION_CLICKED} E_MESSAGE;
	
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Show();
	virtual void Hide();


	//номер выбранного вопроса
	int m_iClickedQuestion;
	SInfoQuestion* m_pClickedQuestion;

	//список вопросов, которые мы можем задавать персонажу
	CUIListWnd			UIQuestionsList;
	CUIStatic			UIAnswer;

	//элементы интерфейса диалога
	CUIFrameWindow		UIQuestionFrame;
	CUIFrameWindow		UIAnswerFrame;
	CUIStatic			UIStaticBottom;

	CUIStatic			UIOurName;
	CUIStatic			UIPartnerName;
	
	CUIButton			UIToTradeButton;
};
////////////////////////////////////////////////
// UITalkDialogWnd.cpp
// меню диалога со сталкером в режиме разговора
////////////////////////////////////////////////

#include "stdafx.h"
#include "UITalkDialogWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"


CUITalkDialogWnd::CUITalkDialogWnd()
{
	m_iClickedQuestion = 0;
	m_pClickedQuestion = NULL;
}
CUITalkDialogWnd::~CUITalkDialogWnd()
{
}

void CUITalkDialogWnd::Init(int x, int y, int width, int height)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$","talk.xml");
	R_ASSERT2(xml_result, "xml file not found");
	CUIXmlInit xml_init;

	inherited::Init(x, y, width, height);

	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);


	//иконки с изображение нас и партнера по торговле
	AttachChild(&UIOurIcon);
	xml_init.InitStatic(uiXml, "static_icon", 0, &UIOurIcon);
	AttachChild(&UIOthersIcon);
	xml_init.InitStatic(uiXml, "static_icon", 1, &UIOthersIcon);
	UIOurIcon.AttachChild(&UICharacterInfoLeft);
	UICharacterInfoLeft.Init(0,0, UIOurIcon.GetWidth(), UIOurIcon.GetHeight(), "trade_character.xml");
	UIOthersIcon.AttachChild(&UICharacterInfoRight);
	UICharacterInfoRight.Init(0,0, UIOthersIcon.GetWidth(), UIOthersIcon.GetHeight(), "trade_character.xml");


	//Вопросы
	AttachChild(&UIQuestionFrame);
	UIQuestionFrame.Init("ui\\ui_frame", 200, 20, 600, 300);
	AttachChild(&UIQuestionsList);
	UIQuestionsList.Init(200, 20, 600, 300);

	//Ответы
	AttachChild(&UIAnswerFrame);
	UIAnswerFrame.Init("ui\\ui_frame", 200, 350,  600, 370);

	AttachChild(&UIAnswer);
	UIAnswer.Init(200, 350,  600, 370);
		

	//кнопка перехода в режим торговли
	AttachChild(&UIToTradeButton);
	xml_init.InitButton(uiXml, "button", 0, &UIToTradeButton);
}


void CUITalkDialogWnd::Show()
{
	inherited::Show(true);
	inherited::Enable(true);

	ResetAll();
}

void CUITalkDialogWnd::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}

//пересылаем сообщение родительскому окну для обработки
//и фильтруем если оно пришло от нашего дочернего окна
void CUITalkDialogWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UIQuestionsList)
	{
		if(msg == CUIListWnd::LIST_ITEM_CLICKED)
		{
			m_iClickedQuestion = ((CUIListItem*)pData)->GetIndex();
			m_pClickedQuestion =(SInfoQuestion*) (((CUIListItem*)pData)->GetData());

			GetParent()->SendMessage(this, QUESTION_CLICKED);
		}
	}
	else if(pWnd == &UIToTradeButton && msg == CUIButton::BUTTON_CLICKED)
	{
		GetTop()->SendMessage(this, TRADE_BUTTON_CLICKED);
	}

	inherited::SendMessage(pWnd, msg, pData);
}
////////////////////////////////////////////////
// UITalkDialogWnd.cpp
// меню диалога со сталкером в режиме разговора
////////////////////////////////////////////////

#include "stdafx.h"
#include "UITalkDialogWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../UI.h"

//////////////////////////////////////////////////////////////////////////

const char * const TALK_XML					= "talk.xml";
const char * const TRADE_CHARACTER_XML		= "trade_character.xml";

//////////////////////////////////////////////////////////////////////////

CUITalkDialogWnd::CUITalkDialogWnd()
	:	m_pNameTextFont		(NULL)
{
	m_iClickedQuestion = -1;
}
CUITalkDialogWnd::~CUITalkDialogWnd()
{
}

void CUITalkDialogWnd::Init(int x, int y, int width, int height)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", TALK_XML);
	R_ASSERT2(xml_result, "xml file not found");
	CUIXmlInit xml_init;

	inherited::Init(x, y, width, height);

	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,UI_BASE_WIDTH,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,UI_BASE_HEIGHT-32,UI_BASE_HEIGHT,32);

	//иконки с изображение нас и партнера по торговле
	AttachChild(&UIOurIcon);
	xml_init.InitStatic(uiXml, "static_icon", 0, &UIOurIcon);
	AttachChild(&UIOthersIcon);
	xml_init.InitStatic(uiXml, "static_icon", 1, &UIOthersIcon);
	UIOurIcon.AttachChild(&UICharacterInfoLeft);
	UICharacterInfoLeft.Init(0,0, UIOurIcon.GetWidth(), UIOurIcon.GetHeight(), TRADE_CHARACTER_XML);
	UIOthersIcon.AttachChild(&UICharacterInfoRight);
	UICharacterInfoRight.Init(0,0, UIOthersIcon.GetWidth(), UIOthersIcon.GetHeight(), TRADE_CHARACTER_XML);

	//основной фрейм диалога
	AttachChild(&UIDialogFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIDialogFrame);
	UIDialogFrame.UITitleText.SetElipsis(CUIStatic::eepEnd, 10);
	// Фрейм с нащими фразами
	AttachChild(&UIOurPhrasesFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 1, &UIOurPhrasesFrame);
	UIOurPhrasesFrame.UITitleText.SetElipsis(CUIStatic::eepEnd, 10);

	// поле отображения имени актра-игрока
//	UIDialogFrame.AttachChild(&UICharacterName);
//	xml_init.InitStatic(uiXml, "static", 0, &UICharacterName);

	//Ответы
	UIDialogFrame.AttachChild(&UIAnswersList);
	xml_init.InitListWnd(uiXml, "list", 1, &UIAnswersList);
	UIAnswersList.EnableScrollBar(true);
	UIAnswersList.ActivateList(false);

	//Вопросы
	//UIDialogFrame.AttachChild(&UIQuestionsList);
	UIOurPhrasesFrame.AttachChild(&UIQuestionsList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIQuestionsList);
//	UIQuestionsList.EnableScrollBar(true);
//	UIQuestionsList.ActivateList(true);
	UIQuestionsList.EnableActiveBackground(false);
	UIQuestionsList.EnableScrollBar(true);
	UIQuestionsList.SetMessageTarget(this);

	//кнопка перехода в режим торговли
	AttachChild(&UIToTradeButton);
	xml_init.InitButton(uiXml, "button", 0, &UIToTradeButton);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	// шрифт для индикации имени персонажа в окне разговора
	xml_init.InitFont(uiXml, "font", 0, m_iNameTextColor, m_pNameTextFont);

	CGameFont * pFont = NULL;
	xml_init.InitFont(uiXml, "font", 1, m_uOurReplicsColor, pFont);
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
			m_iClickedQuestion = ((CUIListItem*)pData)->GetValue();
			//m_pClickedQuestion =(SInfoQuestion*) (((CUIListItem*)pData)->GetData());
			GetMessageTarget()->SendMessage(this, QUESTION_CLICKED);
		}
	}
	else if(pWnd == &UIToTradeButton && msg == CUIButton::BUTTON_CLICKED)
	{
		GetTop()->SendMessage(this, TRADE_BUTTON_CLICKED);
	}

	inherited::SendMessage(pWnd, msg, pData);
}
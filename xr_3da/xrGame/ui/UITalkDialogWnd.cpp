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


	//основной фрейм диалога
	AttachChild(&UIDialogFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIDialogFrame);

	//Вопросы
	UIDialogFrame.AttachChild(&UIQuestionsList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIQuestionsList);

	//Ответы
	UIDialogFrame.AttachChild(&UIAnswersList);
	xml_init.InitListWnd(uiXml, "list", 1, &UIAnswersList);
	UIAnswersList.EnableScrollBar(true);
	UIAnswersList.ActivateList(false);

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

void CUITalkDialogWnd::AddMessageToLog(const CUIString& msg)
{
	const STRING& text =  msg.m_str;
	STRING buf;

	u32 last_pos = 0;

	for(u32 i = 0; i<text.size()-2; ++i)
	{
         // '\n' - переход на новую строку
		if(text[i] == '\\' && text[i+1]== 'n')
		{	
			buf.clear();
			buf.insert(buf.begin(), text.begin()+last_pos, text.begin()+i);
			buf.push_back(0);
			UIAnswersList.AddItem(&buf.front());
			++i;
			last_pos = i+1;
		}	
	}

	if(last_pos<text.size())
	{
		buf.clear();
		buf.insert(buf.begin(), text.begin()+last_pos, text.end());
		buf.push_back(0);
		UIAnswersList.AddItem(&buf.front());
	}

	//добавить строку-разделитель
	UIAnswersList.AddItem("----------------");

	UIAnswersList.ScrollToEnd();
}
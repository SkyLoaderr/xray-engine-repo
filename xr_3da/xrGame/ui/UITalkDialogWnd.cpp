////////////////////////////////////////////////
// UITalkDialogWnd.cpp
// ���� ������� �� ��������� � ������ ���������
////////////////////////////////////////////////

#include "stdafx.h"
#include "UITalkDialogWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../UI.h"

CUITalkDialogWnd::CUITalkDialogWnd()
{
	m_iClickedQuestion = -1;
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
	UIStaticTop.Init("ui\\ui_top_background", 0,0,UI_BASE_WIDTH,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,UI_BASE_HEIGHT-32,UI_BASE_HEIGHT,32);

	//������ � ����������� ��� � �������� �� ��������
	AttachChild(&UIOurIcon);
	xml_init.InitStatic(uiXml, "static_icon", 0, &UIOurIcon);
	AttachChild(&UIOthersIcon);
	xml_init.InitStatic(uiXml, "static_icon", 1, &UIOthersIcon);
	UIOurIcon.AttachChild(&UICharacterInfoLeft);
	UICharacterInfoLeft.Init(0,0, UIOurIcon.GetWidth(), UIOurIcon.GetHeight(), "trade_character.xml");
	UIOthersIcon.AttachChild(&UICharacterInfoRight);
	UICharacterInfoRight.Init(0,0, UIOthersIcon.GetWidth(), UIOthersIcon.GetHeight(), "trade_character.xml");

	//�������� ����� �������
	AttachChild(&UIDialogFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIDialogFrame);

	// ���� ����������� ����� �����-������
	UIDialogFrame.AttachChild(&UICharacterName);
	xml_init.InitStatic(uiXml, "static", 0, &UICharacterName);

	//�������
	//UIDialogFrame.AttachChild(&UIQuestionsList);
	UIDialogFrame.AttachChild(&UIQuestionsList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIQuestionsList);

	//������
	UIDialogFrame.AttachChild(&UIAnswersList);
	xml_init.InitListWnd(uiXml, "list", 1, &UIAnswersList);
	UIAnswersList.EnableScrollBar(true);
	UIAnswersList.ActivateList(false);

	//������ �������� � ����� ��������
	AttachChild(&UIToTradeButton);
	xml_init.InitButton(uiXml, "button", 0, &UIToTradeButton);

	//�������� ��������������� ����������
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	// ����� ��� ��������� ����� ��������� � ���� ���������
	xml_init.InitFont(uiXml, "font", 0, m_iNameTextColor, m_pNameTextFont);
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

//���������� ��������� ������������� ���� ��� ���������
//� ��������� ���� ��� ������ �� ������ ��������� ����
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

/*
void CUITalkDialogWnd::AddMessageToLog(CUIListWnd *List, const CUIString& msg, const char StartShift, const u32 &MsgColor)
{
	const STRING& text =  msg.m_str;
	STRING buf;

	u32 last_pos = 0;

	List->SetTextColor(MsgColor);

	int GroupID = List->GetSize();
	
	for(u32 i = 0; i<text.size()-2; ++i)
	{
         // '\n' - ������� �� ����� ������
		if(text[i] == '\\' && text[i+1]== 'n')
		{	
			buf.clear();
			buf.insert(buf.begin(), StartShift, ' ');
			buf.insert(buf.begin() + StartShift, text.begin()+last_pos, text.begin()+i);
			buf.push_back(0);
			List->AddItem(&buf.front());
			List->GetItem(List->GetSize() - 1)->SetGroupID(GroupID);
			++i;
			last_pos = i+1;
		}	
	}

	if(last_pos<text.size())
	{
		buf.clear();
		buf.insert(buf.begin(), StartShift, ' ');
		buf.insert(buf.begin() + StartShift, text.begin()+last_pos, text.end());
		buf.push_back(0);
		List->AddItem(&buf.front());
		List->GetItem(List->GetSize() - 1)->SetGroupID(GroupID);
	}
}
*/

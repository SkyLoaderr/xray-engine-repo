////////////////////////////////////////////////
// UITalkDialogWnd.cpp
// ���� ������� �� ��������� � ������ ���������
////////////////////////////////////////////////

#include "stdafx.h"
#include "UITalkDialogWnd.h"

CUITalkDialogWnd::CUITalkDialogWnd()
{
	m_iClickedQuestion = 0;
	m_pClickedQuestion = NULL;
}
CUITalkDialogWnd::~CUITalkDialogWnd()
{
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
			m_iClickedQuestion = ((CUIListItem*)pData)->GetIndex();
			m_pClickedQuestion =(SInfoQuestion*) (((CUIListItem*)pData)->GetData());

			GetParent()->SendMessage(this, QUESTION_CLICKED);
		}
	}
	else if(pWnd == &UIToTradeButton && msg == CUIButton::BUTTON_CLICKED)
	{
		GetTop()->SendMessage(this, TRADE_BUTTON_CLICKED);
	}
	//else if()

	inherited::SendMessage(pWnd, msg, pData);
}
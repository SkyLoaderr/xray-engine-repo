////////////////////////////////////////////////
// UITalkDialogWnd.h
// ���� ������� �� ��������� � ������ ���������
////////////////////////////////////////////////

#pragma once


#include "UIStatic.h"
#include "UIButton.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"

#include "../InfoPortion.h"

#include "UICharacterInfo.h"
#include "UIItemInfo.h"

class CUITalkDialogWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUITalkDialogWnd();
	virtual ~CUITalkDialogWnd();
	
	typedef enum{TRADE_BUTTON_CLICKED,
				 QUESTION_CLICKED} E_MESSAGE;

	virtual void Init(int x, int y, int width, int height);
	
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Show();
	virtual void Hide();

	virtual u32 GetHeaderColor() { return m_iNameTextColor; }
	virtual CGameFont *GetHeaderFont() { return m_pNameTextFont; }

	//����� ���������� �������
	int m_iClickedQuestion;

	//������ ��������, ������� �� ����� �������� ���������
	CUIListWnd			UIQuestionsList;
	CUIListWnd			UIAnswersList;

	//�������� ���������� �������
	CUIFrameWindow		UIDialogFrame;

	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;
	CUIStatic			UICharacterName;

	CUIButton			UIToTradeButton;

	//���������� � ���������� 
	CUIStatic			UIOurIcon;
	CUIStatic			UIOthersIcon;
	CUICharacterInfo	UICharacterInfoLeft;
	CUICharacterInfo	UICharacterInfoRight;

private:
	// ����� � ���� ������ � ������ ���������
	CGameFont *m_pNameTextFont;
	u32 m_iNameTextColor;
};
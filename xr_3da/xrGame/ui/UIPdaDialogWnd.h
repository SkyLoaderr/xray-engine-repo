////////////////////////////////////////////////
//	UIPdaDialogWnd.h
// ���� ������� �� ��������� � PDA
////////////////////////////////////////////////


#pragma once

#include "UIFrameWindow.h"
#include "UIListWnd.h"
#include "UICharacterInfo.h"
#include "UIAnimatedStatic.h"

class CUIPdaDialogWnd: public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIPdaDialogWnd();
	virtual ~CUIPdaDialogWnd();

	virtual void Init(int x, int y, int width, int height);

//	typedef enum{BACK_BUTTON_CLICKED,
//				 MESSAGE_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void Show(bool status);

	virtual void Update();

	void ContactLoss();
	void ContactRestore();
	void ContactWaitForReply();

	u32			GetHeaderColor()		{ return m_iNameTextColor; }
	CGameFont *	GetHeaderFont()			{ return m_pNameTextFont; }
	u32			GetOurReplicsColor()	{ return m_uOurReplicsColor; }

private:
	// ����� � ���� ������ � ������ ���������
	CGameFont			*m_pNameTextFont;
	u32					m_iNameTextColor;
	// ���� �e���� � ����� ����� ������
	u32					m_uOurReplicsColor;

public:
	//����� ���������� �������
	int m_iClickedQuestion;

	//����� ������������� ���������
	EPdaMsg m_iMsgNum;

	bool m_bContactActive;
	bool m_bContactWait;

	//��� ���������
	CUIListWnd UILogListWnd;
	//������ ���� ������� ����� �������� �����
	CUIListWnd UIPhrasesListWnd;

	// ������ ������������� ����������
	CUIFrameWindow		UIMsglogFrame;
	CUIFrameWindow		UICharIconFrame;
	CUIFrameWindow		UIPhrasesFrame;
	CUIFrameLineWnd		UIMsglogHeader;
	CUIFrameLineWnd		UICharIconHeader;
	CUIFrameLineWnd		UIPhrasesHeader;
	CUIAnimatedStatic	UIAnimatedIcon;

	//���������� � ���������
	CUIWindow			UICharacterWindow;
	CUICharacterInfo	UICharacterInfo;
	CUIFrameWindow		UIMask;
};
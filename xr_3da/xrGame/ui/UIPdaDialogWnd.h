////////////////////////////////////////////////
//	UIPdaDialogWnd.h
// ���� ������� �� ��������� � PDA
////////////////////////////////////////////////


#pragma once

#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UICharacterInfo.h"
#include "UIAnimatedStatic.h"
#include "UIMultiTextStatic.h"

class CUIPdaDialogWnd: public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIPdaDialogWnd();
	virtual ~CUIPdaDialogWnd();

	virtual void Init(int x, int y, int width, int height);

	typedef enum{BACK_BUTTON_CLICKED,
				 MESSAGE_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void Show();
	void Hide();

	virtual void Update();

	void ContactLoss();
	void ContactRestore();
	void ContactWaitForReply();

	//���������� ��������� � ���
	void AddOurMessageToLog		(EPdaMsg msg, CInventoryOwner* pInvOwner);
	void AddOthersMessageToLog	(EPdaMsg msg, CInventoryOwner* pInvOwner);
	void AddMessageToLog		(EPdaMsg msg, u32 color);

	//������ ������� ������� � ������ �� ����
	void PhrasesAnswer();
	void PhrasesAsk();


	//����� ������������� ���������
	EPdaMsg m_iMsgNum;

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
	CUIMultiTextStatic	UIMTStatic;
};
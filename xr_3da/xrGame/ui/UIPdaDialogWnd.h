////////////////////////////////////////////////
//	UIPdaDialogWnd.h
// меню диалога со сталкером в PDA
////////////////////////////////////////////////


#pragma once

#include "..\inventory.h"

#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"



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

	void ContactLoss();
	void ContactRestore();
	void ContactWaitForReply();


	//номер отправленного сообщения
	int m_iMsgNum;

	CUIListWnd			UILogListWnd;
	CUIButton			UIBackButton;
	
	CUIButton			UIMsgButton1;
	CUIButton			UIMsgButton2;
	CUIButton			UIMsgButton3;
	
	CUIStatic			UIStaticContactName;
	CUIStatic			UIStaticContactStatus;


	char m_sContactName[100];
};



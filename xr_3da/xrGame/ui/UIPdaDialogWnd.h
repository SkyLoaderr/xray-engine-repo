////////////////////////////////////////////////
//	UIPdaDialogWnd.h
// меню диалога со сталкером в PDA
////////////////////////////////////////////////


#pragma once

#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UICharacterInfo.h"



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

	//добавление сообщений в лог
	void AddOurMessageToLog		(EPdaMsg msg, CInventoryOwner* pInvOwner);
	void AddOthersMessageToLog	(EPdaMsg msg, CInventoryOwner* pInvOwner);
	void AddMessageToLog		(EPdaMsg msg, u32 color);

	//Режима задания вопроса и ответа на него
	void PhrasesAnswer();
	void PhrasesAsk();


	//номер отправленного сообщения
	EPdaMsg m_iMsgNum;

	//лог сообщений
	CUIListWnd UILogListWnd;
	//список фраз которые может говорить игрок
	CUIListWnd UIPhrasesListWnd;

	// Фреймы дикоративного оформления
	CUIFrameWindow		UIMsglogFrame;
	CUIFrameWindow		UICharIconFrame;
	CUIFrameWindow		UIPhrasesFrame;

	//информация о персонаже
	CUIWindow		 UICharacterWindow;
	CUICharacterInfo UICharacterInfo;
};
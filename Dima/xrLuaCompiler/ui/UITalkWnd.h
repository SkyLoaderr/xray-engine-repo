// UITalkWnd.h:  окошко для общения персонажей
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\inventory.h"

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"

#include "UITradeWnd.h"
#include "UITalkDialogWnd.h"



///////////////////////////////////////
// Подложка и основные кнопки PDA
///////////////////////////////////////

class CUITalkWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUITalkWnd();
	virtual ~CUITalkWnd();

	virtual void Init();
//	virtual void InitTalk();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();
		
	virtual void Show();
	virtual void Hide();

	void UpdateQuestions();
protected:
	//диалог
	void InitTalkDialog();
	void AskQuestion();


	//для режима торговли
	CUITradeWnd			UITradeWnd;
	CUITalkDialogWnd	UITalkDialogWnd;


	//указатель на владельца инвентаря вызвавшего менюшку
	CInventoryOwner* m_pOurInvOwner;
	CInventoryOwner* m_pOthersInvOwner;
};

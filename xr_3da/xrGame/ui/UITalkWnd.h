// UITalkWnd.h:  окошко для общения персонажей
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"

#include "UITradeWnd.h"
#include "UITalkDialogWnd.h"

#include "../PhraseDialogDefs.h"

class CActor;
class CInventoryOwner;
class CPhraseDialogManager;

///////////////////////////////////////
//
///////////////////////////////////////

class CUITalkWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUITalkWnd();
	virtual ~CUITalkWnd();

	virtual void Init();

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

	// Функции добавления строк в листы вопросов и ответов
	void AddQuestion(const CUIString &str, void* pData, int value = 0);
	void AddAnswer(const CUIString &str, const CUIString &SpeakerName);

	//для режима торговли
	CUITradeWnd			UITradeWnd;
	CUITalkDialogWnd	UITalkDialogWnd;


	//указатель на владельца инвентаря вызвавшего менюшку
	//и его собеседника
	CActor*			 m_pActor;
	CInventoryOwner* m_pOurInvOwner;
	CInventoryOwner* m_pOthersInvOwner;
	
	CPhraseDialogManager* m_pOurDialogManager;
	CPhraseDialogManager* m_pOthersDialogManager;

	//текущий диалог, если NULL, то переходим в режим выбора темы
	DIALOG_SHARED_PTR m_pCurrentDialog;
	bool TopicMode	();
	void ToTopicMode();
};
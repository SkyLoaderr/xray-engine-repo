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
	void NeedUpdateQuestions();
	//инициализации начального диалога собеседника
	void InitOthersStartDialog();
	virtual bool IR_OnKeyboardPress(int dik);
	void SwitchToTrade();
protected:

	//диалог
	void InitTalkDialog();
	void AskQuestion();

	void SayPhrase(PHRASE_ID phrase_id);

	// Функции добавления строк в листы вопросов и ответов
	void AddQuestion(LPCSTR text, void* pData, int value = 0);
	void AddAnswer(LPCSTR text, const CUIString &SpeakerName);

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

	//спец. переменная, нужна для того чтобы RemoveAll
	//могла быть корректно вызвана из SendMessage
	//так как иначе возникает ситуация, что класс, который
	//вызвал нам SendMessage обращается к удаленному объекту pListItem
	bool m_bNeedToUpdateQuestions;

	//текущий диалог, если NULL, то переходим в режим выбора темы
	DIALOG_SHARED_PTR m_pCurrentDialog;
	bool TopicMode	();
	void ToTopicMode();
};
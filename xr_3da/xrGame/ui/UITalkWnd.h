// UITalkWnd.h:  ������ ��� ������� ����������
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
	//������������� ���������� ������� �����������
	void InitOthersStartDialog();
protected:

	//������
	void InitTalkDialog();
	void AskQuestion();

	void SayPhrase(PHRASE_ID phrase_id);

	// ������� ���������� ����� � ����� �������� � �������
	void AddQuestion(CUIString str, void* pData, int value = 0);
	void AddAnswer(CUIString str, const CUIString &SpeakerName);

	//��� ������ ��������
	CUITradeWnd			UITradeWnd;
	CUITalkDialogWnd	UITalkDialogWnd;


	//��������� �� ��������� ��������� ���������� �������
	//� ��� �����������
	CActor*			 m_pActor;
	CInventoryOwner* m_pOurInvOwner;
	CInventoryOwner* m_pOthersInvOwner;
	
	CPhraseDialogManager* m_pOurDialogManager;
	CPhraseDialogManager* m_pOthersDialogManager;

	//������� ������, ���� NULL, �� ��������� � ����� ������ ����
	DIALOG_SHARED_PTR m_pCurrentDialog;
	bool TopicMode	();
	void ToTopicMode();
};
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


#include "../PhraseDialogDefs.h"

class CActor;
class CInventoryOwner;
class CPhraseDialogManager;
class CUITalkDialogWnd;
class CUITradeWnd;
///////////////////////////////////////
//
///////////////////////////////////////

class CUITalkWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
	ref_sound			m_sound;
	void				PlaySnd					(LPCSTR text);
	void				StopSnd					();
public:
						CUITalkWnd();
	virtual				~CUITalkWnd();

	virtual void		Init();

	virtual bool		StopAnyMove					(){return true;}
	virtual void		SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void		Draw();
	virtual void		Update();
		
	virtual void		Show();
	virtual void		Hide();

	void				UpdateQuestions();
	void				NeedUpdateQuestions();
	//������������� ���������� ������� �����������
	void				InitOthersStartDialog();
	virtual bool		IR_OnKeyboardPress(int dik);
	void				SwitchToTrade();
	void				AddIconedMessage(LPCSTR text, LPCSTR texture_name, Frect texture_rect);

protected:

	//������
	void				InitTalkDialog();
	void				AskQuestion();

	void				SayPhrase(PHRASE_ID phrase_id);

	// ������� ���������� ����� � ����� �������� � �������
	void				AddQuestion(LPCSTR text, int value = 0);
	void				AddAnswer(LPCSTR text, const CUIString &SpeakerName);

	//��� ������ ��������
	CUITradeWnd*			UITradeWnd;
	CUITalkDialogWnd*		UITalkDialogWnd;


	//��������� �� ��������� ��������� ���������� �������
	//� ��� �����������
	CActor*				m_pActor;
	CInventoryOwner*	m_pOurInvOwner;
	CInventoryOwner*	m_pOthersInvOwner;
	
	CPhraseDialogManager* m_pOurDialogManager;
	CPhraseDialogManager* m_pOthersDialogManager;

	//����. ����������, ����� ��� ���� ����� RemoveAll
	//����� ���� ��������� ������� �� SendMessage
	//��� ��� ����� ��������� ��������, ��� �����, �������
	//������ ��� SendMessage ���������� � ���������� ������� pListItem
	bool				m_bNeedToUpdateQuestions;

	//������� ������, ���� NULL, �� ��������� � ����� ������ ����
	DIALOG_SHARED_PTR	m_pCurrentDialog;
	bool				TopicMode	();
	void				ToTopicMode();
};
//-----------------------------------------------------------------------------/
//  ���������������� ��������� PDA
//-----------------------------------------------------------------------------/

#ifndef UI_PDA_COMMUNICATION_H_
#define UI_PDA_COMMUNICATION_H_

#include "UIPdaContactsWnd.h"
#include "UIPdaDialogWnd.h"

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"

#include "../PhraseDialogDefs.h"



class CInventoryOwner;
class CActor;
class CPhraseDialogManager;

///////////////////////////////////////
// �������� � �������� ������ PDA
///////////////////////////////////////

class CUIPdaCommunication: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIPdaCommunication();
	virtual ~CUIPdaCommunication();

	virtual void Init();
	virtual void InitPDACommunication();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();

	virtual void Show(bool status);

protected:
	//��������
	virtual void InitPdaContacts();
	virtual	void UpdatePdaContacts();

	//������
	virtual void InitPdaDialog();
	virtual void UpdateMessageLog();
	virtual void UpdateMsgButtons();
public:
	virtual void UpdateDisplay();
protected:

//	CUIFrameWindow		UIMainPdaFrame;
	CUIPdaContactsWnd	UIPdaContactsWnd;
	CUIPdaDialogWnd		UIPdaDialogWnd;

	//��������� �� ��������� ��������� ���������� �������
	CInventoryOwner*	m_pInvOwner;
	CPda*				m_pPda;

	//��������� �� ������� �������
	u32					m_idContact;
	CObject*			m_pContactObject;
	CInventoryOwner*	m_pContactInvOwner;
	CPda*				m_pContactPda;

	//������ �������� �� ����� �������
	bool m_bContactLoss;


	//��������� �� ��������� ��������� ���������� �������
	//� ��� �����������
	CActor*			 m_pActor;
	CInventoryOwner* m_pOurInvOwner;
	CInventoryOwner* m_pOthersInvOwner;

	CPhraseDialogManager* m_pOurDialogManager;
	CPhraseDialogManager* m_pOthersDialogManager;

	//������� ������, ���� NULL, �� ��������� � ����� ������ ����
	DIALOG_SHARED_PTR m_pCurrentDialog;
	PHRASE_ID		  m_iLastPhraseID;

	bool TopicMode	();
	void ToTopicMode();

	//������
	void InitTalkDialog();
	void UpdateQuestions();
	void AskQuestion();

	void SayPhrase(PHRASE_ID phrase_id);

	// ������� ���������� ����� � ����� �������� � �������
	void AddQuestion(CUIString str, void* pData, int value = 0);
	void AddAnswer(CUIString str, const CUIString &SpeakerName);
};

#endif
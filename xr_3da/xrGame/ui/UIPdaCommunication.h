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
#include "UITabControl.h"

class CInventoryOwner;

///////////////////////////////////////
// �������� � �������� ������ PDA
///////////////////////////////////////

class CUIPdaCommunication: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIPdaCommunication();
	virtual ~CUIPdaCommunication();

	virtual void Init();
	virtual void InitPDA();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();

	virtual void Show();

protected:
	//��������
	virtual void InitPdaContacts();
	virtual	void UpdatePdaContacts();

	//������
	virtual void InitPdaDialog();
	virtual void UpdateMessageLog();
	virtual void UpdateMsgButtons();

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

	//������ �������� �� ����� �������
	bool m_bContactLoss;

	//�������� ������������� ����������
//	CUIStatic			UIStaticTop;
//	CUIStatic			UIStaticBottom;

//	//�������� PDA ��� �������� ������������ ������
//	CUIStatic			UIPDAHeader;

	// ������ PDA
//	CUITabControl		UITabControl;
};

#endif
// CUIPdaWnd.h:  ������ PDA
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "..\inventory.h"


#include "UIPdaContactsWnd.h"
#include "UIPdaDialogWnd.h"

#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"



///////////////////////////////////////
// �������� � �������� ������ PDA
///////////////////////////////////////

class CUIPdaWnd: public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIPdaWnd();
	virtual ~CUIPdaWnd();

	virtual void Init();
	virtual void InitPDA();

	virtual bool IR_OnKeyboardPress(int dik);
	virtual bool IR_OnKeyboardRelease(int dik);
	virtual bool IR_OnMouseMove(int dx, int dy);

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void Draw();
	virtual void Update();

protected:
	//��������
	virtual void InitPdaContacts();
	virtual	void UpdatePdaContacts();

	//������
	virtual void InitPdaDialog();
	virtual void UpdateMessageLog();
	virtual void UpdateMsgButtons();


	CUIFrameWindow		UIMainPdaFrame;
	CUIPdaContactsWnd	UIPdaContactsWnd;
	CUIPdaDialogWnd		UIPdaDialogWnd;

	//��������� �� ��������� ��������� ���������� �������
	CInventoryOwner* m_pInvOwner;
	CPda* m_pPda;

	//��������� �� ������� �������
	u32 m_idContact;
	CObject* m_pContactObject;
	CInventoryOwner* m_pContactInvOwner;
	//������ �������� �� ����� �������
	bool m_bContactLoss;

};

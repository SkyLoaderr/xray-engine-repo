////////////////////////////////////////////////
// UIPdaContactsWnd.h
// ���� �������� � PDA
////////////////////////////////////////////////


#pragma once

#include "../inventory.h"

#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"


class CUIPdaContactsWnd: public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIPdaContactsWnd();
	virtual ~CUIPdaContactsWnd();

	virtual void Init(int x, int y, int width, int height);

	//���������, ������������ ������������� ����
	typedef enum{CONTACT_SELECTED} E_MESSAGE;

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Update();
	virtual u32 GetContactID() {return m_idContact;}

	void Show();
	void Hide();

	void AddContact(CObject* pOwnerObject);
	void RemoveContact(CObject* pOwnerObject);
	void RemoveAll();

	bool IsInList(CObject* pOwnerObject);

	//������ ��������� PDA
	CUIListWnd UIListWnd;
protected:
	//ID ���������� �������� InventoryOwner
	u32 m_idContact;
};
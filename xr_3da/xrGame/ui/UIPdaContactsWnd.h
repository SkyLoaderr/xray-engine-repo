////////////////////////////////////////////////
// UIPdaContactsWnd.h
// меню котактов в PDA
////////////////////////////////////////////////


#pragma once

#include "..\inventory.h"

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

	//сообщения, отправляемые родительскому окну
	typedef enum{CONTACT_SELECTED} E_MESSAGE;

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Update();
	virtual u32 GetContactID() {return m_idContact;}

	void Show();
	void Hide();


	CUIListWnd UIListWnd;

protected:
	//ID выбранного контакта InventoryOwner
	u32 m_idContact;
};
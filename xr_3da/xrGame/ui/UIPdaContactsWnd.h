////////////////////////////////////////////////
// UIPdaContactsWnd.h
// меню котактов в PDA
////////////////////////////////////////////////


#pragma once

#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"


class CPda;

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

	void AddContact(CPda* pda);
	void RemoveContact(CPda* pda);
	void RemoveAll();

	bool IsInList(CPda* pda);

	//список контактов PDA
	CUIListWnd UIListWnd;
protected:
	//ID выбранного контакта InventoryOwner
	u32 m_idContact;

	// Фрейм для обрамоения контактов
	CUIFrameWindow		UIFrameContacts;
};
////////////////////////////////////////////////
// UIPdaContactsWnd.h
// ���� �������� � PDA
////////////////////////////////////////////////


#pragma once

#include "UIStatic.h"
#include "UIButton.h"
#include "UIEditBox.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"

extern const char * const PDA_CONTACTS_HEADER_SUFFIX;

class CPda;

class CUIPdaContactsWnd: public CUIWindow  
{
private:
	typedef CUIWindow inherited;
public:
	CUIPdaContactsWnd();
	virtual ~CUIPdaContactsWnd();

	virtual void Init(int x, int y, int width, int height);

	//���������, ������������ ������������� ����
//	typedef enum{CONTACT_SELECTED} E_MESSAGE;

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Update();
	virtual u32 GetContactID() {return m_idContact;}

	virtual void Show(bool status);

	void AddContact(CPda* pda);
	void RemoveContact(CPda* pda);
	void RemoveAll();

	bool IsInList(CPda* pda);

	//������ ��������� PDA
	CUIListWnd UIListWnd;
protected:
	//ID ���������� �������� InventoryOwner
	u32 m_idContact;

	// �������� ����������
	CUIFrameWindow		UIFrameContacts;
	CUIFrameLineWnd		UIContactsHeader;
	CUIFrameWindow		UIRightFrame;
	CUIFrameLineWnd		UIRightFrameHeader;
	CUIStatic			UIArticleHeader;
	CUIAnimatedStatic	UIAnimation;
};
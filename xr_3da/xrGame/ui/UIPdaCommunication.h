//-----------------------------------------------------------------------------/
//  Коммуникационный поддиалог PDA
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
// Подложка и основные кнопки PDA
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
	//контакты
	virtual void InitPdaContacts();
	virtual	void UpdatePdaContacts();

	//диалог
	virtual void InitPdaDialog();
	virtual void UpdateMessageLog();
	virtual void UpdateMsgButtons();

//	CUIFrameWindow		UIMainPdaFrame;
	CUIPdaContactsWnd	UIPdaContactsWnd;
	CUIPdaDialogWnd		UIPdaDialogWnd;

	//указатель на владельца инвентаря вызвавшего менюшку
	CInventoryOwner*	m_pInvOwner;
	CPda*				m_pPda;

	//указатели на текущий контакт
	u32					m_idContact;
	CObject*			m_pContactObject;
	CInventoryOwner*	m_pContactInvOwner;
	CPda*				m_pContactPda;


	//потеря контакта во время диалога
	bool m_bContactLoss;

	//элементы декоративного интерфейса
//	CUIStatic			UIStaticTop;
//	CUIStatic			UIStaticBottom;

//	//верхушка PDA для кнопочек переключения режима
//	CUIStatic			UIPDAHeader;

	// кнопки PDA
//	CUITabControl		UITabControl;
};

#endif
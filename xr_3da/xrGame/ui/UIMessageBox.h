// UIMessageBox.h: 
//
// окшко с собщением и конпками
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiframewindow.h"
#include "uibutton.h"


class CUIMessageBox: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited;
public:
	////////////////////////////////////
	//конструктор/деструктор
	CUIMessageBox();
	virtual ~CUIMessageBox();

	//разновидности MessageBox
	typedef enum {MESSAGEBOX_OK, MESSAGEBOX_YES_NO, MESSAGEBOX_YES_NO_CANCEL } 
				E_MESSAGEBOX_STYLE;

	////////////////////////////////////
	//инициализация
	virtual void Init(LPCSTR base_name, int x, int y, int width, int height);
	virtual void SetStyle(E_MESSAGEBOX_STYLE messageBoxStyle);
	virtual void SetText(LPSTR str);

	virtual void Show();
	virtual void Hide();

	//автоматическая центровка
	void AutoCenter();


	//сообщения, отправляемые родительскому окну
//	typedef enum{OK_CLICKED, YES_CLICKED, NO_CLICKED, CANCEL_CLICKED} E_MESSAGE;

	virtual void OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);


protected:
	CUIButton m_UIButton1;
	CUIButton m_UIButton2;
	CUIButton m_UIButton3;

	CUIStatic m_UIStatic;
	 
	E_MESSAGEBOX_STYLE m_eMessageBoxStyle;
};
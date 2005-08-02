// UIMessageBox.h: 
//
// окшко с собщением и конпками
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiframewindow.h"
#include "uibutton.h"

#include "../script_export_space.h"

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
	virtual void Init(LPCSTR base_name, float x, float y, float width, float height);
	virtual void SetStyle(E_MESSAGEBOX_STYLE messageBoxStyle);
			void SetStyle_script(u32 messageBoxStyle){SetStyle((E_MESSAGEBOX_STYLE)messageBoxStyle);};
	virtual void SetText(LPCSTR str);

	virtual void Show();
	virtual void Hide();

	//автоматическая центровка
	void AutoCenter();


	//сообщения, отправляемые родительскому окну
//	typedef enum{OK_CLICKED, YES_CLICKED, NO_CLICKED, CANCEL_CLICKED} E_MESSAGE;

	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);


protected:
	CUIButton m_UIButtonYesOk;
	CUIButton m_UIButtonNo;
	CUIButton m_UIButtonCancel;

	CUIStatic m_UIStatic;
	 
	E_MESSAGEBOX_STYLE m_eMessageBoxStyle;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIMessageBox)
#undef script_type_list
#define script_type_list save_type_list(CUIMessageBox)

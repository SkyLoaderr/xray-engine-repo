#pragma once


#include "UIStatic.h"
#include "../script_export_space.h"

class CUI3tButton;
class CUIEditBox;

class CUIMessageBox: public CUIStatic
{
private:
	typedef CUIStatic inherited;
public:
				CUIMessageBox		();
	virtual		~CUIMessageBox		();

	//������������� MessageBox
	typedef enum {MESSAGEBOX_OK, MESSAGEBOX_YES_NO, MESSAGEBOX_YES_NO_CANCEL, MESSAGEBOX_DIRECT_IP, MESSAGEBOX_PASSWORD, MESSAGEBOX_QUIT_WINDOWS, MESSAGEBOX_QUIT_GAME } 
				E_MESSAGEBOX_STYLE;

			void Init				(LPCSTR box_template);
			void Clear				();
	virtual void SetText			(LPCSTR str);
	LPCSTR		 GetHost			();
	LPCSTR		 GetPassword		();

	virtual bool OnMouse			(float x, float y, EUIMessages mouse_action);
	virtual void SendMessage		(CUIWindow *pWnd, s16 msg, void *pData);


protected:
	xr_string	m_ret_val;
	CUI3tButton* m_UIButtonYesOk;
	CUI3tButton* m_UIButtonNo;
	CUI3tButton* m_UIButtonCancel;

	CUIStatic*	m_UIStaticPicture;
	CUIStatic*	m_UIStaticText;
	CUIStatic*	m_UIStaticHost;
	CUIStatic*	m_UIStaticPass;
	CUIEditBox* m_UIEditHost;
	CUIEditBox* m_UIEditPass;
	 
	E_MESSAGEBOX_STYLE m_eMessageBoxStyle;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIMessageBox)
#undef script_type_list
#define script_type_list save_type_list(CUIMessageBox)

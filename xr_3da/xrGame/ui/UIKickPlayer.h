#pragma once

#include "UIDialogWnd.h"

class CUIStatic;
class CUI3tButton;
class CUIFrameWindow;
class CUIListBox;
class CUIXml;

class CUIKickPlayer : public CUIDialogWnd {
public:
	using CUIDialogWnd::Init;

	CUIKickPlayer();

			void InitKick(CUIXml& xml_doc);
			void InitBan(CUIXml& xml_doc);

	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, int pData = 0);
	virtual void Update();

	void OnBtnOk();
	void OnBtnCancel();

protected:
	void Init(CUIXml& xml_doc);

	CUIStatic*		bkgrnd;
	CUIFrameWindow* lst_back;
	CUIListBox*		lst;

	CUI3tButton*	btn_ok;
	CUI3tButton*	btn_cancel;

	u32			m_prev_upd_time;

	DEFINE_VECTOR	(LPVOID,ItemVec,ItemIt);
	ItemVec			items;
};
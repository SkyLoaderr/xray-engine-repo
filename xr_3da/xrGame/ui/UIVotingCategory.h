#pragma once

#include "UIDialogWnd.h"

class CUIStatic;
class CUI3tButton;
class CUIKickPlayer;
class CUIChangeMap;
class CUIXml;

class CUIVotingCategory : public CUIDialogWnd {
public:
	using CUIDialogWnd::Init;

	CUIVotingCategory();
	~CUIVotingCategory();

	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);

	void OnBtn(int i);
	void OnBtnCancel();

protected:
	void Init();

	CUIStatic*		header;
	CUI3tButton*	btn[7];
	CUIStatic*		txt[7];
	CUIStatic*		bkgrnd;
	CUI3tButton*	btn_cancel;

	CUIKickPlayer*	kick;
	CUIChangeMap*	change_map;
	CUIXml*			xml_doc;
};
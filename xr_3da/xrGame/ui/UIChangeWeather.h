#pragma once

#include "UIDialogWnd.h"

class CUIStatic;
class CUI3tButton;
class CUIKickPlayer;
class CUIChangeMap;
class CUIXml;

class CUIChangeWeather : public CUIDialogWnd {
public:
	using CUIDialogWnd::Init;

	CUIChangeWeather();
			void Init(CUIXml& xml_doc);

	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = 0);

	void OnBtn(int i);
	void OnBtnCancel();

protected:
	void ParseWeather();
	void AddWeather(LPCSTR name, LPCSTR time);
	u32 weather_counter;
	

	CUIStatic*		header;
	CUI3tButton*	btn[4];
	CUIStatic*		txt[4];
	CUIStatic*		bkgrnd;
	CUI3tButton*	btn_cancel;
};
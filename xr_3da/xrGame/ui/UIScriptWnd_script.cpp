#include "stdafx.h"
#include "UIScriptWnd.h"
#include "../script_space.h"

//UI-controls
#include "UIButton.h"
#include "UIMessageBox.h"
#include "UIPropertiesBox.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UIRadioGroup.h"
#include "UIStatic.h"
#include "UIEditBox.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIProgressBar.h"
#include "UITabControl.h"

using namespace luabind;

void UIScriptWnd::script_register(lua_State *L)
{
	module(L)
	[
		class_<UIScriptWnd, CUIDialogWnd>("CUIScriptWnd")
		.def(					constructor<>())

		.def("Load",			&UIScriptWnd::Load)
		.def("AddCallback",		(void(UIScriptWnd::*)(LPCSTR, s16, const luabind::functor<void>&))UIScriptWnd::AddCallback)
		.def("AddCallback",		(void(UIScriptWnd::*)(LPCSTR, s16, const luabind::object&, LPCSTR))UIScriptWnd::AddCallback)

		.def("test",			&UIScriptWnd::test)

		.def("GetButton",		(CUIButton* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIButton>)
		.def("GetMessageBox",	(CUIMessageBox* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIMessageBox>)
		.def("GetPropertiesBox",(CUIPropertiesBox* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIPropertiesBox>)
		.def("GetCheckButton",	(CUICheckButton* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUICheckButton>)
		.def("GetRadioButton",	(CUIRadioButton* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIRadioButton>)
		.def("GetRadioGroup",	(CUIRadioGroup* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIRadioGroup>)
		.def("GetStatic",	(CUIStatic* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIStatic>)
		.def("GetEditBox",	(CUIEditBox* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIEditBox>)
		.def("GetDialogWnd",	(CUIDialogWnd* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIDialogWnd>)
		.def("GetFrameWindow",	(CUIFrameWindow* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIFrameWindow>)
		.def("GetFrameLineWnd",	(CUIFrameLineWnd* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIFrameLineWnd>)
		.def("GetProgressBar",	(CUIProgressBar* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIProgressBar>)
		.def("GetTabControl",	(CUITabControl* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUITabControl>)

//		.def("GetMessageBox",	(CUIMessageBox* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIMessageBox>)
//		.def("GetMessageBox",	(CUIMessageBox* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIMessageBox>)
		
	];
}
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

template <typename T>
struct CWrapperBase : public T, public luabind::wrap_base {
	typedef T inherited;
	typedef CWrapperBase<T>	self_type;

	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action)
	{ return call_member<bool>(this,"OnKeyboard", dik, keyboard_action);}
	static bool OnKeyboard_static(inherited* ptr, int dik, EUIMessages keyboard_action)
	{ return ptr->self_type::inherited::OnKeyboard(dik,keyboard_action );}

};

template<typename T>
IC T*	UIScriptWnd::GetControl(LPCSTR name){
		ref_str n = name;
		CUIWindow* pWnd = FindChild(n);
		if(pWnd == NULL)
			return NULL;

		return smart_cast<T*>(pWnd);
}

void UIScriptWnd::script_register(lua_State *L)
{
	typedef CWrapperBase<UIScriptWnd> WrapType;
	typedef UIScriptWnd BaseType;

	module(L)
	[
		class_<UIScriptWnd, WrapType, CUIDialogWnd>("CUIScriptWnd")
		.def(					constructor<>())

		.def("Load",			&BaseType::Load)
		.def("AddCallback",		(void(BaseType::*)(LPCSTR, s16, const luabind::functor<void>&))BaseType::AddCallback)
		.def("AddCallback",		(void(BaseType::*)(LPCSTR, s16, const luabind::object&, LPCSTR))BaseType::AddCallback)

		.def("Register",		&BaseType::Register)
		.def("test",			&BaseType::test)

		.def("GetButton",		(CUIButton* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIButton>)
		.def("GetMessageBox",	(CUIMessageBox* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIMessageBox>)
		.def("GetPropertiesBox",(CUIPropertiesBox* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIPropertiesBox>)
		.def("GetCheckButton",	(CUICheckButton* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUICheckButton>)
		.def("GetRadioButton",	(CUIRadioButton* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIRadioButton>)
		.def("GetRadioGroup",	(CUIRadioGroup* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIRadioGroup>)
		.def("GetStatic",	(CUIStatic* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIStatic>)
		.def("GetEditBox",	(CUIEditBox* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIEditBox>)
		.def("GetDialogWnd",	(CUIDialogWnd* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIDialogWnd>)
		.def("GetFrameWindow",	(CUIFrameWindow* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIFrameWindow>)
		.def("GetFrameLineWnd",	(CUIFrameLineWnd* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIFrameLineWnd>)
		.def("GetProgressBar",	(CUIProgressBar* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIProgressBar>)
		.def("GetTabControl",	(CUITabControl* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUITabControl>)
		.def("GetListWnd",		(CUIListWnd* (BaseType::*)(LPCSTR)) BaseType::GetControl<CUIListWnd>)


		.def("OnKeyboard",		&BaseType::OnKeyboard, &WrapType::OnKeyboard_static)

//		.def("GetMessageBox",	(CUIMessageBox* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIMessageBox>)
//		.def("GetMessageBox",	(CUIMessageBox* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIMessageBox>)
		
	];
}
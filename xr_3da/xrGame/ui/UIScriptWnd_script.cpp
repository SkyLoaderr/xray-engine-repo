#include "stdafx.h"
#include "../script_space.h"

//UI-controls
#include "UIScriptWnd.h"
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

#include "uiscriptwnd_script.h"

using namespace luabind;

extern export_class &script_register_ui_window1(export_class &);
extern export_class &script_register_ui_window2(export_class &);

void UIScriptWnd::script_register(lua_State *L)
{
	export_class				instance("CUIScriptWnd");

	module(L)
	[
		script_register_ui_window2(
			script_register_ui_window1(
				instance
			)
		)
		.def("Load",			&BaseType::Load)
	];
}

export_class &script_register_ui_window1(export_class &instance)
{
	instance
		.def(					constructor<>())

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

	;return	(instance);
}

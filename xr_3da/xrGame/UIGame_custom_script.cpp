#include "stdafx.h"
#include "UIGame_custom_script.h"

#include "script_space.h"
#include "xrServer_script_macroses.h"
using namespace luabind;


template <typename T>
struct CWrapperBase : public T, public luabind::wrap_base {
	typedef T inherited;
	typedef CWrapperBase<T>	self_type;

	DEFINE_LUA_WRAPPER_METHOD_V0(Init)
	DEFINE_LUA_WRAPPER_METHOD_V1(SetClGame, game_cl_GameState*)
};


void UIGame_custom_script::AddCustomMessage		(LPCSTR id, float x, float y, float font_size, CGameFont *pFont, u16 alignment, u32 color, LPCSTR def_text )
{
	m_gameCaptions.addCustomMessage(id,x,y,font_size,pFont,(CGameFont::EAligment)alignment,color,def_text);
}

void UIGame_custom_script::CustomMessageOut(LPCSTR id, LPCSTR msg, u32 color)
{
	m_gameCaptions.setCaption(id,msg,color,true);
}


void UIGame_custom_script::script_register(lua_State *L)
{
	typedef CWrapperBase<UIGame_custom_script> WrapType;
	typedef UIGame_custom_script BaseType;
	module(L)
		[
			luabind::class_< UIGame_custom_script, CUIGameCustom, WrapType >("UIGame_custom_script")
			.def(	constructor<>())
			.def("Init",		&BaseType::Init, &WrapType::Init_static)
			.def("SetClGame",		&BaseType::SetClGame, &WrapType::SetClGame_static)

			.def("AddCustomMessage",    &UIGame_custom_script::AddCustomMessage)
			.def("CustomMessageOut",    &UIGame_custom_script::CustomMessageOut)

		];
}

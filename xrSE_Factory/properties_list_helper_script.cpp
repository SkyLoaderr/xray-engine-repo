////////////////////////////////////////////////////////////////////////////
//	Module 		: properties_list_helper_script.cpp
//	Created 	: 14.07.2004
//  Modified 	: 14.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Properties list helper script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_properties_list_helper.h"
#include "ai_space.h"
#include "script_engine.h"

using namespace luabind;

template <typename T>
T& set(T *self, const typename T::TYPE mask, bool value)
{
	return	(self->set(mask,value));
}

template <typename T>
bool is(T *self, const typename T::TYPE mask)
{
	return	(!!self->is(mask));
}

template <typename T>
bool is_any(T *self, const typename T::TYPE mask)
{
	return	(!!self->is_any(mask));
}

template <typename T>
bool test(T *self, const typename T::TYPE mask)
{
	return	(!!self->test(mask));
}

template <typename T>
bool equal(T *self, const T &f)
{
	return	(!!self->equal(f));
}

template <typename T>
bool equal(T *self, const T &f, const typename T::TYPE mask)
{
	return	(!!self->equal(f,mask));
}

extern CScriptPropertiesListHelper *g_property_list_helper;

CScriptPropertiesListHelper *properties_helper()
{
	if (!g_property_list_helper)
		ai().script_engine().script_log	(ScriptStorage::eLuaMessageTypeError,"Editor is not started, therefore prop_helper cannot be accessed!");
	return								(g_property_list_helper);
}

void CScriptPropertiesListHelper::script_register(lua_State *L)
{
	module(L) [
		class_<Flags8>		("flags8")
			.def("get",		&Flags8::get)
			.def("zero",	&Flags8::zero)
			.def("one",		&Flags8::one)
			.def("invert",	(Flags8& (Flags8::*)())(&Flags8::invert))
			.def("invert",	(Flags8& (Flags8::*)(const Flags8&))(&Flags8::invert))
			.def("invert",	(Flags8& (Flags8::*)(const Flags8::TYPE))(&Flags8::invert))
			.def("assign",	(Flags8& (Flags8::*)(const Flags8&))(&Flags8::assign))
			.def("assign",	(Flags8& (Flags8::*)(const Flags8::TYPE))(&Flags8::assign))
			.def("or",		(Flags8& (Flags8::*)(const Flags8::TYPE))(&Flags8::or))
			.def("or",		(Flags8& (Flags8::*)(const Flags8&,const Flags8::TYPE))(&Flags8::or))
			.def("and",		(Flags8& (Flags8::*)(const Flags8::TYPE))(&Flags8::and))
			.def("and",		(Flags8& (Flags8::*)(const Flags8&,const Flags8::TYPE))(&Flags8::and))
			.def("set",		&set<Flags8>)
			.def("is",		&is<Flags8>)
			.def("is_any",	&is_any<Flags8>)
			.def("test",	&test<Flags8>)
			.def("equal",	(bool (*)(Flags8*,const Flags8&))(&equal<Flags8>))
			.def("equal",	(bool (*)(Flags8*,const Flags8&,const Flags8::TYPE))(&equal<Flags8>)),

		class_<Flags16>		("flags16")
			.def("get",		&Flags16::get)
			.def("zero",	&Flags16::zero)
			.def("one",		&Flags16::one)
			.def("invert",	(Flags16& (Flags16::*)())(&Flags16::invert))
			.def("invert",	(Flags16& (Flags16::*)(const Flags16&))(&Flags16::invert))
			.def("invert",	(Flags16& (Flags16::*)(const Flags16::TYPE))(&Flags16::invert))
			.def("assign",	(Flags16& (Flags16::*)(const Flags16&))(&Flags16::assign))
			.def("assign",	(Flags16& (Flags16::*)(const Flags16::TYPE))(&Flags16::assign))
			.def("or",		(Flags16& (Flags16::*)(const Flags16::TYPE))(&Flags16::or))
			.def("or",		(Flags16& (Flags16::*)(const Flags16&,const Flags16::TYPE))(&Flags16::or))
			.def("and",		(Flags16& (Flags16::*)(const Flags16::TYPE))(&Flags16::and))
			.def("and",		(Flags16& (Flags16::*)(const Flags16&,const Flags16::TYPE))(&Flags16::and))
			.def("set",		&set<Flags16>)
			.def("is",		&is<Flags16>)
			.def("is_any",	&is_any<Flags16>)
			.def("test",	&test<Flags16>)
			.def("equal",	(bool (*)(Flags16*,const Flags16&))(&equal<Flags16>))
			.def("equal",	(bool (*)(Flags16*,const Flags16&,const Flags16::TYPE))(&equal<Flags16>)),

		class_<Flags32>		("flags16")
			.def("get",		&Flags32::get)
			.def("zero",	&Flags32::zero)
			.def("one",		&Flags32::one)
			.def("invert",	(Flags32& (Flags32::*)())(&Flags32::invert))
			.def("invert",	(Flags32& (Flags32::*)(const Flags32&))(&Flags32::invert))
			.def("invert",	(Flags32& (Flags32::*)(const Flags32::TYPE))(&Flags32::invert))
			.def("assign",	(Flags32& (Flags32::*)(const Flags32&))(&Flags32::assign))
			.def("assign",	(Flags32& (Flags32::*)(const Flags32::TYPE))(&Flags32::assign))
			.def("or",		(Flags32& (Flags32::*)(const Flags32::TYPE))(&Flags32::or))
			.def("or",		(Flags32& (Flags32::*)(const Flags32&,const Flags32::TYPE))(&Flags32::or))
			.def("and",		(Flags32& (Flags32::*)(const Flags32::TYPE))(&Flags32::and))
			.def("and",		(Flags32& (Flags32::*)(const Flags32&,const Flags32::TYPE))(&Flags32::and))
			.def("set",		&set<Flags32>)
			.def("is",		&is<Flags32>)
			.def("is_any",	&is_any<Flags32>)
			.def("test",	&test<Flags32>)
			.def("equal",	(bool (*)(Flags32*,const Flags32&))(&equal<Flags32>))
			.def("equal",	(bool (*)(Flags32*,const Flags32&,const Flags32::TYPE))(&equal<Flags32>)),

		class_<PropValue>	("prop_value"),
		class_<PropItemVec>	("prop_item_vec"),
		class_<CaptionValue>("caption_value"),
		class_<CanvasValue>	("canvas_value"),
		class_<ButtonValue>	("button_value"),
		class_<ChooseValue>	("choose_value"),
		class_<S16Value>	("s16_value"),
		class_<S32Value>	("s32_value"),
		class_<S8Value>		("u8_value"),
		class_<S16Value>	("u16_value"),
		class_<S32Value>	("u32_value"),
		class_<BOOLValue>	("bool_value"),
		class_<VectorValue>	("vector_value"),
		class_<ColorValue>	("color_value"),
		class_<RTextValue>	("text_value"),
		class_<Flag8Value>	("flag8_value"),
		class_<Flag16Value>	("flag16_value"),
		class_<Flag32Value>	("flag32_value"),
		class_<Token8Value>	("token8_value"),
		class_<Token16Value>("token16_value"),
		class_<Token32Value>("token32_value"),
		class_<RToken8Value>("rtoken8_value"),
		class_<RToken16Value>("rtoken16_value"),
		class_<RToken32Value>("rtoken32_value"),

		class_<CScriptPropertiesListHelper>("properties_list_helper")
			.def("vector_on_after_edit",	&CScriptPropertiesListHelper::FvectorRDOnAfterEdit)
			.def("vector_on_before_edit",	&CScriptPropertiesListHelper::FvectorRDOnBeforeEdit)
			.def("vector_on_draw",			&CScriptPropertiesListHelper::FvectorRDOnDraw)
			.def("float_on_after_edit",		&CScriptPropertiesListHelper::floatRDOnAfterEdit)
			.def("float_on_before_edit",	&CScriptPropertiesListHelper::floatRDOnBeforeEdit)
			.def("float_on_draw",			&CScriptPropertiesListHelper::floatRDOnDraw)
			.def("name_after_edit",			&CScriptPropertiesListHelper::NameAfterEdit)
			.def("name_before_edit",		&CScriptPropertiesListHelper::NameBeforeEdit)
			.def("name_on_draw",			&CScriptPropertiesListHelper::NameDraw)

			.def("create_caption",&CScriptPropertiesListHelper::CreateCaption)
			.def("create_canvas",&CScriptPropertiesListHelper::CreateCanvas)
			.def("create_button",&CScriptPropertiesListHelper::CreateButton)
			
			.def("create_choose", (ChooseValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  LPCSTR*, u32))					(&CScriptPropertiesListHelper::CreateChoose))
			.def("create_choose", (ChooseValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  LPCSTR*, u32, LPCSTR))			(&CScriptPropertiesListHelper::CreateChoose))
			
			.def("create_s8", (S8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  s8*))					(&CScriptPropertiesListHelper::CreateS8))
			.def("create_s8", (S8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  s8*,  s8))				(&CScriptPropertiesListHelper::CreateS8))
			.def("create_s8", (S8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  s8*,  s8,  s8))			(&CScriptPropertiesListHelper::CreateS8))
			.def("create_s8", (S8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  s8*,  s8,  s8,  s8))	(&CScriptPropertiesListHelper::CreateS8))
			
			.def("create_s16",(S16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, s16*))					(&CScriptPropertiesListHelper::CreateS16))
			.def("create_s16",(S16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, s16*, s16))				(&CScriptPropertiesListHelper::CreateS16))
			.def("create_s16",(S16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, s16*, s16, s16))		(&CScriptPropertiesListHelper::CreateS16))
			.def("create_s16",(S16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, s16*, s16, s16, s16))	(&CScriptPropertiesListHelper::CreateS16))

			.def("create_s32",(S32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, s32*))					(&CScriptPropertiesListHelper::CreateS32))
			.def("create_s32",(S32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, s32*, s32))				(&CScriptPropertiesListHelper::CreateS32))
			.def("create_s32",(S32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, s32*, s32, s32))		(&CScriptPropertiesListHelper::CreateS32))
			.def("create_s32",(S32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, s32*, s32, s32, s32))	(&CScriptPropertiesListHelper::CreateS32))
			
			.def("create_u8", (U8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  u8*))					(&CScriptPropertiesListHelper::CreateU8))
			.def("create_u8", (U8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  u8*,  u8))				(&CScriptPropertiesListHelper::CreateU8))
			.def("create_u8", (U8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  u8*,  u8,  u8))			(&CScriptPropertiesListHelper::CreateU8))
			.def("create_u8", (U8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR,  u8*,  u8,  u8,  u8))	(&CScriptPropertiesListHelper::CreateU8))

			.def("create_u16",(U16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, u16*))					(&CScriptPropertiesListHelper::CreateU16))
			.def("create_u16",(U16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, u16*, u16))				(&CScriptPropertiesListHelper::CreateU16))
			.def("create_u16",(U16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, u16*, u16, u16))		(&CScriptPropertiesListHelper::CreateU16))
			.def("create_u16",(U16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, u16*, u16, u16, u16))	(&CScriptPropertiesListHelper::CreateU16))

			.def("create_u32",(U32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, u32*))					(&CScriptPropertiesListHelper::CreateU32))
			.def("create_u32",(U32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, u32*, u32))				(&CScriptPropertiesListHelper::CreateU32))
			.def("create_u32",(U32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, u32*, u32, u32))		(&CScriptPropertiesListHelper::CreateU32))
			.def("create_u32",(U32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, u32*, u32, u32, u32))	(&CScriptPropertiesListHelper::CreateU32))

			.def("create_float",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*))					(&CScriptPropertiesListHelper::CreateFloat))
			.def("create_float",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float))				(&CScriptPropertiesListHelper::CreateFloat))
			.def("create_float",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float, float))		(&CScriptPropertiesListHelper::CreateFloat))
			.def("create_float",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float, float, float))(&CScriptPropertiesListHelper::CreateFloat))
			.def("create_float",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float, float, float,int))(&CScriptPropertiesListHelper::CreateFloat))

			.def("create_bool",&CScriptPropertiesListHelper::CreateBOOL)

			.def("create_vector",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*))					(&CScriptPropertiesListHelper::CreateVector))
			.def("create_vector",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*, float))				(&CScriptPropertiesListHelper::CreateVector))
			.def("create_vector",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*, float, float))		(&CScriptPropertiesListHelper::CreateVector))
			.def("create_vector",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*, float, float, float))(&CScriptPropertiesListHelper::CreateVector))
			.def("create_vector",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*, float, float, float,int))(&CScriptPropertiesListHelper::CreateVector))
			
			.def("create_flag8",(Flag8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags8*, u8))					(&CScriptPropertiesListHelper::CreateFlag8))
			.def("create_flag8",(Flag8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags8*, u8,LPCSTR))			(&CScriptPropertiesListHelper::CreateFlag8))
			.def("create_flag8",(Flag8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags8*, u8,LPCSTR,LPCSTR))		(&CScriptPropertiesListHelper::CreateFlag8))
			.def("create_flag8",(Flag8Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags8*, u8,LPCSTR,LPCSTR,u32))	(&CScriptPropertiesListHelper::CreateFlag8))
			
			.def("create_flag16",(Flag16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags16*, u16))					(&CScriptPropertiesListHelper::CreateFlag16))
			.def("create_flag16",(Flag16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags16*, u16,LPCSTR))			(&CScriptPropertiesListHelper::CreateFlag16))
			.def("create_flag16",(Flag16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags16*, u16,LPCSTR,LPCSTR))		(&CScriptPropertiesListHelper::CreateFlag16))
			.def("create_flag16",(Flag16Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags16*, u16,LPCSTR,LPCSTR,u32))	(&CScriptPropertiesListHelper::CreateFlag16))

			.def("create_flag32",(Flag32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags32*, u32))					(&CScriptPropertiesListHelper::CreateFlag32))
			.def("create_flag32",(Flag32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags32*, u32,LPCSTR))			(&CScriptPropertiesListHelper::CreateFlag32))
			.def("create_flag32",(Flag32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags32*, u32,LPCSTR,LPCSTR))		(&CScriptPropertiesListHelper::CreateFlag32))
			.def("create_flag32",(Flag32Value *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Flags32*, u32,LPCSTR,LPCSTR,u32))	(&CScriptPropertiesListHelper::CreateFlag32))

			.def("create_token8",	&CScriptPropertiesListHelper::CreateToken8)
			.def("create_token16",	&CScriptPropertiesListHelper::CreateToken16)
			.def("create_token32",	&CScriptPropertiesListHelper::CreateToken32)

			.def("create_rtoken8",	&CScriptPropertiesListHelper::CreateRToken8)
			.def("create_rtoken16",	&CScriptPropertiesListHelper::CreateRToken16)
			.def("create_rtoken32",	&CScriptPropertiesListHelper::CreateRToken32)

			.def("create_color",&CScriptPropertiesListHelper::CreateColor)
			.def("create_fcolor",&CScriptPropertiesListHelper::CreateFColor)
			.def("create_vcolor",&CScriptPropertiesListHelper::CreateVColor)
			
			.def("create_text",&CScriptPropertiesListHelper::CreateRText)

			.def("create_time",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*))						(&CScriptPropertiesListHelper::CreateTime))
			.def("create_time",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float))				(&CScriptPropertiesListHelper::CreateTime))
			.def("create_time",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float, float))		(&CScriptPropertiesListHelper::CreateTime))

			.def("create_angle",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*))					(&CScriptPropertiesListHelper::CreateAngle))
			.def("create_angle",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float))				(&CScriptPropertiesListHelper::CreateAngle))
			.def("create_angle",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float, float))		(&CScriptPropertiesListHelper::CreateAngle))
			.def("create_angle",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float, float, float))(&CScriptPropertiesListHelper::CreateAngle))
			.def("create_angle",(FloatValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, float*, float, float, float,int))(&CScriptPropertiesListHelper::CreateAngle))

			.def("create_vangle",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*))					(&CScriptPropertiesListHelper::CreateAngle3))
			.def("create_vangle",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*, float))				(&CScriptPropertiesListHelper::CreateAngle3))
			.def("create_vangle",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*, float, float))		(&CScriptPropertiesListHelper::CreateAngle3))
			.def("create_vangle",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*, float, float, float))(&CScriptPropertiesListHelper::CreateAngle3))
			.def("create_vangle",(VectorValue *(CScriptPropertiesListHelper::*)(PropItemVec&, LPCSTR, Fvector*, float, float, float,int))(&CScriptPropertiesListHelper::CreateAngle3))

		,def("properties_helper",	&properties_helper)
	];
}

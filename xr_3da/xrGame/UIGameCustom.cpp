#include "stdafx.h"
#include "UIGameCustom.h"
#include "ui.h"
#include "level.h"
#include "hudmanager.h"
#include "ui/UIMultiTextStatic.h"
#include "ui/UIXmlInit.h"
#include "object_broker.h"
#include "string_table.h"

CUIGameCustom::CUIGameCustom()
{
	uFlags					= 0;
	shedule.t_min			= 5;
	shedule.t_max			= 20;
	shedule_register		();
	m_pgameCaptions		= xr_new<CUICaption>();
}

CUIGameCustom::~CUIGameCustom()
{
	delete_data				(m_pgameCaptions);
	shedule_unregister		();
	delete_data				(m_custom_statics);
}


float CUIGameCustom::shedule_Scale		() 
{
	return 0.5f;
};

void CUIGameCustom::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);
}

void CUIGameCustom::OnFrame() 
{
	xr_map<shared_str,CUIStatic*>::iterator it = m_custom_statics.begin();
	for(;it!=m_custom_statics.end();++it)
		it->second->Update();
}

void CUIGameCustom::Render()
{
	GameCaptions()->Draw();
	xr_map<shared_str,CUIStatic*>::iterator it = m_custom_statics.begin();
	for(;it!=m_custom_statics.end();++it)
		it->second->Draw();

}

bool CUIGameCustom::IR_OnKeyboardPress(int dik) 
{
	return false;
}

bool CUIGameCustom::IR_OnKeyboardRelease(int dik) 
{
	return false;
}

bool CUIGameCustom::IR_OnMouseMove(int dx,int dy)
{
	return false;
}
bool CUIGameCustom::IR_OnMouseWheel			(int direction)
{
	return false;
}

void CUIGameCustom::AddDialogToRender(CUIWindow* pDialog)
{
	HUD().GetUI()->AddDialogToRender(pDialog);

}

void CUIGameCustom::RemoveDialogToRender(CUIWindow* pDialog)
{
	HUD().GetUI()->RemoveDialogToRender(pDialog);
}

CUIDialogWnd* CUIGameCustom::MainInputReceiver	()
{ 
	return HUD().GetUI()->MainInputReceiver();
};

void CUIGameCustom::AddCustomMessage		(LPCSTR id, float x, float y, float font_size, CGameFont *pFont, u16 alignment, u32 color/* LPCSTR def_text*/ )
{
	GameCaptions()->addCustomMessage(id,x,y,font_size,pFont,(CGameFont::EAligment)alignment,color,"");
}

void CUIGameCustom::AddCustomMessage		(LPCSTR id, float x, float y, float font_size, CGameFont *pFont, u16 alignment, u32 color, /*LPCSTR def_text,*/ float flicker )
{
	AddCustomMessage(id,x,y,font_size, pFont, alignment, color);
	GameCaptions()->customizeMessage(id, CUITextBanner::tbsFlicker)->fPeriod = flicker;
}

void CUIGameCustom::CustomMessageOut(LPCSTR id, LPCSTR msg, u32 color)
{
	GameCaptions()->setCaption(id,msg,color,true);
}

void CUIGameCustom::RemoveCustomMessage		(LPCSTR id)
{
	GameCaptions()->removeCustomMessage(id);
}

void CUIGameCustom::AddCustomStatic			(LPCSTR id)
{
	xr_map<shared_str,CUIStatic*>::iterator it = m_custom_statics.find(id);
	if(it!=m_custom_statics.end())
		RemoveCustomStatic	(id);

	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, "ui_custom_msgs.xml");
	R_ASSERT3						(xml_result, "xml file not found", "ui_custom_msgs.xml");
	CUIXmlInit xml_init;
	CUIStatic* s					= xr_new<CUIStatic>();
	m_custom_statics[id]			= s;
	xml_init.InitStatic				(uiXml, id, 0, s);
}

CUIStatic* CUIGameCustom::GetCustomStatic		(LPCSTR id)
{
	xr_map<shared_str,CUIStatic*>::iterator it = m_custom_statics.find(id);
	if(it!=m_custom_statics.end()){
		return it->second;
	}
	return NULL;
}

void CUIGameCustom::RemoveCustomStatic		(LPCSTR id)
{
	xr_map<shared_str,CUIStatic*>::iterator it = m_custom_statics.find(id);
	if(it!=m_custom_statics.end()){
		xr_delete(it->second);
		m_custom_statics.erase(it);
	}
}


#include "script_space.h"
using namespace luabind;


CUIGameCustom* get_hud(){
	return HUD().GetUI()->UIGame();
}

void CUIGameCustom::script_register(lua_State *L)
{
	module(L)
		[
			class_< CUIGameCustom >("CUIGameCustom")
			.def("AddDialogToRender",		&CUIGameCustom::AddDialogToRender)
			.def("RemoveDialogToRender",	&CUIGameCustom::RemoveDialogToRender)
			.def("AddCustomMessage",		(void(CUIGameCustom::*)(LPCSTR, float, float, float, CGameFont*, u16, u32/*, LPCSTR*/))CUIGameCustom::AddCustomMessage)
			.def("AddCustomMessage",		(void(CUIGameCustom::*)(LPCSTR, float, float, float, CGameFont*, u16, u32/*, LPCSTR*/, float))CUIGameCustom::AddCustomMessage)
			.def("CustomMessageOut",		&CUIGameCustom::CustomMessageOut)
			.def("RemoveCustomMessage",		&CUIGameCustom::RemoveCustomMessage)
			.def("AddCustomStatic",			&CUIGameCustom::AddCustomStatic)
			.def("RemoveCustomStatic",		&CUIGameCustom::RemoveCustomStatic)
			.def("GetCustomStatic",			&CUIGameCustom::GetCustomStatic),
			def("get_hud",					&get_hud)
		];
}

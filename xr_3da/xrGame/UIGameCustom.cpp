#include "stdafx.h"
#include "UIGameCustom.h"
#include "ui.h"

CUIGameCustom::CUIGameCustom()
{
	m_pMainInputReceiver	= NULL;
	uFlags					= 0;
	shedule.t_min			= 5;
	shedule.t_max			= 20;
	shedule_register();
}

CUIGameCustom::~CUIGameCustom()
{
	shedule_unregister();
}




float CUIGameCustom::shedule_Scale		() 
{
	return 0.5f;
};

void CUIGameCustom::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);

	xr_vector<CUIWindow*>::iterator it = m_dialogsToRender.begin();
	for(; it!=m_dialogsToRender.end();++it)
		(*it)->Update();

}

void CUIGameCustom::OnFrame() 
{}

void CUIGameCustom::Render()
{
	xr_vector<CUIWindow*>::iterator it = m_dialogsToRender.begin();
	for(; it!=m_dialogsToRender.end();++it)
		(*it)->Draw();

	m_gameCaptions.Draw();
}

bool CUIGameCustom::IR_OnKeyboardPress(int dik) 
{
	if(m_pMainInputReceiver)
	{
		if(m_pMainInputReceiver->IR_OnKeyboardPress(dik)) 
			return true;
	}

	return false;
}

bool CUIGameCustom::IR_OnKeyboardRelease(int dik) 
{
	if(m_pMainInputReceiver)
	{
		if(m_pMainInputReceiver->IR_OnKeyboardRelease(dik)) 
			return true;
	}
	return false;
}

bool CUIGameCustom::IR_OnMouseMove(int dx,int dy)
{
	if(m_pMainInputReceiver)
	{
		if(m_pMainInputReceiver->IR_OnMouseMove(dx, dy)) 
			return true;
	}
return false;
}

void CUIGameCustom::AddDialogToRender(CUIWindow* pDialog)
{
	if(std::find(m_dialogsToRender.begin(), m_dialogsToRender.end(), pDialog) == m_dialogsToRender.end() )
	{
		m_dialogsToRender.push_back(pDialog);
		pDialog->Show(true);
	}
}

void CUIGameCustom::RemoveDialogToRender(CUIWindow* pDialog)
{
	xr_vector<CUIWindow*>::iterator it = std::find(m_dialogsToRender.begin(),m_dialogsToRender.end(),pDialog);
	if(it != m_dialogsToRender.end())
	{
		(*it)->Show(false);
		m_dialogsToRender.erase(it);
	}
}

#include "script_space.h"
using namespace luabind;



void CUIGameCustom::script_register(lua_State *L)
{
	module(L)
		[
			luabind::class_< CUIGameCustom >("CUIGameCustom")
			.def("AddDialogToRender", &CUIGameCustom::AddDialogToRender)
			.def("RemoveDialogToRender", &CUIGameCustom::RemoveDialogToRender)

		];
}

#include "stdafx.h"
#include "UIScriptWnd.h"
#include "../Level.h"
#include "../HudManager.h"
#include "../script_space.h"
#include <luabind/operator.hpp>
#include "../object_broker.h"

struct SCallbackInfo{
	CScriptCallback		m_callback;
	shared_str				m_controlName;
	s16					m_event;
	SCallbackInfo():m_controlName(""),m_event(-1){}
};
struct event_comparer{
	shared_str				name;
	s16					event;

	event_comparer(shared_str n, s16 e):name(n),event(e){}
	bool operator ()(SCallbackInfo* i){
		return( (i->m_controlName==name) && (i->m_event==event) );
	}
};

UIScriptWnd::UIScriptWnd():inherited()
{
	Hide();
}

UIScriptWnd::~UIScriptWnd()
{
	 delete_data(m_callbacks);
}
void UIScriptWnd::Register			(CUIWindow* pChild)
{
	pChild->SetMessageTarget(this);
}

void UIScriptWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	event_comparer ec(pWnd->WindowName(),msg);

	CALLBACK_IT it = std::find_if(m_callbacks.begin(),m_callbacks.end(),ec);
	if(it==m_callbacks.end())
		return;
	SCRIPT_CALLBACK_EXECUTE_0((*it)->m_callback )

}

bool UIScriptWnd::Load(LPCSTR xml_name)
{
	return true;
}

SCallbackInfo*	UIScriptWnd::NewCallback ()
{
	m_callbacks.push_back( xr_new<SCallbackInfo>() );
	return m_callbacks.back();
}

void UIScriptWnd::AddCallback(LPCSTR control_id, s16 event, const luabind::functor<void> &lua_function)
{
	SCallbackInfo* c	= NewCallback ();
	c->m_callback.set	(lua_function);
	c->m_controlName	= control_id;
	c->m_event			= event;
	
}

void UIScriptWnd::AddCallback (LPCSTR control_id, s16 event, const luabind::object &lua_object, LPCSTR method)
{
	SCallbackInfo* c	= NewCallback ();
	c->m_callback.set	(lua_object,method);
	c->m_controlName	= control_id;
	c->m_event			= event;
}


void UIScriptWnd::test()
{
	CALLBACK_IT it = m_callbacks.begin();
	for(;it!=m_callbacks.end();++it)
		SCRIPT_CALLBACK_EXECUTE_0( (*it)->m_callback )
}

bool UIScriptWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	inherited::OnKeyboard(dik,keyboard_action);
	return true;
}
#include "stdafx.h"
#include "UIScriptWnd.h"

#include "../script_space.h"
#include <luabind/operator.hpp>
#include "../object_broker.h"

UIScriptWnd::UIScriptWnd():inherited()
{
}
UIScriptWnd::~UIScriptWnd()
{
	 delete_data(m_callbacks);
}

void UIScriptWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData )
{
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

void UIScriptWnd::AddCallback(const luabind::functor<void> &lua_function)
{
	SCallbackInfo* c = NewCallback ();
	c->m_pCallback.set(lua_function);
	
}

void UIScriptWnd::AddCallback(const luabind::object &lua_object, LPCSTR method)
{
	SCallbackInfo* c = NewCallback ();
	c->m_pCallback.set(lua_object, method);
}

void UIScriptWnd::test()
{
	CALLBACK_IT it = m_callbacks.begin();
	for(;it!=m_callbacks.end();++it)
		SCRIPT_CALLBACK_EXECUTE_0( (*it)->m_pCallback )
}

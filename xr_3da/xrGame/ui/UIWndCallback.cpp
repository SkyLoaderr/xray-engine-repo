#include "stdafx.h"

#include "UIWndCallback.h"

//#include <luabind/operator.hpp>
#include "../object_broker.h"
#include "../script_callback.h"

#include "UIWindow.h"

struct SCallbackInfo{
	CScriptCallback			m_callback;
	boost::function<void()>	m_cpp_callback;
	shared_str				m_controlName;
	s16						m_event;
	SCallbackInfo():m_controlName(""),m_event(-1){};
};

struct event_comparer{
	shared_str			name;
	s16					event;

	event_comparer(shared_str n, s16 e):name(n),event(e){}
	bool operator ()(SCallbackInfo* i){
		return( (i->m_controlName==name) && (i->m_event==event) );
	}
};

CUIWndCallback::~CUIWndCallback()
{
	delete_data(m_callbacks);
}

void CUIWndCallback::Register			(CUIWindow* pChild)
{
	pChild->SetMessageTarget( smart_cast<CUIWindow*>(this) );
}

void CUIWndCallback::OnEvent(CUIWindow* pWnd, s16 msg, void* pData)
{
	event_comparer ec(pWnd->WindowName(),msg);

	CALLBACK_IT it = std::find_if(m_callbacks.begin(),m_callbacks.end(),ec);
	if(it==m_callbacks.end())
		return ;

	if ((*it)->m_callback.assigned()) {
		SCRIPT_CALLBACK_EXECUTE_0((*it)->m_callback );
	}else 
		if ( (*it)->m_cpp_callback )	
			(*it)->m_cpp_callback();
}


SCallbackInfo*	CUIWndCallback::NewCallback ()
{
	m_callbacks.push_back( xr_new<SCallbackInfo>() );
	return m_callbacks.back();
}
/*
void CUIWndCallback::AddCallback(LPCSTR control_id, s16 event, const luabind::functor<void> &lua_function)
{
	SCallbackInfo* c	= NewCallback ();
	c->m_callback.set	(lua_function);
	c->m_controlName	= control_id;
	c->m_event			= event;
	
}

void CUIWndCallback::AddCallback (LPCSTR control_id, s16 event, const luabind::object &lua_object, LPCSTR method)
{
	SCallbackInfo* c	= NewCallback ();
	c->m_callback.set	(lua_object,method);
	c->m_controlName	= control_id;
	c->m_event			= event;
}
*/
void CUIWndCallback::AddCallback(LPCSTR control_id, s16 event, boost::function<void()> f)
{
	SCallbackInfo* c	= NewCallback ();
	c->m_cpp_callback	= f;
	c->m_controlName	= control_id;
	c->m_event			= event;
}


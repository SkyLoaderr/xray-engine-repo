#pragma once

#include <boost/function.hpp>
#include <boost/bind.hpp>

struct SCallbackInfo;

class CUIWndCallback
{
	typedef xr_vector<SCallbackInfo*>	CALLBACKS;
	typedef CALLBACKS::iterator			CALLBACK_IT;
private:
			CALLBACKS			m_callbacks;
			SCallbackInfo*		NewCallback			();


public:
	virtual void				OnEvent				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			void				Register			(CUIWindow* pChild);
//			void				AddCallback			(LPCSTR control_id, s16 event, const luabind::functor<void> &lua_function);
//			void				AddCallback			(LPCSTR control_id, s16 event, const luabind::object &lua_object, LPCSTR method);
			void				AddCallback			(LPCSTR control_id, s16 event, boost::function<void()> f);

};
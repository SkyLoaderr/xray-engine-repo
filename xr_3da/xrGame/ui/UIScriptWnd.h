#pragma once

#include "UIDialogWnd.h"
#include "../script_callback.h"
#include "../script_space_forward.h"
#include "../script_export_space.h"

struct SCallbackInfo{
	CScriptCallback		m_pCallback;
};

class UIScriptWnd :public CUIDialogWnd
{
typedef CUIDialogWnd				inherited;
typedef xr_vector<SCallbackInfo*>	CALLBACKS;
typedef CALLBACKS::iterator			CALLBACK_IT;

private:
			CALLBACKS			m_callbacks;
	virtual void				SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			SCallbackInfo*		NewCallback			();
protected:
			bool				Load				(LPCSTR xml_name);

public:
								UIScriptWnd			();
	virtual						~UIScriptWnd		();
			void				AddCallback			(const luabind::functor<void> &lua_function);
			void				AddCallback			(const luabind::object &lua_object, LPCSTR method);
			void				test();
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(UIScriptWnd)
#undef script_type_list
#define script_type_list save_type_list(UIScriptWnd)

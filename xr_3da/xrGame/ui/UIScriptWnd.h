#pragma once

#include "UIDialogWnd.h"
#include "../script_callback.h"
#include "../script_space_forward.h"
#include "../script_export_space.h"

struct SCallbackInfo;


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
			void				Register			(CUIWindow* pChild);
								UIScriptWnd			();
	virtual						~UIScriptWnd		();
			void				AddCallback			(LPCSTR control_id, s16 event, const luabind::functor<void> &lua_function);
			void				AddCallback			(LPCSTR control_id, s16 event, const luabind::object &lua_object, LPCSTR method);
	virtual bool				OnKeyboard			(int dik, EUIMessages keyboard_action);
			void				test();

template<typename T>
T*	GetControl(LPCSTR name){
		ref_str n = name;
		CUIWindow* pWnd = FindChild(n);
		if(pWnd == NULL)
			return NULL;

		return dynamic_cast<T*>(pWnd);
}


	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(UIScriptWnd)
#undef script_type_list
#define script_type_list save_type_list(UIScriptWnd)

#pragma once

#include "script_space_forward.h"

class CScriptCallback;
class CGameObject;

class CUsableScriptObject
{
public:
							CUsableScriptObject		();
							~CUsableScriptObject	();
		bool				use						(CGameObject* who_use);
		void				set_callback			(const luabind::object &lua_object, LPCSTR method);
		void				set_callback			(const luabind::functor<void> &lua_function);
		void				clear_callback			();
	
		//строчка по€вл€юща€с€ при наведении на объект (если NULL, то нет)
		virtual LPCSTR		tip_text				();
		void				set_tip_text			(LPCSTR new_text);
		virtual void		set_tip_text_default	();

		//можно ли использовать объект стандартным (не скриптовым) образом
		bool				nonscript_usable		();
		void				set_nonscript_usable	(bool usable);
private:
		CScriptCallback		*callback;
		shared_str				m_sTipText;
		bool				m_bNonscriptUsable;
};

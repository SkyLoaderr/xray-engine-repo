////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback.h
//	Created 	: 06.02.2004
//  Modified 	: 28.06.2004
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : XRay script callbacks
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"

class CScriptCallback {
private:
	luabind::functor<void>			*m_lua_function;
	luabind::object					*m_lua_object;
	ref_str							m_method_name;

public:
	IC								CScriptCallback		();
	virtual							~CScriptCallback	();
	IC		void					set					(const luabind::functor<void> &lua_function);
	IC		void					set					(const luabind::object &lua_object, LPCSTR method);
	IC		void					clear				();
	IC		void					clear				(bool member);
	IC		luabind::functor<void>	*get_function		();
	IC		luabind::object			*get_object			();
	IC		ref_str					get_method			();
	IC		void					callback			();
	IC		bool					assigned			() const;
};

#include "script_callback_inline.h"
#include "script_callback_macroses.h"
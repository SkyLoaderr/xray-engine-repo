#pragma	   once
#include "script_export_space.h"
#include "script_space_forward.h"
class CScriptCallback;
class CUsableScriptObject
{
public:
							CUsableScriptObject		();
							~CUsableScriptObject	();
		bool				use						();
		void				set_callback			(const luabind::object &lua_object, LPCSTR method);
		void				set_callback			(const luabind::functor<void> &lua_function);
		void				clear_callback			();
private:
		CScriptCallback		*callback;
};

#pragma	once

//#include "script_class0.h"
//struct CScriptClass0 : public CScriptClass1 {
struct CScriptClass0 {
	static void script_register(lua_State *)
	{
		printf	("CScriptClass0::script_register is called!\n");
	}
	virtual ~CScriptClass0(){}
};
add_to_type_list(CScriptClass0)
#undef script_type_list
#define script_type_list save_type_list(CScriptClass0)

#pragma	once

#include "script_class0.h"
struct CScriptClass1 : public CScriptClass0 {
//struct CScriptClass1 {
	static void script_register(lua_State *)
	{
		printf	("CScriptClass1::script_register is called!\n");
	}
	virtual ~CScriptClass1(){}
};
add_to_type_list(CScriptClass1)
#undef script_type_list
#define script_type_list save_type_list(CScriptClass1)


#include "stdafx.h"
#include "game_sv_deathmatch.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

template <typename T>
struct CWrapperBase : public T, public luabind::wrap_base {
	typedef T inherited;
	typedef CWrapperBase<T>	self_type;
//		DEFINE_LUA_WRAPPER_METHOD_V1	(save,			NET_Packet&)
//		DEFINE_LUA_WRAPPER_METHOD_V1	(load,			NET_Packet&)
		//	DEFINE_LUA_WRAPPER_METHOD_V1	(save,			IWriter&)
	DEFINE_LUA_WRAPPER_METHOD_1	(IsBuyableItem,	bool,	LPCSTR)
};

void game_sv_Deathmatch::script_register(lua_State *L)
{

	module(L)
		[
/*			luabind_class_pure0(
			game_sv_Deathmatch,
			"game_sv_Deathmatch"
			)
*/
			luabind::class_<DLL_Pure>("dll_pure"),

			luabind::class_< game_sv_Deathmatch, CWrapperBase<game_sv_Deathmatch>,DLL_Pure >("game_sv_Deathmatch")
			.def(	constructor<>())
//			.def("IsBuyableItem", &CWrapperBase<game_sv_Deathmatch>::IsBuyableItem, &CWrapperBase<game_sv_Deathmatch>::IsBuyableItem_static)

//			DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_1(a,b,save,void,NET_Packet&) \
//			DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_1(a,b,load,void,NET_Packet&) 

		];

}

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>

#pragma comment(lib,"x:/xrLUA.lib")

struct CBaseClass;

struct CCustomClass {
					CCustomClass()
	{
		printf	("CCustomClass::CCustomClass() is called!\n");
	}

	virtual			~CCustomClass()
	{
		printf	("CCustomClass::~CCustomClass() is called!\n");
	}

	virtual void	foo				(CBaseClass *base)
	{
		printf	("CCustomClass::foo() is called!\n");
	}
};

struct CCustomClassWrapper : public CCustomClass, public luabind::wrap_base {
	virtual void foo(CBaseClass *base)
	{
		call<void>("foo",base);
	}
	
	static void foo_static(CCustomClass *self, CBaseClass *base)
	{
		self->CCustomClass::foo(base);
	}
};

CCustomClass			*g_custom = 0;

struct CBaseClass {
								CBaseClass		()
	{
		printf					("CBaseClass::CBaseClass() is called!\n");
	}

	virtual						~CBaseClass		()
	{
		printf					("CBaseClass::~CBaseClass() is called!\n");
	}

			void				update			()
	{
		printf					("CBaseClass::update() is called!\n");
		g_custom					= factory_method();
		g_custom->foo				(this);
	}

	virtual CCustomClass*	factory_method	() = 0;
};

struct CBaseClassWrapper : public CBaseClass, public luabind::wrap_base {
	virtual CCustomClass*	factory_method			()
	{
		return					(call<CCustomClass*>("factory_method")[luabind::adopt(luabind::result)]);
	}
	static CCustomClass*	factory_method_static	(CBaseClass *self)
	{
		printf					("ERROR! Pure virtual function call!\n");
		return					(0);
	}
};

void lesha_test()
{
	lua_State			*L = lua_open();

	if (!L)
		lua_error		(L);

	luaopen_base		(L);
	luaopen_string		(L);
	luaopen_math		(L);
	luaopen_table		(L);
	luaopen_debug		(L);

	luabind::open		(L);
	
	luabind::module(L)
	[
		luabind::class_<CCustomClass,CCustomClassWrapper>("CCustomClass")
			.def(luabind::constructor<>())
			.def("foo",&CCustomClass::foo,&CCustomClassWrapper::foo_static),

		luabind::class_<CBaseClass,CBaseClassWrapper>("CBaseClass")
			.def(luabind::constructor<>())
			.def("factory_method",&CBaseClass::factory_method,&CBaseClassWrapper::factory_method_static,luabind::adopt(luabind::result))
			.def("update",&CBaseClass::update)
	];

	lua_dofile			(L,"x:/test.script");
	lua_State			*T = lua_newthread(L);
	assert				(T);
	int					t_ref = luaL_ref(L,LUA_REGISTRYINDEX);
	
	luaL_loadbuffer		(T,"main()",6,"");
	
	for (;;) {
		int				l_iErrorCode = lua_resume(T,0);
		if (l_iErrorCode)
			break;
	}
	luaL_unref			(L,LUA_REGISTRYINDEX,t_ref);

	delete				(g_custom);
	lua_setgcthreshold	(L,0);
	lua_close			(L);
}
// LUA_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "xrCore.h"
#pragma comment(lib,"x:\\xrCore.lib")

// Lua
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#pragma comment(lib,"x:\\xrLUA.lib")

// Lua-bind
#pragma warning(disable:4244)
#include <luabind\\luabind.hpp>
#pragma warning(default:4244)

class etest
{
public:
	float	a;
public:
			etest	()			{}
			etest	(float _a)	{ a=_a;		}
	void	add		(float _b)	{ a+=_b;	}
	void	msg		()			
	{ 
		printf("Hello from C++ :)\n");
	}
};
std::ostream& operator<<(std::ostream& o, etest& v)		{	return o << v.a; }		// tostring

void	cppf		(int x=0)
{
	printf			("CPPF: %d\n",x);
}

void export2lua		(lua_State* L)
{
	using namespace luabind;
	open			(L);

	function		(L,"cppf",cppf);

	class_<etest>	(L,"etest")
		.def		(constructor<>())
		.def		(constructor<float>())
		.def		("add",&etest::add)
		.def		("msg",&etest::msg)
		.def		(tostring(self))
		;

	class_<Fvector>	(L, "vector")
		.def		(constructor<>())
		.def("set", ( void (Fvector::*) (float,float,float) )	&Fvector::set)	// overloads should specify type
		.def("set", ( void (Fvector::*) (const Fvector&) )		&Fvector::set)	// overloads should specify type
		/*
		.def("add", &Fvector::add)
		.def("sub", &Fvector::sub)
		.def("mul", &Fvector::mul)
		.def("div", &Fvector::div)
		.def("normalize", &Fvector::normalize)
		*/
		;
}

// main
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	lua_State*	luaVM	= lua_open();
	if (NULL == luaVM)	{ printf("Error Initializing lua\n"); return -1; }

	// initialize lua standard library functions 
	luaopen_base	(luaVM); 
	luaopen_table	(luaVM);
	luaopen_io		(luaVM);
	luaopen_string	(luaVM);
	luaopen_math	(luaVM);
	luaopen_debug	(luaVM);

	// export fvector
	export2lua		(luaVM);

	// do some stuff
	lua_dofile						(luaVM, "test.lua");

	try 
	{
		luabind::call_function<void>	(luaVM, "a_lua_function");
	} catch(luabind::error& e)
	{
		lua_State*	L	= e.state();
		printf			("%s",lua_tostring(L, 0));
	}

	lua_close		(luaVM);
	return 0;
}

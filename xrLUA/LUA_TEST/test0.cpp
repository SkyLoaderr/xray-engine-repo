////////////////////////////////////////////////////////////////////////////
//	Module 		: test0.h
//	Created 	: 18.04.2004
//  Modified 	: 18.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Adopt policy test
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(disable:4530)
#include "xrCore.h"
#pragma warning(default:4530)

// Lua
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#pragma comment(lib,"x:/xrLUA.lib")

// Lua-bind
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#pragma warning(disable:4244)
#include "luabind/luabind.hpp"
#include "luabind/adopt_policy.hpp"
#include "luabind/dependency_policy.hpp"
#include "luabind/return_reference_to_policy.hpp"
#include "luabind/out_value_policy.hpp"
#include "luabind/discard_result_policy.hpp"
#include "luabind/iterator_policy.hpp"
#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4530)

__declspec(dllimport) LPSTR g_ca_stdout;

using namespace luabind;
using namespace std;

extern lua_State	*L;

struct A {
					A			()
	{
		printf		("A constructor is called!\n");
	}

	virtual			~A			()
	{
		printf		("A virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		printf		("A virtual function a() is called!\n");
	}
};

struct A_wrapper : public A {
public:
	weak_ref		self;

public:
					A_wrapper	(weak_ref self_) :
						self(self_)
	{
	}

	virtual			~A_wrapper	()
	{
		printf		("A_wrapper virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		call_member<void>(self,"a");
	}

	static	void	a_static	(A *a)
	{
		a->A::a_virtual();
	}
};

struct B : public A {
	virtual			~B			()
	{
		printf		("B virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		printf		("B virtual function a() is called!\n");
		A::a_virtual();
	}

	virtual	void	b_virtual	()
	{
		printf		("B virtual function b() is called!\n");
	}
};

struct B_wrapper : public B {
public:
	weak_ref		self;

public:
					B_wrapper	(weak_ref self_) :
						self(self_)
	{
	}

	virtual			~B_wrapper	()
	{
		printf		("B_wrapper virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		call_member<void>(self,"a");
	}

	static	void	a_static	(B *b)
	{
		b->B::a_virtual();
	}

	virtual	void	b_virtual	()
	{
		call_member<void>(self,"b");
	}

	static	void	b_static	(B *b)
	{
		b->B::b_virtual();
	}
};

struct C : public B
{
	virtual			~C			()
	{
		printf		("C virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		printf		("C virtual function a() is called!\n");
//		A::a_virtual();
		B::a_virtual();
	}

	virtual	void	b_virtual	()
	{
		printf		("C virtual function b() is called!\n");
		B::b_virtual();
	}

	virtual	void	c_virtual	()
	{
		printf		("C virtual function c() is called!\n");
	}
};

struct C_wrapper : public C {
public:
	weak_ref		self;

public:
					C_wrapper	(weak_ref self_) :
						self(self_)
	{
	}

	virtual			~C_wrapper	()
	{
		printf		("C_wrapper virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		call_member<void>(self,"a");
	}

	static	void	a_static	(C *c)
	{
		c->C::a_virtual();
	}

	virtual	void	b_virtual	()
	{
		call_member<void>(self,"b");
	}

	static	void	b_static	(C *c)
	{
		c->C::b_virtual();
	}

	virtual	void	c_virtual	()
	{
		call_member<void>(self,"c");
	}

	static	void	c_static	(C *c)
	{
		c->C::c_virtual();
	}
};

struct M {
protected:
	vector<A*>	m_objects;

public:

	virtual			~M			()
	{
		vector<A*>::iterator	I = m_objects.begin();
		vector<A*>::iterator	E = m_objects.end();
		for ( ; I != E; ++I)
			delete	*I;
	}

			void	update		()
	{
		vector<A*>::iterator	I = m_objects.begin();
		vector<A*>::iterator	E = m_objects.end();
		for ( ; I != E; ++I) {
			(*I)->a_virtual		();
//			(*I)->b_virtual		();
//			(*I)->c_virtual		();
		}
	}

			void	add			(A *c)
	{
		m_objects.push_back		(c);
	}
};

M *m;

M &getM()
{
	if (!m)
		m = new M();
	return		(*m);
}

void lua_bind_error(lua_State *L)
{
	for (int i=-1; ; --i)
		if (lua_isstring(L,i)) {
			LPCSTR	S = lua_tostring(L,i);
			printf	("error : %s\n",S);
		}
		else
			return;
}

extern bool load_file_into_namespace(lua_State *L, LPCSTR S, LPCSTR N, bool bCall = true);

void test0()
{
	string4096		SSS;
	strcpy			(SSS,"");
	g_ca_stdout		= SSS;

	L				= lua_open();

	if (!L)
		lua_error	(L);

	luaopen_base	(L);
	luaopen_string	(L);
	luaopen_math	(L);
	luaopen_table	(L);
	luaopen_debug	(L);

	open			(L);

	luabind::set_error_callback(lua_bind_error);

	module(L)
	[
		class_<A>("A")
			.def(constructor<>())
			.def("a",	&A::a_virtual),

//		class_<A,A_wrapper>("A")
//			.def(constructor<>())
//			.def("a",	&A_wrapper::a_static),
//
//		class_<B,B_wrapper,A>("B")
//			.def(constructor<>())
//			.def("a",	&B_wrapper::a_static)
//			.def("b",	&B_wrapper::b_static),
//
//		class_<C,C_wrapper,bases<B> >("C")
//			.def(constructor<>())
//			.def("a",	&C_wrapper::a_static)
//			.def("b",	&C_wrapper::b_static)
//			.def("c",	&C_wrapper::c_static),

		class_<M>("M")
			.def("add",	&M::add,	adopt(_1)),

		def("getM", &getM)
	];

//	load_file_into_namespace(L,"x:\\adopt_test.script","adopt_test",true);
	lua_dofile		(L,"x:\\adopt_test.script");

	if (xr_strlen(SSS)) {
		printf		("\n%s\n",SSS);
		strcpy		(SSS,"");
		lua_close	(L);
		return;
	}

	lua_setgcthreshold	(L,0);
	for (int i=0; i<20; Sleep(100), ++i) {
		getM().update		();
		lua_setgcthreshold	(L,0);
	}

	delete			m;

	lua_close		(L);
}
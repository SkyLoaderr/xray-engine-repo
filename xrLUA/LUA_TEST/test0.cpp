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
#include "lstate.h"
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

#include "boost/shared_ptr.hpp"

__declspec(dllimport) LPSTR g_ca_stdout;

using namespace luabind;
using namespace std;

extern lua_State	*L;

lua_Debug	stack_levels[64];
int			curr_stack_level = 0;

#define LEVELS1	12	/* size of the first part of the stack */
#define LEVELS2	10	/* size of the second part of the stack */

int my_errorfb (lua_State *L) {
	int level = 1;  /* skip level 0 (it's this function) */
	int firstpart = 1;  /* still before eventual `...' */
	lua_Debug ar;
	if (lua_gettop(L) == 0)
		lua_pushliteral(L, "");
	else if (!lua_isstring(L, 1)) return 1;  /* no string message */
	else lua_pushliteral(L, "\n");
	lua_pushliteral(L, "stack traceback:");
	while (lua_getstack(L, level++, &ar)) {
		if (level > LEVELS1 && firstpart) {
			/* no more than `LEVELS2' more levels? */
			if (!lua_getstack(L, level+LEVELS2, &ar))
				level--;  /* keep going */
			else {
				lua_pushliteral(L, "\n\t...");  /* too many levels */
				while (lua_getstack(L, level+LEVELS2, &ar))  /* find last levels */
					level++;
			}
			firstpart = 0;
			continue;
		}
		lua_pushliteral(L, "\n\t");
		lua_getinfo(L, "Snl", &ar);
		lua_pushfstring(L, "%s:", ar.short_src);
		if (ar.currentline > 0)
			lua_pushfstring(L, "%d:", ar.currentline);
		switch (*ar.namewhat) {
	  case 'g':  /* global */ 
	  case 'l':  /* local */
	  case 'f':  /* field */
	  case 'm':  /* method */
		  lua_pushfstring(L, " in function `%s'", ar.name);
		  break;
	  default: {
		  if (*ar.what == 'm')  /* main? */
			  lua_pushfstring(L, " in main chunk");
		  else if (*ar.what == 'C' || *ar.what == 't')
			  lua_pushliteral(L, " ?");  /* C function or tail call */
		  else
			  lua_pushfstring(L, " in function <%s:%d>",
			  ar.short_src, ar.linedefined);
			   }
		}
		lua_concat(L, lua_gettop(L));
	}
	lua_concat(L, lua_gettop(L));
	return 1;
}

void lua_bind_error(lua_State *L)
{
	for (int i=0, n=lua_gettop(L); -i<n; --i)
		if (lua_isstring(L,i)) {
			LPCSTR	S = lua_tostring(L,i);
			printf	("error : %s\n",S);
		}
		else {
			if (i)
				break;
//			my_errorfb(L);
//			for (int j=0; ; --j)
//				if (lua_isstring(L,j)) {
//					LPCSTR	S = lua_tostring(L,j);
//					printf	("error : %s\n",S);
//				}
//				else
//					return;
		}
	for (int j=curr_stack_level - 1, k=0; j>=0; --j, ++k) {
		//			for (int j=0, k=0; j<curr_stack_level; ++j, ++k) {
		lua_Debug		l_tDebugInfo = stack_levels[j];
		if (!l_tDebugInfo.name)
			printf		("%2d : [C  ] C source code : %s\n",k,l_tDebugInfo.short_src);
		else
			if (!xr_strcmp(l_tDebugInfo.what,"C"))
				printf	("%2d : [C  ] C source code : %s\n",k,l_tDebugInfo.name);
			else
				printf	("%2d : [%s] %s(%d) : %s\n",k,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,l_tDebugInfo.name);
	}
	curr_stack_level = 0;
}

void hook(lua_State *L, lua_Debug *ar)
{
//	for (curr_stack_level=0; curr_stack_level<64; ++curr_stack_level) {
//		if (!lua_getstack(L,curr_stack_level,&stack_levels[curr_stack_level]))
//			break;
//		lua_getinfo	(L,"nSlu",&stack_levels[curr_stack_level]);
//	}
//	return;
	switch	(ar->event) {
		case LUA_HOOKCALL : {
			if (!lua_getstack(L,0,&stack_levels[curr_stack_level]))
				break;
			lua_getinfo	(L,"nSlu",&stack_levels[curr_stack_level]);
			if (curr_stack_level && lua_getstack(L,1,&stack_levels[curr_stack_level - 1]))
				lua_getinfo	(L,"nSlu",&stack_levels[curr_stack_level - 1]);
			++curr_stack_level;
			break;
		}
		case LUA_HOOKRET : {
			--curr_stack_level;
			break;
		}
		case LUA_HOOKLINE : {
			lua_getinfo	(L,"l",ar);
			stack_levels[curr_stack_level].currentline = ar->currentline;
			break;
		}
		case LUA_HOOKTAILRET : {
			break;
		}
		case LUA_HOOKCOUNT : {
			lua_getinfo	(L,"l",ar);
			stack_levels[curr_stack_level].currentline = ar->currentline;
			break;
		}
		default : NODEFAULT;
	}
//	for (curr_stack_level=0; curr_stack_level<64; ++curr_stack_level) {
//		if (!lua_getstack(L,curr_stack_level,&stack_levels[curr_stack_level]))
//			break;
//		lua_getinfo	(L,"nSlu",&stack_levels[curr_stack_level]);
//	}
}

void c_bug()
{
	luabind::object			lua_namespace	= luabind::get_globals(L)["thread_test"];
	luabind::functor<void>	lua_function	= luabind::object_cast<luabind::functor<void> >(lua_namespace["bug"]);
	lua_function			();
}

int print_stack(lua_State *L)
{
	lua_Debug l_tDebugInfo;
	for (int curr_stack_level=0; curr_stack_level<64; ++curr_stack_level) {
		if (!lua_getstack(L,curr_stack_level,&l_tDebugInfo))
			break;
		lua_getinfo	(L,"nSlu",&l_tDebugInfo);
		if (!l_tDebugInfo.name)
			printf		("%2d : [C  ] C source code : %s\n",curr_stack_level,l_tDebugInfo.short_src);
		else
			if (!xr_strcmp(l_tDebugInfo.what,"C"))
				printf	("%2d : [C  ] C source code : %s\n",curr_stack_level,l_tDebugInfo.name);
			else
				printf	("%2d : [%s] %s(%d) : %s\n",curr_stack_level,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,l_tDebugInfo.name);
	}
	return 0;
}

extern bool load_file_into_namespace(lua_State *L, LPCSTR S, LPCSTR N, bool bCall = true);

struct CEmptyClassEnum {};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int print_error_stack(lua_State *L)
{
	lua_Debug		debug_info;
	for (int i=0; i<64; ++i) {
		if (!lua_getstack(L,i,&debug_info))
			break;
		lua_getinfo	(L,"nSlu",&debug_info);
		lua_Debug		l_tDebugInfo = debug_info;
		if (!l_tDebugInfo.name)
			printf		("%2d : [C  ] C source code : %s\n",i,l_tDebugInfo.short_src);
		else
			if (!xr_strcmp(l_tDebugInfo.what,"C"))
				printf	("%2d : [C  ] C source code : %s\n",i,l_tDebugInfo.name);
			else
				printf	("%2d : [%s] %s(%d) : %s\n",i,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,l_tDebugInfo.name);
	}
	return		(0);
}

u32 err_func = 1024;

void print_lua_stack(lua_State *L)
{
	printf("\n");
	for (int i=0; lua_type(L, -i-1); i++)
		printf("%2d : %s\n",-i-1,lua_typename(L, lua_type(L, -i-1)));
}

int start_thread(lua_State *L)
{
	print_lua_stack	(L);
	lua_pushstring	(L,"print_error_stack");
	lua_rawget		(L,LUA_GLOBALSINDEX);
	VERIFY			(lua_isfunction(L,-1));
	err_func		= lua_gettop(L);
	lua_pushstring	(L,"thread_test");
	lua_rawget		(L,LUA_GLOBALSINDEX);
	VERIFY			(lua_istable(L,-1));
	lua_pushstring	(L,"main");
	lua_rawget		(L,-2);
	VERIFY			(lua_isfunction(L,-1));
	lua_remove		(L,-2);
	lua_pushstring	(L,"coroutine");
	lua_rawget		(L,LUA_GLOBALSINDEX);
	VERIFY			(lua_istable(L,-1));
	lua_pushstring	(L,"create");
	lua_rawget		(L,-2);
	VERIFY			(lua_isfunction(L,-1));
	print_lua_stack	(L);
	lua_remove		(L,-2);
	return			(lua_pcall(L,1,1,err_func));
}

int resume_thread(lua_State *L)
{
	VERIFY			(lua_isfunction(L,-1));
	lua_pushstring	(L,"coroutine");
	lua_rawget		(L,LUA_GLOBALSINDEX);
	VERIFY			(lua_istable(L,-1));
	lua_pushstring	(L,"resume");
	lua_rawget		(L,-2);
	VERIFY			(lua_isfunction(L,-1));
	return			(lua_pcall(L,1,0,err_func));
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct A {
	int m_aaa;
					A			()
	{
		m_aaa		= -1;
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

	A&				set			(int aaa)
	{
		m_aaa		= aaa;
		return		(*this);
	}

	int				get			()
	{
		return		(m_aaa);
	}
};

namespace luabind {
	A* get_pointer(boost::shared_ptr<A>& p) 
	{ 
		return p.get(); 
	}

	boost::shared_ptr<const A>* 
		get_const_holder(boost::shared_ptr<A>*)
	{
		return 0;
	}
}

struct A_wrapper : public A, public wrap_base {
public:
					A_wrapper	()
	{
	}

	virtual			~A_wrapper	()
	{
		printf		("A_wrapper virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		call_member<void>("a");
	}

	static	void	a_static	(A *a)
	{
		a->A::a_virtual();
	}
};

struct B : public A {
					B			()
	{
		printf		("B constructor is called!\n");
	}

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

struct B_wrapper : public B, public wrap_base {
public:
	virtual			~B_wrapper	()
	{
		printf		("B_wrapper virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		call_member<void>("a");
	}

	static	void	a_static	(B *b)
	{
		b->B::a_virtual();
	}

	virtual	void	b_virtual	()
	{
		call_member<void>("b");
	}

	static	void	b_static	(B *b)
	{
		b->B::b_virtual();
	}
};

struct C : public B
{
					C			()
	{
		printf		("C constructor is called!\n");
	}

	virtual			~C			()
	{
		printf		("C virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		printf		("C virtual function a() is called!\n");
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

struct C_wrapper : public C, public wrap_base {
public:
	virtual			~C_wrapper	()
	{
		printf		("C_wrapper virtual destructor is called!\n");
	}

	virtual	void	a_virtual	()
	{
		call_member<void>("a");
	}

	static	void	a_static	(C *c)
	{
		c->C::a_virtual();
	}

	virtual	void	b_virtual	()
	{
		call_member<void>("b");
	}

	static	void	b_static	(C *c)
	{
		c->C::b_virtual();
	}

	virtual	void	c_virtual	()
	{
		call_member<void>("c");
	}

	static	void	c_static	(C *c)
	{
		c->C::c_virtual();
	}
};


//#define USE_BOOST_SHARED_PTR

struct M {
protected:
#ifndef USE_BOOST_SHARED_PTR
	typedef C*						pointer;
#else
	typedef boost::shared_ptr<C>	pointer;
#endif
	vector<pointer>					m_objects;

public:

	virtual			~M			()
	{
#ifndef USE_BOOST_SHARED_PTR
		vector<pointer>::iterator	I = m_objects.begin();
		vector<pointer>::iterator	E = m_objects.end();
		for ( ; I != E; ++I)
			delete	*I;
#endif
	}

			void	update		()
	{
		vector<pointer>::iterator	I = m_objects.begin();
		vector<pointer>::iterator	E = m_objects.end();
		for ( ; I != E; ++I) {
			(*I)->a_virtual		();
			(*I)->b_virtual		();
			(*I)->c_virtual		();
		}
	}

			void	add			(pointer c)
	{
		m_objects.push_back		(c);
	}
};

M *m;

M &getM()
{
	if (!m)m = new M();
	return		(*m);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

A* g_a = 0;

void set_a(A *a)
{
	g_a = a;
}

A *get_a()
{
	return(g_a);
}

struct AA {
	u32		aa;
	virtual ~AA(){}
};

struct BB {
	u32		bb;
	virtual ~BB(){}
};

struct UU {
	virtual void uu()
	{
	}
};

struct UU_wrapper : public UU, public wrap_base {
	virtual void uu()
	{
		call_member<void>("uu");
	}

	static void uu_static(UU *ptr)
	{
		ptr->UU::uu();
	}
};

struct CC {
	AA		a;
	BB		b;
	UU		*u;

	void setup(AA &aa)
	{
		a	= aa;
	}

	void setup(BB &bb)
	{
		b	= bb;
	}

	void setup(UU *uu)
	{
		u	= uu;
	}
};

CC *getCC()
{
	static	CC cc;
	return	(&cc);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
xr_vector<LPCSTR>	strs;
const xr_vector<LPCSTR> &get_strs()
{
	return			(strs);
}

struct test_shared_ptr {
	test_shared_ptr()
	{
		printf("test_shared_ptr CONSTRUCTOR is called!\n");
	}

	virtual ~test_shared_ptr()
	{
		printf("test_shared_ptr DESTRUCTOR is called!\n");
	}
};

void test_shared()
{
	typedef boost::shared_ptr<test_shared_ptr> _pointer;
	_pointer	a1 = (boost::shared_ptr<test_shared_ptr>)xr_new<test_shared_ptr>();
	{
		_pointer	a2 = a1;
	}
}

struct nmf_test {
	u32 d;
};

template <bool a>
void nmf(const nmf_test &instance)
{
	printf			("ok : %d",instance.d);
}

void test0()
{
	test_shared();
	string4096		SSS;
	strcpy			(SSS,"");
	g_ca_stdout		= SSS;

	strs.push_back	("string0");
	strs.push_back	("string1");
	strs.push_back	("string2");
	strs.push_back	("string3");
	strs.push_back	("string4");
	strs.push_back	("string5");
	strs.push_back	("string6");
	strs.push_back	("string7");
	strs.push_back	("string8");
	strs.push_back	("string9");

	L				= lua_open();

	if (!L)
		lua_error	(L);

	lua_setgcthreshold	(L,1024);

	luaopen_base	(L);
	luaopen_string	(L);
	luaopen_math	(L);
	luaopen_table	(L);
	luaopen_debug	(L);

	open			(L);

//	luabind::set_error_callback(lua_bind_error);

	std::map<LPCSTR,int>	temp;

	temp.insert(std::make_pair("actor0",0));
	temp.insert(std::make_pair("actor1",1));
	temp.insert(std::make_pair("actor2",2));
	temp.insert(std::make_pair("actor3",3));
	temp.insert(std::make_pair("actor4",4));
	temp.insert(std::make_pair("actor5",5));
	temp.insert(std::make_pair("actor6",6));
	temp.insert(std::make_pair("actor7",7));
	temp.insert(std::make_pair("actor8",8));
	temp.insert(std::make_pair("actor9",9));

	class_<CEmptyClassEnum>		instance("clsid");

	std::map<LPCSTR,int>::const_iterator	I = temp.begin();
	std::map<LPCSTR,int>::const_iterator	E = temp.end();
	for ( ; I != E; ++I)
		instance.enum_("_clsid")[value((*I).first,(*I).second)];

	module(L)
	[
		instance,

		class_<nmf_test>("nmf_test")
			.def(				constructor<>())
			.def_readwrite("d",	&nmf_test::d)
			.def("nmf",			&nmf<true>),

		class_<A,A_wrapper>("A")
			.def(				constructor<>())
//			.def("set",			&A::set,	return_reference_to(_1))
//			.def("get",			&A::get)
			.def("a",			&A::a_virtual, &A_wrapper::a_static),

		class_<B,B_wrapper,A>("B")
			.def(				constructor<>())
			.def("a",			&B::a_virtual, &B_wrapper::a_static)
			.def("b",			&B::b_virtual, &B_wrapper::b_static),

		class_<C,C_wrapper,bases<B> >("C")
			.def(				constructor<>())
			.def("a",			&C::a_virtual, &C_wrapper::a_static)
			.def("b",			&C::b_virtual, &C_wrapper::b_static)
			.def("c",			&C::c_virtual, &C_wrapper::c_static),

		class_<M>("M")
			.def("add",	&M::add, adopt(_2)),

		class_<AA>("AA")
			.def(				constructor<>())
			.def_readwrite("aa",&AA::aa),

		class_<BB>("BB")
			.def(				constructor<>())
			.def_readwrite("bb",&BB::bb),

		class_<UU,UU_wrapper>("UU")
			.def(				constructor<>())
			.def("uu",	&UU::uu, &UU_wrapper::uu_static),

		class_<CC>("CC")
			.def("setup",		(void (CC::*)(AA&))(CC::setup))
			.def("setup",		(void (CC::*)(BB&))(CC::setup))
			.def("setup",		(void (CC::*)(UU*))(CC::setup), adopt(_2)),

		def("getCC",			 &getCC),
		def("getM",				 &getM),
		def("c_bug",			 &c_bug),
		def("print_error_stack", &print_error_stack),
		def("set_a",			 &set_a, adopt(_1)),
		def("get_a",			 &get_a),
//		def("strs",				 &strs, return_stl_iterator)
		def("get_strs",			 &get_strs, return_stl_iterator)
//		def("lua_resume", &lua_resume),
//		def("lua_debug", &lua_debug)
	];

	load_file_into_namespace(L,"x:\\thread_test.script","thread_test",true);
	lua_sethook				(L,hook,LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKCOUNT, 1);
	
//	luabind::object			lua_namespace	= luabind::get_globals(L)["thread_test"];
//	luabind::functor<void>	lua_function	= luabind::object_cast<luabind::functor<void> >(lua_namespace["bug"]);
//	lua_function			();
//	lua_dostring			(L,"thread_test.bug()");

	printf			("top : %d\n",lua_gettop(L));
	for (int i=0; i<0+0*10000; ++i)
	{
		printf			("Starting thread %d\n",i);
		lua_State		*t = lua_newthread(L);
		int				thread_reference = luaL_ref(L,LUA_REGISTRYINDEX);

		lua_sethook		(t,hook,LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKCOUNT, 1);
		LPCSTR			s = "thread_test.main()";
		int				err = luaL_loadbuffer	(t,s,xr_strlen(s),"@_thread_main");
		if (err)
			lua_bind_error(t);
//		bool			started = false;
//		int				err;
		do {
//			if (!started) {
//				err		= start_thread(L);
//				started	= true;
//			}
//			else
//				err		= resume_thread(L);
			err			= lua_resume(t,0);
			if (err) {
				lua_bind_error(t);
				curr_stack_level = 0;
//				lua_settop(t,0);
				break;
			}
			Sleep		(1);
		}
		while (t->ci->state & CI_YIELD);
//		while (true);
		
//		bool			ok = false;
//		for (int i=1, n=lua_gettop(L); i<=n; ++i)
//			if ((lua_type(L,i) == LUA_TTHREAD) && (lua_tothread(L,i) == t)) {
//				lua_remove(L,i);
//				ok		= true;
//				break;
//			}
//		VERIFY			(ok);
		xr_delete			(g_a);
		lua_setgcthreshold	(L,0);
		luaL_unref		(L,LUA_REGISTRYINDEX,thread_reference);
		printf			("top : %d\n",lua_gettop(L));
	}

//	xr_delete			(g_a);
	lua_setgcthreshold	(L,0);
	lua_dofile		(L,"x:\\adopt_test.script");

	if (xr_strlen(SSS)) {
		printf		("\n%s\n",SSS);
		strcpy		(SSS,"");
		lua_close	(L);
		return;
	}

	for (int i=0; i<20; Sleep(100), ++i) {
		getM().update		();
		lua_setgcthreshold	(L,0);
	}

	delete			m;
	lua_setgcthreshold	(L,0);
	lua_setgcthreshold	(L,0);

	lua_close		(L);
}

struct CClassA {
	CClassA()
	{
		printf	("constructor CClassA\n");
	}
};

struct CClassB {
	CClassB()
	{
		printf	("constructor CClassB\n");
	}
};

void register_smth0	(module_ &module_ref)
{
	module_ref
	[
		class_<CClassA>("ClassA")
			.def(		constructor<>())
	];
}

void register_smth1	(module_ &module_ref)
{
	module_ref
		[
			class_<CClassB>("ClassB")
			.def(		constructor<>())
		];
}

struct A1 {};
struct A2 : virtual public A1 {};

struct B1{
	virtual void unique(){}
	virtual void unique(const B1&){}
};

struct B2 : public B1 {
	using B1::unique;
	virtual void unique(const B2 &){}
};

struct B3 : public B2 {
	using B2::unique;
	virtual void unique(const B3 &){}
};

void test1()
{
	B1  b1;
	B2	b2;
	B3	b3;
	
	b2.unique		();
	b2.unique		(b1);
	b2.unique		(b2);
	
	b3.unique		();
	b3.unique		(b1);
	b3.unique		(b2);
	b3.unique		(b3);

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

	module_			game_engine = module(L,"game_engine");
	register_smth0	(game_engine);
	register_smth1	(game_engine);

	module(L)
	[
		class_<A1>("A1"),
		class_<A2,bases<A1> >("A2")
	];

	lua_dofile		(L,"x:\\split_test.script");
	if (xr_strlen(SSS)) {
		printf		("\n%s\n",SSS);
		strcpy		(SSS,"");
		lua_close	(L);
		return;
	}

	lua_close		(L);
}
////////////////////////////////////////////////////////////////////////////
//	Module 		: test0.h
//	Created 	: 18.04.2004
//  Modified 	: 18.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Adopt policy test
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#pragma warning(pop)

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

void print_local_table(lua_State *L, LPCSTR S, bool bRecursive);


//#define LEVELS1	12	/* size of the first part of the stack */
//#define LEVELS2	10	/* size of the second part of the stack */
//
//int my_errorfb (lua_State *L) {
//	int level = 1;  /* skip level 0 (it's this function) */
//	int firstpart = 1;  /* still before eventual `...' */
//	lua_Debug ar;
//	if (lua_gettop(L) == 0)
//		lua_pushliteral(L, "");
//	else if (!lua_isstring(L, 1)) return 1;  /* no string message */
//	else lua_pushliteral(L, "\n");
//	lua_pushliteral(L, "stack traceback:");
//	while (lua_getstack(L, level++, &ar)) {
//		if (level > LEVELS1 && firstpart) {
//			/* no more than `LEVELS2' more levels? */
//			if (!lua_getstack(L, level+LEVELS2, &ar))
//				level--;  /* keep going */
//			else {
//				lua_pushliteral(L, "\n\t...");  /* too many levels */
//				while (lua_getstack(L, level+LEVELS2, &ar))  /* find last levels */
//					level++;
//			}
//			firstpart = 0;
//			continue;
//		}
//		lua_pushliteral(L, "\n\t");
//		lua_getinfo(L, "Snl", &ar);
//		lua_pushfstring(L, "%s:", ar.short_src);
//		if (ar.currentline > 0)
//			lua_pushfstring(L, "%d:", ar.currentline);
//		switch (*ar.namewhat) {
//	  case 'g':  /* global */ 
//	  case 'l':  /* local */
//	  case 'f':  /* field */
//	  case 'm':  /* method */
//		  lua_pushfstring(L, " in function `%s'", ar.name);
//		  break;
//	  default: {
//		  if (*ar.what == 'm')  /* main? */
//			  lua_pushfstring(L, " in main chunk");
//		  else if (*ar.what == 'C' || *ar.what == 't')
//			  lua_pushliteral(L, " ?");  /* C function or tail call */
//		  else
//			  lua_pushfstring(L, " in function <%s:%d>",
//			  ar.short_src, ar.linedefined);
//			   }
//		}
//		lua_concat(L, lua_gettop(L));
//	}
//	lua_concat(L, lua_gettop(L));
//	return 1;
//}
//
//void lua_bind_error(lua_State *L)
//{
//	for (int i=0, n=lua_gettop(L); -i<n; --i)
//		if (lua_isstring(L,i)) {
//			LPCSTR	S = lua_tostring(L,i);
//			printf	("error : %s\n",S);
//		}
//		else {
//			if (i)
//				break;
////			my_errorfb(L);
////			for (int j=0; ; --j)
////				if (lua_isstring(L,j)) {
////					LPCSTR	S = lua_tostring(L,j);
////					printf	("error : %s\n",S);
////				}
////				else
////					return;
//		}
//	for (int j=curr_stack_level - 1, k=0; j>=0; --j, ++k) {
//		//			for (int j=0, k=0; j<curr_stack_level; ++j, ++k) {
//		lua_Debug		l_tDebugInfo = stack_levels[j];
//		if (!l_tDebugInfo.name)
//			printf		("%2d : [C  ] C source code : %s\n",k,l_tDebugInfo.short_src);
//		else
//			if (!xr_strcmp(l_tDebugInfo.what,"C"))
//				printf	("%2d : [C  ] C source code : %s\n",k,l_tDebugInfo.name);
//			else
//				printf	("%2d : [%s] %s(%d) : %s\n",k,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,l_tDebugInfo.name);
//	}
//	curr_stack_level = 0;
//}
//
void print_local_variables(lua_State *);

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
			lua_getinfo	(L,"nSluf",&stack_levels[curr_stack_level]);
			if (curr_stack_level && lua_getstack(L,1,&stack_levels[curr_stack_level - 1]))
				lua_getinfo	(L,"nSluf",&stack_levels[curr_stack_level - 1]);
			print_local_variables(L);
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
			print_local_variables(L);
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
//
//void c_bug()
//{
//	luabind::object			lua_namespace	= luabind::get_globals(L)["thread_test"];
//	luabind::functor<void>	lua_function	= luabind::object_cast<luabind::functor<void> >(lua_namespace["bug"]);
//	lua_function			();
//}
//
//int print_stack(lua_State *L)
//{
//	lua_Debug l_tDebugInfo;
//	for (int curr_stack_level=0; curr_stack_level<64; ++curr_stack_level) {
//		if (!lua_getstack(L,curr_stack_level,&l_tDebugInfo))
//			break;
//		lua_getinfo	(L,"nSlu",&l_tDebugInfo);
//		if (!l_tDebugInfo.name)
//			printf		("%2d : [C  ] C source code : %s\n",curr_stack_level,l_tDebugInfo.short_src);
//		else
//			if (!xr_strcmp(l_tDebugInfo.what,"C"))
//				printf	("%2d : [C  ] C source code : %s\n",curr_stack_level,l_tDebugInfo.name);
//			else
//				printf	("%2d : [%s] %s(%d) : %s\n",curr_stack_level,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,l_tDebugInfo.name);
//	}
//	return 0;
//}
//
//extern bool load_file_into_namespace(lua_State *L, LPCSTR S, LPCSTR N, bool bCall = true);
//
//struct CEmptyClassEnum {};
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
//int print_error_stack(lua_State *L)
//{
//	lua_Debug		debug_info;
//	for (int i=0; i<64; ++i) {
//		if (!lua_getstack(L,i,&debug_info))
//			break;
//		lua_getinfo	(L,"nSlu",&debug_info);
//		lua_Debug		l_tDebugInfo = debug_info;
//		if (!l_tDebugInfo.name)
//			printf		("%2d : [C  ] C source code : %s\n",i,l_tDebugInfo.short_src);
//		else
//			if (!xr_strcmp(l_tDebugInfo.what,"C"))
//				printf	("%2d : [C  ] C source code : %s\n",i,l_tDebugInfo.name);
//			else
//				printf	("%2d : [%s] %s(%d) : %s\n",i,l_tDebugInfo.what,l_tDebugInfo.short_src,l_tDebugInfo.currentline,l_tDebugInfo.name);
//	}
//	return		(0);
//}
//
//u32 err_func = 1024;
//
//void print_lua_stack(lua_State *L)
//{
//	printf("\n");
//	for (int i=0; lua_type(L, -i-1); i++)
//		printf("%2d : %s\n",-i-1,lua_typename(L, lua_type(L, -i-1)));
//}
//
//int start_thread(lua_State *L)
//{
//	print_lua_stack	(L);
//	lua_pushstring	(L,"print_error_stack");
//	lua_rawget		(L,LUA_GLOBALSINDEX);
//	VERIFY			(lua_isfunction(L,-1));
//	err_func		= lua_gettop(L);
//	lua_pushstring	(L,"thread_test");
//	lua_rawget		(L,LUA_GLOBALSINDEX);
//	VERIFY			(lua_istable(L,-1));
//	lua_pushstring	(L,"main");
//	lua_rawget		(L,-2);
//	VERIFY			(lua_isfunction(L,-1));
//	lua_remove		(L,-2);
//	lua_pushstring	(L,"coroutine");
//	lua_rawget		(L,LUA_GLOBALSINDEX);
//	VERIFY			(lua_istable(L,-1));
//	lua_pushstring	(L,"create");
//	lua_rawget		(L,-2);
//	VERIFY			(lua_isfunction(L,-1));
//	print_lua_stack	(L);
//	lua_remove		(L,-2);
//	return			(lua_pcall(L,1,1,err_func));
//}
//
//int resume_thread(lua_State *L)
//{
//	VERIFY			(lua_isfunction(L,-1));
//	lua_pushstring	(L,"coroutine");
//	lua_rawget		(L,LUA_GLOBALSINDEX);
//	VERIFY			(lua_istable(L,-1));
//	lua_pushstring	(L,"resume");
//	lua_rawget		(L,-2);
//	VERIFY			(lua_isfunction(L,-1));
//	return			(lua_pcall(L,1,0,err_func));
//}
//
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//struct A {
//	int m_aaa;
//					A			()
//	{
//		m_aaa		= -1;
//		printf		("A constructor is called!\n");
//	}
//
//	virtual			~A			()
//	{
//		printf		("A virtual destructor is called!\n");
//	}
//
//	virtual	void	a_virtual	()
//	{
//		printf		("A virtual function a() is called!\n");
//	}
//
//	A&				set			(int aaa)
//	{
//		m_aaa		= aaa;
//		return		(*this);
//	}
//
//	int				get			()
//	{
//		return		(m_aaa);
//	}
//};
//
//namespace luabind {
//	A* get_pointer(boost::shared_ptr<A>& p) 
//	{ 
//		return p.get(); 
//	}
//
//	boost::shared_ptr<const A>* 
//		get_const_holder(boost::shared_ptr<A>*)
//	{
//		return 0;
//	}
//}
//
//struct A_wrapper : public A, public wrap_base {
//public:
//					A_wrapper	()
//	{
//	}
//
//	virtual			~A_wrapper	()
//	{
//		printf		("A_wrapper virtual destructor is called!\n");
//	}
//
//	virtual	void	a_virtual	()
//	{
//		call_member<void>("a");
//	}
//
//	static	void	a_static	(A *a)
//	{
//		a->A::a_virtual();
//	}
//};
//
//struct B : public A {
//					B			()
//	{
//		printf		("B constructor is called!\n");
//	}
//
//	virtual			~B			()
//	{
//		printf		("B virtual destructor is called!\n");
//	}
//
//	virtual	void	a_virtual	()
//	{
//		printf		("B virtual function a() is called!\n");
//		A::a_virtual();
//	}
//
//	virtual	void	b_virtual	()
//	{
//		printf		("B virtual function b() is called!\n");
//	}
//};
//
//struct B_wrapper : public B, public wrap_base {
//public:
//	virtual			~B_wrapper	()
//	{
//		printf		("B_wrapper virtual destructor is called!\n");
//	}
//
//	virtual	void	a_virtual	()
//	{
//		call_member<void>("a");
//	}
//
//	static	void	a_static	(B *b)
//	{
//		b->B::a_virtual();
//	}
//
//	virtual	void	b_virtual	()
//	{
//		call_member<void>("b");
//	}
//
//	static	void	b_static	(B *b)
//	{
//		b->B::b_virtual();
//	}
//};
//
//struct C : public B
//{
//					C			()
//	{
//		printf		("C constructor is called!\n");
//	}
//
//	virtual			~C			()
//	{
//		printf		("C virtual destructor is called!\n");
//	}
//
//	virtual	void	a_virtual	()
//	{
//		printf		("C virtual function a() is called!\n");
//		B::a_virtual();
//	}
//
//	virtual	void	b_virtual	()
//	{
//		printf		("C virtual function b() is called!\n");
//		B::b_virtual();
//	}
//
//	virtual	void	c_virtual	()
//	{
//		printf		("C virtual function c() is called!\n");
//	}
//};
//
//struct C_wrapper : public C, public wrap_base {
//public:
//	virtual			~C_wrapper	()
//	{
//		printf		("C_wrapper virtual destructor is called!\n");
//	}
//
//	virtual	void	a_virtual	()
//	{
//		call_member<void>("a");
//	}
//
//	static	void	a_static	(C *c)
//	{
//		c->C::a_virtual();
//	}
//
//	virtual	void	b_virtual	()
//	{
//		call_member<void>("b");
//	}
//
//	static	void	b_static	(C *c)
//	{
//		c->C::b_virtual();
//	}
//
//	virtual	void	c_virtual	()
//	{
//		call_member<void>("c");
//	}
//
//	static	void	c_static	(C *c)
//	{
//		c->C::c_virtual();
//	}
//};
//
//
////#define USE_BOOST_SHARED_PTR
//
//struct M {
//protected:
//#ifndef USE_BOOST_SHARED_PTR
//	typedef C*						pointer;
//#else
//	typedef boost::shared_ptr<C>	pointer;
//#endif
//	vector<pointer>					m_objects;
//
//public:
//
//	virtual			~M			()
//	{
//#ifndef USE_BOOST_SHARED_PTR
//		vector<pointer>::iterator	I = m_objects.begin();
//		vector<pointer>::iterator	E = m_objects.end();
//		for ( ; I != E; ++I)
//			delete	*I;
//#endif
//	}
//
//			void	update		()
//	{
//		vector<pointer>::iterator	I = m_objects.begin();
//		vector<pointer>::iterator	E = m_objects.end();
//		for ( ; I != E; ++I) {
//			(*I)->a_virtual		();
//			(*I)->b_virtual		();
//			(*I)->c_virtual		();
//		}
//	}
//
//			void	add			(pointer c)
//	{
//		m_objects.push_back		(c);
//	}
//};
//
//M *m;
//
//M &getM()
//{
//	if (!m)m = new M();
//	return		(*m);
//}
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
//A* g_a = 0;
//
//void set_a(A *a)
//{
//	g_a = a;
//}
//
//A *get_a()
//{
//	return(g_a);
//}
//
//struct AA {
//	u32		aa;
//	virtual ~AA(){}
//};
//
//struct BB {
//	u32		bb;
//	virtual ~BB(){}
//};
//
//struct UU {
//	virtual void uu()
//	{
//	}
//};
//
//struct UU_wrapper : public UU, public wrap_base {
//	virtual void uu()
//	{
//		call_member<void>("uu");
//	}
//
//	static void uu_static(UU *ptr)
//	{
//		ptr->UU::uu();
//	}
//};
//
//struct CC {
//	AA		a;
//	BB		b;
//	UU		*u;
//
//	void setup(AA &aa)
//	{
//		a	= aa;
//	}
//
//	void setup(BB &bb)
//	{
//		b	= bb;
//	}
//
//	void setup(UU *uu)
//	{
//		u	= uu;
//	}
//};
//
//CC *getCC()
//{
//	static	CC cc;
//	return	(&cc);
//}
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//xr_vector<LPCSTR>	strs;
//const xr_vector<LPCSTR> &get_strs()
//{
//	return			(strs);
//}
//
//struct test_shared_ptr {
//	test_shared_ptr()
//	{
//		printf("test_shared_ptr CONSTRUCTOR is called!\n");
//	}
//
//	virtual ~test_shared_ptr()
//	{
//		printf("test_shared_ptr DESTRUCTOR is called!\n");
//	}
//};
//
//void test_shared()
//{
//	typedef boost::shared_ptr<test_shared_ptr> _pointer;
//	_pointer	a1 = (boost::shared_ptr<test_shared_ptr>)xr_new<test_shared_ptr>();
//	{
//		_pointer	a2 = a1;
//	}
//}
//
//struct nmf_test {
//	u32 d;
//};
//
//template <bool a>
//void nmf(const nmf_test &instance)
//{
//	printf			("ok : %d",instance.d);
//}
//
//void test0()
//{
//	test_shared();
//	string4096		SSS;
//	strcpy			(SSS,"");
//	g_ca_stdout		= SSS;
//
//	strs.push_back	("string0");
//	strs.push_back	("string1");
//	strs.push_back	("string2");
//	strs.push_back	("string3");
//	strs.push_back	("string4");
//	strs.push_back	("string5");
//	strs.push_back	("string6");
//	strs.push_back	("string7");
//	strs.push_back	("string8");
//	strs.push_back	("string9");
//
//	L				= lua_open();
//
//	if (!L)
//		lua_error	(L);
//
//	lua_setgcthreshold	(L,1024);
//
//	luaopen_base	(L);
//	luaopen_string	(L);
//	luaopen_math	(L);
//	luaopen_table	(L);
//	luaopen_debug	(L);
//
//	open			(L);
//
////	luabind::set_error_callback(lua_bind_error);
//
//	std::map<LPCSTR,int>	temp;
//
//	temp.insert(std::make_pair("actor0",0));
//	temp.insert(std::make_pair("actor1",1));
//	temp.insert(std::make_pair("actor2",2));
//	temp.insert(std::make_pair("actor3",3));
//	temp.insert(std::make_pair("actor4",4));
//	temp.insert(std::make_pair("actor5",5));
//	temp.insert(std::make_pair("actor6",6));
//	temp.insert(std::make_pair("actor7",7));
//	temp.insert(std::make_pair("actor8",8));
//	temp.insert(std::make_pair("actor9",9));
//
//	class_<CEmptyClassEnum>		instance("clsid");
//
//	std::map<LPCSTR,int>::const_iterator	I = temp.begin();
//	std::map<LPCSTR,int>::const_iterator	E = temp.end();
//	for ( ; I != E; ++I)
//		instance.enum_("_clsid")[value((*I).first,(*I).second)];
//
//	module(L)
//	[
//		instance,
//
//		class_<nmf_test>("nmf_test")
//			.def(				constructor<>())
//			.def_readwrite("d",	&nmf_test::d)
//			.def("nmf",			&nmf<true>),
//
//		class_<A,A_wrapper>("A")
//			.def(				constructor<>())
////			.def("set",			&A::set,	return_reference_to(_1))
////			.def("get",			&A::get)
//			.def("a",			&A::a_virtual, &A_wrapper::a_static),
//
//		class_<B,B_wrapper,A>("B")
//			.def(				constructor<>())
//			.def("a",			&B::a_virtual, &B_wrapper::a_static)
//			.def("b",			&B::b_virtual, &B_wrapper::b_static),
//
//		class_<C,C_wrapper,bases<B> >("C")
//			.def(				constructor<>())
//			.def("a",			&C::a_virtual, &C_wrapper::a_static)
//			.def("b",			&C::b_virtual, &C_wrapper::b_static)
//			.def("c",			&C::c_virtual, &C_wrapper::c_static),
//
//		class_<M>("M")
//			.def("add",	&M::add, adopt(_2)),
//
//		class_<AA>("AA")
//			.def(				constructor<>())
//			.def_readwrite("aa",&AA::aa),
//
//		class_<BB>("BB")
//			.def(				constructor<>())
//			.def_readwrite("bb",&BB::bb),
//
//		class_<UU,UU_wrapper>("UU")
//			.def(				constructor<>())
//			.def("uu",	&UU::uu, &UU_wrapper::uu_static),
//
//		class_<CC>("CC")
//			.def("setup",		(void (CC::*)(AA&))(CC::setup))
//			.def("setup",		(void (CC::*)(BB&))(CC::setup))
//			.def("setup",		(void (CC::*)(UU*))(CC::setup), adopt(_2)),
//
//		def("getCC",			 &getCC),
//		def("getM",				 &getM),
//		def("c_bug",			 &c_bug),
//		def("print_error_stack", &print_error_stack),
//		def("set_a",			 &set_a, adopt(_1)),
//		def("get_a",			 &get_a),
////		def("strs",				 &strs, return_stl_iterator)
//		def("get_strs",			 &get_strs, return_stl_iterator)
////		def("lua_resume", &lua_resume),
////		def("lua_debug", &lua_debug)
//	];
//
//	load_file_into_namespace(L,"x:\\thread_test.script","thread_test",true);
//	lua_sethook				(L,hook,LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKCOUNT, 1);
//	
////	luabind::object			lua_namespace	= luabind::get_globals(L)["thread_test"];
////	luabind::functor<void>	lua_function	= luabind::object_cast<luabind::functor<void> >(lua_namespace["bug"]);
////	lua_function			();
////	lua_dostring			(L,"thread_test.bug()");
//
//	printf			("top : %d\n",lua_gettop(L));
//	for (int i=0; i<0+0*10000; ++i)
//	{
//		printf			("Starting thread %d\n",i);
//		lua_State		*t = lua_newthread(L);
//		int				thread_reference = luaL_ref(L,LUA_REGISTRYINDEX);
//
//		lua_sethook		(t,hook,LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKCOUNT, 1);
//		LPCSTR			s = "thread_test.main()";
//		int				err = luaL_loadbuffer	(t,s,xr_strlen(s),"@_thread_main");
//		if (err)
//			lua_bind_error(t);
////		bool			started = false;
////		int				err;
//		do {
////			if (!started) {
////				err		= start_thread(L);
////				started	= true;
////			}
////			else
////				err		= resume_thread(L);
//			err			= lua_resume(t,0);
//			if (err) {
//				lua_bind_error(t);
//				curr_stack_level = 0;
////				lua_settop(t,0);
//				break;
//			}
//			Sleep		(1);
//		}
//		while (t->ci->state & CI_YIELD);
////		while (true);
//		
////		bool			ok = false;
////		for (int i=1, n=lua_gettop(L); i<=n; ++i)
////			if ((lua_type(L,i) == LUA_TTHREAD) && (lua_tothread(L,i) == t)) {
////				lua_remove(L,i);
////				ok		= true;
////				break;
////			}
////		VERIFY			(ok);
//		xr_delete			(g_a);
//		lua_setgcthreshold	(L,0);
//		luaL_unref		(L,LUA_REGISTRYINDEX,thread_reference);
//		printf			("top : %d\n",lua_gettop(L));
//	}
//
////	xr_delete			(g_a);
//	lua_setgcthreshold	(L,0);
//	lua_dofile		(L,"x:\\adopt_test.script");
//
//	if (xr_strlen(SSS)) {
//		printf		("\n%s\n",SSS);
//		strcpy		(SSS,"");
//		lua_close	(L);
//		return;
//	}
//
//	for (int i=0; i<20; Sleep(100), ++i) {
//		getM().update		();
//		lua_setgcthreshold	(L,0);
//	}
//
//	delete			m;
//	lua_setgcthreshold	(L,0);
//	lua_setgcthreshold	(L,0);
//
//	lua_close		(L);
//}
//
//struct CClassA {
//	CClassA()
//	{
//		printf	("constructor CClassA\n");
//	}
//};
//
//struct CClassB {
//	CClassB()
//	{
//		printf	("constructor CClassB\n");
//	}
//};
//
//void register_smth0	(module_ &module_ref)
//{
//	module_ref
//	[
//		class_<CClassA>("ClassA")
//			.def(		constructor<>())
//	];
//}
//
//void register_smth1	(module_ &module_ref)
//{
//	module_ref
//		[
//			class_<CClassB>("ClassB")
//			.def(		constructor<>())
//		];
//}
//
//struct A1 {};
//struct A2 : virtual public A1 {};
//
//class CRegistrator {
//public:
//	typedef void (*REGISTER_FUNCTION)(lua_State*);
//	typedef xr_vector<REGISTER_FUNCTION>	REGISTRY;
//	typedef REGISTRY::iterator				iterator;
//	typedef REGISTRY::const_iterator		const_iterator;
//
//protected:
//	REGISTRY	m_registry;
//
//public:
//	IC	void	add				(REGISTER_FUNCTION function)
//	{
//		m_registry.push_back	(function);
//	}
//
//	IC	void	script_register	(lua_State *L)
//	{
//		iterator	I = m_registry.begin();
//		iterator	E = m_registry.end();
//		for ( ; I != E; ++I)
//			(*I)(L);
//	}
//};
//
//IC	CRegistrator &registrator()
//{
//	static CRegistrator object;
//	return				(object);
//}
//
//struct CUniqueA{};
//struct CUniqueB : public CUniqueA {};
//
//void register_classA(lua_State *L)
//{
//	module(L)
//	[
//		class_<CUniqueA>("A")
//			.def(	constructor<>())
//	];
//}
//
//void register_classB(lua_State *L)
//{
//	module(L)
//	[
//		class_<CUniqueB,CUniqueA>("B")
////			.def(	constructor<>())
//	];
//}
//
void print_local_variables(lua_State *L)
{
	return;
////	lua_Debug ar = stack_levels[curr_stack_level];
////	int i;
////	const char *name;
////	i = 1;
////	while ((name = lua_getlocal(L, &ar, i++)) != NULL) {
////		printf("local %d %s\n", i-1, name);
////		lua_pop(L, 1);  /* remove variable value */
////	}
//////	lua_getinfo(L, "f", &ar);  /* retrieves function */
////	i = 1;
////	while ((name = lua_getupvalue(L, -1, i++)) != NULL) {
////		printf("upvalue %d %s\n", i-1, name);
////		lua_pop(L, 1);  /* remove upvalue value */
////	}
//	if (!curr_stack_level)
//		return;
//	lua_Debug ar;
//	int i;
//	const char *name;
//	if (lua_getstack(L, curr_stack_level-1, &ar) == 0)
//		return ;  /* failure: no such level in the stack */
//	i = 1;
//	while ((name = lua_getlocal(L, &ar, i++)) != NULL) {
//		printf("local %d %s\n", i-1, name);
//		if (lua_istable(L,-1))
//			print_local_table(L,"",true);
//		lua_pop(L, 1);  /* remove variable value */
//	}
//	lua_getinfo(L, "f", &ar);  /* retrieves function */
//	i = 1;
//	while ((name = lua_getupvalue(L, -1, i++)) != NULL) {
//		printf("upvalue %d %s\n", i-1, name);
//		lua_pop(L, 1);  /* remove upvalue value */
//	}
}

struct CInternal {
	IC		CInternal	()
	{
		printf	("Internal constructor is called!\n");
	}

	virtual ~CInternal()
	{
		printf	("Internal destructor is called!\n");
	}

	virtual void update()
	{
		printf	("Internal update is called!\n");
	}
};

struct CInternalWrapper : public CInternal, public wrap_base {
	virtual void update()
	{
		printf	("InternalWrapper update is called!\n");
		call<void>("update");
	}

	static void update_static(CInternal *self)
	{
		self->CInternal::update();
	}
};

struct CExternal : public CInternal {
	IC		CExternal	()
	{
		printf	("External constructor is called!\n");
	}

	virtual ~CExternal()
	{
		printf	("External destructor is called!\n");
	}
};

struct CInternalStorage {
	typedef CInternal _type;
	xr_map<u32,_type*>	m_objects;

	void	add			(_type *object, u32 id)
	{
		m_objects.insert	(std::make_pair(id,object));
	}

	_type *get	(u32 id)
	{
		xr_map<u32,_type*>::iterator	I = m_objects.find(id);
		if (I != m_objects.end())
			return	((*I).second);
		return		(0);
	}
};

CInternalStorage *get_internals()
{
	static CInternalStorage storage;
	return					(&storage);
}

//struct CSomeClass {
//	int	some_function(int x)
//	{
//		printf		("%d\n",x);
//		return		(x);
//	}
//};
//
//struct CMemCallbackHolder {
//	boost::function1<int, int>	OnSomeEvent;
//};
//
//#define _CLOSURE_THIS(A)	std::bind1st(std::mem_fun(&A),this)
//#define _CLOSURE(A,B)		std::bind1st(std::mem_fun(&A),B)

//template <class T, class U>
//struct Typelist
//{
//	typedef T Head;
//	typedef U Tail;
//};
//
//struct _A1{
//	virtual ~_A1(){}
//};
//declare1(_A1) // typedef Typelist<_A1,Typelist<CEmptyClass,CEmptyClass> > TypeList__A1;
//#undef final
//#define final declare2(_A1) // #define final TypeList__A1
//
//struct _A2{
//	virtual ~_A2(){}
//};
//declare1(_A2) // typedef Typelist<_A2,Typelist<_A1,Typelist<CEmptyClass,CEmptyClass> > TypeList__A2;
//#undef final
//#define final declare2(_A2) // #define final TypeList__A2
//
//Typelist<_A2,Typelist<_A1,Typelist<CEmptyClass,CEmptyClass> > >

extern void script_test();
extern void delegate_test();
extern void registry_test();
extern void abstract_registry_test();
extern void broker_test();
extern void box_collision_test();
extern void directx_test();

class CLuaValue {
protected:
	luabind::object	m_object;
	LPCSTR			m_name;

public:
	IC				CLuaValue	(luabind::object &object, LPCSTR name)
	{
		m_object	= object;
		m_name		= name;
	}

	virtual	void	assign		() = 0;
};

template <typename _type>
class CLuaValueWrapper : public CLuaValue {
private:
	typedef CLuaValue inherited;

private:
	_type			m_value;

public:
	IC				CLuaValueWrapper	(luabind::object &object, LPCSTR name) :
	  inherited		(object,name)
	  {
	  }

	  virtual	void	assign		()
	  {
		  m_object[m_name] = m_value;
	  }
};

class CLuaValueHolder {
protected:
	xr_vector<CLuaValue*>	m_values;

public:
	template <typename T>
		IC	void	add			(luabind::object object, LPCSTR name)
	{
		CLuaValue			*value = 0;
		xr_vector<CLuaValue*>::const_iterator	I = m_values.begin();
		xr_vector<CLuaValue*>::const_iterator	E = m_values.end();
		for ( ; I != E; ++I)
			if (!xr_strcmp((*I)->name() == name)) {
				value		= *I;
				break;
			}

			if (value)
				return;

			m_values.push_back	(xr_new<CLuaValueWrapper<T> >(object,name));
	}

	template <typename T>
		IC	void	assign		()
	{
		xr_vector<CLuaValue*>::iterator	I = m_values.begin();
		xr_vector<CLuaValue*>::iterator	E = m_values.end();
		for ( ; I != E; ++I)
			(*I)->assign	();
	}
};

void test_object(luabind::object a)
{
	//	luabind::get_globals(a.lua_state())[a] = 5;
	a = 5;
}

void test_object1(luabind::object a, LPCSTR name)
{
	a[name] = 5;
}

void test_object2(luabind::object a, luabind::object b)
{
	a[b] = 5;
}

struct MI0 {
	int a;
	virtual ~MI0(){}
};

struct MI2;

struct MI1 {
	virtual ~MI1(){}
	void add(MI2 *obj)
	{
		obj;
	}
};

struct MI2 : public MI1, public MI0 {
	virtual ~MI2(){};
	virtual void vf(int a)
	{
		a = a + 1;
	}
};

struct MI2W : public MI2, public wrap_base
{
	virtual void vf(int a)
	{
		call<void>("vf",a);
	}
	
	static int vf_static(MI2 *self, int a)
	{
		self->MI2::vf(a);
		return	(a);
	}
};

template <typename T>
struct AW : public T, public wrap_base {};

struct CAbstractClass {
	CAbstractClass(int a){}
	virtual void pure_vf() = 0;
};

struct CAbstractClassWrapper : public CAbstractClass, public wrap_base {
	typedef CAbstractClass inherited;
	CAbstractClassWrapper(int a) : inherited(a) {}
	virtual void pure_vf()
	{
		call_member<void>(this,"pure_vf");
	}

	static void pure_vf_static(CAbstractClass *a)
	{
		printf("Pure virtual function call!\n");
	}
};

struct vA{virtual ~vA(){}};
struct vB : virtual public vA{virtual ~vB(){}};

//#define x 5
//#define z(a) #a
//#define y z(__LINE__)
//
//void aa(const char *s)
//{
//	printf("%s",y);
//}
//
#define z(x) #x
#define y(x) z(x)
#define x y(__LINE__)

extern void print_help(lua_State *);

void test1()
{
	broker_test();
//	printf("%s",y(__LINE__));
//	aa(z(x));
//	test0();
//	box_collision_test	();
//	directx_test		();

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

	module(L)
	[
		class_<CInternal,CInternalWrapper>("internal")
			.def(constructor<>())
			.def("update",&CInternal::update,&CInternalWrapper::update_static),

		class_<CExternal,CInternal>("external")
			.def(constructor<>()),

		class_<CInternalStorage>("internal_storage")
			.def("add",&CInternalStorage::add,adopt(_2))
			.def("get",&CInternalStorage::get),

		class_<MI0>("mi0")
			.def_readonly("a",&MI0::a)
			.def(constructor<>()),

		class_<MI1>("mi1")
			.def(constructor<>())
			.def("add",&MI1::add,adopt(_2)),

		class_<MI2,MI2W,bases<MI1,MI0> >("mi2")
			.def(constructor<>())
			.def("vf",&MI2::vf,&MI2W::vf_static),

		class_<CAbstractClass,CAbstractClassWrapper>("abstract_class")
			.def(constructor<int>())
			.def("pure_vf",&CAbstractClass::pure_vf,&CAbstractClassWrapper::pure_vf_static),

//		class_<vA>("vA"),
//		class_<vB,bases<vA> >("vB"),

		def("test_object",&test_object),
		def("test_object",&test_object1),
		def("test_object",&test_object2),
		def("get_internals",&get_internals)
	];

	print_help		(L);

	lua_sethook		(L,hook,LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKCOUNT, 1);
	lua_dofile		(L,"x:\\beta7_rc4_test.script");
//	lua_dofile		(L,"x:\\heritage_test.script");
	if (xr_strlen(SSS)) {
		printf		("\n%s\n",SSS);
		strcpy		(SSS,"");
		lua_close	(L);
		return;
	}

	lua_close		(L);
}

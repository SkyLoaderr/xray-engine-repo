////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.cpp
//	Created 	: 01.04.2004
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script Engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "ai_space.h"
#include "object_factory.h"
#include "script_process.h"
#include "script_space.h"

#ifdef USE_DEBUGGER
#	include "script_debugger.h"
#endif

extern void export_classes(lua_State *L);

CScriptEngine::CScriptEngine			()
{
//	lua_setgcthreshold		(lua(),64*1024);
	m_current_thread		= 0;
	m_stack_level			= 0;
	m_reload_modules		= false;
	m_global_script_loaded	= false;
	m_return_passed_object_functor = 0;
#ifdef USE_DEBUGGER
	m_scriptDebugger		= xr_new<CScriptDebugger>();
#endif
}

CScriptEngine::~CScriptEngine			()
{
	xr_delete				(m_return_passed_object_functor);
	while (!m_script_processes.empty())
		remove_script_process(m_script_processes.begin()->first);
	flush_log				();
#ifdef USE_DEBUGGER
	xr_delete (m_scriptDebugger);
#endif
}

void CScriptEngine::unload				()
{
	lua_settop				(lua(),m_stack_level);
}

int CScriptEngine::lua_panic			(CLuaVirtualMachine *L)
{
	script_log		(eLuaMessageTypeError,"PANIC");
	if (!print_output(L,"unknown script"))
		print_error(L,LUA_ERRRUN);
	return			(0);
}

void CScriptEngine::lua_hook_call		(CLuaVirtualMachine *L, lua_Debug *tpLuaDebug)
{
	ai().script_engine().script_stack_tracker().script_hook(L,tpLuaDebug);
}

void CScriptEngine::lua_error			(CLuaVirtualMachine *L)
{
	print_error				(L,LUA_ERRRUN);

	Debug.fatal				("LUA error: %s",lua_tostring(L,-1));
}

void lua_cast_failed(CLuaVirtualMachine *L, LUABIND_TYPE_INFO info)
{
//	print_output			(L,ai().script_engine().current_thread(),0);
	ai().script_engine().print_error	(L,LUA_ERRRUN);
	Debug.fatal				("LUA error: cannot cast lua value to %s",info->name());
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifndef BOOST_NO_STRINGSTREAM
#	include <sstream>
#else
#	include <strstream>
#endif

std::string to_string					(luabind::object const& o)
{
	using namespace luabind;
	if (o.type() == LUA_TSTRING) return object_cast<std::string>(o);
	lua_State* L = o.lua_state();
	LUABIND_CHECK_STACK(L);

#ifdef BOOST_NO_STRINGSTREAM
	std::strstream s;
#else
	std::stringstream s;
#endif

	if (o.type() == LUA_TNUMBER)
	{
		s << object_cast<float>(o);
		return s.str();
	}

	s << "<" << lua_typename(L, o.type()) << ">";
#ifdef BOOST_NO_STRINGSTREAM
	s << std::ends;
#endif
	return s.str();
}

void strreplaceall						(std::string &str, LPCSTR S, LPCSTR N)
{
	LPSTR	A;
	int		S_len = xr_strlen(S);
	while ((A = strstr(str.c_str(),S)) != 0)
		str.replace(A - str.c_str(),S_len,N);
}

std::string &process_signature				(std::string &str)
{
	strreplaceall	(str,"custom [","");
	strreplaceall	(str,"]","");
	strreplaceall	(str,"float","number");
	strreplaceall	(str,"lua_State*, ","");
	strreplaceall	(str," ,lua_State*","");
	return			(str);
}

std::string member_to_string			(luabind::object const& e, LPCSTR function_signature)
{
#if !defined(LUABIND_NO_ERROR_CHECKING)
    using namespace luabind;
	lua_State* L = e.lua_state();
	LUABIND_CHECK_STACK(L);

	if (e.type() == LUA_TFUNCTION)
	{
		e.pushvalue();
		detail::stack_pop p(L, 1);

		{
			if (lua_getupvalue(L, -1, 3) == 0) return to_string(e);
			detail::stack_pop p2(L, 1);
			if (lua_touserdata(L, -1) != reinterpret_cast<void*>(0x1337)) return to_string(e);
		}

#ifdef BOOST_NO_STRINGSTREAM
		std::strstream s;
#else
		std::stringstream s;
#endif
		{
			lua_getupvalue(L, -1, 2);
			detail::stack_pop p2(L, 1);
		}

		{
			lua_getupvalue(L, -1, 1);
			detail::stack_pop p2(L, 1);
			detail::method_rep* m = static_cast<detail::method_rep*>(lua_touserdata(L, -1));

			for (std::vector<detail::overload_rep>::const_iterator i = m->overloads().begin();
				i != m->overloads().end(); ++i)
			{
				std::string str;
				i->get_signature(L, str);
				if (i != m->overloads().begin())
					s << "\n";
				s << function_signature << process_signature(str) << ";";
			}
		}
#ifdef BOOST_NO_STRINGSTREAM
		s << std::ends;
#endif
		return s.str();
	}

    return to_string(e);
#else
    return "";
#endif
}

void print_class						(lua_State *L, luabind::detail::class_rep *crep)
{
	std::string			S;
	// print class and bases
	{
		S				= (crep->get_class_type() != luabind::detail::class_rep::cpp_class) ? "LUA class " : "C++ class ";
		S.append		(crep->name());
		typedef std::vector<luabind::detail::class_rep::base_info> BASES;
		const BASES &bases = crep->bases();
		BASES::const_iterator	I = bases.begin(), B = I;
		BASES::const_iterator	E = bases.end();
		if (B != E)
			S.append	(" : ");
		for ( ; I != E; ++I) {
			if (I != B)
				S.append(",");
			S.append	((*I).base->name());
		}
		Msg				("%s {",S.c_str());
	}
	// print class constants
	{
		const luabind::detail::class_rep::STATIC_CONSTANTS	&constants = crep->static_constants();
		luabind::detail::class_rep::STATIC_CONSTANTS::const_iterator	I = constants.begin();
		luabind::detail::class_rep::STATIC_CONSTANTS::const_iterator	E = constants.end();
		for ( ; I != E; ++I)
			Msg		("    const %s = %d;",(*I).first,(*I).second);
		if (!constants.empty())
			Msg		("    ");
	}
	// print class properties
	{
		typedef std::map<const char*, luabind::detail::class_rep::callback, luabind::detail::ltstr> PROPERTIES;
		const PROPERTIES &properties = crep->properties();
		PROPERTIES::const_iterator	I = properties.begin();
		PROPERTIES::const_iterator	E = properties.end();
		for ( ; I != E; ++I)
			Msg	("    property %s;",(*I).first);
		if (!properties.empty())
			Msg		("    ");
	}
	// print class constructors
	{
		const std::vector<luabind::detail::construct_rep::overload_t>	&constructors = crep->constructors().overloads;
		std::vector<luabind::detail::construct_rep::overload_t>::const_iterator	I = constructors.begin();
		std::vector<luabind::detail::construct_rep::overload_t>::const_iterator	E = constructors.end();
		for ( ; I != E; ++I) {
			std::string S;
			(*I).get_signature(L,S);
			strreplaceall	(S,"custom [","");
			strreplaceall	(S,"]","");
			strreplaceall	(S,"float","number");
			strreplaceall	(S,"lua_State*, ","");
			strreplaceall	(S," ,lua_State*","");
			Msg		("    %s %s;",crep->name(),S.c_str());
		}
		if (!constructors.empty())
			Msg		("    ");
	}
	// print class methods
	{
		crep->get_table	(L);
		luabind::object	table(L);
		table.set		();
		for (luabind::object::iterator i = table.begin(); i != table.end(); ++i) {
			luabind::object	object = *i;
			std::string	S;
			S			= "    function ";
			S.append	(to_string(i.key()).c_str());

			strreplaceall	(S,"function __add","operator +");
			strreplaceall	(S,"function __sub","operator -");
			strreplaceall	(S,"function __mul","operator *");
			strreplaceall	(S,"function __div","operator /");
			strreplaceall	(S,"function __pow","operator ^");
			strreplaceall	(S,"function __lt","operator <");
			strreplaceall	(S,"function __le","operator <=");
			strreplaceall	(S,"function __eq","operator ==");
			Msg			("%s",member_to_string(object,S.c_str()).c_str());
		}
	}
	Msg			("};\n");
}

void print_free_functions				(lua_State *L, const luabind::object &object, LPCSTR header, const std::string &indent)
{
	u32							count = 0;
	luabind::object::iterator	I = object.begin();
	luabind::object::iterator	E = object.end();
	for ( ; I != E; ++I) {
		if ((*I).type() != LUA_TFUNCTION)
			continue;
		(*I).pushvalue();
		luabind::detail::free_functions::function_rep* rep = 0;
		if (lua_iscfunction(L, -1))
		{
			if (lua_getupvalue(L, -1, 2) != 0)
			{
				// check the magic number that identifies luabind's functions
				if (lua_touserdata(L, -1) == (void*)0x1337)
				{
					if (lua_getupvalue(L, -2, 1) != 0)
					{
						if (!count)
							Msg("\n%snamespace %s {",indent.c_str(),header);
						++count;
						rep = static_cast<luabind::detail::free_functions::function_rep*>(lua_touserdata(L, -1));
						std::vector<luabind::detail::free_functions::overload_rep>::const_iterator	i = rep->overloads().begin();
						std::vector<luabind::detail::free_functions::overload_rep>::const_iterator	e = rep->overloads().end();
						for ( ; i != e; ++i) {
							std::string	S;
							(*i).get_signature(L,S);
							Msg("    %sfunction %s%s;",indent.c_str(),rep->name(),process_signature(S).c_str());
						}
						lua_pop(L, 1);
					}
				}
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 1);
	}
	{
		std::string				_indent = indent;
		_indent.append			("    ");
		object.pushvalue();
		lua_pushnil		(L);
		while (lua_next(L, -2) != 0) {
			if (lua_type(L, -1) == LUA_TTABLE) {
				if (xr_strcmp("_G",lua_tostring(L, -2))) {
					LPCSTR				S = lua_tostring(L, -2);
					luabind::object		object(L);
					object.set			();
					if (!xr_strcmp("security",S)) {
						S = S;
					}
					print_free_functions(L,object,S,_indent);
				}
			}
#pragma todo("Dima to Dima : Remove this hack if find out why")
			if (lua_isnumber(L,-2)) {
				lua_pop(L,1);
				lua_pop(L,1);
				break;
			}
			lua_pop	(L, 1);
		}
	}
	if (count)
		Msg("%s};",indent.c_str());
}

void print_help							(lua_State *L)
{
	Msg					("\nList of the classes exported to LUA\n");
	luabind::detail::class_registry::get_registry(L)->iterate_classes(L,&print_class);
	Msg					("End of list of the classes exported to LUA\n");
	Msg					("\nList of the namespaces exported to LUA\n");
	print_free_functions(L,luabind::get_globals(L),"","");
	Msg					("End of list of the namespaces exported to LUA\n");
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CScriptEngine::script_export		()
{
	luabind::open						(lua());
	
#ifdef USE_DEBUGGER
	m_scriptDebugger->PrepareLuaBind	();
#endif
	
#ifdef USE_DEBUGGER
	if (!CScriptDebugger::GetDebugger()->Active())
#endif
		luabind::set_error_callback		(CScriptEngine::lua_error);


	luabind::set_cast_failed_callback	(lua_cast_failed);
	lua_atpanic							(lua(),CScriptEngine::lua_panic);
	
	export_classes						(lua());

	load_class_registrators				();
	object_factory().register_script	();

#ifdef DEBUG
#	ifdef USE_DEBUGGER
		if( !CScriptDebugger::GetDebugger()->Active() )
#	endif
			lua_sethook					(lua(),CScriptEngine::lua_hook_call,	LUA_HOOKCALL | LUA_HOOKRET | LUA_HOOKLINE | LUA_HOOKTAILRET,	0);
#endif

#ifdef XRGAME_EXPORTS
	load_common_scripts					();
#endif
	m_stack_level						= lua_gettop(lua());
}

bool CScriptEngine::load_file			(LPCSTR caScriptName, bool bCall)
{
	VERIFY			(bCall);
	string256		l_caNamespaceName;
	_splitpath		(caScriptName,0,0,l_caNamespaceName,0);
	if (!xr_strlen(l_caNamespaceName))
		return		(load_file_into_namespace(caScriptName,"_G",bCall));
	else
		return		(load_file_into_namespace(caScriptName,l_caNamespaceName,bCall));
}

void CScriptEngine::remove_script_process	(LPCSTR process_name)
{
	CScriptProcessStorage::iterator	I = m_script_processes.find(process_name);
	if (I != m_script_processes.end()) {
		xr_delete						((*I).second);
		m_script_processes.erase		(I);
	}
}

void CScriptEngine::add_file			(LPCSTR file_name)
{
	m_load_queue.push_back	(xr_strdup(file_name));
}

void CScriptEngine::load_common_scripts()
{
#ifdef DBG_DISABLE_SCRIPTS
	return;
#endif
	string256		S;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);
	if (!l_tpIniFile->section_exist("common")) {
		xr_delete			(l_tpIniFile);
		return;
	}

	if (l_tpIniFile->line_exist("common","script")) {
		LPCSTR			caScriptString = l_tpIniFile->r_string("common","script");
		u32				n = _GetItemCount(caScriptString);
		string256		I;
		for (u32 i=0; i<n; ++i) {
			add_file	(_GetItem(caScriptString,i,I));
			process		();
			if (object("_G",strcat(I,"_initialize"),LUA_TFUNCTION))
				lua_dostring(lua(),strcat(I,"()"));
		}
	}

	xr_delete			(l_tpIniFile);
}

void CScriptEngine::process	()
{
	string256					S,S1;
	for (u32 i=0, n=m_load_queue.size(); !m_load_queue.empty(); ++i) {
		LPSTR					S2 = m_load_queue.front();
		m_load_queue.pop_front	();
		R_ASSERT2				(xr_strcmp(S2,"_G"),"File name \"_G.script\" is reserved and cannot be used!");
		if ((!*S2 && !m_global_script_loaded) || ((m_reload_modules && (i < n)) || !namespace_loaded(S2))) {
			if (!*S2)
				m_global_script_loaded = true;
			FS.update_path		(S,"$game_scripts$",strconcat(S1,S2,".script"));
			Msg					("* loading script %s",S1);
			load_file			(S,true);
		}
		xr_free					(S2);
	}
	m_reload_modules			= false;
}

void CScriptEngine::register_script_classes	()
{
	u32							n = _GetItemCount(*m_class_registrators);
	string256					I;
	for (u32 i=0; i<n; ++i) {
		_GetItem				(*m_class_registrators,i,I);
		luabind::functor<void>	result;
		if (!functor(I,result)) {
			script_log			(eLuaMessageTypeError,"Cannot load class registrator %s!",I);
			continue;
		}
		result					(const_cast<CObjectFactory*>(&object_factory()));
	}
}

void CScriptEngine::load_class_registrators		()
{
#ifdef DBG_DISABLE_SCRIPTS
	return;
#endif
	string256		S;
	FS.update_path	(S,"$game_data$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);
	if (!l_tpIniFile->section_exist("common")) {
		xr_delete			(l_tpIniFile);
		return;
	}

	if (l_tpIniFile->line_exist("common","class_registrators"))
		m_class_registrators = l_tpIniFile->r_string("common","class_registrators");
	else
		m_class_registrators = "";

	xr_delete			(l_tpIniFile);
}

bool CScriptEngine::function_object(LPCSTR function_to_call, luabind::object &object)
{
	if (!xr_strlen(function_to_call))
		return				(false);

	string256				name_space, function;

	parse_script_namespace	(function_to_call,name_space,function);
	if (xr_strcmp(name_space,"_G")) {
		add_file			(name_space);
		process				();
	}

	if (!this->object(name_space,function,LUA_TFUNCTION))
		return				(false);

	luabind::object			lua_namespace	= this->name_space(name_space);
	object					= lua_namespace[function];
	return					(true);
}

void CScriptEngine::initialize_return_passed_object	()
{
	static bool			initialized = false;
	if (!initialized) {
		initialized		= true;
		lua_dostring	(lua(),"function return_passed_object(obj) return obj end");
		m_return_passed_object_functor = xr_new<luabind::object>();
		R_ASSERT		(function_object("return_passed_object",*m_return_passed_object_functor));
	}
}


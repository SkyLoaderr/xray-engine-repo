////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_ex_non_void.h
//	Created 	: 06.02.2004
//  Modified 	: 11.01.2005
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : Script callbacks with return value
////////////////////////////////////////////////////////////////////////////

#pragma once

#define		macros_header(c,a,b)	IC _return_type operator() (b) {
#define		macros_body(c,a,b)		abstract_body(return _return_type,c,a,b)

template <typename _return_type>
class CScriptCallbackEx : public CScriptCallbackEx_<_return_type> {
public:
#	undef	SCRIPT_CALLBACK_EX_GENERATORS
#	include "script_callback_ex_generators.h"
};

#undef		macros_header
#undef		macros_body

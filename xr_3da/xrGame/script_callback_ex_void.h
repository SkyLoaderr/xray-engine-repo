////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_ex_void.h
//	Created 	: 06.02.2004
//  Modified 	: 11.01.2005
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : Script callbacks with return value but specialized with void
////////////////////////////////////////////////////////////////////////////

#pragma once

#define		macros_header(c,a,b)	IC void operator() (b) {
#define		macros_body(c,a,b)		abstract_body(,c,a,b)

template <>
class CScriptCallbackEx<void> : public CScriptCallbackEx_<void> {
public:
#	undef	SCRIPT_CALLBACK_EX_GENERATORS
#	include "script_callback_ex_generators.h"
};

#undef		macros_header
#undef		macros_body

////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_ex_generators.h
//	Created 	: 06.02.2004
//  Modified 	: 11.01.2005
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : Script callbacks with return value generators
////////////////////////////////////////////////////////////////////////////

#ifndef SCRIPT_CALLBACK_EX_GENERATORS
#define SCRIPT_CALLBACK_EX_GENERATORS

#define callback0(keyword)
#define _callback0 
macros_specialization		(,,callback0(typename _))
macros_header				(,callback0(_),_callback0)
macros_body					(,callback0(__),callback0(__))

#define callback1(keyword)	keyword##1
#define _callback1			make_param(1)
macros_specialization		(template <,>,callback1(typename _))
macros_header				(comma,callback1(_),_callback1)
macros_body					(comma,callback1(__),callback1(__))

#define callback2(keyword)	callback1(keyword), keyword##2
#define _callback2			_callback1, make_param(2)
macros_specialization		(template <,>,callback2(typename _))
macros_header				(comma,callback2(_),_callback2)
macros_body					(comma,callback2(__),callback2(__))

#define callback3(keyword)	callback2(keyword), keyword##3
#define _callback3			_callback2, make_param(3)
macros_specialization		(template <,>,callback3(typename _))
macros_header				(comma,callback3(_),_callback3)
macros_body					(comma,callback3(__),callback3(__))

#define callback4(keyword)	callback3(keyword), keyword##4
#define _callback4			_callback3, make_param(4)
macros_specialization		(template <,>,callback4(typename _))
macros_header				(comma,callback4(_),_callback4)
macros_body					(comma,callback4(__),callback4(__))

#define callback5(keyword)	callback4(keyword), keyword##5
#define _callback5			_callback4, make_param(5)
macros_specialization		(template <,>,callback5(typename _))
macros_header				(comma,callback5(_),_callback5)
macros_body					(comma,callback5(__),callback5(__))

#define callback6(keyword)	callback5(keyword), keyword##6
#define _callback6			_callback5, make_param(6)
macros_specialization		(template <,>,callback6(typename _))
macros_header				(comma,callback6(_),_callback6)
macros_body					(comma,callback6(__),callback6(__))

#define callback7(keyword)	callback6(keyword), keyword##7
#define _callback7			 _callback6, make_param(7)
macros_specialization		(template <,>,callback7(typename _))
macros_header				(comma,callback7(_),_callback7)
macros_body					(comma,callback7(__),callback7(__))

#define callback8(keyword)	callback7(keyword), keyword##8
#define _callback8			_callback7, make_param(8)
macros_specialization		(template <,>,callback8(typename _))
macros_header				(comma,callback8(_),_callback8)
macros_body					(comma,callback8(__),callback8(__))

#undef  callback8
#undef _callback8
#undef  callback7
#undef _callback7
#undef  callback6
#undef _callback6
#undef  callback5
#undef _callback5
#undef  callback4
#undef _callback4
#undef  callback3
#undef _callback3
#undef  callback2
#undef _callback2
#undef  callback1
#undef _callback1
#undef  callback0
#undef _callback0
#endif // SCRIPT_CALLBACK_EX_GENERATORS
////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_ex.h
//	Created 	: 06.02.2004
//  Modified 	: 11.01.2005
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : Script callbacks with return value
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"

#define comma						,
#define concatenizer(a,b)			a##b
#define left_comment				concatenizer(/,*)
#define right_comment				concatenizer(*,/)
#define param_generator(a,b,c,d)	a##b##c d##b
#define function_body(_1,_2,_3,_4,_5,_6) \
	_1 _3 _2\
	IC return_type operator() (_4)\
	{\
		try {					\
			if (m_functor) {\
				if (m_object)\
					macros_return_operator ((*m_functor)(*m_object _5 _6));\
				else\
					macros_return_operator ((*m_functor)(_6));\
			}\
		}\
		catch (...) {\
			clear			();\
		}			\
		macros_return_operator (0);\
	}


template <typename _return_type>
class CScriptCallbackEx_ {
public:
	typedef _return_type							return_type;

private:
	typedef luabind::functor<_return_type>			functor_type;
	typedef luabind::object							object_type;
	typedef functor_type *(CScriptCallbackEx_::*unspecified_bool_type) () const;

protected:
	functor_type				*m_functor;
	object_type					*m_object;

public:
	IC							CScriptCallbackEx_	();
	IC							CScriptCallbackEx_	(const CScriptCallbackEx_ &callback);
	IC	virtual					~CScriptCallbackEx_	();
	IC			void			set					(const functor_type &functor);
	IC			void			set					(const functor_type &functor, const object_type &object);
	IC			void			init				();
	IC			void			clear				();
	IC			functor_type	*functor			() const;
	IC			object_type		*object				() const;
	IC			bool			assigned			() const;
	IC			bool			operator!			() const {return (!m_functor);}
	IC			operator unspecified_bool_type		() const {return (!m_functor ? 0 : &CScriptCallbackEx_::functor);}
};

#include "script_callback_ex_inline.h"
#include "script_callback_ex_nonvoid.h"
#include "script_callback_ex_void.h"

#undef comma
#undef concatenizer
#undef left_comment
#undef right_comment
#undef param_generator
#undef function_body
////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_ex.h
//	Created 	: 06.02.2004
//  Modified 	: 11.01.2005
//	Author		: Sergey Zhemeitsev and Dmitriy Iassenev
//	Description : Script callbacks with return value
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space.h"

#define comma							,
#define make_param(a)					_##a __##a
#define macros_specialization(c,b,a)	c a b
#define abstract_body(r,c,a,b) \
	try {					\
		if (m_functor) {\
			if (m_object)\
				r ((*m_functor)(*m_object c a));\
			else\
				r ((*m_functor)(b));\
		}\
	}\
	catch (...) {\
		clear			();\
	}			\
	r (0);\
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
#undef make_param
#undef macros_specialization
#undef abstract_body
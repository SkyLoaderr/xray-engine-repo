////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export_macroses.h
//	Created 	: 24.06.2004
//  Modified 	: 24.06.2004
//	Author		: Andy Kolomiets, Dmitriy Iassenev
//	Description : XRay Script export macroses
////////////////////////////////////////////////////////////////////////////

#pragma once

#define MAKE_WRAPPER_NAME(cls)                                                          \
	cls##_wrapper

#define DEFINE_LUA_WRAPPER_HEADER_0	(cls)                                               \
	struct MAKE_WRAPPER_NAME(cls) : public cls, public luabind::wrap_base {				\
		typedef cls inherited;                                                          \
		MAKE_WRAPPER_NAME(cls) ():inherited() {}

#define DEFINE_LUA_WRAPPER_FOOTER(cls)													\
	};

#define DEFINE_LUA_WRAPPER_METHOD_0	(v_func_name,ret_type)								\
		virtual ret_type v_func_name()													\	
		{																				\	
			call_member<ret_type>(#v_func_name);										\	
		}																				\	
		static ret_type v_func_name##_static(inherited* ptr)							\	
		{                                                                               \	
			return ptr->inherited::v_func_name();										\	
		}

#define DEFINE_LUA_WRAPPER_METHOD_1	(v_func_name,ret_type,t1)							\
		virtual ret_type v_func_name(t1 p1)                                    			\	
		{																				\	
			call_member<ret_type>(#v_func_name,p1);										\	
		}                                   											\	
		static  ret_type v_func_name##_static(inherited* ptr, t1 p1)            		\	
		{																				\	
			return ptr->inherited::v_func_name(p1);                          			\	
		}

#define DEFINE_LUA_WRAPPER_METHOD_2	(v_func_name,ret_type,t1,t2)						\
		virtual ret_type v_func_name(t1 p1, t2 p2)                                    	\	
		{																				\	
			call_member<ret_type>(#v_func_name,p1,p2);									\	
		}                                   											\	
		static  ret_type v_func_name##_static(inherited* ptr, t1 p1, t2 p2)            	\	
		{																				\	
			return ptr->inherited::v_func_name(p1,p2);                          		\	
		}

#define DEFINE_LUA_WRAPPER_METHOD_3	(v_func_name,ret_type,t1,t2,t3)						\
		virtual ret_type v_func_name(t1 p1, t2 p2, t3 p3)                               \	
		{																				\	
			call_member<ret_type>(#v_func_name,p1,p2,p3);								\	
		}                                   											\	
		static  ret_type v_func_name##_static(inherited* ptr, t1 p1, t2 p2, t3 p3)      \	
		{																				\	
			return ptr->inherited::v_func_name(p1,p2,p3);                          		\	
		}

#define DEFINE_LUA_WRAPPER_METHOD_4	(v_func_name,ret_type,t1,t2,t3,t4)					\
		virtual ret_type v_func_name(t1 p1, t2 p2, t3 p3, t4 p4)						\	
		{																				\	
			call_member<ret_type>(#v_func_name,p1,p2,p3,p4);							\	
		}                                   											\	
		static  ret_type v_func_name##_static(inherited* ptr, t1 p1, t2 p2, t3 p3, t4 p4)\	
		{																				\	
			return ptr->inherited::v_func_name(p1,p2,p3,p4);                          	\	
		}

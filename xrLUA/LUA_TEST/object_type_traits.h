////////////////////////////////////////////////////////////////////////////
//	Module 		: object_type_traits.h
//	Created 	: 21.01.2003
//  Modified 	: 12.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Object type traits
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OBJECT_TYPE_TRAITS
#define XRAY_OBJECT_TYPE_TRAITS

//#define USE_BOOST

#ifdef USE_BOOST
#	include <boost/type_traits/is_base_and_derived.hpp>
#	include <boost/type_traits/is_pointer.hpp>
#	include <boost/type_traits/remove_pointer.hpp>
#	include <boost/type_traits/remove_const.hpp>
	namespace object_type_traits = boost;
#else
	#define declare_has(a) \
		template <typename T>\
		struct has_##a {\
			template <typename P> static detail::yes	select(typename P::a*);\
			template <typename P> static detail::no		select(...);\
			enum { value = sizeof(detail::yes) == sizeof(select<T>(0)) };\
		};

	namespace object_type_traits {
		namespace detail {

			struct yes {char a[1];};
			struct no  {char a[2];};

			template <typename T> static T &invoke();
			template <typename T> struct other{};
		};

		template <typename T> struct remove_pointer{
			typedef T type;
		};

		template <typename T> struct remove_pointer<T*>{
			typedef T type;
		};

		template <typename T> struct remove_pointer<T* const>{
			typedef T type;
		};

		template <typename T> struct remove_reference{
			typedef T type;
		};

		template <typename T> struct remove_reference<T&>{
			typedef T type;
		};

		template <typename T> struct remove_reference<T const &>{
			typedef T type;
		};

		template <typename T> struct remove_const{
			typedef T type; 
		};

		template <typename T> struct remove_const<T const>{
			typedef T type; 
		};

		template <typename T>
		struct is_pointer {
			template <typename P> static detail::yes select(P*);
			static detail::no select(...);

			enum { value = sizeof(detail::yes) == sizeof(select(detail::invoke<T>()))};
		};

		template <typename T>
		struct is_reference {
			template <typename P> static detail::yes select(detail::other<P&>);
			static detail::no select(...);

			enum { value = sizeof(detail::yes) == sizeof(select(detail::other<T>()))};
		};

		template <typename T1, typename T2>
		struct is_same {
			template <typename T> static detail::yes select(T*,T*);
								  static detail::no	 select(...);

			enum { 
				value = 
					is_class<T1>::result && 
					is_class<T2>::result && 
					sizeof(detail::yes) == sizeof(
						select(
							&detail::invoke<T1>(),
							&detail::invoke<T2>()
						)
					)
			};
		};

		template <typename T1, typename T2>
		struct is_base_and_derived {
			static detail::yes	select(T1*);
			static detail::no	select(...);

			enum { 
				value = 
					is_class<T1>::result && 
					is_class<T2>::result && 
					sizeof(detail::yes) == sizeof(select(&detail::invoke<T2>())) &&
					!is_same<T1,T2>::value
			};
		};

		declare_has(iterator);
		declare_has(const_iterator);
//		declare_has(reference);
//		declare_has(const_reference);
		declare_has(value_type);
		declare_has(size_type);

		template <typename T>
		struct is_stl_container {
			enum { 
				value = 
					has_iterator<T>::value &&
					has_const_iterator<T>::value &&
//					has_reference<T>::value &&
//					has_const_reference<T>::value &&
					has_size_type<T>::value &&
					has_value_type<T>::value
			};
		};
	};
#endif
#endif
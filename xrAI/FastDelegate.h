//						FastDelegate.h 
//	Efficient delegates in standard C++ that generate only two lines of asm code!
//  Documentation is found at http://www.codeproject.com/cpp/FastDelegate.asp
//
//						- Don Clugston, Mar 2004.
// History:
// 24-Apr-04 1.0  Submitted to CodeProject.
// 28-Apr-04 1.1  Some improvement to portability, code clarity, and comments.
//				  * Prevent unsafe use of evil static function hack.
//				  * Improved syntax for horrible_cast (thanks Paul Bludov).
//				  * Tested on Metrowerks MWCC and Intel ICL (IA32)
//				  * Compiled, but not run, on Comeau C++ and Intel Itanium ICL.

// Uncomment the following #define for optimally-sized of delegates.
// In this case, the generated asm code is almost identical to the code you'd get
// if the compiler had native support for delegates.
// It will not work on systems where sizeof(dataptr) < sizeof(codeptr). 
// Thus, it will not work for DOS compilers using the medium model. It will also
// probably fail on some DSP systems.
#define FASTDELEGATE_USESTATICFUNCTIONHACK

// This next macro ensures that class 'CLASSNAME' uses full generality member function pointers.
// It is only required for the virtual inheritance case in Intel, 
// and in MSVC when the /vmg compiler option is not used. 
// It just declares a harmless constant in an private namespace, but in doing so,
// it forces the compiler to use full generality member function pointers for the class.
#define FASTDELEGATEDECLARE(CLASSNAME)		\
class CLASSNAME;							\
namespace fastdelegate { namespace detail {	\
const int CLASSNAME##workaround = sizeof( void (CLASSNAME::*)(void)); } }

namespace fastdelegate {

namespace detail {	// we'll hide the implementation details in a nested namespace.

////////////////////////////////////////////////////////////////////////////////
//						Helper templates
//
////////////////////////////////////////////////////////////////////////////////

//		horrible_cast< >
// This is truly evil. It completely subverts C++'s type system, allowing you to cast 
// from any class to any other class. Don't use it unless you absolutely have to.
// Usage is identical to reinterpret_cast<>
template <class OutputClass, class InputClass>
inline OutputClass horrible_cast(InputClass input){
	union {
		OutputClass out;
		InputClass in;
	} u;
	u.in = input;
	return u.out;
}

////////////////////////////////////////////////////////////////////////////////
//						Fast Delegates, part 1:
//
//		Conversion of member function pointer to a standard form
//
////////////////////////////////////////////////////////////////////////////////

	// fake class, ONLY used to provide a type.
	// It is vitally important that it is never defined, so that the compiler doesn't
	// think it can optimize the invocation. For example, Borland generates simpler
	// code if it knows the class only uses single inheritance.

// Microsoft and Intel need to be treated as a special case.
// Metrowerks CodeWarrior and Intel fraudulently define Microsoft's identifier, _MSC_VER
#if defined(_MSC_VER) && !defined(__MWERKS__)
	class __single_inheritance GenericClass;
	// For Microsoft, we want to ensure that it's the most efficient type of MFP (4 bytes),
	// even when the /vmg option is used. Declaring an empty class would give 16 byte
	// pointers in this case.
#else
	class GenericClass;
#endif

// The size of a single inheritance member function pointer.
const int SINGLE_MEMFUNCPTR_SIZE = sizeof(void (GenericClass::*)());

//						SimplifyMemFunc< >::Convert()
//
//	A template function that converts an arbitrary member function pointer into the 
//	simplest possible form of member function pointer, using a supplied 'this' pointer.
//  According to the standard, this can be done legally with reinterpret_cast<>.
//	For (non-standard) compilers which use member function pointers which vary in size 
//  depending on the class, we need to use	knowledge of the internal structure of a 
//  member function pointer, as used by the compiler. Template specialization is used
//  to distinguish between the sizes. Because some compilers don't support partial 
//	template specialisation, I use full specialisation of a wrapper struct.

// general case -- don't know how to convert it. Force a compile failure
template <int N>
struct SimplifyMemFunc {
	template <class X, class XFuncType, class GenericMemFuncType>
	static GenericClass *Convert(X *pthis, XFuncType function_to_bind, 
		GenericMemFuncType &bound_func) { 
		// Unsupported member function type -- force a compile failure.
	    // (it's illegal to have a array with negative size).
		typedef char ERROR_Unsupported_member_function_pointer_on_this_compiler[N-100];
		return 0; 
	}
};

// For compilers where all member func ptrs are the same size, everything goes here.
// For non-standard compilers, only single_inheritance classes go here.
template <>
struct SimplifyMemFunc<SINGLE_MEMFUNCPTR_SIZE>  {	
	template <class X, class XFuncType, class GenericMemFuncType>
	static GenericClass *Convert(X *pthis, XFuncType function_to_bind, 
			GenericMemFuncType &bound_func) {
#if defined __DMC__  
		// Digital Mars doesn't allow you to cast between abitrary PMF's, 
		// even though the standard says you can.
		bound_func = horrible_cast<GenericMemFuncType>(function_to_bind);
#else 
        bound_func = reinterpret_cast<GenericMemFuncType>(function_to_bind);
#endif
        return reinterpret_cast<GenericClass *>(pthis);
	}
};

// Compilers with member function pointers which violate the standard (MSVC, Intel),
// need to be treated as a special case. Metrowerks and Intel both fraudently pretend
// to be MSVC, so we need to filter Metrowerks out.
#if defined(_MSC_VER) && !defined(__MWERKS__)

// __multiple_inheritance classes go here
// Nasty hack for Microsoft and Intel (IA32 and Itanium)
template<>
struct SimplifyMemFunc< SINGLE_MEMFUNCPTR_SIZE + sizeof(int) >  {
	template <class X, class XFuncType, class GenericMemFuncType>
	static GenericClass *Convert(X *pthis, XFuncType function_to_bind, 
		GenericMemFuncType &bound_func) { 
		// We need to use a horrible_cast to do this conversion.
		// In MSVC, a multiple inheritance member pointer is internally defined as:
        union {
			XFuncType func;
			struct {	 
				GenericMemFuncType m_funcaddress; // points to the actual member function
				int m_delta;	     // #BYTES to be added to the 'this' pointer
			};
        } u;
        u.func = function_to_bind;
		bound_func = u.m_funcaddress;
		return reinterpret_cast<GenericClass *>(reinterpret_cast<char *>(pthis) + u.m_delta); 
	}
};

// virtual inheritance is a real nuisance. It's inefficient and complicated.
// On MSVC, we don't have enough information to convert it to a closure pointer,
// so we generate a compile-time error.
template <>
struct SimplifyMemFunc<SINGLE_MEMFUNCPTR_SIZE + 2*sizeof(int) >
{
	template <class X, class XFuncType, class GenericMemFuncType>
	static GenericClass *Convert(X *pthis, XFuncType function_to_bind, 
		GenericMemFuncType &bound_func) { 
		// Virtual inheritance pointers are non-standard in MSVC -- force a compile failure.
	    // (it's illegal to have a array with negative size).
		// There are two alternative workarounds:
		// (a) use the FASTDELEGATEDECLARE(CLASSNAME) macro before the declaration
		//     of the class; or
		// (b) compile your project with the /vmg compiler option.
		// For Intel, you have to use the macro.
		typedef char ERROR_VirtualInheritanceIsUnsafeOnMSVC[-100];
		return 0; 
	}
};

// Nasty hack for Microsoft and Intel (IA32 and Itanium)
// unknown_inheritance classes go here 
// This is probably the ugliest bit of code I've ever written. Look at the casts!
template <>
struct SimplifyMemFunc<SINGLE_MEMFUNCPTR_SIZE + 3*sizeof(int) >
{
	template <class X, class XFuncType, class GenericMemFuncType>
	inline static GenericClass *Convert(X *pthis, XFuncType function_to_bind, 
			GenericMemFuncType &bound_func) {
		// The member function pointer is 16 bytes long. We can't use a normal cast, but
		// we can use a union to do the conversion.
		union {
			XFuncType func;
			// In VC++ and ICL, an unknown_inheritance member pointer is internally defined as:
			struct {
				GenericMemFuncType m_funcaddress; // points to the actual member function
				int m_delta;		// #bytes to be added to the 'this' pointer
				int m_vtordisp;		// #bytes to add to 'this' to find the vtable
				int m_vtable_index; // or 0 if no virtual inheritance
			};
		} u;
		u.func = function_to_bind;
		bound_func = u.m_funcaddress;
		int virtual_delta = 0;
		if (u.m_vtable_index) { // Virtual inheritance is used
			// First, get to the vtable. It is 'vtordisp' bytes from the start of the class.
			int * vtable = *reinterpret_cast<int **>(
				reinterpret_cast<char *>(pthis) + u.m_vtordisp );

			// 'vtable_index' tells us where in the table we should be looking.
			virtual_delta = u.m_vtordisp + *reinterpret_cast<int *>( 
				reinterpret_cast<char *>(vtable) + u.m_vtable_index);
		};
		// The int at 'virtual_delta' gives us the amount to add to 'this'.
        // Finally we can add the three components together. Phew!
        return reinterpret_cast<GenericClass *>(
			reinterpret_cast<char *>(pthis) + u.m_delta + virtual_delta);
	};
};


#endif

////////////////////////////////////////////////////////////////////////////////
//						Fast Delegates, part 2:
//
//	Define the delegate storage, and cope with static functions
//
////////////////////////////////////////////////////////////////////////////////

//						ClosurePtr<>
// 
// This class stores a delegate. GenericMemFunc must be a type of GenericClass 
// member function pointer. StaticFuncPtr must be a type of function pointer 
// with the same signature as GenericMemFunc.
// 
// There's no 'varargs' template functionality in C++ at present, so we need a 
// wrapper class to do the invoking and the function typedefs. 
// This class does everything else.

// There are a few ways of dealing with static function pointers.
// There's a standard-compliant, but tricky method.
// There's also a straightforward hack, that won't work on DOS compilers using the
// medium memory model. It's so evil that I can't recommend it, but I've
// implemented it anyway.
// Both methods are identical for member functions.

#if !defined(FASTDELEGATE_USESTATICFUNCTIONHACK)

//				ClosurePtr<> - Safe version
//
// This implementation is standard-compliant, but a bit tricky.
// I store the function pointer inside the class, and the delegate then
// points to itself. Whenever the delegate is copied, these self-references
// must be transformed, and this complicates the = and == operators.

template < class GenericMemFunc, class StaticFuncPtr>
class ClosurePtr {
private:
	GenericClass *m_pthis;
	GenericMemFunc m_pFunction;
	StaticFuncPtr m_pStaticFunction;

	template< class DerivedClass >
	inline const GenericClass *GetSelf() const {
		return reinterpret_cast<const GenericClass *>(
			static_cast<const DerivedClass *>(this)); }

	template< class DerivedClass >
	inline GenericClass *GetSelf() {
		return reinterpret_cast<GenericClass *>(static_cast<DerivedClass *>(this)); }

	template< class DerivedClass >
	inline bool IsSelfFunction() const { // Is the delegate pointing to itself?		
	  return GetSelf<DerivedClass>()==m_pthis; }
protected:
	// The next two functions are for operator ==, =, and the copy constructor.
	// We may need to convert the m_pthis pointers, so that
	// they remain as self-references.
	template< class DerivedClass >
	inline bool IsEqual(const DerivedClass &y) const {
		const ClosurePtr &x = static_cast<const ClosurePtr &>(y);
		if (m_pFunction!=x.m_pFunction) return false;
		if (!IsSelfFunction<DerivedClass>()) return m_pthis==x.m_pthis;
		if (!x.IsSelfFunction<DerivedClass>()) return false;
		return m_pStaticFunction==x.m_pStaticFunction;
	}

	template< class DerivedClass >
	inline void CopyFrom (const DerivedClass &y) {
		const ClosurePtr &x = static_cast<const ClosurePtr &>(y);
		if (x.IsSelfFunction<DerivedClass>()) {
			// transform self references...
			m_pthis=GetSelf<DerivedClass>(); 
			m_pStaticFunction=x.m_pStaticFunction;
		} else m_pthis=x.m_pthis;
		m_pFunction = x.m_pFunction;
	}

	// These functions are for setting the delegate to a member function, or to a 
	// static function.
	// Here's the clever bit: we convert an arbitrary member function into a 
	// standard form. XMemFunc should be a member function of class X, but I can't 
	// enforce that here. It needs to be enforced by the wrapper class.
	template < class X, class XMemFunc >
	inline void bindmemfunc(X *pthis, XMemFunc function_to_bind ) {
		m_pthis= SimplifyMemFunc< sizeof(function_to_bind) >
			::Convert(pthis, function_to_bind, m_pFunction);
	}
	// For static functions, the 'static_function_invoker' class in the parent 
	// will be called. The parent then needs to call GetStaticFunction() to find out 
	// the actual function to invoke.
	template < class DerivedClass, class ParentInvokerSig >
	inline void bindstaticfunc(DerivedClass *pParent, ParentInvokerSig static_function_invoker, 
				StaticFuncPtr function_to_bind ) {
		m_pStaticFunction=function_to_bind; 
		bindmemfunc(pParent, static_function_invoker);
	}
	// These functions are required for invoking the stored function
	inline GenericClass *GetClosureThis() const { return m_pthis; };
	inline GenericMemFunc GetClosureMemPtr() const { return m_pFunction; };
	inline StaticFuncPtr GetStaticFunction() const { return m_pStaticFunction; };
	ClosurePtr() : m_pthis(0), m_pFunction(0) {};
public:
	inline bool operator ! () const		// Is it bound to anything?
	{ return (m_pFunction)==0; };
};

#else

//				ClosurePtr<> - Evil version
//
// For compilers where data pointers are at least as big as code pointers, it is 
// possible to store the function pointer in the this pointer, using another 
// horrible_cast. Invocation isn't any faster, but it saves 4 bytes, and
// speeds up comparison and assignment. If C++ provided direct language support
// for delegates, they would produce asm code that was almost identical to this.

template < class GenericMemFunc, class StaticFuncPtr>
class ClosurePtr {
private:
	GenericClass *m_pthis;
	GenericMemFunc m_pFunction;
protected:
	// The next two functions are for operator ==, =, and the copy constructor.
	inline bool IsEqual(const ClosurePtr &x) const {
		return m_pFunction==x.m_pFunction && m_pthis==x.m_pthis;
	};
	inline void CopyFrom (const ClosurePtr &x) {
		m_pFunction= x.m_pFunction; m_pthis=x.m_pthis;
	};
	// These functions are for setting the delegate to a member function, or to a 
	// static function.
	// Here's the clever bit: we convert an arbitrary member function into a 
	// standard form. XMemFunc should be a member function of class X, but I can't 
	// enforce that here. It needs to be enforced by the wrapper class.
	template < class X, class XMemFunc >
	inline void bindmemfunc(X *pthis, XMemFunc function_to_bind ) {
		m_pthis= SimplifyMemFunc< sizeof(function_to_bind) >
			::Convert(pthis, function_to_bind, m_pFunction);
	}
	// For static functions, the 'static_function_invoker' class in the parent 
	// will be called. The parent then needs to call GetStaticFunction() to find out 
	// the actual function to invoke.
	// ******** EVIL, EVIL CODE! *******
	template < 	class DerivedClass, class ParentInvokerSig>
	inline void bindstaticfunc(DerivedClass *pParent, ParentInvokerSig static_function_invoker, 
				StaticFuncPtr function_to_bind) {
		// We'll be ignoring the 'this' pointer, but we need to make sure we pass
		// a valid value to bindmemfunc().
		bindmemfunc(pParent, static_function_invoker);

		// WARNING! Evil hack. We store the function in the 'this' pointer!
		// Ensure that there's a compilation failure if function pointers 
		// and data pointers have different sizes.
		// If you get this error, you need to #undef FASTDELEGATE_USESTATICFUNCTIONHACK.
		do { 
			typedef int ERROR_CantUseEvilMethod[sizeof(GenericClass *)==sizeof(function_to_bind) ? 1 : -1]; } while(0);
		m_pthis = horrible_cast<GenericClass *>(function_to_bind);
	}
	// These functions are required for invoking the stored function
	inline GenericClass *GetClosureThis() const { return m_pthis; };
	inline GenericMemFunc GetClosureMemPtr() const { return m_pFunction; };
	// ******** EVIL, EVIL CODE! *******
	// This function will be called with an invalid 'this' pointer!!
	// We're just returning the 'this' pointer, converted into
	// a function pointer!
	inline StaticFuncPtr GetStaticFunction() const {
		return horrible_cast<StaticFuncPtr>(this);
	};
	ClosurePtr() : m_pthis(0), m_pFunction(0) {};
public:
	inline bool operator ! () const		// Is it bound to anything?
	{ return (m_pFunction)==0; };
};

#endif // !defined(FASTDELEGATE_USESTATICFUNCTIONHACK)

} // namespace detail

////////////////////////////////////////////////////////////////////////////////
//						Fast Delegates, part 3:
//
//				Wrapper classes to ensure type safety
//
////////////////////////////////////////////////////////////////////////////////


// Once we have the member function conversion templates, it's easy to make the
// wrapper classes. So that they will work with as many compilers as possible, 
// the classes are of the form
//   FastDelegate3<int, char *, double>
// They can cope with any combination of parameters. The max number of parameters
// allowed is 8, but it is trivial to increase this limit.
// If C++ had some kind of 'varargs' facility for templates, implementation 
// would be very simple. Without them, it's tedious, and leads to very repetitive
// code. 
// I'm lazy, and I hate repetitive code (it's too easy to make a typographical 
// error), so I've abused the preprocessor. An alternative approach would be to 
// use the boost preprocessor library.
// Note that we need to treat const member functions seperately.

// Because of the weird rule about the class of derived member function pointers,
// you sometimes need to apply a downcast to the 'this' pointer.
// This is the reason for the use of "static_cast<X*>(pthis)" in the code below. 
// If CDerivedClass is derived from CBaseClass, but doesn't override SimpleVirtualFunction,
// without this trick you'd need to write:
//		MyDelegate(static_cast<CBaseClass *>(&d), &CDerivedClass::SimpleVirtualFunction);
// but with the trick you can write
//		MyDelegate(&d, &CDerivedClass::SimpleVirtualFunction);


// Wrapper class. Defined as a macro because I'm lazy.
// All this class does is to enforce type safety, and invoke the delegate with
// the correct list of parameters.

#define DLGT_DECLAREDELEGATE(NUM, FUNCLIST, INVOKELIST)							\
class FastDelegate##NUM : public detail::ClosurePtr< 							\
	void (detail::GenericClass::*)FUNCLIST,										\
	void (*)FUNCLIST> {															\
private:																		\
	typedef void (detail::GenericClass::*GenericMemFn)FUNCLIST;					\
	typedef void (*StaticFunctionPtr)FUNCLIST;									\
public:																			\
	FastDelegate##NUM() {};														\
	FastDelegate##NUM(const FastDelegate##NUM &x) { this->CopyFrom(x); };				\
	void operator = (const FastDelegate##NUM &x)  {	this->CopyFrom(x); };				\
	bool operator ==(const FastDelegate##NUM &x) const { return this->IsEqual(x);	};	\
	bool operator !=(const FastDelegate##NUM &x) const { return !this->IsEqual(x);};	\
	/* Binding to non-const member functions */									\
	template < class X, class Y >												\
	FastDelegate##NUM(Y *pthis, void (X::* function_to_bind)FUNCLIST ) {		\
		this->bindmemfunc(static_cast<X*>(pthis), function_to_bind);	}		\
	template < class X, class Y >												\
	inline void bind(Y *pthis, void (X::* function_to_bind)FUNCLIST ) {			\
		this->bindmemfunc(static_cast<X*>(pthis), function_to_bind);	}				\
	/* Binding to const member functions */										\
	template < class X, class Y >												\
	FastDelegate##NUM(Y *pthis, void (X::* function_to_bind)FUNCLIST const) {	\
		this->bindmemfunc(static_cast<X*>(pthis), function_to_bind);	}				\
	template < class X, class Y >												\
	inline void bind(Y *pthis, void (X::* function_to_bind)FUNCLIST const) {	\
		this->bindmemfunc(static_cast<X *>(pthis), function_to_bind);	}				\
	/* Static functions. We convert them into a member function call. */		\
	FastDelegate##NUM(void (*function_to_bind)FUNCLIST ) {						\
		bind(function_to_bind);	};												\
	inline void bind(void (*function_to_bind)FUNCLIST) {						\
		this->bindstaticfunc(this, &FastDelegate##NUM::InvokeStaticFunction,			\
					   function_to_bind); };									\
																				\
	void operator() FUNCLIST const { /* Invoke the delegate */					\
		/* this next line is the only one that violates the standard */			\
		(this->GetClosureThis()->*(this->GetClosureMemPtr()))INVOKELIST; };					\
																				\
public:	/* Invoker for static functions */										\
	void InvokeStaticFunction FUNCLIST const {									\
	(*(this->GetStaticFunction()))INVOKELIST; };										\
};

// Declare FastDelegate classes for zero to 8 arguments.
// It's trivial to extend it to more arguments, if required.

// For member functions with no arguments
DLGT_DECLAREDELEGATE( 0, (), () )

// For member functions with one argument
template <class Param1>
DLGT_DECLAREDELEGATE(1, (Param1 p1), (p1))

template <class Param1, class Param2>
DLGT_DECLAREDELEGATE(2, (Param1 p1, Param2 p2), (p1, p2))

template <class Param1, class Param2, class Param3>
DLGT_DECLAREDELEGATE(3, (Param1 p1, Param2 p2, Param3 p3), (p1, p2, p3))

template <class Param1, class Param2, class Param3, class Param4>
DLGT_DECLAREDELEGATE(4, (Param1 p1, Param2 p2, Param3 p3, Param4 p4), (p1, p2, p3, p4))

template <class Param1, class Param2, class Param3, class Param4, class Param5>
DLGT_DECLAREDELEGATE(5, (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5), 
					 (p1, p2, p3, p4, p5))

template <class Param1, class Param2, class Param3, class Param4, class Param5, 
		  class Param6>
DLGT_DECLAREDELEGATE(6, 
 (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6),
 (p1, p2, p3, p4, p5, p6))

template <class Param1, class Param2, class Param3, class Param4, class Param5, 
		  class Param6, class Param7>
DLGT_DECLAREDELEGATE(7, 
 (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7),
 (p1, p2, p3, p4, p5, p6, p7))

template <class Param1, class Param2, class Param3, class Param4, class Param5, 
		  class Param6, class Param7, class Param8>
DLGT_DECLAREDELEGATE(8, 
 (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8),
 (p1, p2, p3, p4, p5, p6, p7, p8))

 // clean up after ourselves...
#undef DLGT_DECLAREDELEGATE

} // namespace fastdelegate

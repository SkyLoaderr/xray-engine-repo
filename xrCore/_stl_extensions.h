#ifndef _STL_EXT_internal
#define _STL_EXT_internal

#ifdef __BORLANDC__

#define xr_vector std::vector
#define xr_list std::list
#define xr_deque std::deque
#define xr_stack std::stack
#define xr_set std::set
#define xr_multiset std::multiset
#define xr_map std::map
#define xr_multimap std::multimap

#else

template <class T>
class	xr_allocator_t	{
public:
	typedef	size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef T			value_type;

public:
	template<class _Other>	
	struct rebind			{	typedef xr_allocator_t<_Other> other;	};
public:
							pointer					address			(reference _Val) const					{	return (&_Val);	}
							const_pointer			address			(const_reference _Val) const			{	return (&_Val);	}
													xr_allocator_t	()										{	}
													xr_allocator_t	(const xr_allocator_t<T>&)				{	}
	template<class _Other>							xr_allocator_t	(const xr_allocator_t<_Other>&)			{	}
	template<class _Other>	xr_allocator_t<T>&		operator=		(const xr_allocator_t<_Other>&)			{	return (*this);	}
							pointer					allocate		(size_type n, const void* p=0) const	{	return xr_alloc<T>((u32)n);	}
							void					deallocate		(pointer p, size_type n) const			{	xr_free	(p);				}
							void					construct		(pointer p, const T& _Val)				{	std::_Construct(p, _Val);	}
							void					destroy			(pointer p)								{	std::_Destroy(p);			}
							size_type				max_size		() const								{	size_type _Count = (size_type)(-1) / sizeof (T);	return (0 < _Count ? _Count : 1);	}
};
template<class _Ty,	class _Other>	inline	bool operator==(const xr_allocator_t<_Ty>&, const xr_allocator_t<_Other>&)			{	return (true);							}
template<class _Ty, class _Other>	inline	bool operator!=(const xr_allocator_t<_Ty>&, const xr_allocator_t<_Other>&)			{	return (false);							}

namespace std
{
	template<class _Tp1, class _Tp2>	inline	xr_allocator_t<_Tp2>&	__stl_alloc_rebind(xr_allocator_t<_Tp1>& __a, const _Tp2*)	{	return (xr_allocator_t<_Tp2>&)(__a);	}
	template<class _Tp1, class _Tp2>	inline	xr_allocator_t<_Tp2>	__stl_alloc_create(xr_allocator_t<_Tp1>&, const _Tp2*)		{	return xr_allocator_t<_Tp2>();			}
};

template	<typename T>									class	xr_vector		: public std::vector<T,xr_allocator_t<T> >								{ public: 
	u32		size() const									{ return (u32)__super::size(); } 
	void	clear()											{ erase(begin(),end());} 
	void	free()											{ _Tidy(); };
	const_reference operator[](size_type _Pos) const		{ {VERIFY(_Pos<size());} return (*(begin() + _Pos)); }
	reference operator[](size_type _Pos)					{ {VERIFY(_Pos<size());} return (*(begin() + _Pos)); }
};
template	<>												class	xr_vector<bool>	: public std::vector<bool,xr_allocator_t<bool> >						{ public: 
	u32		size() const									{ return (u32)__super::size();	} 
	void	clear()											{ erase(begin(),end());			} 
	void	free()											{ _Tidy(); };
};

template	<typename T>									class	xr_list 		: public std::list<T,xr_allocator_t<T> >								{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename T>									class	xr_deque 		: public std::deque<T,xr_allocator_t<T> >								{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename T>									class	xr_stack 		: public std::stack<T,xr_deque<T> >										{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename K, class P=std::less<K> >				class	xr_set			: public std::set<K,P,xr_allocator_t<K> >								{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename K, class P=std::less<K> >				class	xr_multiset		: public std::multiset<K,P,xr_allocator_t<K> >							{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename K, class V, class P=std::less<K> >	class	xr_map 			: public std::map<K,V,P,xr_allocator_t<std::pair<const K,V> > >			{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename K, class V, class P=std::less<K> >	class	xr_multimap		: public std::multimap<K,V,P,xr_allocator_t<std::pair<const K,V> > >	{ public: u32 size() const {return (u32)__super::size(); } };

#endif

template	<class _Ty1, class _Ty2> inline	std::pair<_Ty1, _Ty2>	mk_pair			(_Ty1 _Val1, _Ty2 _Val2)	{	return (std::pair<_Ty1, _Ty2>(_Val1, _Val2));	}

struct pred_str		: public std::binary_function<char*, char*, bool>	{	
	IC bool operator()(const char* x, const char* y) const				{	return strcmp(x,y)<0;	}
};
struct pred_stri	: public std::binary_function<char*, char*, bool>	{	
	IC bool operator()(const char* x, const char* y) const				{	return strcmp(x,y)<0;	}
};

// STL extensions
#define DEF_VECTOR(N,T)				typedef xr_vector< T > N;		typedef N::iterator N##_it;
#define DEF_LIST(N,T)				typedef xr_list< T > N;			typedef N::iterator N##_it;
#define DEF_DEQUE(N,T)				typedef xr_deque< T > N;		typedef N::iterator N##_it;
#define DEF_MAP(N,K,T)				typedef xr_map< K, T > N;		typedef N::iterator N##_it;

#define DEFINE_LIST(T,N,I)			typedef xr_list< T > N;			typedef N::iterator I;
#define DEFINE_VECTOR(T,N,I)		typedef xr_vector< T > N;		typedef N::iterator I;
#define DEFINE_MAP(K,T,N,I)			typedef xr_map< K , T > N;		typedef N::iterator I;
#define DEFINE_MAP_PRED(K,T,N,I,P)	typedef xr_map< K, T, P > N;	typedef N::iterator I;
#define DEFINE_MMAP(K,T,N,I)		typedef xr_multimap< K, T > N;	typedef N::iterator I;
#define DEFINE_SVECTOR(T,C,N,I)		typedef svector< T, C > N;		typedef N::iterator I;
#define DEFINE_SET(T,N,I)			typedef xr_set< T > N;			typedef N::iterator I;
#define DEFINE_SET_PRED(T,N,I,P)	typedef xr_set< T, P > N;		typedef N::iterator I;
#define DEFINE_STACK(T,N)			typedef xr_stack< T > N;

#include "FixedVector.h"

// auxilary definition
DEFINE_VECTOR(bool,boolVec,boolIt);
DEFINE_VECTOR(BOOL,BOOLVec,BOOLIt);
DEFINE_VECTOR(BOOL*,LPBOOLVec,LPBOOLIt);
DEFINE_VECTOR(Fplane,PlaneVec,PlaneIt);
DEFINE_VECTOR(Fvector2,Fvector2Vec,Fvector2It);
DEFINE_VECTOR(Fvector,FvectorVec,FvectorIt);
DEFINE_VECTOR(Fvector*,LPFvectorVec,LPFvectorIt);
DEFINE_VECTOR(Fcolor,FcolorVec,FcolorIt);
DEFINE_VECTOR(Fcolor*,LPFcolorVec,LPFcolorIt);
DEFINE_VECTOR(LPSTR,LPSTRVec,LPSTRIt);
DEFINE_VECTOR(LPCSTR,LPCSTRVec,LPCSTRIt);
DEFINE_VECTOR(string64,string64Vec,string64It);

DEFINE_VECTOR(s8,S8Vec,S8It);
DEFINE_VECTOR(s8*,LPS8Vec,LPS8It);
DEFINE_VECTOR(s16,S16Vec,S16It);
DEFINE_VECTOR(s16*,LPS16Vec,LPS16It);
DEFINE_VECTOR(s32,S32Vec,S32It);
DEFINE_VECTOR(s32*,LPS32Vec,LPS32It);
DEFINE_VECTOR(u8,U8Vec,U8It);
DEFINE_VECTOR(u8*,LPU8Vec,LPU8It);
DEFINE_VECTOR(u16,U16Vec,U16It);
DEFINE_VECTOR(u16*,LPU16Vec,LPU16It);
DEFINE_VECTOR(u32,U32Vec,U32It);
DEFINE_VECTOR(u32*,LPU32Vec,LPU32It);
DEFINE_VECTOR(float,FloatVec,FloatIt);
DEFINE_VECTOR(float*,LPFloatVec,LPFloatIt);
DEFINE_VECTOR(int,IntVec,IntIt);
DEFINE_VECTOR(int*,LPIntVec,LPIntIt);

#ifdef __BORLANDC__
DEFINE_VECTOR(AnsiString,AStringVec,AStringIt);
DEFINE_VECTOR(AnsiString*,LPAStringVec,LPAStringIt);
#endif

#endif
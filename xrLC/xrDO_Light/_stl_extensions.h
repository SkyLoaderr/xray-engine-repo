#ifndef _STL_EXT_internal
#define _STL_EXT_internal

template <typename T>
class	xr_allocator	: public std::allocator<T>	{
public:
	IC pointer			allocate	(size_type n, xr_allocator<void>::const_pointer p=0)	{	return xr_alloc<T>((u32)n);	}
	IC void				deallocate	(pointer p, size_type n)								{	xr_free	(p);				}
};

template	<typename T>							class	xr_vector	: public std::vector<T,xr_allocator<T> >		{};
template	<typename T>							class	xr_list 	: public std::list<T,xr_allocator<T> >			{};
template	<typename T>							class	xr_deque 	: public std::deque<T,xr_allocator<T> >			{};
template	<typename T>							class	xr_stack 	: public std::stack<T,xr_deque<T> >				{};
template	<typename K, class P=less<K> >			class	xr_set		: public std::set<K,P,xr_allocator<K> >			{};
template	<typename K, class P=less<K> >			class	xr_multiset	: public std::multiset<K,P,xr_allocator<K> >	{};
template	<typename K, class V, class P=less<K> >	class	xr_map 		: public std::map<K,V,P,xr_allocator<T> >		{};
template	<typename K, class V, class P=less<K> >	class	xr_multimap : public std::multimap<K,V,P,xr_allocator<T> >	{};

struct pred_str		: public std::binary_function<char*, char*, bool>	{	
	IC bool operator()(const char* x, const char* y) const				{	return strcmp(x,y)<0;	}
};
struct pred_stri	: public std::binary_function<char*, char*, bool>	{	
	IC bool operator()(const char* x, const char* y) const				{	return strcmp(x,y)<0;	}
};

// STL extensions
#define DEF_VECTOR(N,T)				typedef std::vector<T> N;	typedef N::iterator N##_it; typedef N::reference N##_ref
#define DEF_LIST(N,T)				typedef std::list<T> N;		typedef N::iterator N##_it; typedef N::reference N##_ref
#define DEF_DEQUE(N,T)				typedef std::deque<T> N;	typedef N::iterator N##_it; typedef N::reference N##_ref
#define DEF_MAP(N,K,T)				typedef std::map<K,T> N;	typedef N::iterator N##_it; typedef N::reference N##_ref

#define DEFINE_VECTOR(T,N,I)		typedef std::vector<T> N;	typedef N::iterator I;
#define DEFINE_MAP(K,T,N,I)			typedef std::map<K,T> N;	typedef N::iterator I;
#define DEFINE_MAP_PRED(K,T,N,I,P)	typedef std::map<K,T,P> N;	typedef N::iterator I;
#define DEFINE_MMAP(K,T,N,I)		typedef std::multimap<K,T> N;typedef N::iterator I;
#define DEFINE_SVECTOR(T,C,N,I)		typedef svector<T,C> N;		typedef N::iterator I;
#define DEFINE_SET(T,N,I)			typedef std::set<T> N;		typedef N::iterator I;
#define DEFINE_SET_PRED(T,N,I,P)	typedef std::set<T,P> N;	typedef N::iterator I;
#define DEFINE_STACK(T,N)			typedef std::stack<T> N;

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
#ifndef _STL_EXT_internal
#define _STL_EXT_internal

// STL extensions
#define DEF_VECTOR(N,T)				typedef std::vector<T> N;	typedef N::iterator N##_it; typedef N::reference N##_ref
#define DEF_LIST(N,T)				typedef std::list<T> N;		typedef N::iterator N##_it; typedef N::reference N##_ref
#define DEF_DEQUE(N,T)				typedef std::deque<T> N;	typedef N::iterator N##_it; typedef N::reference N##_ref
#define DEF_MAP(N,K,T)				typedef std::map<K,T> N;	typedef N::iterator N##_it; typedef N::reference N##_ref

#define DEFINE_MAP(K,T,N,I)			typedef std::map<K,T> N;	typedef N::iterator I;
#define DEFINE_VECTOR(T,N,I)		typedef std::vector<T> N;	typedef N::iterator I;
#define DEFINE_MAP_PRED(K,T,N,I,P)	typedef std::map<K,T,P> N;	typedef N::iterator I;
#define DEFINE_SVECTOR(T,C,N,I)		typedef svector<T,C> N;		typedef N::iterator I;

#endif
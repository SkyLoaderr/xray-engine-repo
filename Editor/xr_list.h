#ifndef __XR_LIST_H__
#define __XR_LIST_H__

#define	XR_LIST_INITSIZE		16
#define	XR_LIST_INCSIZE			32
//-----------------------------------------------------------------------------------------------------------
//array
//-----------------------------------------------------------------------------------------------------------
typedef int						(_cdecl *SORT_CALLBACK)		( const void*, const void* );

template <class TYPE> class		CList
{
	int							array_size;
	TYPE*						data;
	BOOL						bSorted;

	void	__forceinline		Allocate				( int size );
	int		__cdecl				CompareDefault			( const void *a, const void *b );
public:
	int							count;

public:
								CList					( int = XR_LIST_INITSIZE );
								~CList					( );

	void	__forceinline		SetSize					( int size );

	void	__forceinline		Add						( TYPE );
	BOOL	__forceinline		AddUnique				( TYPE );
	void	__forceinline		Del						( TYPE );
	void	__forceinline		DelIndex				( int );
	void	__forceinline		Clear					( )					{ count = 0; };
	void	__forceinline		Pack					( );
	void	__forceinline		DelDup					( );
	void	__forceinline		DelDup					( CList<int> &cl );

	void	__forceinline		Append					( CList<TYPE>* );
	void	__forceinline		Append					( TYPE*, int );
	void	__forceinline		AppendUnique			( CList<TYPE>* );
	int		__forceinline		GetMemoryUsage			( );

	int		__forceinline		Search					( TYPE entry );	// returns INDEX of entrys
	int		__forceinline		Contains				( TYPE entry );	// returns COUNT of entrys
	void	__forceinline		Sort					( SORT_CALLBACK _ComparePtr );
	TYPE	__forceinline		SearchSorted			( TYPE,  SORT_CALLBACK _ComparePtr );

	void	__forceinline		ZeroData				( )					{ ZeroMemory( data, (sizeof(TYPE)*count) );};

	TYPE	__forceinline		&operator[]				( int i )			{ VERIFY(i>=0 && i<count); return data[i]; };
	TYPE	__forceinline		*Pointer				( )					{ return data; };
//	TYPE	__forceinline		*operator&				( )					{ return data; }
};

//----------------------------------------------------------------------
// Class	: CXR_List
//----------------------------------------------------------------------
template <class TYPE>
CList<TYPE>::CList		( int ainit_size )
{
	bSorted						= FALSE;
	array_size					= ainit_size;											// initial size
	count						= 0;													// initial count
	data						= NULL;
	Allocate					( array_size );
}
//----------------------------------------------------------------------
// wt_array destructor
//----------------------------------------------------------------------
template <class TYPE>
CList<TYPE>::~CList		( )
{
	free    					(data);
}

//----------------------------------------------------------------------
// create array. internal use only
//----------------------------------------------------------------------
template <class TYPE>
void CList<TYPE>::Allocate	( int s )
{
	VERIFY						( s >= count );
	if( s > 0 ) {
		if (data)				data = (TYPE *)realloc(data,s*sizeof(TYPE));
		else					data = (TYPE *)malloc(s*sizeof(TYPE));
		VERIFY					(data);
		array_size				= s;
	}
}

template <class TYPE>
void CList<TYPE>::SetSize	( int s )
{
	count						= s;
	Allocate					( s );
}
																					//
//----------------------------------------------------------------------
// storing unique pointer
//----------------------------------------------------------------------
template <class TYPE>
BOOL CList<TYPE>::AddUnique	( TYPE ptr ) {
	VERIFY						( data );
	if( Contains(ptr) )			return false;

	if (count >= array_size)	Allocate(array_size + XR_LIST_INCSIZE);

	data [count++]				= ptr;
	bSorted						= FALSE;
	return true;
}

//----------------------------------------------------------------------
// deleting unique pointer
//----------------------------------------------------------------------
template <class TYPE>
void		CList<TYPE>::Del	( TYPE ptr ) {
	VERIFY						( data );						// exit if no data
	for( int i=0; i<count; i++ )								// loop for items - searching
		if( data[i]==ptr ){										// compare data
			DelIndex(i);
			return;
		}														//
}

//----------------------------------------------------------------------
// deleting item by index
//----------------------------------------------------------------------
template <class TYPE>
void CList<TYPE>::DelIndex	( int index ) {
	VERIFY						( index < count );
	if ( index<(count-1) )
		MoveMemory(&data[index], &data[index+1], (count - index - 1)*sizeof(TYPE));
	count--;
}

//----------------------------------------------------------------------
// storing pointer
//----------------------------------------------------------------------
template <class TYPE>
void		CList<TYPE>::Add	( TYPE ptr ) {
	VERIFY						( data );
	if (count >= array_size)	Allocate(array_size + XR_LIST_INCSIZE);
	data [count++]				= ptr;
	bSorted						= FALSE;
}

//----------------------------------------------------------------------
// packing array - release free slots
//----------------------------------------------------------------------
template <class TYPE>
void	CList<TYPE>::Pack	( ) {
	VERIFY						( data );								// exit if no handler
	if( count==array_size )		return;									// no need for packing
	Allocate					( count );								// create smaller array
}

//----------------------------------------------------------------------
// scan array for some entry
//----------------------------------------------------------------------
template <class TYPE>
int CList<TYPE>::Contains	( TYPE entry ) {
	int		cnt=0;
	for(int i=0;i<count;i++) {
		if(data[i] == entry) cnt++;
	}
	return cnt;
}

//----------------------------------------------------------------------
// scan array for some entry
//----------------------------------------------------------------------
template <class TYPE>
int	CList<TYPE>::Search	( TYPE entry ) {
	VERIFY						( data );
	for (int i=0;i<count;i++)	if( data[i]==entry )	return i;
	return -1;
}

//----------------------------------------------------------------------
// append array
//----------------------------------------------------------------------
template <class TYPE>
void	CList<TYPE>::Append	( CList<TYPE>* L ) {
	if ( (count+L->count) >= array_size )	Allocate( array_size + L->array_size + XR_LIST_INCSIZE );
	CopyMemory					( data+count, L->data, L->count*(sizeof(TYPE) ));
	count						+= L->count;

	bSorted = FALSE;
}

//----------------------------------------------------------------------
// append array
//----------------------------------------------------------------------
template <class TYPE>
void	CList<TYPE>::Append	( TYPE* L, int list_count ) {
	if ( (count+list_count) >= array_size )	Allocate( array_size + list_count );
	CopyMemory					( data+count, L, list_count*(sizeof(TYPE) ));
	count						+= list_count;

	bSorted = FALSE;
}

//----------------------------------------------------------------------
// append array	unique
//----------------------------------------------------------------------
template <class TYPE>
void	CList<TYPE>::AppendUnique( CList<TYPE>* L ) {
	for (int i=0;i<L->count;i++) AddUnique( L->data[i] );
	bSorted = FALSE;
}

//----------------------------------------------------------------------
// delete repeated items
//----------------------------------------------------------------------
template <class TYPE>
void	CList<TYPE>::DelDup	( ) {
	TYPE t;
	Sort( );
	bSorted = FALSE;
	if ( count ){
		t = data[0];
		for (int i = 1; i < count; i++){
			while ( (t == data[i]) && (i < count) ) Del( data[i] );
			t = data[i];
		}
	}
}

//----------------------------------------------------------------------
// delete repeated items
//----------------------------------------------------------------------
template <class TYPE>
void	CList<TYPE>::DelDup	( CList<int> &cl ) {
	TYPE t;
	int cnt;
	Sort( );
	bSorted = FALSE;
	if ( count ){
		t = data[0];
		for (int i = 1; i < count; i++){
			cnt = 1;
			while ( (t == data[i]) && (i < count) ){ Del( data[i] ); cnt++;}
			t = data[i];
			cl.Add(cnt);
		}
	}
}

//----------------------------------------------------------------------
// calculating size in bytes
//----------------------------------------------------------------------
template <class TYPE>
int	CList<TYPE>::GetMemoryUsage( ) {
	return	sizeof(*this) + array_size*sizeof(TYPE);
}

//----------------------------------------------------------------------
template <class TYPE>
void	CList<TYPE>::Sort	( SORT_CALLBACK scb ) {
	VERIFY(scb);
	qsort(data, count, sizeof(TYPE), scb);
	bSorted = TRUE;
}

//----------------------------------------------------------------------
template <class TYPE>
TYPE CList<TYPE>::SearchSorted( TYPE key,  SORT_CALLBACK scb ) {
	VERIFY(scb);
	if ( !bSorted )	Sort( scb );
	TYPE* a = NULL;
	a = (TYPE*)bsearch(&key, data, count, sizeof(TYPE), scb);
	return	(a) ? *a : NULL;
}

// **************************** Some extensions
#define _call_for_each(a,b)		{ for (int i=0; i<a.count; i++) a[i]->b; }
#define _clear_ptr(a)			{ while (a.count) if (a[0]) delete a[0]; a.SetSize(0); }
/*
template <class T>	void _clear_ptr(CList<T*> &a) {
	for( int i=0; i<a.count; i++ ) {
		if (a[i]) delete a[i];
	}
	a.SetSize(0);
}
*/
;// ';' - need for borland precompiled header
#endif //__XR_LIST_H__

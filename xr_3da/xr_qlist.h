#ifndef __XR_QLIST_H__
#define __XR_QLIST_H__

//-----------------------------------------------------------------------------------------------------------
//square list
//-----------------------------------------------------------------------------------------------------------
template <class TYPE> class		CQList
{
	TYPE**			data;
public:
	int				z_count, x_count;

public:
					CQList				( );
					~CQList				( );

	IC		void	SetSize					( int x, int z );
	IC		int		GetMemoryUsage			( );
	IC		TYPE	&operator()				( int x, int z ){ clamp(z,0,z_count-1); clamp(x,0,x_count-1); return data[x][z]; }
};

//----------------------------------------------------------------------
// Class	: CXR_QList
//----------------------------------------------------------------------
template <class TYPE>
CQList<TYPE>::CQList	( ) {
	x_count						= 0;													// initial count
	z_count						= 0;													// initial count
	data						= NULL;
}
//----------------------------------------------------------------------
// wt_array destructor
//----------------------------------------------------------------------
template <class TYPE>
CQList<TYPE>::~CQList	( )
{
	if (!data) return;
	for (int i=0; i<x_count; i++)	_DELETEARRAY(data[i]);
	_FREE						(data);
}

//----------------------------------------------------------------------
// create array.
// after resize all data losing
//----------------------------------------------------------------------
template <class TYPE>
void CQList<TYPE>::SetSize( int x, int z )
{
	VERIFY						( (x > 0) && (z > 0) );

	for (int i=0; i<x_count; i++)	_DELETEARRAY(data[i]);
	_FREE						(data);

	data						= (TYPE**) malloc(x * sizeof(TYPE*));
	VERIFY						( data );
	ZeroMemory					( data, x*sizeof(TYPE*) );
	for (i=0; i<x; i++){
		data[i]					= new TYPE[z];
		VERIFY					( data[i] );
	}

	x_count						= x;
	z_count						= z;
}

//----------------------------------------------------------------------
// calculating size in bytes
//----------------------------------------------------------------------
template <class TYPE>
int	CQList<TYPE>::GetMemoryUsage( )
{
	return	sizeof(*this) + x_count*z_count*sizeof(TYPE);
}

#endif //__XR_QLIST_H__

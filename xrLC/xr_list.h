#ifndef __XR_LIST_H__
#define __XR_LIST_H__

//-----------------------------------------------------------------------------------------------------------
template <class T> class		CList
{
	enum	{
		XR_LIST_INITSIZE		= 32
	};
private:
	T*							data;
	int							_count;
	int							_size;

	IC void			_realloc	( )
	{
		if (_size)	_size		*=2;
		else		_size		= XR_LIST_INITSIZE;
		data					= (T *)xr_realloc(data,_size*sizeof(T));
	}
	IC void			_realloc	( int sz )
	{
		if (0==_size) _size		= XR_LIST_INITSIZE;
		while (_size < sz)		_size*=2;
		data					= (T *)xr_realloc(data,_size*sizeof(T));
	}
public:
	typedef T*		iterator;

public:
	CList					( int V = XR_LIST_INITSIZE )
	{
		data				= (T *)	xr_malloc	(V*sizeof(T));
		_size				= V;								// initial size
		_count				= 0;								// initial count
	}
	CList					(const CList<T>& from)
	{
		_size				= from._count;
		_count				= from._count;
		data				= (T *) xr_malloc	(_size*sizeof(T));
		for (int it=0; it<_count; it++)
			(*this)[it] = from[it];
	}

	~CList									( )
	{	_FREE		(data);	}

	IC	void		resize					( int V )
	{
		if (V>_size) _realloc(V);
		_count = V;
	}
	IC	void		reserve					( int V )
	{
		if (V>_size) _realloc(V);
	}
	IC	void		push_back				( const T& V)
	{
		if (_count >= _size)	_realloc();
		data [_count++]			= V;
	}
	IC	bool		push_back_unique		( const T& V)
	{
		T*	_end = end();
		if (find(begin(),_end,V)==_end)		{ push_back(V); return true; }
		else								return false;
	}
	IC	void		pop_back				( )
	{
		VERIFY(_count>0);
		count--;
	}
	IC	void		erase					( int id)
	{
		VERIFY(id>=0);
		VERIFY(id<_count);
		if ( id < (_count-1) )
			MoveMemory(data+id, data+id+1, (_count - id - 1)*sizeof(T));
		_count--;
	}
	IC	void		erase					( T* it)
	{	
		erase(it-data);	
	}
	IC	void		erase					( T& V)
	{
		T*	_end	= end();
		T*	_it		= find(begin(),_end,V);
		if (_it!=_end)	erase(_it);
	};
	IC	void		clear					( )					
	{	_count = 0; }
	IC	void		append					( T* L, int cnt )
	{
		if ( _count+cnt >= _size )	_realloc( _count+cnt );
		CopyMemory					( data+_count, L, cnt*sizeof(T) );
		_count						+= cnt;
	}
	IC	void		append					( T* b, T* e)
	{
		append		(b,e-b);
	}
	IC	void		compact					( )
	{
		if (_size==_count)	return;
		_size				= _count;
		if (0==_size)		{ _FREE	(data); }
		else				{ data	= (T *) xr_realloc(data,_size*sizeof(T)); }
	}

	IC	T&			operator[]				( int i )	const	{ VERIFY(i>=0 && i<_count); return data[i]; };
	IC	T*			begin					( )					{ return data;			}
	IC	T*			end						( )					{ return data+_count;	}
	IC	u32		size					( )					{ return _count;		}
	IC	bool		empty					( )					{ return 0==_count;		}
	IC  T&			front					( )					{ return (*this)[0];	}
	IC	T&			back					( )					{ VERIFY(_count>0); return data[_count-1]; }
};


#endif //__XR_LIST_H__

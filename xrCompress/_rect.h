#ifndef __FRECT
#define __FRECT

template <class T>
struct _rect {
public:
	typedef T			TYPE;
	typedef _rect<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	union{
		struct{
			T 	x1, y1, x2, y2;
		};
		struct{
			_vector2<TYPE> lt;
			_vector2<TYPE> rb;
		};
		T		m[4];
	};

	IC	SelfRef	set(const T _x1, const T _y1, const T _x2, const T _y2)	{ x1=_x1;	y1=_y1;		x2=_x2;		y2=_y2;		return *this;	};
	IC	SelfRef	set(const _vector2<T> &mn, const _vector2<T> &mx)		{ x1=mn.x;	y1=mn.y;	x2=mx.x;	y2=mx.y;	return *this;	};
	IC	SelfRef	set(const Self &r)										{ x1=r.x1;	y1=r.y1;	x2=r.x2;	y2=r.y2;	return *this;	};
	IC	SelfRef	null( )										{ x1=T(0);	y1=T(0);	x2=T(0);	y2=T(0);		return *this;	};
	
	IC	SelfRef	add(T x, T y)								{ x1+=x;	y1+=y;		x2+=x;		y2+=y;			return *this;	};
	IC	SelfRef	sub(T x, T y)								{ x1-=x;	y1-=y;		x2-=x;		y2-=y;			return *this;	};
	IC	SelfRef	mul(T x, T y)								{ x1*=x;	y1*=y;		x2*=x;		y2*=y;			return *this;	};
	IC	SelfRef	div(T x, T y)								{ x1/=x;	y1/=y;		x2/=x;		y2/=y;			return *this;	};

	IC	SelfRef	add(const Self& r, T x, T y)				{ x1=r.x1+x;	y1=r.y1+y;	x2=r.x2+x;	y2=r.y2+y;	return *this;	};
	IC	SelfRef	sub(const Self& r, T x, T y)				{ x1=r.x1-x;	y1=r.y1-y;	x2=r.x2-x;	y2=r.y2-y;	return *this;	};
	IC	SelfRef	mul(const Self& r, T x, T y)				{ x1=r.x1*x;	y1=r.y1*y;	x2=r.x2*x;	y2=r.y2*y;	return *this;	};
	IC	SelfRef	div(const Self& r, T x, T y)				{ x1=r.x1/x;	y1=r.y1/y;	x2=r.x2/x;	y2=r.y2/y;	return *this;	};

	IC	BOOL	in (T  x, T  y)								{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2);	};
	IC	BOOL	in (_vector2<T> &p)							{ return (p.x>=x1) && (p.x<=x2) && (p.y>=y1) && (p.y<=y2);	};
	IC	BOOL	cmp(_rect<int> &r)							{ return x1==r.x1 && y1==r.y1 && x2==r.x2 && y2==r.y2; };
	IC	BOOL	cmp(_rect<float> &r)						{ return fsimilar(x1,r.x1) && fsimilar(y1,r.y1) && fsimilar(x2,r.x2) && fsimilar(y2,r.y2); };
	
	IC	void	getcenter(_vector2<T>& center)				{ center.add(rb,lt); center.div(2); }
	IC	void	getsize(_vector2<T>& sz)					{ sz.sub(rb,lt); }

	IC	T		width()										{return rb.x-lt.x;}
	IC	T		height()									{return rb.y-lt.y;}

	IC	SelfRef	shrink(T x, T y)							{ lt.x+=x; lt.y+=y; rb.x-=x; rb.y-=y;	return *this;	};
	IC	SelfRef	grow(T x, T y)								{ lt.x-=x; lt.y-=y; rb.x+=x; rb.y+=y;	return *this;	};
};

typedef _rect<float>	Frect;
typedef _rect<double>	Drect;
typedef _rect<int>		Irect;

template <class T>
BOOL	_valid			(const _rect<T>& m)		
{ 
	return lt._valid() && rb._valid();
}

#endif

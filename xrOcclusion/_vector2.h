#ifndef __V2D__
#define __V2D__

template <class T>
struct _vector2 
{
public:
	typedef T			TYPE;
	typedef _vector2<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T x,y;

	IC SelfRef set(float _u, float _v)				{ x=_u; y=_v;					return *this;	}
	IC SelfRef set(double _u, double _v)			{ x=_u; y=_v;					return *this;	}
	IC SelfRef set(int _u, int _v)					{ x=_u; y=_v;					return *this;	}
	IC SelfRef set(const Self &p)					{ x=p.x; y=p.y;					return *this;	}
	IC SelfRef abs(const Self &p)					{ x=_abs(p.x); y=_abs(p.y);		return *this;	}
	IC SelfRef min(const Self &p)					{ x=_min(x,p.x); y=_min(y,p.y);	return *this;	}
	IC SelfRef min(T _x, T _y)						{ x=_min(x,_x);  y=_min(y,_y);	return *this;	}
	IC SelfRef max(const Self &p)					{ x=_max(x,p.x); y=_max(y,p.y);	return *this;	}
	IC SelfRef max(T _x, T _y)						{ x=_max(x,_x);  y=_max(y,_y);	return *this;	}
	IC SelfRef sub(T p)								{ x-=p; y-=p;					return *this;	}
	IC SelfRef sub(const Self &p)					{ x-=p.x; y-=p.y;				return *this;	}
	IC SelfRef sub(const Self &p1, const Self &p2)	{ x=p1.x-p2.x; y=p1.y-p2.y;		return *this;	}
	IC SelfRef sub(const Self &p, float d)			{ x=p.x-d; y=p.y-d;				return *this;	}
	IC SelfRef add(T p)								{ x+=p; y+=p;					return *this;	}
	IC SelfRef add(const Self &p)					{ x+=p.x; y+=p.y;				return *this;	}
	IC SelfRef add(const Self &p1, const Self &p2)	{ x=p1.x+p2.x; y=p1.y+p2.y;		return *this;	}
	IC SelfRef add(const Self &p, float d)			{ x=p.x+d; y=p.y+d;				return *this;	}
	IC SelfRef mul(const T s)						{ x*=s; y*=s;					return *this;	}
	IC SelfRef mul(Self &p)							{ x*=p.x; y*=p.y;				return *this;	}
	IC SelfRef div(const T s)						{ x/=s; y/=s;					return *this;	}
	IC SelfRef rot90(void)							{ float t=-x; x=y; y=t;			return *this;	}
    IC SelfRef cross(Self &D)						{ x = D.y; y = -D.x;            return *this;	}
	IC T dot(Self &p)								{ return x*p.x + y*p.y;			}
	IC T dot(const Self &p) const					{ return x*p.x + y*p.y;			}
	IC SelfRef norm(void)							{ float m=_sqrt(x*x+y*y); x/=m; y/=m;			return *this;	}
	IC SelfRef norm_safe(void)						{ float m=_sqrt(x*x+y*y); if(m) {x/=m; y/=m;}	return *this;	}
	IC T distance_to(const Self &p) const 			{ return _sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y)); } 
	IC T square_magnitude(void) const				{ return x*x + y*y; }
	IC T magnitude(void) const						{ return _sqrt(square_magnitude());	}

    IC SelfRef mad	(const Self &p, const Self& d, T r) 
	{
        x = p.x + d.x*r;
        y = p.y + d.y*r;
		return *this;	
    }
    IC Self Cross()                   
	{
        // vector3 orthogonal to (x,y) is (y,-x)
        Self kCross;
        kCross.x = y;
        kCross.y = -x;
        return kCross;
    }

	IC bool similar(Self &p, T eu, T ev) const
	{ 
		return _abs(x-p.x)<eu && _abs(y-p.y)<ev;
	}
	
	IC bool similar(const Self &p, float E=EPS_L) const
	{ 
		return _abs(x-p.x)<E && _abs(y-p.y)<E;
	};

	// average arithmetic
	IC SelfRef averageA(Self &p1, Self &p2) 
	{
		x = (p1.x + p2.x)*.5f;
		y = (p1.y + p2.y)*.5f;
		return *this;	
	}
	// average geometric
	IC SelfRef averageG(Self &p1, Self &p2) 
	{
		x = sqrtf(p1.x*p2.x);
		y = sqrtf(p1.y*p2.y);
		return *this;	
	}

    T& operator[] (int i) const
    {
        // assert:  0 <= i < 2; x and y are packed into 2*sizeof(float) bytes
        return (T&) *(&x + i);
    }
};

typedef _vector2<float>		Fvector2;
typedef _vector2<double>	Dvector2;
typedef _vector2<int>		Ivector2;

template <class T>
BOOL	_valid			(const _vector2<T>& v)	{ return _valid((T)v.x) && _valid((T)v.y);	}

#endif
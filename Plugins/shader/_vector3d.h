#ifndef __V3D__
#define __V3D__

// Inline call
#ifndef IC
#define IC __forceinline
#endif

template <class T>
struct _vector {
private:
	IC T _abs	(T x) const { return (T)fabs((T)x); }
	IC T _sqrt	(T x) const { return (T)sqrt((T)x); }
public:
	typedef T TYPE;
public:
	T	x,y,z;

	// access operators
	IC	D3DVECTOR*	d3d(void)							{ return asD3D();					};
	IC	D3DVECTOR*	asD3D(void)							{ return (D3DVECTOR *)this;			};
	IC	T*			asDATA(void)						{ return (T *)this;					};
	IC	DWORD*		asDWORD(void)						{ return (DWORD *)this;				};
	IC	T&			operator[] (int i)					{ return *((T*)this + i); }
	IC	T&			operator[] (int i)	const			{ return *((T*)this + i); }

	IC	void	set(T _x, T _y, T _z)					{ x = _x;	y = _y;		z = _z;		};
	IC  void	set(const _vector<float> &v)			{ x = v.x;	y = v.y;	z = v.z;	};
	IC  void	set(const _vector<double> &v)			{ x = v.x;	y = v.y;	z = v.z;	};
	IC	void	set(float* p)							{ x = p[0];	y = p[1];	z = p[2];	};
	IC	void	set(double* p)							{ x = p[0];	y = p[1];	z = p[2];	};

	IC	void	add(const _vector<T> &v)				{ x+=v.x;	y+=v.y;		z+=v.z;		};
	IC  void	add(T s)								{ x+=s;		y+=s;		z+=s;		};
	IC	void	add(const _vector<T> &a, const _vector<T> &v){ x=a.x+v.x;y=a.y+v.y;	z=a.z+v.z;	};
	IC  void	add(const _vector<T> &a, T s)			{ x=a.x+s;  y=a.y+s;	z=a.z+s;	};

	IC	void	sub(const _vector<T> &v)				{ x-=v.x;	y-=v.y;		z-=v.z;		};
	IC  void	sub(T s)								{ x-=s;		y-=s;		z-=s;		};
	IC	void	sub(const _vector<T> &a, const _vector<T> &v)	{ x=a.x-v.x;y=a.y-v.y;	z=a.z-v.z;	};
	IC  void	sub(const _vector<T> &a, T s)			{ x=a.x-s;  y=a.y-s;	z=a.z-s;	};

	IC	void	mul(const _vector<T> &v)				{ x*=v.x;	y*=v.y;		z*=v.z;		};
	IC  void	mul(T s)								{ x*=s;		y*=s;		z*=s;		};
	IC	void	mul(const _vector<T> &a, const _vector<T> &v)	{ x=a.x*v.x;y=a.y*v.y;	z=a.z*v.z;	};
	IC  void	mul(const _vector<T> &a, T s)			{ x=a.x*s;  y=a.y*s;	z=a.z*s;	};

	IC	void	div(const _vector<T> &v)				{ x/=v.x;	y/=v.y;  z/=v.z;		};
	IC  void	div(T s)								{ x/=s;		y/=s;    z/=s;			};
	IC	void	div(const _vector<T> &a, const _vector<T> &v)	{ x=a.x/v.x;y=a.y/v.y;	z=a.z/v.z;	};
	IC  void	div(const _vector<T> &a, T s)			{ x=a.x/s;  y=a.y/s;	z=a.z/s;	};

	IC	void	invert()								{ x=-x; y=-y; z=-z;					}
	IC	void	invert(const _vector<T> &a)				{ x=-a.x; y=-a.y; z=-a.z;			}

	IC	void	min(const _vector<T> &v)				{ x = std::_cpp_min(x,v.x);	y = std::_cpp_min(y,v.y);	z = std::_cpp_min(z,v.z);	}
	IC	void	max(const _vector<T> &v)				{ x = std::_cpp_max(x,v.x);	y = std::_cpp_max(y,v.y);	z = std::_cpp_max(z,v.z);	}

	IC	void	abs(const _vector<T> &v)				{ x = _abs(v.x); y=_abs(v.y); z=_abs(v.z);	}
	IC  BOOL	similar(const _vector<T> &v, T E=EPS_L)	{ return _abs(x-v.x)<E && _abs(y-v.y)<E && _abs(z-v.z)<E;};

	IC	void	set_length(T l){
		mul(l/magnitude());
	} 
	// Align vector by axis (!y)
	IC	void	align() {
		y = 0;
		if (_abs(z)>=_abs(x))	{ z /= _abs(z?z:1);	x = 0; }
		else					{ x /= _abs(x);		z = 0; }
	}

	// Squeeze
	IC void		squeeze(T Epsilon){
		if (_abs(x) < Epsilon) x = 0;
		if (_abs(y) < Epsilon) y = 0;
		if (_abs(z) < Epsilon) z = 0;
	}

	// Clamp vector
	IC	void	clamp(const _vector<T> &min, const _vector<T> max) {
		::clamp(x,min.x,min.x);
		::clamp(y,min.y,min.y);
		::clamp(z,min.z,min.z);
	}

	IC	void	clamp(const _vector<T> &_v) {
		_vector<T> v;	v.x = _abs(_v.x);	v.y = _abs(_v.y);	v.z = _abs(_v.z);
		::clamp(x,-v.x,v.x);
		::clamp(y,-v.y,v.y);
		::clamp(z,-v.z,v.z);
	}

	// Interpolate vectors (inertion)
	IC	void	inertion(const _vector<T> &p, T v) {
		T inv = 1.f-v;
		x = v*x + inv*p.x;
		y = v*y + inv*p.y;
		z = v*z + inv*p.z;
	}
	IC	void	average(const _vector<T> &p) {
		x = (x+p.x)*0.5f;
		y = (y+p.y)*0.5f;
		z = (z+p.z)*0.5f;
	}
	IC	void	average(const _vector<T> &p1, const _vector<T> &p2) {
		x = (p1.x+p2.x)*0.5f;
		y = (p1.y+p2.y)*0.5f;
		z = (p1.z+p2.z)*0.5f;
	}
	IC	void	lerp(const _vector<T> &p1, const _vector<T> &p2, T t ){
		T invt = 1.f-t;
		x = p1.x*invt + p2.x*t;
		y = p1.y*invt + p2.y*t;
		z = p1.z*invt + p2.z*t;
	}

	// Direct vector from point P by dir D with length M
	IC	void	direct(const _vector<T> &p, const _vector<T> &d, T m) {
		x = p.x + d.x*m;
		y = p.y + d.y*m;
		z = p.z + d.z*m;
	}
	IC	void	mad(const _vector<T> &p, const _vector<T> &d, T m) {
		x = p.x + d.x*m;
		y = p.y + d.y*m;
		z = p.z + d.z*m;
	}
	IC	void	mad(const _vector<T> &p, const _vector<T> &d, const _vector<T> &s) {
		x = p.x + d.x*s.x;
		y = p.y + d.y*s.y;
		z = p.z + d.z*s.z;
	}

	// SQ magnitude
	IC	T	square_magnitude(void) const {
		return x*x + y*y + z*z;
	}
	// magnitude
	IC	T	magnitude(void) const {
		return _sqrt(square_magnitude());
	}

	// Normalize
	IC	void	normalize(void) {
		VERIFY(square_magnitude() > std::numeric_limits<T>::min());
		T mag=_sqrt(1/(x*x + y*y + z*z));
		x *= mag;
		y *= mag;
		z *= mag;
	}

	// Safe-Normalize
	IC	void	normalize_safe(void) {
		T magnitude=x*x + y*y + z*z;
		if (magnitude> std::numeric_limits<T>::min()) {
			magnitude=sqrtf(1/magnitude);
			x *= magnitude;
			y *= magnitude;
			z *= magnitude;
		}
	}

	// Normalize
	IC	void	normalize(const _vector<T> &v) {
		VERIFY((v.x*v.x+v.y*v.y+v.z*v.z)>flt_zero);
		T mag=_sqrt(1/(v.x*v.x + v.y*v.y + v.z*v.z));
		x = v.x*mag;
		y = v.y*mag;
		z = v.z*mag;
	}

	// Safe-Normalize
	IC	void	normalize_safe(const _vector<T> &v) {
		T magnitude=v.x*v.x + v.y*v.y + v.z*v.z;
		if (magnitude>numeric_limits<T>::min()) {
			magnitude=_sqrt(1/magnitude);
			x = v.x*magnitude;
			y = v.y*magnitude;
			z = v.z*magnitude;
		}
	}

	// DotProduct
	IC	T	dotproduct(const _vector<T> &v) const		   // v1*v2
	{	return x*v.x + y*v.y + z*v.z; }

	// CrossProduct
	IC	void	crossproduct(const _vector<T> &v1, const _vector<T> &v2) // (v1,v2) -> this
	{
		x = v1.y  * v2.z  - v1.z  * v2.y ;
		y = v1.z  * v2.x  - v1.x  * v2.z ;
		z = v1.x  * v2.y  - v1.y  * v2.x ;
	}

	// Distance calculation
	IC	T	distance_to_xz(const _vector<T> &v) const
	{	return _sqrt( (x-v.x)*(x-v.x) + (z-v.z)*(z-v.z) );	}

	// Distance calculation
	IC	T	distance_to_sqr(const _vector<T> &v) const
	{	return (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z);	}

	// Distance calculation
	IC	T	distance_to(const _vector<T> &v) const 
	{	return _sqrt(distance_to_sqr(v));	}

	// Barycentric coords
	IC	void	from_bary			(const _vector<T> &V1, const _vector<T> &V2, const _vector<T> &V3, T u, T v, T w)
	{
		x = V1.x*u + V2.x*v + V3.x*w;
		y = V1.y*u + V2.y*v + V3.y*w;
		z = V1.z*u + V2.z*v + V3.z*w;
	}
	IC	void	from_bary			(const _vector<T> &V1, const _vector<T> &V2, const _vector<T> &V3, const _vector<T> &B)
	{	from_bary(V1,V2,V3,B.x,B.y,B.z); }

	IC	void	from_bary4			(const _vector<T> &V1, const _vector<T> &V2, const _vector<T> &V3, const _vector<T> &V4, T u, T v, T w, T t)
	{
		x = V1.x*u + V2.x*v + V3.x*w + V4.x*t;
		y = V1.y*u + V2.y*v + V3.y*w + V4.y*t;
		z = V1.z*u + V2.z*v + V3.z*w + V4.z*t;
	}

    IC void mknormal_non_normalized	(const _vector<T> &p0, const _vector<T> & p1, const _vector<T> &p2 )
	{
    	_vector v01,v12;
    	v01.sub( p1, p0 );
    	v12.sub( p2, p1 );
    	crossproduct( v01, v12 );
    };
    IC void mknormal( const _vector<T> &p0, const _vector<T> &p1, const _vector<T> &p2 )
	{
		mknormal_non_normalized(p0,p1,p2);
    	normalize_safe();
    };
	IC void random_dir(){
		z = rnd(2)-1.f;
		float a = rnd(PI_MUL_2);
		float r = sqrtf(1.f-z*z);
		float sa,ca; _sincos(a,sa,ca);
		x = r * ca;
		y = r * sa;
	};
};

typedef _vector<float>	Fvector;
typedef _vector<double> Dvector;

#endif

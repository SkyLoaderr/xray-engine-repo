#ifndef __V3D__
#define __V3D__

// Inline call
#ifndef IC
#define IC __forceinline
#endif

template <class T>
struct _vector {
public:
	typedef T			TYPE;
	typedef _vector<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T	x,y,z;

	// access operators
	IC	T&			operator[] (int i)					{ return *((T*)this + i); }
	IC	T&			operator[] (int i)	const			{ return *((T*)this + i); }

	IC	void	set(T _x, T _y, T _z)					{ x = _x;		y = _y;		z = _z;		};
	IC  void	set(const _vector<float> &v)			{ x = T(v.x);	y = T(v.y);	z = T(v.z);	};
	IC  void	set(const _vector<double> &v)			{ x = T(v.x);	y = T(v.y);	z = T(v.z);	};
	IC	void	set(float* p)							{ x = p[0];	y = p[1];	z = p[2];	};
	IC	void	set(double* p)							{ x = p[0];	y = p[1];	z = p[2];	};

	IC	void	add(const Self &v)						{ x+=v.x;	y+=v.y;		z+=v.z;		};
	IC  void	add(T s)								{ x+=s;		y+=s;		z+=s;		};
	IC	void	add(const Self &a, const Self &v)		{ x=a.x+v.x;y=a.y+v.y;	z=a.z+v.z;	};
	IC  void	add(const Self &a, T s)					{ x=a.x+s;  y=a.y+s;	z=a.z+s;	};

	IC	void	sub(const Self &v)						{ x-=v.x;	y-=v.y;		z-=v.z;		};
	IC  void	sub(T s)								{ x-=s;		y-=s;		z-=s;		};
	IC	void	sub(const Self &a, const Self &v)		{ x=a.x-v.x;y=a.y-v.y;	z=a.z-v.z;	};
	IC  void	sub(const Self &a, T s)					{ x=a.x-s;  y=a.y-s;	z=a.z-s;	};

	IC	void	mul(const Self &v)						{ x*=v.x;	y*=v.y;		z*=v.z;		};
	IC  void	mul(T s)								{ x*=s;		y*=s;		z*=s;		};
	IC	void	mul(const Self &a, const Self &v)		{ x=a.x*v.x;y=a.y*v.y;	z=a.z*v.z;	};
	IC  void	mul(const Self &a, T s)					{ x=a.x*s;  y=a.y*s;	z=a.z*s;	};

	IC	void	div(const Self &v)						{ x/=v.x;	y/=v.y;  z/=v.z;		};
	IC  void	div(T s)								{ x/=s;		y/=s;    z/=s;			};
	IC	void	div(const Self &a, const Self &v)		{ x=a.x/v.x;y=a.y/v.y;	z=a.z/v.z;	};
	IC  void	div(const Self &a, T s)					{ x=a.x/s;  y=a.y/s;	z=a.z/s;	};

	IC	void	invert()								{ x=-x; y=-y; z=-z;					}
	IC	void	invert(const Self &a)					{ x=-a.x; y=-a.y; z=-a.z;			}

	IC	void	min(const Self &v1,const Self &v2)		{ x = _min(v1.x,v2.x); y = _min(v1.y,v2.y); z = _min(v1.z,v2.z);	}
	IC	void	min(const Self &v)						{ x = _min(x,v.x);	y = _min(y,v.y);	z = _min(z,v.z);	}
	IC	void	max(const Self &v1,const Self &v2)		{ x = _max(v1.x,v2.x); y = _max(v1.y,v2.y);	z = _max(v1.z,v2.v.z);	}
	IC	void	max(const Self &v)						{ x = _max(x,v.x);	y = _max(y,v.y);	z = _max(z,v.z);	}

	IC	void	abs(const Self &v)						{ x = _abs(v.x); y=_abs(v.y); z=_abs(v.z);	}
	IC  BOOL	similar(const Self &v, T E=EPS_L) const	{ return _abs(x-v.x)<E && _abs(y-v.y)<E && _abs(z-v.z)<E;};

	IC	void	set_length(T l){
		mul(l/magnitude());
	} 

	// Align vector3 by axis (!y)
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

	// Clamp vector3
	IC	void	clamp(const Self &min, const Self max) {
		::clamp(x,min.x,max.x);
		::clamp(y,min.y,max.y);
		::clamp(z,min.z,max.z);
	}

	IC	void	clamp(const Self &_v) {
		Self v;	v.x = _abs(_v.x);	v.y = _abs(_v.y);	v.z = _abs(_v.z);
		::clamp(x,-v.x,v.x);
		::clamp(y,-v.y,v.y);
		::clamp(z,-v.z,v.z);
	}

	// Interpolate vectors (inertion)
	IC	void	inertion(const Self &p, T v) {
		T inv = 1.f-v;
		x = v*x + inv*p.x;
		y = v*y + inv*p.y;
		z = v*z + inv*p.z;
	}
	IC	void	average(const Self &p) {
		x = (x+p.x)*0.5f;
		y = (y+p.y)*0.5f;
		z = (z+p.z)*0.5f;
	}
	IC	void	average(const Self &p1, const Self &p2) {
		x = (p1.x+p2.x)*0.5f;
		y = (p1.y+p2.y)*0.5f;
		z = (p1.z+p2.z)*0.5f;
	}
	IC	void	lerp(const Self &p1, const Self &p2, T t ){
		T invt = 1.f-t;
		x = p1.x*invt + p2.x*t;
		y = p1.y*invt + p2.y*t;
		z = p1.z*invt + p2.z*t;
	}

	// Direct vector3 from point P by dir D with length M
	IC	void	mad(const Self &d, T m) {
		x += d.x*m;
		y += d.y*m;
		z += d.z*m;
	}
	IC	void	mad(const Self &p, const Self &d, T m) {
		x = p.x + d.x*m;
		y = p.y + d.y*m;
		z = p.z + d.z*m;
	}
	IC	void	mad(const Self& d, const Self& s) {
		x += d.x*s.x;
		y += d.y*s.y;
		z += d.z*s.z;
	}
	IC	void	mad(const Self &p, const Self &d, const Self &s) {
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
	IC	void	normalize(const Self &v) {
		VERIFY((v.x*v.x+v.y*v.y+v.z*v.z)>flt_zero);
		T mag=_sqrt(1/(v.x*v.x + v.y*v.y + v.z*v.z));
		x = v.x*mag;
		y = v.y*mag;
		z = v.z*mag;
	}

	// Safe-Normalize
	IC	void	normalize_safe(const Self &v) {
		T magnitude=v.x*v.x + v.y*v.y + v.z*v.z;
		if (magnitude>std::numeric_limits<T>::min()) {
			magnitude=_sqrt(1/magnitude);
			x = v.x*magnitude;
			y = v.y*magnitude;
			z = v.z*magnitude;
		}
	}
	IC void		random_dir		(CRandom& R = ::Random)
	{
		z	= R.randF(-1,1);
		T a = R.randF(PI_MUL_2);
		T r = _sqrt(1-z*z);
		T sa,ca; _sincos(a,sa,ca);
		x	= r * ca;
		y	= r * sa;
	};
	IC void		random_dir		(const Self& ConeAxis, float ConeAngle, CRandom& R = ::Random)
	{
		Self				rnd;
		rnd.random_dir		(R);
		mad					(ConeAxis,rnd,R.randF(tanf(ConeAngle)));
		normalize			();
	}
	IC void		random_point	(const Self& BoxSize, CRandom& R = ::Random)
	{
		x					= R.randFs(BoxSize.x);
		y					= R.randFs(BoxSize.y);
		z					= R.randFs(BoxSize.z);
	}
	IC void		random_point	(T r, CRandom& R = ::Random)
	{	
		random_dir			(R);
		mul					(R.randF(r));
	}

	// DotProduct
	IC	T		dotproduct(const Self &v) const		   // v1*v2
	{	return x*v.x + y*v.y + z*v.z; }

	// CrossProduct
	IC	void	crossproduct(const Self &v1, const Self &v2) // (v1,v2) -> this
	{
		x = v1.y  * v2.z  - v1.z  * v2.y ;
		y = v1.z  * v2.x  - v1.x  * v2.z ;
		z = v1.x  * v2.y  - v1.y  * v2.x ;
	}

	// Distance calculation
	IC	T		distance_to_xz(const Self &v) const
	{	return _sqrt( (x-v.x)*(x-v.x) + (z-v.z)*(z-v.z) );	}

	// Distance calculation
	IC	T		distance_to_sqr(const Self &v) const
	{	return (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z);	}

	// Distance calculation
	IC	T		distance_to(const Self &v) const 
	{	return _sqrt(distance_to_sqr(v));	}

	// Barycentric coords
	IC	void	from_bary			(const Self &V1, const Self &V2, const Self &V3, T u, T v, T w)
	{
		x = V1.x*u + V2.x*v + V3.x*w;
		y = V1.y*u + V2.y*v + V3.y*w;
		z = V1.z*u + V2.z*v + V3.z*w;
	}
	IC	void	from_bary			(const Self &V1, const Self &V2, const Self &V3, const Self &B)
	{	from_bary(V1,V2,V3,B.x,B.y,B.z); }

	IC	void	from_bary4			(const Self &V1, const Self &V2, const Self &V3, const Self &V4, T u, T v, T w, T t)
	{
		x = V1.x*u + V2.x*v + V3.x*w + V4.x*t;
		y = V1.y*u + V2.y*v + V3.y*w + V4.y*t;
		z = V1.z*u + V2.z*v + V3.z*w + V4.z*t;
	}

    IC void		mknormal_non_normalized	(const Self &p0, const Self & p1, const Self &p2 )
	{
    	_vector v01,v12;
    	v01.sub( p1, p0 );
    	v12.sub( p2, p1 );
    	crossproduct( v01, v12 );
    };
    IC void		mknormal( const Self &p0, const Self &p1, const Self &p2 )
	{
		mknormal_non_normalized(p0,p1,p2);
    	normalize_safe();
    };
	IC	void	setHP	(T h, T p)
	{
        T _ch, _cp, _sh, _sp;
        _sincos(h,_sh,_ch);
        _sincos(p,_sp,_cp);

        x = -_cp*_sh;
        y = _sp;
        z = _cp*_ch;
    }
    IC	void	getHP	(T& h, T& p) const
    {
        float hyp;

        if (fis_zero(x)&& fis_zero(z)){
            h = 0.0f;
            if (!fis_zero(float(y)))	p = (y>0.0f)?PI_DIV_2:-PI_DIV_2;
            else            			p = 0.0f;
        }else{
            if (fis_zero(z))			h = (x>0.0f)?-PI_DIV_2:PI_DIV_2;
            else if (z<0.0f)			h = -(atanf(x/z)-PI);
            else            			h = -atanf(x/z);
            hyp = _sqrt(x*x+z*z);
            if (fis_zero(float(hyp)))	p = (y>0.0f)?PI_DIV_2:-PI_DIV_2;
            else						p = atanf(y/hyp);
        }
    }

	IC	void	reflect(const Self& dir, const Self& norm){
		mad(dir,norm,-2*dir.dotproduct(norm));
	}
	IC	void	slide(const Self& dir, const Self& norm){	// non normalized
		mad(dir,norm,-dir.dotproduct(norm));
	}
};
typedef _vector<float>	Fvector;
typedef _vector<float>	Fvector3;
typedef _vector<double> Dvector;
typedef _vector<double> Dvector3;
typedef _vector<s32>	Ivector;
typedef _vector<s32>	Ivector3;

#endif

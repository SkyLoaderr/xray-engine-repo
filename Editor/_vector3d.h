#ifndef __V3D__
#define __V3D__

// Inline call
#ifndef IC
#define IC __forceinline
#endif

typedef struct _vector {
public:
	union {
		struct{
			float	x,y,z;
		};
		float		m[3];
	};

	IC	D3DVECTOR*	d3d(void)						{ return asD3D();					};
	IC	D3DVECTOR*	asD3D(void)						{ return (D3DVECTOR *)this;			};
	IC	float*		asFLOAT(void)					{ return (float *)this;				};
	IC	DWORD*		asDWORD(void)					{ return (DWORD *)this;				};

	IC	void	set(float _x, float _y, float _z)	{ x = _x;	y = _y;		z = _z;		};
	IC  void	set(_vector &v)						{ x = v.x;	y = v.y;	z = v.z;	};
	IC	void	set(float* p)						{ x = p[0];	y = p[1];	z = p[2];	};
	IC	void	get(float* p)						{ p[0] = x;	p[1] = y;	p[2] = z;	};

	IC	void	add(_vector &v)						{ x+=v.x;	y+=v.y;		z+=v.z;		};
	IC  void	add(float s)						{ x+=s;		y+=s;		z+=s;		};
	IC	void	add(_vector &a, _vector &v)			{ x=a.x+v.x;y=a.y+v.y;	z=a.z+v.z;	};
	IC  void	add(_vector &a, float s)			{ x=a.x+s;  y=a.y+s;	z=a.z+s;	};

	IC	void	sub(_vector &v)						{ x-=v.x;	y-=v.y;		z-=v.z;		};
	IC  void	sub(float s)						{ x-=s;		y-=s;		z-=s;		};
	IC	void	sub(_vector &a, _vector &v)			{ x=a.x-v.x;y=a.y-v.y;	z=a.z-v.z;	};
	IC  void	sub(_vector &a, float s)			{ x=a.x-s;  y=a.y-s;	z=a.z-s;	};

	IC	void	mul(_vector &v)						{ x*=v.x;	y*=v.y;		z*=v.z;		};
	IC  void	mul(float s)						{ x*=s;		y*=s;		z*=s;		};
	IC	void	mul(_vector &a, _vector &v)			{ x=a.x*v.x;y=a.y*v.y;	z=a.z*v.z;	};
	IC  void	mul(_vector &a, float s)			{ x=a.x*s;  y=a.y*s;	z=a.z*s;	};

	IC	void	div(_vector &v)						{ x/=v.x;	y/=v.y;  z/=v.z;		};
	IC  void	div(float s)						{ x/=s;		y/=s;    z/=s;			};
	IC	void	div(_vector &a, _vector &v)			{ x=a.x/v.x;y=a.y/v.y;	z=a.z/v.z;	};
	IC  void	div(_vector &a, float s)			{ x=a.x/s;  y=a.y/s;	z=a.z/s;	};

	IC  BOOL	similar(_vector &v, float E=EPS_L)	{ return fabsf(x-v.x)<E && fabsf(y-v.y)<E && fabsf(z-v.z)<E; };

	IC	void	min(_vector &v)						{ x = _MIN(x,v.x);	y = _MIN(y,v.y);	z = _MIN(z,v.z);	}
	IC	void	max(_vector &v)						{ x = _MAX(x,v.x);	y = _MAX(y,v.y);	z = _MAX(z,v.z);	}

	// Align vector by axis (!y)
	IC	void	align() {
		y = 0;
		if (fabsf(z)>=fabsf(x))	{ z /= fabsf(z?z:1);	x = 0; }
		else					{ x /= fabsf(x);		z = 0; }
	}

	// Clamp vector
	IC	void	clamp(_vector &min, _vector max) {
		if (x<min.x) x = min.x; else if (x>max.x) x = max.x;
		if (y<min.y) y = min.y; else if (y>max.y) y = max.y;
		if (z<min.z) z = min.z; else if (z>max.z) z = max.z;
	}

	IC	void	clamp(_vector &_v) {
		_vector v;	v.x = fabsf(_v.x);	v.y = fabsf(_v.y);	v.z = fabsf(_v.z);
		if (x<-v.x) x = -v.x; else if (x>v.x) x = v.x;
		if (y<-v.y) y = -v.y; else if (y>v.y) x = v.y;
		if (z<-v.z) z = -v.z; else if (z>v.z) z = v.z;
	}

	// Interpolate vectors (inertion)
	IC	void	inertion(_vector &p, float v) {
		x = v*x + (1.f-v)*p.x;
		y = v*y + (1.f-v)*p.y;
		z = v*z + (1.f-v)*p.z;
	}

	// Direct vector from point P by dir D with length M
	IC	void	direct(_vector &p, _vector &d, float m) {
		x = p.x + d.x*m;
		y = p.y + d.y*m;
		z = p.z + d.z*m;
	}

	// SQ magnitude
	IC	float	square_magnitude(void) {
		return x*x + y*y + z*z;
	}
	// magnitude
	IC	float	magnitude(void) {
		return sqrtf(square_magnitude());
	}

	// Normalize
	IC	void	normalize(void) {
		VERIFY(square_magnitude()>EPS_S);
		float mag=sqrtf(1.0f/(x*x + y*y + z*z));
		x *= mag;
		y *= mag;
		z *= mag;
	}

	// Safe-Normalize
	IC	void	normalize_safe(void) {
		float magnitude=x*x + y*y + z*z;
		if (magnitude>EPS_S) {
			magnitude=sqrtf(1/magnitude);
			x *= magnitude;
			y *= magnitude;
			z *= magnitude;
		}
	}

	// Normalize
	IC	void	normalize(_vector &v) {
		VERIFY((v.x*v.x+v.y*v.y+v.z*v.z)>EPS_S);
		float mag=sqrtf(1.0f/(v.x*v.x + v.y*v.y + v.z*v.z));
		x = v.x*mag;
		y = v.y*mag;
		z = v.z*mag;
	}

	// Safe-Normalize
	IC	void	normalize_safe(_vector &v) {
		float magnitude=v.x*v.x + v.y*v.y + v.z*v.z;
		if (magnitude>EPS_S) {
			magnitude=sqrtf(1/magnitude);
			x = v.x*magnitude;
			y = v.y*magnitude;
			z = v.z*magnitude;
		}
	}

	// DotProduct
	IC	float	dotproduct(const _vector &v)				   // v1*v2
	{	return x*v.x + y*v.y + z*v.z; }

	// CrossProduct
	IC	void	crossproduct(const _vector &v1, const _vector &v2) // (v1,v2) -> this
	{
		x = v1.y  * v2.z  - v1.z  * v2.y ;
		y = v1.z  * v2.x  - v1.x  * v2.z ;
		z = v1.x  * v2.y  - v1.y  * v2.x ;
	}

	// Distance calculation
	IC	float	distance_to_xz(const _vector &v)
	{	return sqrtf( (x-v.x)*(x-v.x) + (z-v.z)*(z-v.z) );	}

	// Distance calculation
	IC	float	distance_to_sqr(const _vector &v)
	{	return (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z);	}

	// Distance calculation
	IC	float	distance_to(const _vector &v)
	{	return sqrtf(distance_to_sqr(v));	}

	// Barycentric coords
	IC	void	from_bary			(_vector &V1, _vector &V2, _vector &V3, float u, float v, float w)
	{
		x = V1.x*u + V2.x*v + V3.x*w;
		y = V1.y*u + V2.y*v + V3.y*w;
		z = V1.z*u + V2.z*v + V3.z*w;
	}
	IC	void	from_bary4			(_vector &V1, _vector &V2, _vector &V3, _vector &V4, float u, float v, float w, float t)
	{
		x = V1.x*u + V2.x*v + V3.x*w + V4.x*t;
		y = V1.y*u + V2.y*v + V3.y*w + V4.y*t;
		z = V1.z*u + V2.z*v + V3.z*w + V4.z*t;
	}
	IC	void	from_bary			(_vector &V1, _vector &V2, _vector &V3, _vector &B)
	{	from_bary(V1,V2,V3,B.x,B.y,B.z); }

	IC	void	position_of_camera	(const _matrix &m);
	IC	void	transform_tiny		(const _matrix &mat);
	IC	void	transform			(const _matrix &mat);
	IC	void	transform_dir		(const _matrix &mat);
	IC	void	transform_tiny		(const _vector &v, const _matrix &mat); // preferred to use
	IC	void	transform			(const _vector &v, const _matrix &mat); // preferred to use
	IC	void	transform_dir		(const _vector &v, const _matrix &mat); // preferred to use

    //--------------------------------------------------------------------------
    // RAPID extension
    //--------------------------------------------------------------------------
    IC void sMxVpV(float s1, _matrix33& M1, _vector& V1, _vector& V2);
    IC void MTxV(_matrix33& M1, _vector& V1);
    IC void sMTxV(float s1, _matrix33& M1, _vector& V1);
    //---------------------------------------------------
    // unused
    //---------------------------------------------------
    IC void McolcV(_matrix33& M, int c);
    IC void MxV(_matrix33& M1, _vector& V1);
    IC void MxVpV(_matrix33& M1, _vector& V1, _vector& V2);
    IC void VpVxS(_vector& V1, _vector& V2, float s)
    {
      x = V1.x + V2.x * s;
      y = V1.y + V2.y * s;
      z = V1.z + V2.z * s;
    }

    // AlexRR
    IC void translate( _vector& v, float scal ){
  		x += v.x * scal; y += v.y * scal; z += v.z * scal;
    };
    IC void mknormal( _vector& p0, _vector& p1, _vector& p2 ){
    	_vector v01,v12;
    	v01.sub( p1, p0 );
    	v12.sub( p2, p1 );
    	crossproduct( v01, v12 );
    	normalize_safe();
    };
} Fvector;

#endif

#ifndef __V3D__
#define __V3D__

typedef struct _vector {
public:
	union {
		struct{
			float	x,y,z;
		};
		float		m[3];
	};

	__forceinline	D3DVECTOR	*d3d(void)						{ return (D3DVECTOR *)this;	};
	__forceinline	void	set(float _x, float _y, float _z)	{ x = _x;	y = _y;		z = _z;		};
	__forceinline   void	set(_vector &v)						{ x = v.x;	y = v.y;	z = v.z;	};
	__forceinline	void	set(float* p)						{ x = p[0];	y = p[1];	z = p[2];	};
	__forceinline	void	get(float* p)						{ p[0] = x;	p[1] = y;	p[2] = z;	};

	__forceinline	void	add(_vector &v)						{ x+=v.x;	y+=v.y;		z+=v.z;		};
	__forceinline   void	add(float s)						{ x+=s;		y+=s;		z+=s;		};
	__forceinline	void	add(_vector &a, _vector &v)			{ x=a.x+v.x;y=a.y+v.y;	z=a.z+v.z;	};
	__forceinline   void	add(_vector &a, float s)			{ x=a.x+s;  y=a.y+s;	z=a.z+s;	};

	__forceinline	void	sub(_vector &v)						{ x-=v.x;	y-=v.y;		z-=v.z;		};
	__forceinline   void	sub(float s)						{ x-=s;		y-=s;		z-=s;		};
	__forceinline	void	sub(_vector &a, _vector &v)			{ x=a.x-v.x;y=a.y-v.y;	z=a.z-v.z;	};
	__forceinline   void	sub(_vector &a, float s)			{ x=a.x-s;  y=a.y-s;	z=a.z-s;	};

	__forceinline	void	mul(_vector &v)						{ x*=v.x;	y*=v.y;		z*=v.z;		};
	__forceinline   void	mul(float s)						{ x*=s;		y*=s;		z*=s;		};
	__forceinline	void	mul(_vector &a, _vector &v)			{ x=a.x*v.x;y=a.y*v.y;	z=a.z*v.z;	};
	__forceinline   void	mul(_vector &a, float s)			{ x=a.x*s;  y=a.y*s;	z=a.z*s;	};

	__forceinline	void	div(_vector &v)						{ x/=v.x;	y/=v.y;  z/=v.z;		};
	__forceinline   void	div(float s)						{ x/=s;		y/=s;    z/=s;			};
	__forceinline	void	div(_vector &a, _vector &v)			{ x=a.x/v.x;y=a.y/v.y;	z=a.z/v.z;	};
	__forceinline   void	div(_vector &a, float s)			{ x=a.x/s;  y=a.y/s;	z=a.z/s;	};

	__forceinline   bool	cmp(_vector &v)						{ return x==v.x && y==v.y && z == v.z; };

	// Align vector by axis (!y)
	__forceinline	void	align() {
		y = 0;
		if (fabsf(z)>=fabsf(x))	{ z /= fabsf(z?z:1);	x = 0; }
		else					{ x /= fabsf(x);		z = 0; }
	}

	// Clamp vector
	__forceinline	void	clamp(_vector &min, _vector max) {
		if (x<min.x) x = min.x; else if (x>max.x) x = max.x;
		if (y<min.y) y = min.y; else if (y>max.y) y = max.y;
		if (z<min.z) z = min.z; else if (z>max.z) z = max.z;
	}

	__forceinline	void	clamp(_vector &_v) {
		Fvector v;	v.x = fabsf(_v.x);	v.y = fabsf(_v.y);	v.z = fabsf(_v.z);
		if (x<-v.x) x = -v.x; else if (x>v.x) x = v.x;
		if (y<-v.y) y = -v.y; else if (y>v.y) x = v.y;
		if (z<-v.z) z = -v.z; else if (z>v.z) z = v.z;
	}

	// Interpolate vectors (inertion)
	__forceinline	void	inertion(_vector &p, float v) {
		x = v*x + (1.f-v)*p.x;
		y = v*y + (1.f-v)*p.y;
		z = v*z + (1.f-v)*p.z;
	}

	// Direct vector from point P by dir D with length M
	__forceinline	void	direct(_vector &p, _vector &d, float m) {
		x = p.x + d.x*m;
		y = p.y + d.y*m;
		z = p.z + d.z*m;
	}

	// magnitude
	__forceinline	float	magnitude(void) {
		return sqrtf(x*x + y*y + z*z);
	}

	// SQ magnitude
	__forceinline	float	square_magnitude(void) {
		return x*x + y*y + z*z;
	}

	// Normalize
	_E(__forceinline	void	normalize(void))
		VERIFY((x*x+y*y+z*z)>0.00000001f);
		float mag=sqrtf(1.0f/(x*x + y*y + z*z));
		x *= mag;
		y *= mag;
		z *= mag;
	_end;

	// Safe-Normalize
	__forceinline	void	normalize_safe(void) {
		float magnitude=x*x + y*y + z*z;
		if (magnitude>0.000001f) {
			magnitude=sqrtf(1/magnitude);
			x *= magnitude;
			y *= magnitude;
			z *= magnitude;
		}
	}

	// Normalize
	_E(__forceinline	void	normalize(_vector &v))
		VERIFY((v.x*v.x+v.y*v.y+v.z*v.z)>0.00000001f);
		float mag=sqrtf(1.0f/(v.x*v.x + v.y*v.y + v.z*v.z));
		x = v.x*mag;
		y = v.y*mag;
		z = v.z*mag;
	_end;

	// Safe-Normalize
	__forceinline	void	normalize_safe(_vector &v) {
		float magnitude=v.x*v.x + v.y*v.y + v.z*v.z;
		if (magnitude>0.000001f) {
			magnitude=sqrtf(1/magnitude);
			x = v.x*magnitude;
			y = v.y*magnitude;
			z = v.z*magnitude;
		}
	}

	// DotProduct
	__forceinline	float	dotproduct(const _vector &v)				   // v1*v2
	{
		return x*v.x + y*v.y + z*v.z;
	}

	// CrossProduct
	__forceinline	void	crossproduct(const _vector &v1, const _vector &v2) // (v1,v2) -> this
	{
		x = v1.y  * v2.z  - v1.z  * v2.y ;
		y = v1.z  * v2.x  - v1.x  * v2.z ;
		z = v1.x  * v2.y  - v1.y  * v2.x ;
	}

	// Distance calculation
	__forceinline	float	distance_to_xz(const _vector &v)
	{
		return sqrtf( (x-v.x)*(x-v.x) + (z-v.z)*(z-v.z) );
	}

	// Distance calculation
	__forceinline	float	distance_to(const _vector &v)
	{
		return sqrtf( (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z) );
	}

	// Distance calculation
	__forceinline	float	distance_to_sqr(const _vector &v)
	{
		return (x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z);
	}
	__forceinline	void	position_of_camera	(const _matrix &m);
	__forceinline	void	transform_tiny		(const _matrix &mat);
	__forceinline	void	transform			(const _matrix &mat);
	__forceinline	void	transform_dir		(const _matrix &mat);
	__forceinline	void	transform_tiny		(const _vector &v, const _matrix &mat); // preferred to use
	__forceinline	void	transform			(const _vector &v, const _matrix &mat); // preferred to use
	__forceinline	void	transform_dir		(const _vector &v, const _matrix &mat); // preferred to use
} Fvector;

#endif

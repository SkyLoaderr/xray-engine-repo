#ifndef _vector_included
#define _vector_included

#define EPS			0.000001f
#define V_DIFF		0.000100f
//#define PI			3.1415926535897932384626433832795f
#define PI_MUL_2	6.2831853071795864769252867665590f
#define PI_DIV_2	1.5707963267948966192313216916398f
#define PI_DIV_4	0.7853981633974483096156608458199f
#define PI_DIV_6	0.5235987755982988730771072305466f
#define PI_DIV_8	0.3926990816987241548078304229099f

#define uchar unsigned char

struct _vector;		// vector
struct _matrix;		// matrix

struct _fpoint;		// fpoint
struct _lpoint;		// lpoint

struct _lrect;		// lrect
struct _frect;		// frect
struct _fbox;		// fbox

struct _TLpolygon;	// tlpolygon
struct _Lpolygon;	// lpolygon

struct _3dbuffer;	// direct sound 3d buffer
struct _3dlistener;	// direct sound listener

struct _vertex;		// standart vertex (like D3DVERTEX)
struct _Lvertex;	// already lit veretx
struct _TLvertex;	// transformed&lit vertex
struct _LITvertex;	// already lit, but can be litted addititionally

struct _color;		// floating point based color definition
struct _Pcolor;		// Packed color - DWORD

struct _lminmax;	// Min Max
struct _fminmax;	// Min Max

#pragma pack(push)
#pragma pack(1)

__forceinline uchar ClampColor(float a) {
	if (a<0.f) a=0.f; else if (a>1.f) a=1.f;
	return uchar(a*255.f);
}

typedef struct _Pcolor {
public:
	union {
		struct {
			uchar b,g,r,a;
		};
		DWORD c;
	};
	__forceinline	void	set(DWORD color)							{
		c=color;
	};
	__forceinline	void	set(_Pcolor &color)							{
		c=color.c;
	};
	__forceinline	void	set(uchar _r, uchar _g, uchar _b, uchar _a)	{
		a=_a; r=_r; g=_g; b=_b;
	};
	__forceinline	void	setf(float _r, float _g, float _b, float _a)	{
		a=ClampColor(_a);
		r=ClampColor(_r);
		g=ClampColor(_g);
		b=ClampColor(_b);
	};
	__forceinline	void	mul(float s) {
		a = uchar(float(a)*s);
		r = uchar(float(r)*s);
		g = uchar(float(g)*s);
		b = uchar(float(b)*s);
	}
} FPcolor;

typedef struct _color {
public:
	float r,g,b,a;

	__forceinline	void	set(DWORD dw)
	{
		const float f = 1.0f / 255.0f;
		a = f * (float) (uchar) (dw >> 24);
		r = f * (float) (uchar) (dw >> 16);
		g = f * (float) (uchar) (dw >>  8);
		b = f * (float) (uchar) (dw >>  0);
	};
	__forceinline	void	set(_Pcolor dw)
	{
		const float f = 1.0f / 255.0f;
		a = f * (float) dw.a;
		r = f * (float) dw.r;
		g = f * (float) dw.g;
		b = f * (float) dw.b;
	};
	__forceinline	void	set(float _r, float _g, float _b, float _a)
	{	r = _r; g = _g; b = _b; a = _a;		};
	__forceinline	void	set(_color dw)
	{	r=dw.r; g=dw.g; b=dw.b; a = dw.a;	};
	__forceinline	DWORD	get( )
	{
		uchar _a, _r, _g, _b;
		_a = (uchar)(a*255.f);
		_r = (uchar)(r*255.f);
		_g = (uchar)(g*255.f);
		_b = (uchar)(b*255.f);
		return ((DWORD)(_a<<24) | (_r<<16) | (_g<<8) | (_b));
	};
	__forceinline	DWORD	get_ideal( )
	{
		float max, dif;

		max = a;
		if (r>max) max=r;
		if (g>max) max=g;
		if (b>max) max=b;
		dif = 255.f/max;
#define CNV(x) DWORD(uchar((x<0?0:x)*dif))
		return ((DWORD)(CNV(a)<<24) | (CNV(r)<<16) | (CNV(g)<<8) | (CNV(b)));
#undef CNV
	};
	__forceinline	DWORD	get_scale( )
	{
		uchar _a, _r, _g, _b;
		float min, max, dif;

		min = max = a;
		if (r<min) min=r; if (r>max) max=r;
		if (g<min) min=g; if (g>max) max=g;
		if (b<min) min=b; if (b>max) max=b;
		dif = 255.f/(max-min);

		_a = (uchar)((a+min)*dif);
		_r = (uchar)((r+min)*dif);
		_g = (uchar)((g+min)*dif);
		_b = (uchar)((b+min)*dif);

		return ((DWORD)(_a<<24) | (_r<<16) | (_g<<8) | (_b));
	};
	__forceinline	void	adjust_contrast(float f)				// >1 - contrast will be increased
	{
		r = 0.5f + f * (r - 0.5f);
		g = 0.5f + f * (g - 0.5f);
		b = 0.5f + f * (b - 0.5f);
	};
	__forceinline	void	adjust_contrast(_color &in, float f)	// >1 - contrast will be increased
	{
		r = 0.5f + f * (in.r - 0.5f);
		g = 0.5f + f * (in.g - 0.5f);
		b = 0.5f + f * (in.b - 0.5f);
	};
	__forceinline	void	adjust_saturation(float s)
	{
		// Approximate values for each component's contribution to luminance.
		// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
		float grey = r * 0.2125f + g * 0.7154f + b * 0.0721f;

		r = grey + s * (r - grey);
		g = grey + s * (g - grey);
		b = grey + s * (b - grey);
	};
	__forceinline	void	adjust_saturation(_color &in, float s)
	{
		// Approximate values for each component's contribution to luminance.
		// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
		float grey = in.r * 0.2125f + in.g * 0.7154f + in.b * 0.0721f;

		r = grey + s * (in.r - grey);
		g = grey + s * (in.g - grey);
		b = grey + s * (in.b - grey);
	};
	__forceinline	void	adjust_hue(float s)
	{
// AlexMX
/*		float grey = r2 * 0.2125f + g2 * 0.7154f + b2 * 0.0721f;

		r1= (r2 - grey + s*grey)/s;
		g1= (g2 - grey + s*grey)/s;
		b1= (b2 - grey + s*grey)/s;
		s*grey = (r2 - grey + s*grey) * 0.2125f + (g2 - grey + s*grey) * 0.7154f +  (b2 - grey + s*grey) * 0.0721f;
		s*grey-s*grey*0.2125f = (r2-grey)*0.2125f
*/	};
	__forceinline	void	modulate(_color &in)
	{
		r*=in.r;
		g*=in.g;
		b*=in.b;
		a*=in.a;
	};
	__forceinline	void	modulate(_color &in1, _color &in2)
	{
		r=in1.r*in2.r;
		g=in1.g*in2.g;
		b=in1.b*in2.b;
		a=in1.a*in2.a;
	};
	__forceinline	void	negative(_color &in)
	{
		r=1.0f-in.r;
		g=1.0f-in.g;
		b=1.0f-in.b;
		a=1.0f-in.a;
	};
	__forceinline	void	negative(void)
	{
		r=1.0f-r;
		g=1.0f-g;
		b=1.0f-b;
		a=1.0f-a;
	};
	__forceinline	void	sub(float s)
	{
		r-=s;
		g-=s;
		b-=s;
//		a=1.0f-a;
	};
	__forceinline	void	mul(float s) {
		r*=s;
		g*=s;
		b*=s;
//		a*=s;
	};
	__forceinline	void	mul(_color &c, float s) {
		r=c.r*s;
		g=c.g*s;
		b=c.b*s;
//		a=c.a*s;
	};
/*
	__forceinline	FPcolor	pack(void) {
	};
*/
} Fcolor;

typedef struct _vector {
public:
	union {
		struct{
			float	x,y,z;
		};
		float		m[3];
	};

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

	__forceinline	bool	equal(_vector &o, float d=V_DIFF)
	{
		if ((fabsf(x-o.x)<d)&&(fabsf(y-o.y)<d)&&(fabsf(z-o.z)<d)) return true;
		else return false;
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
	__forceinline	void	interpolate(_vector &p, float v) {
		x = v*x + (1.f-v)*p.x;
		y = v*y + (1.f-v)*p.y;
		z = v*z + (1.f-v)*p.z;
	}

	// Direct vector from point P by dir D with length M
	__forceinline	void	direct(_vector &p, _vector &d, float m) {
		x = p.x+d.x*m;
		y = p.y+d.y*m;
		z = p.z+d.z*m;
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
	__forceinline	void	normalize(void) {
		assert((x*x+y*y+z*z)>0.00000001f);
		float mag=sqrtf(1.0f/(x*x + y*y + z*z));
		x *= mag;
		y *= mag;
		z *= mag;
	}

	// Safe-Normalize
	__forceinline	void	normalize_safe(void) {
		float magnitude=x*x + y*y + z*z;
		if (magnitude>0.000001f) {
			magnitude=sqrtf(1/magnitude);
			x *= magnitude;
			y *= magnitude;
			z *= magnitude;
		} else {
			_vector t;
			t.set(0,0,0);
			if (x>y && x>z) t.x = 1.f;
			if (y>x && y>z) t.y = 1.f;
			if (z>x && z>y) t.z = 1.f;
			set(t);
		}
	}

	// Normalize
	__forceinline	void	normalize(_vector &v) {
		assert((v.x*v.x+v.y*v.y+v.z*v.z)>0.00000001f);
		float mag=sqrtf(1.0f/(v.x*v.x + v.y*v.y + v.z*v.z));
		x = v.x*mag;
		y = v.y*mag;
		z = v.z*mag;
	}

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
	__forceinline	void	minv(_vector &v)
	{
		x = min(x,v.x);
		y = min(y,v.y);
		z = min(z,v.z);
	}
	__forceinline	void	maxv(_vector &v)
	{
		x = max(x,v.x);
		y = max(y,v.y);
		z = max(z,v.z);
	}
	__forceinline	void	position_of_camera	(const _matrix &m);
	__forceinline	void	transform_tiny		(const _matrix &mat);
	__forceinline	void	transform			(const _matrix &mat);
	__forceinline	void	transform_dir		(const _matrix &mat);
	__forceinline	void	transform_tiny		(const _vector &v, const _matrix &mat); // preferred to use
	__forceinline	void	transform			(const _vector &v, const _matrix &mat); // preferred to use
	__forceinline	void	transform_dir		(const _vector &v, const _matrix &mat); // preferred to use
} Fvector;

typedef struct _lpoint {
public:
	union{
		struct{
			int 	x, y;
		};
		int 		f[2];
	};

	__forceinline	void	set(int  _x, int  _y)	{ x=_x;	y=_y; };
	__forceinline   void	set(_lpoint &p)			{ x=p.x;y=p.y;};
	__forceinline   void	set(_fpoint &p);
	__forceinline   void	null( )					{ x=0;	y=0;  };

	__forceinline   void	add(int  s)				{ x+=s;	y+=s; };
	__forceinline   void	sub(int  s)				{ x-=s;	y-=s; };
	__forceinline   void	mul(int  s)				{ x*=s;	y*=s; };
	__forceinline   void	div(int  s)				{ x/=s;	y/=s; };

	__forceinline   void	add(int   s1, int   s2)	{ x+=s1; y+=s2; };
	__forceinline   void	sub(int   s1, int   s2)	{ x-=s1; y-=s2; };
	__forceinline   void	mul(int   s1, int   s2)	{ x*=s1; y*=s2; };
	__forceinline   void	div(int   s1, int   s2)	{ x/=s1; y/=s2; };

	__forceinline   bool	cmp(_lpoint &p)			{ return x==p.x && y==p.y; };
	__forceinline	bool	operator == (_lpoint &a )
	{
		return (a.x==x)&&(a.y==y);
	};
} Lpoint;

typedef struct _fpoint {
public:
	union{
		struct{
			float 	x, y;
		};
		float		f[2];
	};

	__forceinline	void	set(float _x, float _y)	{ x=_x;	y=_y; };
	__forceinline   void	set(_fpoint &p)			{ x=p.x;y=p.y;};
	__forceinline   void	set(_lpoint &p);
	__forceinline   void	null( )					{ x=0;	y=0;  };

	__forceinline   void	add(float s)			{ x+=s;	y+=s; };
	__forceinline   void	sub(float s)			{ x-=s;	y-=s; };
	__forceinline   void	mul(float s)			{ x*=s;	y*=s; };
	__forceinline   void	div(float s)			{ x/=s;	y/=s; };

	__forceinline   void	add(float s1, float s2)	{ x+=s1; y+=s2; };
	__forceinline   void	sub(float s1, float s2)	{ x-=s1; y-=s2; };
	__forceinline   void	mul(float s1, float s2)	{ x*=s1; y*=s2; };
	__forceinline   void	div(float s1, float s2)	{ x/=s1; y/=s2; };

	__forceinline   bool	cmp(_fpoint &p)			{ return x==p.x && y==p.y; };
} Fpoint;

void Lpoint::set(_fpoint &p){
	x=int(p.x);
	y=int(p.y);
};

void Fpoint::set(_lpoint &p){
	x=float(p.x);
	y=float(p.y);
};

typedef struct _lrect {
public:
	union{
		struct{
			int 	x1, y1, x2, y2;
		};
		int 		m[4];
	};

	__forceinline	RECT*	get_LPRECT()								{ return LPRECT(&m); };

	__forceinline	void	set(int  _x1, int  _y1, int  _x2, int  _y2)	{ x1=_x1;	y1=_y1;		x2=_x2;		y2=_y2;		};
	__forceinline   void	set(_lrect &r)								{ x1=r.x1;	y1=r.y1;	x2=r.x2;	y2=r.y2;	};
	__forceinline   void	null( )										{ x1=0;		y1=0;		x2=0;		y2=0;		};

	__forceinline   void	add(int x, int y)							{ x1+=x;	y1+=y;		x2+=x;		y2+=y;		};
	__forceinline   void	sub(int x, int y)							{ x1-=x;	y1-=y;		x2-=x;		y2-=y;		};
	__forceinline   void	mul(int x, int y)							{ x1*=x;	y1*=y;		x2*=x;		y2*=y;		};
	__forceinline   void	div(int x, int y)							{ x1/=x;	y1/=y;		x2/=x;		y2/=y;		};

	__forceinline   void	add(_lrect r, int x, int y)					{ x1=r.x1+x;	y1=r.y1+y;	x2=r.x2+x;	y2=r.y2+y; };
	__forceinline   void	sub(_lrect r, int x, int y)					{ x1=r.x1-x;	y1=r.y1-y;	x2=r.x2-x;	y2=r.y2-y; };
	__forceinline   void	mul(_lrect r, int x, int y)					{ x1=r.x1*x;	y1=r.y1*y;	x2=r.x2*x;	y2=r.y2*y; };
	__forceinline   void	div(_lrect r, int x, int y)					{ x1=r.x1/x;	y1=r.y1/y;	x2=r.x2/x;	y2=r.y2/y; };

	__forceinline   bool	in (int  x, int  y)							{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2);	};
	__forceinline   bool	cmp(_lrect &r)								{ return x1==r.x1 && y1==r.y1 && x2==r.x2 && y2==r.y2; };
} Lrect;

typedef struct _frect {
public:
	union{
		struct{
			float 	x1, y1, x2, y2;
		};
		float		m[4];
	};

	__forceinline	void	set(float _x1, float _y1, float _x2, float _y2)	{ x1=_x1;	y1=_y1;		x2=_x2;		y2=_y2;		};
	__forceinline   void	set(_frect &r)								{ x1=r.x1;	y1=r.y1;	x2=r.x2;	y2=r.y2;	};
	__forceinline   void	null( )										{ x1=0;		y1=0;		x2=0;		y2=0;		};

	__forceinline   void	add(float x, float y)						{ x1+=x;	y1+=y;		x2+=x;		y2+=y;		};
	__forceinline   void	sub(float x, float y)						{ x1-=x;	y1-=y;		x2-=x;		y2-=y;		};
	__forceinline   void	mul(float x, float y)						{ x1*=x;	y1*=y;		x2*=x;		y2*=y;		};
	__forceinline   void	div(float x, float y)						{ x1/=x;	y1/=y;		x2/=x;		y2/=y;		};

	__forceinline   void	add(_frect r, float x, float y)				{ x1=r.x1+x;	y1=r.y1+y;	x2=r.x2+x;	y2=r.y2+y; };
	__forceinline   void	sub(_frect r, float x, float y)				{ x1=r.x1-x;	y1=r.y1-y;	x2=r.x2-x;	y2=r.y2-y; };
	__forceinline   void	mul(_frect r, float x, float y)				{ x1=r.x1*x;	y1=r.y1*y;	x2=r.x2*x;	y2=r.y2*y; };
	__forceinline   void	div(_frect r, float x, float y)				{ x1=r.x1/x;	y1=r.y1/y;	x2=r.x2/x;	y2=r.y2/y; };

	__forceinline   bool	in (float  x, float  y)						{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2);	};
	__forceinline   bool	in (_fpoint &p)								{ return (p.x>=x1) && (p.x<=x2) && (p.y>=y1) && (p.y<=y2);	};
	__forceinline   bool	cmp(_frect &r)								{ return x1==r.x1 && y1==r.y1 && x2==r.x2 && y2==r.y2; };
} Frect;

typedef struct _fbox {
public:
	union{
		struct{
			Fvector	min;
			Fvector max;
		};
		struct{
			float x1, y1, z1;
			float x2, y2, z2;
		};
	};

	__forceinline	void	set(Fvector &_min, Fvector &_max)			{ min.set(_min);	max.set(_max);	};
	__forceinline   void	set(_fbox &b)								{ min.set(b.min);	max.set(b.max);	};
	__forceinline   void	null( )										{ min.set(0,0,0);	max.set(0,0,0);	};

	__forceinline   void	contract(float  s)							{ x1+=s;	y1+=s;	z1+=s;	x2-=s;	y2-=s;	z2-=s;	};
	__forceinline   void	expand(float  s)							{ x1-=s;	y1-=s;	z1-=s;	x2+=s;	y2+=s;	z2+=s;	};

	__forceinline   void	add(Fvector &p)								{ min.add(p); max.add(p);				};
	__forceinline   void	add(_fbox &b, Fvector &p)					{ min.add(b.min, p); max.add(b.max, p);	};

	__forceinline   bool	in (Fvector &p)								{ return (p.x>=x1) && (p.x<=x2) && (p.y>=y1) && (p.y<=y2) && (p.z>=z1) && (p.z<=z2); };
	__forceinline   bool	in (float x, float y, float z)				{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2) && (z>=z1) && (z<=z2); };
	__forceinline   bool	cmp(_fbox &b)								{ return x1==b.x1 && y1==b.y1 && z1==b.z1 && x2==b.x2 && y2==b.y2 && z2==b.z2; };
	__forceinline   bool	overlap (_fbox &b)							{ return in(b.min) || in(b.max) || b.in(min) || b.in(max); };
} Fbox;

typedef struct _quaternion {
	float x,y,z,w;

	__forceinline	void	RotationYawPitchRoll(const _vector &ypr) {
		float fSinYaw   = sinf(ypr.x/2);
		float fSinPitch = sinf(ypr.y/2);
		float fSinRoll  = sinf(ypr.z/2);
		float fCosYaw   = cosf(ypr.x/2);
		float fCosPitch = cosf(ypr.y/2);
		float fCosRoll  = cosf(ypr.z/2);

		x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
		y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
	}
	__forceinline	void	RotationYawPitchRoll(float x, float y, float z) {
		float fSinYaw   = sinf(x/2);
		float fSinPitch = sinf(y/2);
		float fSinRoll  = sinf(z/2);
		float fCosYaw   = cosf(x/2);
		float fCosPitch = cosf(y/2);
		float fCosRoll  = cosf(z/2);

		x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
		y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
	}
} Fquaternion;

extern _matrix precalc_identity;

typedef struct _matrix {
public:
	union {
		struct {						// Direct definition
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
		};

		float m[4][4];					// Array
	};

	// Class members
	__forceinline void set(_matrix &m) {
		CopyMemory(this,&m,16*sizeof(float));
	}
	__forceinline void identity(void) {
		CopyMemory(this,&precalc_identity,16*sizeof(float));
	}
	__forceinline void get_rapid(float R[3][3]){
		R[0][0]	=  m[0][0];	R[0][1]	=  m[0][1];	R[0][2]	= -m[0][2];
		R[1][0]	=  m[1][0];	R[1][1]	=  m[1][1];	R[1][2]	= -m[1][2];
		R[2][0]	= -m[2][0];	R[2][1]	= -m[2][1];	R[2][2]	=  m[2][2];
	}
	__forceinline void rotation(_quaternion &Q) {
		float xx = Q.x*Q.x; float yy = Q.y*Q.y; float zz = Q.z*Q.z;
		float xy = Q.x*Q.y; float xz = Q.x*Q.z; float yz = Q.y*Q.z;
		float wx = Q.w*Q.x; float wy = Q.w*Q.y; float wz = Q.w*Q.z;

		_11 = 1 - 2 * ( yy + zz );
		_12 =     2 * ( xy - wz );
		_13 =     2 * ( xz + wy );

		_21 =     2 * ( xy + wz );
		_22 = 1 - 2 * ( xx + zz );
		_23 =     2 * ( yz - wx );

		_31 =     2 * ( xz - wy );
		_32 =     2 * ( yz + wx );
		_33 = 1 - 2 * ( xx + yy );

		_14 = _24 = _34 = 0.0f;
		_41 = _42 = _43 = 0.0f;
		_44 = 1.0f;
	}

	__forceinline void mul(_matrix &b) {								// recomended to use second variation
		float* pA = (float*)this;
		float* pB = (float*)&b;
		float  pM[16];

		// i=0
		pM[4*0+0] = pA[4*0+0] * pB[4*0+0] + pA[4*1+0] * pB[4*0+1] + pA[4*2+0] * pB[4*0+2] + pA[4*3+0] * pB[4*0+3];
		pM[4*0+1] = pA[4*0+1] * pB[4*0+0] + pA[4*1+1] * pB[4*0+1] + pA[4*2+1] * pB[4*0+2] + pA[4*3+1] * pB[4*0+3];
		pM[4*0+2] = pA[4*0+2] * pB[4*0+0] + pA[4*1+2] * pB[4*0+1] + pA[4*2+2] * pB[4*0+2] + pA[4*3+2] * pB[4*0+3];
		pM[4*0+3] = pA[4*0+3] * pB[4*0+0] + pA[4*1+3] * pB[4*0+1] + pA[4*2+3] * pB[4*0+2] + pA[4*3+3] * pB[4*0+3];

		// i=1
		pM[4*1+0] = pA[4*0+0] * pB[4*1+0] + pA[4*1+0] * pB[4*1+1] + pA[4*2+0] * pB[4*1+2] + pA[4*3+0] * pB[4*1+3];
		pM[4*1+1] = pA[4*0+1] * pB[4*1+0] + pA[4*1+1] * pB[4*1+1] + pA[4*2+1] * pB[4*1+2] + pA[4*3+1] * pB[4*1+3];
		pM[4*1+2] = pA[4*0+2] * pB[4*1+0] + pA[4*1+2] * pB[4*1+1] + pA[4*2+2] * pB[4*1+2] + pA[4*3+2] * pB[4*1+3];
		pM[4*1+3] = pA[4*0+3] * pB[4*1+0] + pA[4*1+3] * pB[4*1+1] + pA[4*2+3] * pB[4*1+2] + pA[4*3+3] * pB[4*1+3];

		// i=2
		pM[4*2+0] = pA[4*0+0] * pB[4*2+0] + pA[4*1+0] * pB[4*2+1] + pA[4*2+0] * pB[4*2+2] + pA[4*3+0] * pB[4*2+3];
		pM[4*2+1] = pA[4*0+1] * pB[4*2+0] + pA[4*1+1] * pB[4*2+1] + pA[4*2+1] * pB[4*2+2] + pA[4*3+1] * pB[4*2+3];
		pM[4*2+2] = pA[4*0+2] * pB[4*2+0] + pA[4*1+2] * pB[4*2+1] + pA[4*2+2] * pB[4*2+2] + pA[4*3+2] * pB[4*2+3];
		pM[4*2+3] = pA[4*0+3] * pB[4*2+0] + pA[4*1+3] * pB[4*2+1] + pA[4*2+3] * pB[4*2+2] + pA[4*3+3] * pB[4*2+3];

		// i=3
		pM[4*3+0] = pA[4*0+0] * pB[4*3+0] + pA[4*1+0] * pB[4*3+1] + pA[4*2+0] * pB[4*3+2] + pA[4*3+0] * pB[4*3+3];
		pM[4*3+1] = pA[4*0+1] * pB[4*3+0] + pA[4*1+1] * pB[4*3+1] + pA[4*2+1] * pB[4*3+2] + pA[4*3+1] * pB[4*3+3];
		pM[4*3+2] = pA[4*0+2] * pB[4*3+0] + pA[4*1+2] * pB[4*3+1] + pA[4*2+2] * pB[4*3+2] + pA[4*3+2] * pB[4*3+3];
		pM[4*3+3] = pA[4*0+3] * pB[4*3+0] + pA[4*1+3] * pB[4*3+1] + pA[4*2+3] * pB[4*3+2] + pA[4*3+3] * pB[4*3+3];

		CopyMemory(this,pM,16*sizeof(float));
	}
	__forceinline void mul(_matrix &a,_matrix &b)					// faster than mul to self
	{
		float* pA = (float*)&a;
		float* pB = (float*)&b;
		float* pM = (float*)this;

		// i=0
		pM[4*0+0] = pA[4*0+0] * pB[4*0+0] + pA[4*1+0] * pB[4*0+1] + pA[4*2+0] * pB[4*0+2] + pA[4*3+0] * pB[4*0+3];
		pM[4*0+1] = pA[4*0+1] * pB[4*0+0] + pA[4*1+1] * pB[4*0+1] + pA[4*2+1] * pB[4*0+2] + pA[4*3+1] * pB[4*0+3];
		pM[4*0+2] = pA[4*0+2] * pB[4*0+0] + pA[4*1+2] * pB[4*0+1] + pA[4*2+2] * pB[4*0+2] + pA[4*3+2] * pB[4*0+3];
		pM[4*0+3] = pA[4*0+3] * pB[4*0+0] + pA[4*1+3] * pB[4*0+1] + pA[4*2+3] * pB[4*0+2] + pA[4*3+3] * pB[4*0+3];

		// i=1
		pM[4*1+0] = pA[4*0+0] * pB[4*1+0] + pA[4*1+0] * pB[4*1+1] + pA[4*2+0] * pB[4*1+2] + pA[4*3+0] * pB[4*1+3];
		pM[4*1+1] = pA[4*0+1] * pB[4*1+0] + pA[4*1+1] * pB[4*1+1] + pA[4*2+1] * pB[4*1+2] + pA[4*3+1] * pB[4*1+3];
		pM[4*1+2] = pA[4*0+2] * pB[4*1+0] + pA[4*1+2] * pB[4*1+1] + pA[4*2+2] * pB[4*1+2] + pA[4*3+2] * pB[4*1+3];
		pM[4*1+3] = pA[4*0+3] * pB[4*1+0] + pA[4*1+3] * pB[4*1+1] + pA[4*2+3] * pB[4*1+2] + pA[4*3+3] * pB[4*1+3];

		// i=2
		pM[4*2+0] = pA[4*0+0] * pB[4*2+0] + pA[4*1+0] * pB[4*2+1] + pA[4*2+0] * pB[4*2+2] + pA[4*3+0] * pB[4*2+3];
		pM[4*2+1] = pA[4*0+1] * pB[4*2+0] + pA[4*1+1] * pB[4*2+1] + pA[4*2+1] * pB[4*2+2] + pA[4*3+1] * pB[4*2+3];
		pM[4*2+2] = pA[4*0+2] * pB[4*2+0] + pA[4*1+2] * pB[4*2+1] + pA[4*2+2] * pB[4*2+2] + pA[4*3+2] * pB[4*2+3];
		pM[4*2+3] = pA[4*0+3] * pB[4*2+0] + pA[4*1+3] * pB[4*2+1] + pA[4*2+3] * pB[4*2+2] + pA[4*3+3] * pB[4*2+3];

		// i=3
		pM[4*3+0] = pA[4*0+0] * pB[4*3+0] + pA[4*1+0] * pB[4*3+1] + pA[4*2+0] * pB[4*3+2] + pA[4*3+0] * pB[4*3+3];
		pM[4*3+1] = pA[4*0+1] * pB[4*3+0] + pA[4*1+1] * pB[4*3+1] + pA[4*2+1] * pB[4*3+2] + pA[4*3+1] * pB[4*3+3];
		pM[4*3+2] = pA[4*0+2] * pB[4*3+0] + pA[4*1+2] * pB[4*3+1] + pA[4*2+2] * pB[4*3+2] + pA[4*3+2] * pB[4*3+3];
		pM[4*3+3] = pA[4*0+3] * pB[4*3+0] + pA[4*1+3] * pB[4*3+1] + pA[4*2+3] * pB[4*3+2] + pA[4*3+3] * pB[4*3+3];
	}

	__forceinline	void	invert(const _matrix & a ) {			// faster than self-invert
		identity();

		float fDetInv = ( a._11 * ( a._22 * a._33 - a._23 * a._32 ) -
			a._12 * ( a._21 * a._33 - a._23 * a._31 ) +
			a._13 * ( a._21 * a._32 - a._22 * a._31 ) );

		assert(fabsf(fDetInv)>.0001f);
		fDetInv=1.0f/fDetInv;

		_11 =  fDetInv * ( a._22 * a._33 - a._23 * a._32 );
		_12 = -fDetInv * ( a._12 * a._33 - a._13 * a._32 );
		_13 =  fDetInv * ( a._12 * a._23 - a._13 * a._22 );
		_14 = 0.0f;

		_21 = -fDetInv * ( a._21 * a._33 - a._23 * a._31 );
		_22 =  fDetInv * ( a._11 * a._33 - a._13 * a._31 );
		_23 = -fDetInv * ( a._11 * a._23 - a._13 * a._21 );
		_24 = 0.0f;

		_31 =  fDetInv * ( a._21 * a._32 - a._22 * a._31 );
		_32 = -fDetInv * ( a._11 * a._32 - a._12 * a._31 );
		_33 =  fDetInv * ( a._11 * a._22 - a._12 * a._21 );
		_34 = 0.0f;

		_41 = -( a._41 * _11 + a._42 * _21 + a._43 * _31 );
		_42 = -( a._41 * _12 + a._42 * _22 + a._43 * _32 );
		_43 = -( a._41 * _13 + a._42 * _23 + a._43 * _33 );
		_44 = 1.0f;
	}

	__forceinline	void	invert(void)				// slower than invert other matrix
	{
		_matrix a;
		CopyMemory(&a,this,16*sizeof(float));					// save matrix
		invert(a);
	}
	__forceinline	void	transpose(const _matrix &matSource)	// faster version of transpose
	{
		_11=matSource._11;	_12=matSource._21;	_13=matSource._31;	_14=matSource._41;
		_21=matSource._12;	_22=matSource._22;	_23=matSource._32;	_24=matSource._42;
		_31=matSource._13;	_32=matSource._23;	_33=matSource._33;	_34=matSource._43;
		_41=matSource._14;	_42=matSource._24;	_43=matSource._34;	_44=matSource._44;
	}
	__forceinline	void	transpose(void)						// self transpose - slower
	{
		_matrix a;
		CopyMemory(&a,this,16*sizeof(float));					// save matrix
		transpose(a);
	}
	__forceinline	void	translate(const _vector &Loc )		// setup translation matrix
	{
		identity();
		m[3][0] = Loc.x;
		m[3][1] = Loc.y;
		m[3][2] = Loc.z;
	}
	__forceinline	void	scale(const _vector &v )			// setup scale matrix
	{
		identity();
		m [0][0] = v.x;
		m [1][1] = v.y;
		m [2][2] = v.z;
	}
	__forceinline	void	rotateX (float Angle )
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m [1][1] 	= Cosine;
		m [2][1] 	= -Sine;
		m [1][2] 	= Sine;
		m [2][2] 	= Cosine;
	}
	__forceinline	void	rotateY (float Angle )				// rotation about Y axis
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m[0][0] 	= Cosine;
		m[2][0] 	= -Sine;
		m[0][2] 	= Sine;
		m[2][2] 	= Cosine;
	}
	__forceinline	void	rotateZ (float Angle )
	{
		identity();
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m [0][0] 	= Cosine;
		m [1][0] 	= -Sine;
		m [0][1] 	= Sine;
		m [1][1] 	= Cosine;
	}

	__forceinline	void	rotation( const _vector &vdir, const _vector &vnorm )
	{
		_vector vright;
		vright.crossproduct(vnorm,vdir);
		vright.normalize();
		_11 = vright.x;		_12 = vright.y;		_13 = vright.z;
		_21 = vnorm.x;		_22 = vnorm.y;		_23 = vnorm.z;
		_31 = vdir.x;		_32 = vdir.y;		_33 = vdir.z;
		_14 = _24 = _34 = 0; _44 = 1;
	}

	__forceinline	void	rotation ( const _vector &axis, float Angle )
	{
		float Cosine= cosf(Angle);
		float Sine	= sinf(Angle);
		m [0][0] 	= axis.x * axis.x + ( 1 - axis.x * axis.x) * Cosine;
		m [0][1] 	= axis.x * axis.y * ( 1 - Cosine ) + axis.z * Sine;
		m [0][2] 	= axis.x * axis.z * ( 1 - Cosine ) - axis.y * Sine;
		m [0][3] 	= 0;
		m [1][0] 	= axis.x * axis.y * ( 1 - Cosine ) - axis.z * Sine;
		m [1][1] 	= axis.y * axis.y + ( 1 - axis.y * axis.y) * Cosine;
		m [1][2] 	= axis.y * axis.z * ( 1 - Cosine ) + axis.x * Sine;
		m [1][3] 	= 0;
		m [2][0] 	= axis.x * axis.z * ( 1 - Cosine ) + axis.y * Sine;
		m [2][1] 	= axis.y * axis.z * ( 1 - Cosine ) - axis.x * Sine;
		m [2][2] 	= axis.z * axis.z + ( 1 - axis.z - axis.z) * Cosine;
		m [2][3] 	= 0; m [3][0] = 0; m [3][1] = 0;
		m [3][2] 	= 0; m [3][3] = 1;
	}

	__forceinline	void	mirrorX ()
	{
		identity();
		m[0][0] = -1;
	}

	__forceinline	void	mirrorY ()
	{
		identity();
		m [1][1] = -1;
	}

	__forceinline	void	mirrorZ ()
	{
		identity();
		m [2][2] = -1;
	}
	__forceinline	void	add( const _matrix &A, const _matrix &B)
	{
		m [0][0] = A.m [0][0] + B.m [0][0];
		m [0][1] = A.m [0][1] + B.m [0][1];
		m [0][2] = A.m [0][2] + B.m [0][2];
		m [0][3] = A.m [0][3] + B.m [0][3];

		m [1][0] = A.m [1][0] + B.m [1][0];
		m [1][1] = A.m [1][1] + B.m [1][1];
		m [1][2] = A.m [1][2] + B.m [1][2];
		m [1][3] = A.m [1][3] + B.m [1][3];

		m [2][0] = A.m [2][0] + B.m [2][0];
		m [2][1] = A.m [2][1] + B.m [2][1];
		m [2][2] = A.m [2][2] + B.m [2][2];
		m [2][3] = A.m [2][3] + B.m [2][3];

		m [3][0] = A.m [3][0] + B.m [3][0];
		m [3][1] = A.m [3][1] + B.m [3][1];
		m [3][2] = A.m [3][2] + B.m [3][2];
		m [3][3] = A.m [3][3] + B.m [3][3];

	}
	__forceinline	void	add( const _matrix &A)
	{
		m [0][0] += A.m [0][0];
		m [0][1] += A.m [0][1];
		m [0][2] += A.m [0][2];
		m [0][3] += A.m [0][3];

		m [1][0] += A.m [1][0];
		m [1][1] += A.m [1][1];
		m [1][2] += A.m [1][2];
		m [1][3] += A.m [1][3];

		m [2][0] += A.m [2][0];
		m [2][1] += A.m [2][1];
		m [2][2] += A.m [2][2];
		m [2][3] += A.m [2][3];

		m [3][0] += A.m [3][0];
		m [3][1] += A.m [3][1];
		m [3][2] += A.m [3][2];
		m [3][3] += A.m [3][3];
	}
	__forceinline	void	sub( const _matrix &A, const _matrix &B)
	{
		m [0][0] = A.m [0][0] - B.m [0][0];
		m [0][1] = A.m [0][1] - B.m [0][1];
		m [0][2] = A.m [0][2] - B.m [0][2];
		m [0][3] = A.m [0][3] - B.m [0][3];

		m [1][0] = A.m [1][0] - B.m [1][0];
		m [1][1] = A.m [1][1] - B.m [1][1];
		m [1][2] = A.m [1][2] - B.m [1][2];
		m [1][3] = A.m [1][3] - B.m [1][3];

		m [2][0] = A.m [2][0] - B.m [2][0];
		m [2][1] = A.m [2][1] - B.m [2][1];
		m [2][2] = A.m [2][2] - B.m [2][2];
		m [2][3] = A.m [2][3] - B.m [2][3];

		m [3][0] = A.m [3][0] - B.m [3][0];
		m [3][1] = A.m [3][1] - B.m [3][1];
		m [3][2] = A.m [3][2] - B.m [3][2];
		m [3][3] = A.m [3][3] - B.m [3][3];
	}
	__forceinline	void	sub( const _matrix &A)
	{
		m [0][0] -= A.m [0][0];
		m [0][1] -= A.m [0][1];
		m [0][2] -= A.m [0][2];
		m [0][3] -= A.m [0][3];

		m [1][0] -= A.m [1][0];
		m [1][1] -= A.m [1][1];
		m [1][2] -= A.m [1][2];
		m [1][3] -= A.m [1][3];

		m [2][0] -= A.m [2][0];
		m [2][1] -= A.m [2][1];
		m [2][2] -= A.m [2][2];
		m [2][3] -= A.m [2][3];

		m [3][0] -= A.m [3][0];
		m [3][1] -= A.m [3][1];
		m [3][2] -= A.m [3][2];
		m [3][3] -= A.m [3][3];
	}
	__forceinline	void	mul( const _matrix &A, float v )
	{
		m[0][0] = A.m [0][0] * v;
		m[0][1] = A.m [0][1] * v;
		m[0][2] = A.m [0][2] * v;
		m[0][3] = A.m [0][3] * v;

		m[1][0] = A.m [1][0] * v;
		m[1][1] = A.m [1][1] * v;
		m[1][2] = A.m [1][2] * v;
		m[1][3] = A.m [1][3] * v;

		m[2][0] = A.m [2][0] * v;
		m[2][1] = A.m [2][1] * v;
		m[2][2] = A.m [2][2] * v;
		m[2][3] = A.m [2][3] * v;

		m[3][0] = A.m [3][0] * v;
		m[3][1] = A.m [3][1] * v;
		m[3][2] = A.m [3][2] * v;
		m[3][3] = A.m [3][3] * v;
	}
	__forceinline	void	mul( float v )
	{
		m[0][0] *= v;
		m[0][1] *= v;
		m[0][2] *= v;
		m[0][3] *= v;

		m[1][0] *= v;
		m[1][1] *= v;
		m[1][2] *= v;
		m[1][3] *= v;

		m[2][0] *= v;
		m[2][1] *= v;
		m[2][2] *= v;
		m[2][3] *= v;

		m[3][0] *= v;
		m[3][1] *= v;
		m[3][2] *= v;
		m[3][3] *= v;
	}
	__forceinline	void	div( const _matrix &A, float v ) {
		assert(fabsf(v)>0.000001f);
		mul(A,1.0f/v);
	};
	__forceinline	void	div( float v ) {
		assert(fabsf(v)>0.000001f);
		mul(1.0f/v);
	};
	__forceinline	void	build_projection(float fFOV, float fAspect, float fNearPlane, float fFarPlane) {
		assert( fabsf(fFarPlane-fNearPlane) > 0.01f );
		assert( fabsf(sinf(fFOV/2)) > 0.01f );

		fFOV/=2.0f;
		float cot	= cosf(fFOV)/sinf(fFOV);
		float w		= fAspect * cot;
		float h		= 1.0f    * cot;
		float Q		= fFarPlane / ( fFarPlane - fNearPlane );

		ZeroMemory( this, sizeof(_matrix) );
		_11 = w;
		_22 = h;
		_33 = Q;
		_34 = 1.0f;
		_43 = -Q*fNearPlane;
	};
	__forceinline	void	build_camera(Fvector &vFrom, Fvector &vAt, Fvector &vWorldUp) {
//		D3DXMatrixLookAtLH((D3DXMATRIX *)this,(D3DXVECTOR3 *)&vEye,(D3DXVECTOR3 *)&vAt,(D3DXVECTOR3 *)&vUp);

		// Get the z basis vector, which points straight ahead. This is the
		// difference from the eyepoint to the lookat point.
		Fvector vView;
		vView.sub(vAt,vFrom);
		vView.normalize();

		// Get the dot product, and calculate the projection of the z basis
		// vector onto the up vector. The projection is the y basis vector.
		FLOAT fDotProduct = vWorldUp.dotproduct( vView );

		Fvector vUp;
		vUp.mul(vView, -fDotProduct);
		vUp.add(vWorldUp);
		vUp.normalize();

		// The x basis vector is found simply with the cross product of the y
		// and z basis vectors
		Fvector vRight;
		vRight.crossproduct( vUp, vView );

		// Start building the Device.mView. The first three rows contains the basis
		// vectors used to rotate the view to point at the lookat point
		_11 = vRight.x;  _12 = vUp.x;  _13 = vView.x;  _14 = 0.0f;
		_21 = vRight.y;  _22 = vUp.y;  _23 = vView.y;  _24 = 0.0f;
		_31 = vRight.z;  _32 = vUp.z;  _33 = vView.z;  _34 = 0.0f;

		// Do the translation values (rotations are still about the eyepoint)
		_41 = - vFrom.dotproduct(vRight);
		_42 = - vFrom.dotproduct( vUp  );
		_43 = - vFrom.dotproduct(vView );
		_44 = 1.0f;
	};
	__forceinline	void	interpolate(_matrix &mat, float v)
	{
		float iv = 1.f-v;
		for (int i=0; i<4; i++)
		{
			m[i][0] = m[i][0]*v + mat.m[i][0]*iv;
			m[i][1] = m[i][1]*v + mat.m[i][1]*iv;
			m[i][2] = m[i][2]*v + mat.m[i][2]*iv;
			m[i][3] = m[i][3]*v + mat.m[i][3]*iv;
		}
	}
} Fmatrix;

struct FCvertex {
	Fvector p;
	DWORD	c;
};

/*
 * Untransformed/unlit vertices
 */

typedef struct _vertex {
public:
	union {
		union {
			struct {
				float x, y, z;
				float nx,ny,nz;
				float tu,tv;
			};
			struct {
				Fvector p;
				Fvector n;
				float tu,tv;
			};
		};
	};

	///////////////////
	// Class members //
	///////////////////
	__forceinline void set(_vector &pp, _vector &nn, float _tu, float _tv)
	{
		p.set(pp);
		n.set(nn);
		tu=_tu;
		tv=_tv;
	};
	__forceinline void set(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _tu, float _tv)
	{
		x=_x;	y=_y;	z=_z;
		nx=_nx;	ny=_ny;	nz=_nz;
		tu=_tu;	tv=_tv;
	};
	__forceinline void set(_vertex &v)
	{
		CopyMemory(this, &v, sizeof(_vertex));
	};
	__forceinline void transform(const _matrix &M)					// slower
	{
		p.transform_tiny(M);
		n.transform_dir (M);
	};
	__forceinline void transform(const _vertex &v,const _matrix &M)	// faster
	{
		p.transform_tiny(v.p,M);
		n.transform_dir (v.n,M);
		tu=v.tu;
		tv=v.tv;
	};
} Fvertex;

typedef struct _LITvertex {
public:
	union {
		union {
			struct {
				float	x, y, z;
				float	nx,ny,nz;
				FPcolor color;
				float	tu,tv;
			};
			struct {
				Fvector p;
				Fvector n;
				FPcolor color;
				float tu,tv;
			};
		};
	};

	///////////////////
	// Class members //
	///////////////////
	__forceinline void set(_vector &pp, _vector &nn, FPcolor c, float _tu, float _tv)
	{
		p.set(pp);
		n.set(nn);
		color=c;
		tu=_tu;
		tv=_tv;
	};
	__forceinline void set(float _x, float _y, float _z, float _nx, float _ny, float _nz, FPcolor c, float _tu, float _tv)
	{
		x=_x;	y=_y;	z=_z;
		nx=_nx;	ny=_ny;	nz=_nz;
		color=c;tu=_tu;	tv=_tv;
	};
	__forceinline void set(_LITvertex &v)
	{
		CopyMemory(this, &v, sizeof(_LITvertex));
	};
	__forceinline void transform(const _matrix &M)					// slower
	{
		p.transform_tiny(M);
		n.transform_dir (M);
	};
	__forceinline void transform(const _LITvertex &v,const _matrix &M)	// faster
	{
		p.transform_tiny(v.p,M);
		n.transform_dir (v.n,M);
		color=v.color;
		tu=v.tu;
		tv=v.tv;
	};
} FLITvertex;

typedef struct _Lvertex {
public:
	union {
		struct {
			float	x, y, z;
			DWORD   reserved;
			FPcolor color;
			FPcolor specular;
			float	tu,tv;
		};
		struct {
			Fvector p;
			DWORD	reserved;
			FPcolor color;
			FPcolor specular;
			float	tu,tv;
		};
	};

	///////////////////
	// Class members //
	///////////////////
	__forceinline void set(_vector &pp, FPcolor c, FPcolor s, float _tu, float _tv)
	{
		p.set(pp);
		color=c;
		specular=s;
		tu=_tu;
		tv=_tv;
	};
	__forceinline void set(float _x, float _y, float _z, FPcolor c, FPcolor s, float _tu, float _tv)
	{
		x=_x;	y=_y;	z=_z;
		color=c; specular=s;
		tu=_tu;	tv=_tv;
	};
	__forceinline void set(_Lvertex &v)
	{
		CopyMemory(this, &v, sizeof(_Lvertex));
	};
	__forceinline void transform(const _matrix &M)					// faster
	{
		p.transform_tiny(M);
	};
	__forceinline void transform(const _Lvertex &v,const _matrix &M)	// slower
	{
		p.transform_tiny(v.p,M);
		color=v.color;
		specular=v.specular;
		tu=v.tu;
		tv=v.tv;
	};
} FLvertex;

typedef struct _TLvertex {
public:
	union {
		struct {
			float	sx, sy, sz, rhw;
			FPcolor color;
			FPcolor specular;
			float	tu,tv;
		};
		struct {
			Fvector p;
			float	rhw;
			FPcolor color;
			FPcolor specular;
			float	tu,tv;
		};
	};

	///////////////////
	// Class members //
	///////////////////
	__forceinline void set(_vector &pp, float _rhw, FPcolor c, FPcolor s, float _tu, float _tv)
	{
		p.set(pp);
		rhw=_rhw;
		color=c;
		specular=s;
		tu=_tu;
		tv=_tv;
	};
	__forceinline void set(float _sx, float _sy, float _sz, float _rhw, FPcolor c, FPcolor s, float _tu, float _tv)
	{
		sx=_sx;	sy=_sy;	sz=_sz;
		rhw=_rhw;
		color=c; specular=s;
		tu=_tu;	tv=_tv;
	};
	__forceinline void set(_TLvertex &v)
	{
		CopyMemory(this, &v, sizeof(_TLvertex));
	};
	__forceinline void s_transform(const _vector &v,const _matrix &matSet)
	{
		// Transform it through the current matrix set
		FLOAT xp = matSet._11*v.x + matSet._21*v.y + matSet._31*v.z + matSet._41;
		FLOAT yp = matSet._12*v.x + matSet._22*v.y + matSet._32*v.z + matSet._42;
		FLOAT zp = matSet._13*v.x + matSet._23*v.y + matSet._33*v.z + matSet._43;
		FLOAT wp = matSet._14*v.x + matSet._24*v.y + matSet._34*v.z + matSet._44;

		// Finally, scale the vertices to screen coords. This step first
		// Note 1: device coords range from -1 to +1 in the viewport.
		// Note 2: the sz-coordinate will be used in the z-buffer.
		sx  = xp/wp;
		sy  = -yp/wp;
		sz  = zp/wp;
		rhw = wp;
	};
} FTLvertex;

#define TO_REAL(_X_, _S_)	((_X_)+1.f)*float(_S_/2)

typedef struct _TLpolygon {
public:
	union{
		struct{
			_TLvertex 	lb, lt, rb, rt;
		};
		_TLvertex		m[4];
	};

	__forceinline	void	setpos	(Frect &r)								{ lb.sx=r.x1;	lb.sy=r.y2;	lt.sx=r.x1;	lt.sy=r.y1;	rb.sx=r.x2;	rb.sy=r.y2;	rt.sx=r.x2;	rt.sy=r.y1;}
	__forceinline	void	setpos	(float x1, float y1, float x2, float y2){ lb.sx=x1;		lb.sy=y2;	lt.sx=x1;	lt.sy=y1;	rb.sx=x2;	rb.sy=y2;	rt.sx=x2;	rt.sy=y1;}
	__forceinline	void	setdepth(float z = 0.f, float rhw = 1.f)		{ lb.sz=z;		rb.sz= z;	lt.sz = z;	rt.sz = z;	lb.rhw = rhw; rb.rhw = rhw; lt.rhw = rhw; rt.rhw = rhw;};
	__forceinline	void	setcolor(FPcolor c, FPcolor s)					{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c);   lb.specular.set(c); rb.specular.set(c); lt.specular.set(c); rt.specular.set(c);};
	__forceinline	void	setcolor(DWORD c, DWORD s)						{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c);   lb.specular.set(c); rb.specular.set(c); lt.specular.set(c); rt.specular.set(c);};
	__forceinline	void	setcolor(DWORD c)								{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c); };
	__forceinline	void	setcolor(FPcolor c)								{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c); };
	__forceinline	void	settex	(Frect &r)								{ lb.tu = r.x1; lb.tv = r.y2; lt.tu = r.x1; lt.tv = r.y1; rb.tu = r.x2; rb.tv = r.y2;	rt.tu = r.x2; rt.tv = r.y1; }
	__forceinline	void	settex	(float x1, float y1, float x2, float y2){ lb.tu=x1;		lb.tv=y2;	lt.tu=x1;	lt.tv=y1;	rb.tu=x2;	rb.tv=y2;	rt.tu=x2;	rt.tv=y1;}

	__forceinline	void	to_real	(int w, int h)
	{
		lt.sx=lb.sx=TO_REAL(lb.sx, w);	rb.sx=rt.sx=TO_REAL(rb.sx, w);
		lt.sy=rt.sy=TO_REAL(lt.sy, h);	lb.sy=rb.sy=TO_REAL(lb.sy, h);
	}

	__forceinline	void	to_real	(Frect &r, int w, int h)
	{
		lt.sx=lb.sx=TO_REAL(r.x1, w);	lt.sy=rt.sy=TO_REAL(r.y1, h);
		rb.sx=rt.sx=TO_REAL(r.x2, w);	lb.sy=rb.sy=TO_REAL(r.y2, h);
	}

	__forceinline	void	add	(float dx, float dy)
	{
		lb.sx+=dx;	lb.sy+=dy;	lt.sx+=dx;	lt.sy+=dy;
		rb.sx+=dx;	rb.sy+=dy;	rt.sx+=dx;	rt.sy+=dy;
	}

	__forceinline	void	add	(Frect &r, float dx, float dy)
	{
		lb.sx=r.x1+dx;	lb.sy=r.y2+dy;	lt.sx=r.x1+dx;	lt.sy=r.y1+dy;
		rb.sx=r.x2+dx;	rb.sy=r.y2+dy;	rt.sx=r.x2+dx;	rt.sy=r.y1+dy;
	}

	__forceinline	void	sub	(float dx, float dy)
	{
		lb.sx-=dx;	lb.sy-=dy;	lt.sx-=dx;	lt.sy-=dy;
		rb.sx-=dx;	rb.sy-=dy;	rt.sx-=dx;	rt.sy-=dy;
	}

	__forceinline	void	sub	(Frect &r, float dx, float dy)
	{
		lb.sx=r.x1-dx;	lb.sy=r.y2-dy;	lt.sx=r.x1-dx;	lt.sy=r.y1-dy;
		rb.sx=r.x2-dx;	rb.sy=r.y2-dy;	rt.sx=r.x2-dx;	rt.sy=r.y1-dy;
	}
} FTLpolygon;

typedef struct _Lpolygon {
public:
	union{
		struct{
			_Lvertex 	lb, lt, rb, rt;
		};
		_Lvertex		m[4];
	};

	__forceinline	void	setcolor(FPcolor c, FPcolor s)					{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c);   lb.specular.set(c); rb.specular.set(c); lt.specular.set(c); rt.specular.set(c);};
	__forceinline	void	setcolor(DWORD c, DWORD s)						{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c);   lb.specular.set(c); rb.specular.set(c); lt.specular.set(c); rt.specular.set(c);};
	__forceinline	void	setcolor(DWORD c)								{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c); };
	__forceinline	void	setcolor(FPcolor c)								{ lb.color.set(c); rb.color.set(c); lt.color.set(c); rt.color.set(c); };
	__forceinline	void	settex	(Frect &r)								{ lb.tu = r.x1; lb.tv = r.y2; lt.tu = r.x1; lt.tv = r.y1; rb.tu = r.x2; rb.tv = r.y2;	rt.tu = r.x2; rt.tv = r.y1; }
	__forceinline	void	settex	(float x1, float y1, float x2, float y2){ lb.tu=x1;		lb.tv=y2;	lt.tu=x1;	lt.tv=y1;	rb.tu=x2;	rb.tv=y2;	rt.tu=x2;	rt.tv=y1;}

} FLpolygon;

typedef struct _light {
public:
    DWORD           type;             /* Type of light source */
    Fcolor          diffuse;          /* Diffuse color of light */
    Fcolor          specular;         /* Specular color of light */
    Fcolor          ambient;          /* Ambient color of light */
    Fvector         position;         /* Position in world space */
    Fvector         direction;        /* Direction in world space */
    float		    range;            /* Cutoff range */
    float	        falloff;          /* Falloff */
    float	        attenuation0;     /* Constant attenuation */
    float	        attenuation1;     /* Linear attenuation */
    float	        attenuation2;     /* Quadratic attenuation */
    float	        theta;            /* Inner angle of spotlight cone */
    float	        phi;              /* Outer angle of spotlight cone */

	__forceinline	_light() {
	}
	__forceinline	void	set(DWORD ltType, float x, float y, float z) {
		ZeroMemory( this, sizeof(_light) );
		type=ltType;
		diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
		specular.set(diffuse);
		position.set(x,y,z);
		direction.set(x,y,z);
		direction.normalize();
		range= 1000.f;
	}
} Flight;

typedef struct _material {
public:
    Fcolor			diffuse;        /* Diffuse color RGBA */
    Fcolor			ambient;        /* Ambient color RGB */
    Fcolor		    specular;       /* Specular 'shininess' */
    Fcolor			emissive;       /* Emissive color RGB */
    float			power;          /* Sharpness if specular highlight */

	__forceinline	void	Set(float r, float g, float b)
	{
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = r;
		diffuse.g = ambient.g = g;
		diffuse.b = ambient.b = b;
		diffuse.a = ambient.a = 1.0f;
		power	  = 0;
	}
	__forceinline	void	Set(float r, float g, float b, float a)
	{
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = r;
		diffuse.g = ambient.g = g;
		diffuse.b = ambient.b = b;
		diffuse.a = ambient.a = a;
		power	  = 0;
	}
	__forceinline	void	Set(Fcolor &c)
	{
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = c.r;
		diffuse.g = ambient.g = c.g;
		diffuse.b = ambient.b = c.b;
		diffuse.a = ambient.a = c.a;
		power	  = 0;
	}
} Fmaterial;

/*
typedef struct _3dbuffer{
public:
    DWORD			dwSize;
    Fvector			vPosition;
    Fvector			vVelocity;
    DWORD			dwInsideConeAngle;
    DWORD			dwOutsideConeAngle;
    Fvector			vConeOrientation;
    int				lConeOutsideVolume;
    float			flMinDistance;
    float			flMaxDistance;
    DWORD			dwMode;

	__forceinline	void	Init( )
	{
		dwSize				= sizeof(DS3DBUFFER);
		vPosition.set		( 0.0f, 0.0f, 0.0f );
		vVelocity.set		( 0.0f, 0.0f, 0.0f );
		vConeOrientation.set( 0.0f, 0.0f, 1.0f );
		flMinDistance		= DS3D_DEFAULTMINDISTANCE;
		flMaxDistance		= DS3D_DEFAULTMAXDISTANCE;
		dwInsideConeAngle	= DS3D_DEFAULTCONEANGLE;
		dwOutsideConeAngle	= DS3D_DEFAULTCONEANGLE;
		lConeOutsideVolume	= DS3D_DEFAULTCONEOUTSIDEVOLUME;
		dwMode				= DS3DMODE_NORMAL;
	}
	__forceinline	void	set( _3dbuffer b )
	{
		dwSize				= b.dwSize;
		vPosition.set		( b.vPosition );
		vVelocity.set		( b.vVelocity);
		vConeOrientation.set( b.vConeOrientation );
		flMinDistance		= b.flMinDistance;
		flMaxDistance		= b.flMaxDistance;
		dwInsideConeAngle	= b.dwInsideConeAngle;
		dwOutsideConeAngle	= b.dwOutsideConeAngle;
		lConeOutsideVolume	= b.lConeOutsideVolume;
		dwMode				= b.dwMode;
	}
}F3dbuffer;

typedef struct _3dlistener{
public:
    DWORD			dwSize;
    Fvector			vPosition;
    Fvector			vVelocity;
    Fvector			vOrientFront;
    Fvector			vOrientTop;
    float			flDistanceFactor;
    float			flRolloffFactor;
    float			flDopplerFactor;

	__forceinline	void	Init( )
	{
		dwSize				= sizeof(DS3DLISTENER);
		vPosition.set		( 0.0f, 0.0f, 0.0f );
		vVelocity.set		( 0.0f, 0.0f, 0.0f );
		vOrientFront.set	( 0.0f, 0.0f, 1.0f );
		vOrientTop.set		( 0.0f, 1.0f, 0.0f );
		flDistanceFactor	= 1.0f;
		flRolloffFactor		= DS3D_DEFAULTROLLOFFFACTOR;
		flDopplerFactor		= DS3D_DEFAULTDOPPLERFACTOR;
	}
}F3dlistener;
*/
typedef struct _lminmax {
public:
	union{
		struct{
			int 	min, max;
		};
		int 		f[2];
	};

	__forceinline	void	set(int  _min, int  _max)	{ min = _min;	max = _max; };
	__forceinline	void	set(_lminmax p)				{ min = p.min;	max = p.max; };
	__forceinline   void	null( )						{ min = 0;		max = 0;  };
	__forceinline   bool	isnull( )					{ return ((!min)&&(!max));  };

	__forceinline   void	clamp( int& x )				{ if(x>max) x=max; else if (x<min) x=min; };

} Lminmax;

typedef struct _fminmax {
public:
	union{
		struct{
			float 	min, max;
		};
		float		f[2];
	};

	__forceinline	void	set(float  _min, float  _max)	{ min = _min;	max = _max; };
	__forceinline	void	set(_fminmax p)					{ min = p.min;	max = p.max; };
	__forceinline   void	null( )							{ min = 0;		max = 0;  };
	__forceinline   bool	isnull( )						{ return ((!min)&&(!max));  };

	__forceinline   void	clamp( float& x )				{ if(x>max) x=max; else if (x<min) x=min; };

} Fminmax;

#pragma pack(pop)

__forceinline	void	Fvector::transform_tiny(const _matrix &mat)
{
	float xx = (x*mat._11 + y*mat._21 + z* mat._31 + mat._41);
	float yy = (x*mat._12 + y*mat._22 + z* mat._32 + mat._42);
	float zz = (x*mat._13 + y*mat._23 + z* mat._33 + mat._43);
	x=xx;
	y=yy;
	z=zz;
}
__forceinline	void	Fvector::transform(const _matrix &mat) {
	float w = x*mat._14 + y*mat._24 + z* mat._34 + mat._44;
	assert(fabsf(w)>0.000001f);
	float xx = (x*mat._11 + y*mat._21 + z* mat._31 + mat._41)/w;
	float yy = (x*mat._12 + y*mat._22 + z* mat._32 + mat._42)/w;
	float zz = (x*mat._13 + y*mat._23 + z* mat._33 + mat._43)/w;
	x=xx;
	y=yy;
	z=zz;
}
__forceinline	void	Fvector::transform_dir(const _matrix &mat)
{
	float xx = x*mat._11 + y*mat._21 + z* mat._31;
	float yy = x*mat._12 + y*mat._22 + z* mat._32;
	float zz = x*mat._13 + y*mat._23 + z* mat._33;
	x=xx;
	y=yy;
	z=zz;
}
__forceinline	void	Fvector::transform_tiny(const _vector &v, const _matrix &mat)
{
	x = (v.x*mat._11 + v.y*mat._21 + v.z* mat._31 + mat._41);
	y = (v.x*mat._12 + v.y*mat._22 + v.z* mat._32 + mat._42);
	z = (v.x*mat._13 + v.y*mat._23 + v.z* mat._33 + mat._43);
}
__forceinline	void	Fvector::transform(const _vector &v,const _matrix &mat) {
	float w = v.x*mat._14 + v.y*mat._24 + v.z*mat._34 + mat._44;
	assert(fabsf(w)>0.000001f);
	x = (v.x*mat._11 + v.y*mat._21 + v.z* mat._31 + mat._41)/w;
	y = (v.x*mat._12 + v.y*mat._22 + v.z* mat._32 + mat._42)/w;
	z = (v.x*mat._13 + v.y*mat._23 + v.z* mat._33 + mat._43)/w;
}
__forceinline	void	Fvector::transform_dir(const _vector &v,const _matrix &mat)
{
	x = v.x*mat._11 + v.y*mat._21 + v.z*mat._31;
	y = v.x*mat._12 + v.y*mat._22 + v.z*mat._32;
	z = v.x*mat._13 + v.y*mat._23 + v.z*mat._33;
}
__forceinline	void	Fvector::position_of_camera(const _matrix &m)
{
    z = (-(m._41*(m._11*(m._12*m._23-m._13*m._22)+m._12*m._13*(m._21-1))-m._11*(m._42*(m._11*m._23-m._13)+m._43*(m._12*m._21-m._11*m._22)))/(m._11*m._11*(m._22*m._33-m._23*m._32)-m._11*(m._12*(m._21*m._33-m._23*m._31)+m._13*(m._22*m._31-m._32))+m._12*m._13*m._31*(m._21-1)));
    y = -(z*(m._11*m._32-m._12*m._31)+m._42*m._11-m._41*m._12)/(m._11*m._22-m._12*m._21);
    x = (-m._41 -y*m._21-z*m._31)/m._11;
}

#endif

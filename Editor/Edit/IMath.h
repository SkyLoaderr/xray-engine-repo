// file: IMath.h

#ifndef _INCDEF_IMATH_H_
#define _INCDEF_IMATH_H_

//-------------------------------------------

#ifndef M_PI
    #define M_PI 3.14159f
#endif
#define DEFAULT_COMPARE_PRECISION 0.0001f
#define FACETEST_PRECISION 0.001f

//-------------------------------------------

__inline int is_positive( float val ){
    return !((*(unsigned long*)&val) & 0x10000000);
}

__inline void clamp( float& val, float _low, float _high ){
    if( val<_low ) val = _low; else if( val>_high ) val = _high;
};

__inline void clamp( int& val, int _low, int _high ){
    if( val<_low ) val = _low; else if( val>_high ) val = _high;
};

__inline void clamp( long& val, long _low, long _high ){
    if( val<_low ) val = _low; else if( val>_high ) val = _high;
};

__inline void clamp_i( int& val, int _low, int _high ){
    if( val<_low ) val = _low; else if( val>=_high ) val = _high-1;
};

__inline float line_attack( float _min, float _max, float _end, float _T ){
	if( _T <= _min ) return (_T / _end);
	if( _T >= _max ) return (1.f-(_T / _end));
	return 1.f;
};

__inline bool cmpflt( float v1, float v2, float precision = 0.01f ){
	return (abs(v1-v2)<=precision);
};


__inline float rndfltn(){
	return (((rand() / (float)RAND_MAX) * 2.f) - 1.f );
};

//-------------------------------------------



#pragma pack(push)
#pragma pack(1)


/*
class IColor {
public:
    union{
    	struct { unsigned char b,g,r,a; };
  	unsigned char bytes[4];
    	DWORD value; };

    __inline void set( unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 0xff ){
    	r = _r; g = _g; b = _b; a = _a; };
    __inline void set( IColor& c ){
    	value = c.value; };
    __inline void set( const IColor& c ){
    	value = c.value; };
    __inline float gray(){
    	return ((r * 0.35f + g * 0.5f + b * 0.15f) / 255.f); };
    __inline void scale( float k ){
		int _r = (int)( r * k );
		int _g = (int)( g * k );
		int _b = (int)( b * k );
		int _a = (int)( a * k );
		clamp( _r, 0, 255 );
		clamp( _g, 0, 255 );
		clamp( _b, 0, 255 );
		clamp( _a, 0, 255 );
		r = _r;
		g = _g;
		b = _b;
		a = _a; };
    __inline void fillfloat4( float *dst ){
    	dst[0] = (float)r / 255.f;
    	dst[1] = (float)g / 255.f;
    	dst[2] = (float)b / 255.f;
    	dst[3] = (float)a / 255.f; };
	__inline COLORREF colorref(){
  		return RGB(r,g,b); };
};


class IFColor{
public:
	union{
  		struct{ float r,g,b,a; };
  		float val[4]; };

    __inline void set( float _r, float _g, float _b, float _a = 1.0f ){
    	r = _r; g = _g; b = _b; a = _a; };
    __inline void set( IFColor &c ){
    	r = c.r; g = c.g; b = c.b; a = c.a; };
    __inline void set( const IFColor &c ){
    	r = c.r; g = c.g; b = c.b; a = c.a; };
    __inline void set( const int &c ){
    	b = (float)RGBA_GETRED(c)/255.f;
        g = (float)RGBA_GETGREEN(c)/255.f;
        r = (float)RGBA_GETBLUE(c)/255.f;
        a = (float)RGBA_GETALPHA(c)/255.f; };
    __inline void filldword( IColor& _color){
    	_color.r = (unsigned char)( r * 255.f );
    	_color.g = (unsigned char)( g * 255.f );
    	_color.b = (unsigned char)( b * 255.f );
    	_color.a = (unsigned char)( a * 255.f ); }
	__inline void fillfloat4( float *_color){
  		_color[0] = r; _color[1] = g; _color[2] = b; _color[3] = a; }
	__inline COLORREF colorref(){
		int _r = (int)( r * 255.f );
		int _g = (int)( g * 255.f );
		int _b = (int)( b * 255.f );
  		return RGB(_r,_g,_b); };
	__inline bool cmp( IFColor& to, float precision = 0.01f ){
		return (
			cmpflt( r, to.r, precision ) &&
			cmpflt( g, to.g, precision ) &&
			cmpflt( b, to.b, precision ) &&
			cmpflt( a, to.a, precision )
		); };
};


class IVector {
public:
    union{
    	float a[3];
    	struct { float x,y,z; }; };

    __inline void set( IVector& v ){
    	x = v.x; y = v.y; z = v.z; };
    __inline void set( const IVector& v ){
    	x = v.x; y = v.y; z = v.z; };
    __inline void set( float _x, float _y, float _z ){
    	x = _x; y = _y; z = _z; };

    __inline void inverse( IVector& v ){
    	x = -v.x; y = -v.y; z = -v.z; };

    __inline bool cmp( IVector& v, float prec = DEFAULT_COMPARE_PRECISION ){
    	if( abs( x-v.x ) > prec ) return false;
    	if( abs( y-v.y ) > prec ) return false;
    	if( abs( z-v.z ) > prec ) return false;
    	return true; };

    __inline void swap( IVector& v ){
    	IVector tmp;
    	tmp.set( v );
    	v.set( (*this) );
    	set( tmp ); };

    __inline void add( IVector& v1, IVector& v2 ){
    	x = v1.x + v2.x; y = v1.y + v2.y; z = v1.z + v2.z; };
    __inline void add( IVector& v ){
    	x += v.x; y += v.y; z += v.z; };
    __inline void add( IVector& v1, float v ){
    	x = v1.x + v; y = v1.y + v; z = v1.z + v; };
    __inline void add( float v ){
    	x += v; y += v; z += v; };

    __inline void sub( IVector& v1, IVector& v2 ){
    	x = v1.x - v2.x; y = v1.y - v2.y; z = v1.z - v2.z; };
    __inline void sub( IVector& v ){
    	x -= v.x; y -= v.y; z -= v.z; };
    __inline void sub( IVector& v1, float v ){
    	x = v1.x - v; y = v1.y - v; z = v1.z - v; };
    __inline void sub( float v ){
    	x -= v; y -= v; z -= v; };

    __inline void mul( float scal ){
    	x *= scal; y *= scal; z *= scal; };
    __inline void mul( IVector& v, float scal ){
    	x = v.x * scal; y = v.y * scal; z = v.z * scal; };

    __inline float dot( IVector& v ){
    	return x*v.x + y*v.y + z*v.z; };
    __inline void cross( IVector& v1, IVector& v2 ){
    	x = v1.y*v2.z - v1.z*v2.y;
    	y = v1.z*v2.x - v1.x*v2.z;
    	z = v1.x*v2.y - v1.y*v2.x; };

	__inline void translate( IVector& v, float scal ){
  		x += v.x * scal; y += v.y * scal; z += v.z * scal; };

    __inline float lenght(){
    	return sqrt(x*x+y*y+z*z); };

    __inline void normalize(){
    	float l = lenght();
    	_ASSERTE( !is_zero(l) );
    	x /= l;
    	y /= l;
    	z /= l; };

    __inline void safe_normalize(){
    	float l = lenght();
    	if( is_zero(l) ){
    		set(0,0,0);
    		return; }
    	x /= l;
    	y /= l;
    	z /= l; };

    __inline void mknormal( IVector& p0, IVector& p1, IVector& p2 ){
    	IVector v01,v12;
    	v01.sub( p1, p0 );
    	v12.sub( p2, p1 );
    	cross( v01, v12 );
    	safe_normalize(); };
};



class IIntVector {
public:
	union{
  		int a[3];
  		struct { int x,y,z; }; };

	__inline void set( IIntVector& v ){
  		x = v.x; y = v.y; z = v.z; };
	__inline void set( const IIntVector& v ){
  		x = v.x; y = v.y; z = v.z; };
	__inline void set( int _x, int _y, int _z ){
  		x = _x; y = _y; z = _z; };

	__inline void inverse( IIntVector& v ){
  		x = -v.x; y = -v.y; z = -v.z; };

	__inline bool cmp( IIntVector& v ){
  		if( x!=v.x ) return false;
  		if( y!=v.y ) return false;
  		if( z!=v.z ) return false;
  		return true; };

	__inline void swap( IIntVector& v ){
  		IIntVector tmp;
  		tmp.set( v );
  		v.set( (*this) );
  		set( tmp ); };

	__inline void add( IIntVector& v1, IIntVector& v2 ){
  		x = v1.x + v2.x; y = v1.y + v2.y; z = v1.z + v2.z; };
	__inline void add( IIntVector& v ){
  		x += v.x; y += v.y; z += v.z; };
	__inline void add( IIntVector& v1, int v ){
  		x = v1.x + v; y = v1.y + v; z = v1.z + v; };
	__inline void add( int v ){
  		x += v; y += v; z += v; };

	__inline void sub( IIntVector& v1, IIntVector& v2 ){
  		x = v1.x - v2.x; y = v1.y - v2.y; z = v1.z - v2.z; };
	__inline void sub( IIntVector& v ){
  		x -= v.x; y -= v.y; z -= v.z; };
	__inline void sub( IIntVector& v1, int v ){
  		x = v1.x - v; y = v1.y - v; z = v1.z - v; };
	__inline void sub( int v ){
  		x -= v; y -= v; z -= v; };

	__inline void mul( int scal ){
  		x *= scal; y *= scal; z *= scal; };
	__inline void mul( IIntVector& v, int scal ){
 		x = v.x * scal; y = v.y * scal; z = v.z * scal; };

	__inline float dot( IIntVector& v ){
  		return x*v.x + y*v.y + z*v.z; };
	__inline void cross( IIntVector& v1, IIntVector& v2 ){
  		x = v1.y*v2.z - v1.z*v2.y;
  		y = v1.z*v2.x - v1.x*v2.z;
  		z = v1.x*v2.y - v1.y*v2.x; };

	__inline float lenght(){
  		return sqrt(x*x+y*y+z*z); };
};



class IMatrix {
public:
    union {
    	float a[16];
    	struct{
    		float _11,_12,_13,_14;
    		float _21,_22,_23,_24;
    		float _31,_32,_33,_34;
    		float _41,_42,_43,_44; };
    	struct{
    		IVector i; float _14_;
    		IVector j; float _24_;
    		IVector k; float _34_;
    		IVector c; float _44_; }; };

    __inline void set( IMatrix& m ){
    	_11 = m._11; _12=m._12; _13=m._13; _14=m._14;
    	_21 = m._21; _22=m._22; _23=m._23; _24=m._24;
    	_31 = m._31; _32=m._32; _33=m._33; _34=m._34;
    	_41 = m._41; _42=m._42; _43=m._43; _44=m._44; };

    __inline void set( const IMatrix& m ){
    	_11 = m._11; _12=m._12; _13=m._13; _14=m._14;
    	_21 = m._21; _22=m._22; _23=m._23; _24=m._24;
    	_31 = m._31; _32=m._32; _33=m._33; _34=m._34;
    	_41 = m._41; _42=m._42; _43=m._43; _44=m._44; };

    __inline void identity(){
    	_11=1; _12=0; _13=0; _14=0;
    	_21=0; _22=1; _23=0; _24=0;
    	_31=0; _32=0; _33=1; _34=0;
    	_41=0; _42=0; _43=0; _44=1; };

    __inline void transform( IVector& dst, IVector& src ){
    	dst.x = src.x * _11 + src.y * _21 + src.z * _31 + _41;
    	dst.y = src.x * _12 + src.y * _22 + src.z * _32 + _42;
    	dst.z = src.x * _13 + src.y * _23 + src.z * _33 + _43;
    	float w = src.x * _14 + src.y * _24 + src.z * _34 + _44;
    	_ASSERTE( !is_zero(w) );
    	dst.x /= w;
    	dst.y /= w;
    	dst.z /= w; };

    __inline void transform( IVector& dst ){
    	IVector v; v.set( dst );
    	transform( dst, v ); };

    __inline void shorttransform( IVector& dst, IVector& src ){
    	dst.x = src.x * _11 + src.y * _21 + src.z * _31;
    	dst.y = src.x * _12 + src.y * _22 + src.z * _32;
    	dst.z = src.x * _13 + src.y * _23 + src.z * _33; };
    __inline void shorttransform( IVector& dst ){
    	IVector v; v.set( dst );
    	shorttransform( dst, v ); };

    __inline void mul( IMatrix& m1, IMatrix& m2 ){
    	_11 = m1._11*m2._11 + m1._12*m2._21 + m1._13*m2._31 + m1._14*m2._41;
    	_12 = m1._11*m2._12 + m1._12*m2._22 + m1._13*m2._32 + m1._14*m2._42;
    	_13 = m1._11*m2._13 + m1._12*m2._23 + m1._13*m2._33 + m1._14*m2._43;
    	_14 = m1._11*m2._14 + m1._12*m2._24 + m1._13*m2._34 + m1._14*m2._44;
    	_21 = m1._21*m2._11 + m1._22*m2._21 + m1._23*m2._31 + m1._24*m2._41;
    	_22 = m1._21*m2._12 + m1._22*m2._22 + m1._23*m2._32 + m1._24*m2._42;
    	_23 = m1._21*m2._13 + m1._22*m2._23 + m1._23*m2._33 + m1._24*m2._43;
    	_24 = m1._21*m2._14 + m1._22*m2._24 + m1._23*m2._34 + m1._24*m2._44;
    	_31 = m1._31*m2._11 + m1._32*m2._21 + m1._33*m2._31 + m1._34*m2._41;
    	_32 = m1._31*m2._12 + m1._32*m2._22 + m1._33*m2._32 + m1._34*m2._42;
    	_33 = m1._31*m2._13 + m1._32*m2._23 + m1._33*m2._33 + m1._34*m2._43;
    	_34 = m1._31*m2._14 + m1._32*m2._24 + m1._33*m2._34 + m1._34*m2._44;
    	_41 = m1._41*m2._11 + m1._42*m2._21 + m1._43*m2._31 + m1._44*m2._41;
    	_42 = m1._41*m2._12 + m1._42*m2._22 + m1._43*m2._32 + m1._44*m2._42;
    	_43 = m1._41*m2._13 + m1._42*m2._23 + m1._43*m2._33 + m1._44*m2._43;
    	_44 = m1._41*m2._14 + m1._42*m2._24 + m1._43*m2._34 + m1._44*m2._44; };

    __inline void mul1( IMatrix& m ){
    	IMatrix src1; src1.set( (*this) );
    	mul( src1, m ); };
    __inline void mul2( IMatrix& m ){
    	IMatrix src2; src2.set( (*this) );
    	mul( m, src2 ); };

    __inline void rx( float angle ){
    	float _cos = cos(angle);
    	float _sin = sin(angle);
    	a[0] = 1;     a[1] = 0;     a[2] = 0;      a[3] = 0;
    	a[4] = 0;     a[5] = _cos;  a[6] = _sin;   a[7] = 0;
    	a[8] = 0;     a[9] = -_sin; a[10] = _cos;  a[11] = 0;
    	a[12] = 0;    a[13] = 0;    a[14] = 0;     a[15] = 1; };
    __inline void ry( float angle ){
    	float _cos = cos(angle);
    	float _sin = sin(angle);
    	a[0] = _cos;   a[1] = 0;    a[2] = -_sin;  a[3] = 0;
    	a[4] = 0;      a[5] = 1;    a[6] = 0;      a[7] = 0;
    	a[8] = _sin;   a[9] = 0;    a[10] = _cos;  a[11] = 0;
    	a[12] = 0;     a[13] = 0;   a[14] = 0;     a[15] = 1; };

    __inline void rz( float angle ){
    	float _cos = cos(angle);
    	float _sin = sin(angle);
    	a[0] = _cos;  a[1] = _sin;  a[2] = 0;      a[3] = 0;
    	a[4] = -_sin; a[5] = _cos;  a[6] = 0;      a[7] = 0;
    	a[8] = 0;     a[9] = 0;     a[10] = 1;     a[11] = 0;
    	a[12] = 0;    a[13] = 0;    a[14] = 0;     a[15] = 1; };

    __inline void r( float angle, IVector& axis ){
    	float _cos = cos(angle);
    	float _sin = sin(angle);
    	a[0]  = axis.x * axis.x + ( 1 - axis.x * axis.x) * _cos;
    	a[1]  = axis.x * axis.y * ( 1 - _cos ) + axis.z * _sin;
    	a[2]  = axis.x * axis.z * ( 1 - _cos ) - axis.y * _sin;
    	a[3]  = 0;
    	a[4]  = axis.x * axis.y * ( 1 - _cos ) - axis.z * _sin;
    	a[5]  = axis.y * axis.y + ( 1 - axis.y * axis.y) * _cos;
    	a[6]  = axis.y * axis.z * ( 1 - _cos ) + axis.x * _sin;
    	a[7]  = 0;
    	a[8]  = axis.x * axis.z * ( 1 - _cos ) + axis.y * _sin;
    	a[9]  = axis.y * axis.z * ( 1 - _cos ) - axis.x * _sin;
    	a[10] = axis.z * axis.z + ( 1 - axis.z * axis.z) * _cos;
    	a[11] = 0;
    	a[12] = 0;
    	a[13] = 0;
    	a[14] = 0;
    	a[15] = 1; };

    __inline void t( float dx, float dy, float dz ){
    	a[0] = 1;     a[1] = 0;     a[2] = 0;      a[3] = 0;
    	a[4] = 0;     a[5] = 1;     a[6] = 0;      a[7] = 0;
    	a[8] = 0;     a[9] = 0;     a[10] = 1;     a[11] = 0;
    	a[12] = dx;   a[13] = dy;   a[14] = dz;    a[15] = 1; };

    __inline void t( IVector& v ){
    	t( v.x, v.y, v.z ); }

    __inline void s( float sx, float sy, float sz ){
    	a[0] = sx;    a[1] = 0;     a[2] = 0;      a[3] = 0;
    	a[4] = 0;     a[5] = sy;    a[6] = 0;      a[7] = 0;
    	a[8] = 0;     a[9] = 0;     a[10] = sz;    a[11] = 0;
    	a[12] = 0;    a[13] = 0;    a[14] = 0;     a[15] = 1; };

    __inline void s( float scalevalue ){
    	s( scalevalue, scalevalue, scalevalue ); };
    __inline void s( IVector& v ){
    	s( v.x, v.y, v.z ); };

    __inline float det(){
    	  float p0,p1,p2,p3;
    	  p0 = a[0] * ( a[5] * ( a[10] * a[15] - a[11] * a[14] ) -
    			 a[6] * ( a[9] *   a[15] - a[11] * a[13] ) +
    			 a[7] * ( a[9] *   a[14] - a[10] * a[13] ) );
    	  p1 = a[1] * ( a[4] * ( a[10] * a[15] - a[11] * a[14] ) -
    			 a[6] * ( a[8] *   a[15] - a[11] * a[12] ) +
    			 a[7] * ( a[8] *   a[14] - a[10] * a[12] ) );
    	  p2 = a[2] * ( a[4] * ( a[9] * a[15] - a[11] * a[13] ) -
    			 a[5] * ( a[8] *   a[15] - a[11] * a[12] ) +
    			 a[7] * ( a[8] *   a[13] - a[9] * a[12] ) );
    	  p3 = a[3] * ( a[4] * ( a[9] * a[14] - a[10] * a[13] ) -
    			 a[5] * ( a[8] *   a[14] - a[10] * a[12] ) +
    			 a[6] * ( a[8] *   a[13] - a[9] * a[12] ) );
    	  return (p0 - p1 + p2 - p3); };

    __inline void invert( IMatrix& src ){

    	float c00, c01, c02,
    		  c10, c11, c12,
    		  c20, c21, c22;
    	float a00, a01, a02,
    		  a10, a11, a12,
    		  a20, a21, a22;
    	float det, oodet;
    	float t0, t1, t2;

    	c00 =    src.a[5] * src.a[10] - src.a[9] * src.a[6];
    	c01 = -( src.a[4] * src.a[10] - src.a[8] * src.a[6] );
    	c02 =    src.a[4] * src.a[9] - src.a[8] * src.a[5];
    	c10 = -( src.a[1] * src.a[10] - src.a[9] * src.a[2] );
    	c11 =    src.a[0] * src.a[10] - src.a[8] * src.a[2];
    	c12 = -( src.a[0] * src.a[9] - src.a[8] * src.a[1] );
    	c20 =    src.a[1] * src.a[6] - src.a[5] * src.a[2];
    	c21 = -( src.a[0] * src.a[6] - src.a[4] * src.a[2] );
    	c22 =    src.a[0] * src.a[5] - src.a[4] * src.a[1];
    	a00 = c00; a01 = c10; a02 = c20;
    	a10 = c01; a11 = c11; a12 = c21;
    	a20 = c02; a21 = c12; a22 = c22;
    	det = a00 * src.a[0] + a01 * src.a[4] + a02 * src.a[8];

    	if ( is_zero(det) ){
    		identity();
    		return; }

    	oodet = 1.0f / det;
    	a[0] = oodet * a00;
    	a[1] = oodet * a01;
    	a[2] = oodet * a02;
    	a[4] = oodet * a10;
    	a[5] = oodet * a11;
    	a[6] = oodet * a12;
    	a[8] = oodet * a20;
    	a[9] = oodet * a21;
    	a[10] = oodet * a22;
    	t0 = -src.a[12];
    	t1 = -src.a[13];
    	t2 = -src.a[14];
    	a[12] = t0 * a[0] + t1 * a[4] + t2 * a[8];
    	a[13] = t0 * a[1] + t1 * a[5] + t2 * a[9];
    	a[14] = t0 * a[2] + t1 * a[6] + t2 * a[10];
    	a[3] = a[7] = a[11] = 0.0f;
    	a[15] = 1.0f; };

    __inline void projection( float fov, float aspect, float zn, float zf ){
    	_ASSERTE( !is_zero( sin(fov) ) );
    	_ASSERTE( !is_zero( (zf-zn) ) );
    	float h = cos(fov*0.5f) / sin(fov*0.5f);
    	float w = aspect * h;
    	float Q = zf/(zf-zn);
    	float Qz = -Q*zn;
    	_11 = w;   _12 = 0;   _13 = 0;   _14 = 0;
    	_21 = 0;   _22 = h;   _23 = 0;   _24 = 0;
    	_31 = 0;   _32 = 0;   _33 = Q;   _34 = 1;
    	_41 = 0;   _42 = 0;   _43 = Qz;  _44 = 0; };
};
*/

class IAABox {
public:
    Fvector vmin,vmax;

    __inline void sort(){
    	float tmp;
    	if( vmin.x > vmax.x ){
    		tmp = vmin.x;
    		vmin.x = vmax.x;
    		vmax.x = tmp; }
    	if( vmin.y > vmax.y ){
    		tmp = vmin.y;
    		vmin.y = vmax.y;
    		vmax.y = tmp; }
    	if( vmin.z > vmax.z ){
    		tmp = vmin.z;
    		vmin.z = vmax.z;
    		vmax.z = tmp; } };

    __inline void offset( Fvector& amount ){
    	vmin.add( amount );
    	vmax.add( amount ); };

    __inline void set( Fvector& _vmin, Fvector& _vmax ){
    	vmin.set( _vmin );
    	vmax.set( _vmax );
    	sort(); };

    __inline void set( IAABox& box ){
    	vmin.set( box.vmin );
    	vmax.set( box.vmax );
    	sort(); };

    __inline void grow( float delta ){
    	vmin.sub( delta );
    	vmax.add( delta ); };

    __inline void growrange( float x, float y, float z ){
    	vmin.x = min( x, vmin.x );
    	vmin.y = min( y, vmin.y );
    	vmin.z = min( z, vmin.z );
    	vmax.x = max( x, vmax.x );
    	vmax.y = max( y, vmax.y );
    	vmax.z = max( z, vmax.z ); };

    __inline void ground_lb( Fvector& result ){
    	result.x = vmin.x;
    	result.z = vmin.z;
    	result.y = 0; };

    __inline void ground_lt( Fvector& result ){
    	result.x = vmin.x;
    	result.z = vmax.z;
    	result.y = 0; };

    __inline void ground_rb( Fvector& result ){
    	result.x = vmax.x;
    	result.z = vmin.z;
    	result.y = 0; };

    __inline void ground_rt( Fvector& result ){
    	result.x = vmax.x;
    	result.z = vmax.z;
    	result.y = 0; };

	__inline void getpoint( int index,  Fvector& result ){
		switch( index ){
		case 0: result.set( vmin.x, vmin.y, vmin.z ); break;
		case 1: result.set( vmin.x, vmin.y, vmax.z ); break;
		case 2: result.set( vmax.x, vmin.y, vmax.z ); break;
		case 3: result.set( vmax.x, vmin.y, vmin.z ); break;
		case 4: result.set( vmin.x, vmax.y, vmin.z ); break;
		case 5: result.set( vmin.x, vmax.y, vmax.z ); break;
		case 6: result.set( vmax.x, vmax.y, vmax.z ); break;
		case 7: result.set( vmax.x, vmax.y, vmin.z ); break;
		default: result.set( 0, 0, 0 ); break; }
	};

	__inline void getsize( Fvector& result ){
		result.sub( vmax, vmin );
	};

	__inline bool intersect( IAABox& box ){
		if( vmax.x < box.vmin.x )
			return false;
		if( vmax.y < box.vmin.y )
			return false;
		if( vmax.z < box.vmin.z )
			return false;
		if( vmin.x > box.vmax.x )
			return false;
		if( vmin.y > box.vmax.y )
			return false;
		if( vmin.z > box.vmax.z )
			return false;
		return true;
	};

	__inline void center( Fvector& result ){
		result.x = (vmin.x + vmax.x) * 0.5f;
		result.y = (vmin.y + vmax.y) * 0.5f;
		result.z = (vmin.z + vmax.z) * 0.5f;
	};

	__inline bool Pick( Fvector& start, Fvector& dir ){

		float alpha,xt,yt,zt;
		Fvector rvmin,rvmax;

		rvmin.sub( vmin, start );
		rvmax.sub( vmax, start );

		if( !is_zero(dir.x) ){
			alpha = rvmin.x / dir.x;
			yt = alpha * dir.y;
			if( yt >= rvmin.y && yt <= rvmax.y ){
				zt = alpha * dir.z;
				if( zt >= rvmin.z && zt <= rvmax.z )
					return true;
			}
			alpha = rvmax.x / dir.x;
			yt = alpha * dir.y;
			if( yt >= rvmin.y && yt <= rvmax.y ){
				zt = alpha * dir.z;
				if( zt >= rvmin.z && zt <= rvmax.z )
					return true;
			}
		}

		if( !is_zero(dir.y) ){
			alpha = rvmin.y / dir.y;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x ){
				zt = alpha * dir.z;
				if( zt >= rvmin.z && zt <= rvmax.z )
					return true;
			}
			alpha = rvmax.y / dir.y;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x ){
				zt = alpha * dir.z;
				if( zt >= rvmin.z && zt <= rvmax.z )
					return true;
			}
		}

		if( !is_zero(dir.z) ){
			alpha = rvmin.z / dir.z;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x ){
				yt = alpha * dir.y;
				if( yt >= rvmin.y && yt <= rvmax.y )
					return true;
			}
			alpha = rvmax.z / dir.z;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x ){
				yt = alpha * dir.y;
				if( yt >= rvmin.y && yt <= rvmax.y )
					return true;
			}
		}

		return false;
	};

};


class ICullPlane {
public:
    Fvector n,p;
    float d;

    __inline void set( Fvector& _n, Fvector& _p ){
    	n.set( _n ); p.set( _p );
    	d = - _n.dotproduct( _p ); };

    __inline float dist_point( Fvector& v ){
    	return ( n.dotproduct( v ) + d ); };

    __inline bool front_box( IAABox& box ){
    	Fvector testpoint;  // can be optimized
    	testpoint.x = ( n.x >= 0 ) ? box.vmin.x : box.vmax.x;
    	testpoint.y = ( n.y >= 0 ) ? box.vmin.y : box.vmax.y;
    	testpoint.z = ( n.z >= 0 ) ? box.vmin.z : box.vmax.z;
    	return ( dist_point( testpoint ) > 0.f); };
};


class IGroundPoint {
public:
    float x,z;

    __inline void set( float _x, float _z ){
    	x = _x;   z = _z; };

    __inline void set( IGroundPoint& point ){
    	x = point.x;
    	z = point.z; };

    __inline void set( Fvector& point ){
    	x = point.x;
    	z = point.z; };
};


class IGroundBox {
public:
    float xmin,xmax;
    float zmin,zmax;

    __inline void set( float _xmin, float _xmax, float _zmin, float _zmax ){
    	xmin = min( _xmin, _xmax );
    	xmax = max( _xmin, _xmax );
    	zmin = min( _zmin, _zmax );
    	zmax = max( _zmin, _zmax ); };

    __inline void set( IGroundBox& box ){
    	xmin = box.xmin; xmax = box.xmax;
    	zmin = box.zmin; zmax = box.zmax; };

    __inline void set( IAABox& box ){
    	xmin = box.vmin.x;  xmax = box.vmax.x;
    	zmin = box.vmin.z;  zmax = box.vmax.z; };

    __inline void growbypoint( Fvector& point ){
    	if( xmax < point.x ) xmax = point.x; else if( xmin > point.x ) xmin = point.x;
    	if( zmax < point.z ) zmax = point.z; else if( zmin > point.z ) zmin = point.z; };

    __inline void growbypoint( float _x, float _z ){
    	if( xmax < _x ) xmax = _x; else if( xmin > _x ) xmin = _x;
    	if( zmax < _z ) zmax = _z; else if( zmin > _z ) zmin = _z; };

    __inline float size_x(){
    	return (xmax - xmin); };

    __inline float size_z(){
    	return (zmax - zmin); };

    __inline void offset( IGroundPoint& to ){
    	xmin += to.x; xmax += to.x;
    	zmin += to.z; zmax += to.z; };

    __inline void offset( Fvector& to ){
    	xmin += to.x; xmax += to.x;
    	zmin += to.z; zmax += to.z; };
};


class IFaceDef {
public:
    union {
    	unsigned short p[3];
    	struct { unsigned short p0, p1, p2; };
    };
    
    __inline void set( unsigned short _p0, unsigned short _p1, unsigned short _p2 ){
    	p0 = _p0; p1 = _p1; p2 = _p2; }
};


template <class _T>
class IRange {
public:
	_T from;
	_T to;

	__inline void set( _T _from, _T _to ){
  		from = _from;   to = _to; };
	__inline void set( IRange<_T>& src ){
  		set(src.from,src.to); };

	__inline float size(){
  		return (to-from); }
	__inline bool valid(){
  		return (from<=to); }
};


#pragma pack(pop)


/*
__inline bool PickFace(
	float *distance,
	IVector& start, IVector& direction,
	IVector& facenormal, IVector& p0, IVector& p1, IVector& p2 )
{
	float clcheck = facenormal.dot( direction );
	if( clcheck != 0.f ){
  		
  		float d = - facenormal.dot( p0 );
  		(*distance) = - ( (d + facenormal.dot(start)) / clcheck );
		if( (*distance) <= 0 )
			return false;
  		
  		IVector hitpoint;
  		hitpoint.x = start.x + direction.x * (*distance);
  		hitpoint.y = start.y + direction.y * (*distance);
  		hitpoint.z = start.z + direction.z * (*distance);

  		IVector np_from_p1;
  		np_from_p1.sub( hitpoint, p1 );
  		np_from_p1.safe_normalize();

  		IVector tridir_10;
  		tridir_10.sub( p0, p1 );
  		tridir_10.safe_normalize();

  		IVector tstn1;
  		tstn1.cross( tridir_10, facenormal );

  		if( tstn1.dot( np_from_p1 ) >= (-FACETEST_PRECISION) ){

  			IVector np_from_p2;
  			np_from_p2.sub( hitpoint, p2 );
  			np_from_p2.safe_normalize();

  			IVector tridir_21;
  			tridir_21.sub(p1,p2);
  			tridir_21.safe_normalize();

  			IVector tstn2;
  			tstn2.cross( tridir_21, facenormal );

  			if( tstn2.dot( np_from_p2 ) >= (-FACETEST_PRECISION) ){

  				IVector np_from_p0;
  				np_from_p0.sub( hitpoint, p0 );
  				np_from_p0.safe_normalize();

  				IVector tridir_02;
  				tridir_02.sub(p2,p0);
  				tridir_02.safe_normalize();

  				IVector tstn0;
  				tstn0.cross( tridir_02, facenormal );

  				if( tstn0.dot( np_from_p0 ) >= (-FACETEST_PRECISION) ){
  					return true;
  				}

  			}
  		}
	}
	return false;
};
*/


#endif /*_INCDEF_IMATH_H_*/

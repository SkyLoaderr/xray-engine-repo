#ifndef _vector_included
#define _vector_included

// Undef some macros
#ifdef M_PI
#undef M_PI
#endif

#ifdef PI
#undef PI
#endif

#ifdef FLT_MAX
#undef FLT_MAX
#undef FLT_MIN
#endif

#ifdef abs
#undef abs
#endif

#ifdef _MIN
#undef _MIN
#undef _MAX
#undef min
#undef max
#endif

// Select platform
#ifdef	_MSC_VER
#define	M_VISUAL
#endif
#ifdef	__BORLANDC__
#define M_BORLAND
#endif

// Define types and namespaces (CPU & FPU)
#include	"_types.h"
#include	"_math.h"

// Constants
#ifdef M_VISUAL
    const	float		EPS_S		= 0.0000001f;
    const	float		EPS			= 0.0000100f;
    const	float		EPS_L		= 0.0010000f;

    const	float		M_PI		= 3.1415926535897932384626433832795f;
    const	float		PI			= M_PI;
    const	float		PI_MUL_2	= 6.2831853071795864769252867665590f;
    const	float		PI_MUL_4	= 12.566370614359172953850573533118f;
    const	float		PI_MUL_6	= 18.849555921538759430775860299677f;
    const	float		PI_MUL_8	= 25.132741228718345907701147066236f;
    const	float		PI_DIV_2	= 1.5707963267948966192313216916398f;
    const	float		PI_DIV_4	= 0.7853981633974483096156608458199f;
    const	float		PI_DIV_6	= 0.5235987755982988730771072305466f;
	const	float		PI_DIV_8	= 0.3926990816987241548078304229099f;
#endif
#ifdef M_BORLAND
    #define				EPS_S		0.0000001f
    #define				EPS			0.0000100f
    #define				EPS_L		0.0010000f

    #define				M_PI		3.1415926535897932384626433832795f
    #define				PI			M_PI
    #define				PI_MUL_2	6.2831853071795864769252867665590f
    #define				PI_MUL_4	12.566370614359172953850573533118f
    #define				PI_MUL_6	18.849555921538759430775860299677f
    #define				PI_MUL_8	25.132741228718345907701147066236f
    #define				PI_MUL_2	6.2831853071795864769252867665590f
    #define				PI_DIV_2	1.5707963267948966192313216916398f
    #define				PI_DIV_4	0.7853981633974483096156608458199f
    #define				PI_DIV_6	0.5235987755982988730771072305466f
    #define				PI_DIV_8	0.3926990816987241548078304229099f
#endif

// comparisions
IC BOOL  fsimilar		( float		a, float	b, float	cmp=EPS )		{ return fabsf(a-b)<cmp;	}
IC BOOL  dsimilar		( double	a, double	b, double	cmp=EPS )		{ return fabs(a-b)<cmp;		}

IC BOOL  fis_zero		( float		val, float	cmp=EPS_S )					{ return fabsf(val)<cmp;	}
IC BOOL  dis_zero		( double	val, double	cmp=EPS_S )					{ return fabs(val)<cmp;		}

// degree 2 radians and vice-versa
template <class T>		
IC T	deg2rad			( T val )	{ return (val*T(M_PI)/T(180)); };
template <class T>		
IC T	rad2deg			( T val )	{ return (val*T(180)/T(M_PI)); };

// assembler accelerated simultaneus SIN/COS
#ifdef	M_VISUAL
#define _sincos(_a,_s,_c)	{ __asm fld _a __asm fsincos __asm fstp _c __asm fstp _s }
#endif
#ifdef	M_BORLAND
#define _sincos(a,s,c)		{ s = sinf(a); c=cosf(a); }
#endif

// clamping/snapping
template <class T>
IC void clamp( T& val, const T _low, const T _high ){
    if( val<_low ) val = _low; else if( val>_high ) val = _high;
};
IC float snapto( float value, float snap ){
    if( snap<=0.f ) return value;
	return floorf((value+(snap*0.5f)) / snap ) * snap;
};

// pre-definitions
struct _matrix33;	// matrix
struct _quaternion;	// quaternion

struct _fpoint;		// fpoint
struct _lpoint;		// lpoint

struct _lrect;		// lrect
struct _frect;		// frect
struct _fbox;		// fbox
struct _fbox2;		// fbox
struct Fobb;		// oriented bbox

struct _3dbuffer;	// Dsound 3d buffer
struct _3dlistener;	// Dsound listener


struct _color;		// floating point based color definition

#pragma pack(push)
#pragma pack(1)

#include "_bitwise.h"
#include "_std_extensions.h"
#include "_random.h"

#ifndef NO_XR_COLOR
#include "_color.h"
#endif
#include "_vector3d.h"
#include "_vector2.h"
#include "_vector4.h"
#include "_matrix.h"
#include "_matrix33.h"
#include "_quaternion.h"
#include "_ipoint.h"
#include "_fpoint.h"
#include "_rect.h"
#include "_fbox.h"
#include "_fbox2.h"
#include "_obb.h"
#include "_sphere.h"
#include "_random.h"
#include "_compressed_normal.h"
#include "_plane.h"

#include "_stl_extensions.h"
#include "_d3d_extensions.h"

#pragma pack(pop)


// normalize angle
IC float		angle_normalize	(float a)
{
	float		div	 =	a/PI_MUL_2;
	int			rnd  =	(div>0)?iFloor(div):iCeil(div);
	float		frac =	div-rnd;
	if (frac<0)	frac +=	1.f;
	return		frac *	PI_MUL_2;
}
IC float		angle_difference(float a, float b)
{
	float diff	= angle_normalize(a) - angle_normalize(b);
	if (diff>0) {
		if (diff>PI)
			diff	-= PI_MUL_2;
	} else {
		if (diff<-PI)	
			diff	+= PI_MUL_2;
	}
	return _abs	(diff);
}

template <class T>
IC void _matrix<T>::get_rapid(_matrix33& R)const{
	R.m[0][0]	=  m[0][0]; R.m[0][1] =  m[1][0]; R.m[0][2] = m[2][0];
	R.m[1][0]	=  m[0][1]; R.m[1][1] =  m[1][1]; R.m[1][2] = m[2][1];
	R.m[2][0]	=  m[0][2]; R.m[2][1] =  m[1][2]; R.m[2][2] = m[2][2];
}

template <class T>
IC void _matrix<T>::rotation	(const _quaternion &Q) {
    T xx = Q.x*Q.x; T yy = Q.y*Q.y; T zz = Q.z*Q.z;
    T xy = Q.x*Q.y; T xz = Q.x*Q.z; T yz = Q.y*Q.z;
    T wx = Q.w*Q.x; T wy = Q.w*Q.y; T wz = Q.w*Q.z;

    _11 = 1 - 2 * ( yy + zz );	_12 =     2 * ( xy - wz );	_13 =     2 * ( xz + wy );	_14 = 0;
    _21 =     2 * ( xy + wz );	_22 = 1 - 2 * ( xx + zz );	_23 =     2 * ( yz - wx );	_24 = 0;
    _31 =     2 * ( xz - wy );	_32 =     2 * ( yz + wx );	_33 = 1 - 2 * ( xx + yy );	_34 = 0;
    _41 = 0;					_42 = 0;					_43 = 0;					_44 = 1;
}

template <class T>
IC void _matrix<T>::mk_xform	(const _quaternion &Q, const Tvector &V) {
    T xx = Q.x*Q.x; T yy = Q.y*Q.y; T zz = Q.z*Q.z;
    T xy = Q.x*Q.y; T xz = Q.x*Q.z; T yz = Q.y*Q.z;
    T wx = Q.w*Q.x; T wy = Q.w*Q.y; T wz = Q.w*Q.z;

    _11 = 1 - 2 * ( yy + zz );	_12 =     2 * ( xy - wz );	_13 =     2 * ( xz + wy );	_14 = 0;
    _21 =     2 * ( xy + wz );	_22 = 1 - 2 * ( xx + zz );	_23 =     2 * ( yz - wx );	_24 = 0;
    _31 =     2 * ( xz - wy );	_32 =     2 * ( yz + wx );	_33 = 1 - 2 * ( xx + yy );	_34 = 0;
    _41 = V.x;					_42 = V.y;					_43 = V.z;					_44 = 1;
}
#endif

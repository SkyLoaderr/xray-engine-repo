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
#endif
#ifdef FLT_MIN
#undef FLT_MIN
#endif
#ifdef abs
#undef abs
#endif
#ifdef _MIN
#undef _MIN
#undef _MAX
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
    #define				PI_DIV_2	1.5707963267948966192313216916398f
    #define				PI_DIV_4	0.7853981633974483096156608458199f
    #define				PI_DIV_6	0.5235987755982988730771072305466f
    #define				PI_DIV_8	0.3926990816987241548078304229099f
#endif

// Min/Max
#define _MIN(a,b)		(a)<(b)?(a):(b)
#define _MAX(a,b)		(a)>(b)?(a):(b)

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
struct _matrix;		// matrix
struct _matrix33;	// matrix

struct _fpoint;		// fpoint
struct _lpoint;		// lpoint

struct _lrect;		// lrect
struct _frect;		// frect
struct _fbox;		// fbox
struct _fbox2;		// fbox
struct Fobb;		// oriented bbox

struct _3dbuffer;	// direct sound 3d buffer
struct _3dlistener;	// direct sound listener


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
#include "_quaternion.h"
#include "_matrix.h"
#include "_matrix33.h"
#include "_ipoint.h"
#include "_fpoint.h"
#include "_irect.h"
#include "_frect.h"
#include "_fbox.h"
#include "_fbox2.h"
#include "_obb.h"
#include "_sphere.h"
#include "_random.h"
#include "_compressed_normal.h"

#include "_plane.h"
#include "_intersection_quad.h"

#include "_stl_extensions.h"
#include "_d3d_extensions.h"

#pragma pack(pop)

IC void Fmatrix::get_rapid(_matrix33& R)const{
	R.m[0][0]	=  m[0][0]; R.m[0][1] =  m[1][0]; R.m[0][2] = m[2][0];
	R.m[1][0]	=  m[0][1]; R.m[1][1] =  m[1][1]; R.m[1][2] = m[2][1];
	R.m[2][0]	=  m[0][2]; R.m[2][1] =  m[1][2]; R.m[2][2] = m[2][2];
}
#endif
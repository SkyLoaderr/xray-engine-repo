#ifndef _VECTOR4_H
#define _VECTOR4_H
#pragma once

template <class T>
struct _vector4 {
	typedef T			TYPE;
	typedef _vector4<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T x,y,z,w;

	IC	T&			operator[] (int i)					{ return *((T*)this + i); }
	IC	T&			operator[] (int i)	const			{ return *((T*)this + i); }

	IC void 		set(T _x, T _y, T _z, T _w=1)		{ x=_x; y=_y; z=_z; w=_w;		}
	IC void 		set(const Self& v)					{ x=v.x; y=v.y; z=v.z; w=v.w;	}

	IC BOOL 		similar(const Self& v, T E=EPS_L)	{ return _abs(x-v.x)<E && _abs(y-v.y)<E && _abs(z-v.z)<E && _abs(w-v.w)<E;};
};

typedef _vector4<float>		Fvector4;
typedef _vector4<double>	Dvector4;
typedef _vector4<s32>		Ivector4;

#endif
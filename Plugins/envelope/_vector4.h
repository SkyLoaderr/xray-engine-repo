#pragma once

template <class T>
class _vector4 {
public:
	float x,y,z,w;

	IC void set(T _x, T _y, T _z, T _w=1)	{ x=_x; y=_y; z=_z; w=_w;		}
	IC void set(const _vector4<T>& v)		{ x=v.x; y=v.y; z=v.z; w=v.w;	}
};

typedef _vector4<float>		Fvector4;
typedef _vector4<double>	Dvector4;

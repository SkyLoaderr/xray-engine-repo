#ifndef _L_VERT
#define _L_VERT

#define FLVERTEX	D3DFVF_LVERTEX

typedef struct _Lvertex {
public:
	union {
		struct {
			float	x, y, z;
		};
		Fvector p;
	};
    DWORD	reserved;
    DWORD  	color;
    DWORD 	specular;
    float	tu,tv;

public:
	IC	D3DLVERTEX	*d3d(void) { return (D3DLVERTEX *)this; };
	///////////////////
	// Class members //
	///////////////////
	IC void set(Fvector &pp, DWORD c, DWORD s, float _tu, float _tv)
	{
		p.set(pp);
		color=c;
		specular=s;
		tu=_tu;
		tv=_tv;
	};
	IC void set(float _x, float _y, float _z, DWORD c, DWORD s, float _tu, float _tv)
	{
		x=_x;	y=_y;	z=_z;
		color=c; specular=s;
		tu=_tu;	tv=_tv;
	};
	IC void set(_Lvertex &v)
	{
		CopyMemory(this, &v, sizeof(_Lvertex));
	};
	IC void transform(const _matrix &M)					// faster
	{
		M.transform_tiny(p);
	};
	IC void transform(const _Lvertex &v,const _matrix &M)	// slower
	{
		M.transform_tiny(p,v.p);
		color=v.color;
		specular=v.specular;
		tu=v.tu;
		tv=v.tv;
	};
} FLvertex;

#endif

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
    FPcolor color;
    FPcolor specular;
    float	tu,tv;

public:
	__forceinline	D3DLVERTEX	*d3d(void) { return (D3DLVERTEX *)this; };
	///////////////////
	// Class members //
	///////////////////
	__forceinline void set(_vector &pp, FPcolor c, FPcolor s, float _tu, float _tv)
	{
		p.set(pp);
		color.set(c);
		specular.set(s);
		tu=_tu;
		tv=_tv;
	};
	__forceinline void set(_vector &pp, DWORD c, DWORD s, float _tu, float _tv)
	{
		p.set(pp);
		color.set(c);
		specular.set(s);
		tu=_tu;
		tv=_tv;
	};
	__forceinline void set(float _x, float _y, float _z, FPcolor c, FPcolor s, float _tu, float _tv)
	{
		x=_x;	y=_y;	z=_z;
		color=c; specular=s;
		tu=_tu;	tv=_tv;
	};
	__forceinline void set(float _x, float _y, float _z, DWORD c, DWORD s, float _tu, float _tv)
	{
		x=_x;	y=_y;	z=_z;
		color.set(c); specular.set(s);
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

#endif

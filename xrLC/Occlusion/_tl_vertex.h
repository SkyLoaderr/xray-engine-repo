#ifndef _TL_VERT
#define _TL_VERT

#define FTLVERTEX	D3DFVF_TLVERTEX

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

	__forceinline	D3DTLVERTEX	*d3d(void) { return (D3DTLVERTEX *)this; };
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
	__forceinline void set(float _sx, float _sy, float _sz, float _rhw, DWORD c, DWORD s, float _tu, float _tv)
	{
		sx=_sx;	sy=_sy;	sz=_sz;	rhw=_rhw;
		color.c=c; specular.c=s;
		tu=_tu;	tv=_tv;
	};
	__forceinline void set(const _TLvertex &v)
	{
		CopyMemory(this, &v, sizeof(_TLvertex));
	};
	__forceinline void s_transform(const _vector &v,const _matrix &matSet)
	{
		// Transform it through the matrix set. Takes in mean projection.
		// Finally, scale the vertices to screen coords.
		// Note 1: device coords range from -1 to +1 in the viewport.
		// Note 2: the sz-coordinate will be used in the z-buffer.
		rhw =   matSet._14*v.x + matSet._24*v.y + matSet._34*v.z + matSet._44;
		sx	=  (matSet._11*v.x + matSet._21*v.y + matSet._31*v.z + matSet._41)/rhw;
		sy	= -(matSet._12*v.x + matSet._22*v.y + matSet._32*v.z + matSet._42)/rhw;
		sz	=  (matSet._13*v.x + matSet._23*v.y + matSet._33*v.z + matSet._43)/rhw;
	};
} FTLvertex;

#endif

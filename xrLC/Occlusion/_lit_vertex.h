#ifndef _LIT_VERT
#define _LIT_VERT

#define FLITVERTEX	(D3DFVF_DIFFUSE|D3DFVF_NORMAL|D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0))

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

#endif

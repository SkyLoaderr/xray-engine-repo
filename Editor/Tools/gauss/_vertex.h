#ifndef _VERT
#define _VERT

#define FVERTEX		D3DFVF_VERTEX

/*
 * Untransformed/unlit vertices
 */

typedef struct _vertex {
public:
	union {
		struct {
			float x, y, z;
			float nx,ny,nz;
		};
		struct {
			Fvector p;
			Fvector n;
		};
	};
	float tu,tv;

public:
	__forceinline	D3DVERTEX	*d3d(void) { return (D3DVERTEX *)this; };

	///////////////////
	// Class members //
	///////////////////
	__forceinline void set(Fvector &pp, Fvector &nn, float _tu, float _tv)
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
		M.transform_tiny(p);
		M.transform_dir (n);
	};
	__forceinline void transform(const _vertex &v,const _matrix &M)	// faster
	{
		M.transform_tiny(p,v.p);
		M.transform_dir (n,v.n);
		tu=v.tu;
		tv=v.tv;
	};
} Fvertex;

#endif

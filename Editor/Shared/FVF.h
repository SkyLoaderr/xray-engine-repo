#pragma once

//-----------------------------------------------------------------------------
namespace FVF {
#pragma pack(push,4)
	struct L {
		Fvector		p;
		DWORD		color;
		IC void		set(float x, float y, float z, DWORD C) { p.set(x,y,z); color=C; }
		IC void		set(const Fvector& _p, DWORD C) { p.set(_p); color=C; }
	};
	const DWORD F_L		= D3DFVF_XYZ | D3DFVF_DIFFUSE;
	struct LIT {
		Fvector		p;
		DWORD		color;
		Fvector2	t;
		IC void		set(float x, float y, float z, DWORD C, float u, float v) { p.set(x,y,z); color=C; t.set(u,v);}
		IC void		set(const Fvector& _p, DWORD C, float u, float v) { p.set(_p); color=C; t.set(u,v);}
	};
	const DWORD F_LIT = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	
	struct TL {
		Fvector4	p;
		DWORD		color;
		float		tu,tv;
		IC void	set	(float x, float y, DWORD c, float u, float v)
		{	set	(x,y,.0001f,.9999f,c,u,v); };
		IC void	set	(int x, int y, DWORD c, float u, float v)
		{	set	(float(x),float(y),.0001f,.9999f,c,u,v); };
		IC void	set	(float x, float y, float z, float w, DWORD c, float u, float v)
		{	p.set	(x,y,z,w); color = c;	tu=u; tv=v;	};
		IC void transform(const Fvector &v,const Fmatrix &matSet)
		{
			// Transform it through the matrix set. Takes in mean projection.
			// Finally, scale the vertices to screen coords.
			// Note 1: device coords range from -1 to +1 in the viewport.
			// Note 2: the p.z-coordinate will be used in the z-buffer.
			p.w =   matSet._14*v.x + matSet._24*v.y + matSet._34*v.z + matSet._44;
			p.x	=  (matSet._11*v.x + matSet._21*v.y + matSet._31*v.z + matSet._41)/p.w;
			p.y	= -(matSet._12*v.x + matSet._22*v.y + matSet._32*v.z + matSet._42)/p.w;
			p.z	=  (matSet._13*v.x + matSet._23*v.y + matSet._33*v.z + matSet._43)/p.w;
		};
	};
	const DWORD F_TL	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
#pragma pack(pop)
};
//-----------------------------------------------------------------------------

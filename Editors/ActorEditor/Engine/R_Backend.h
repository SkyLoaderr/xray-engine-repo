#ifndef r_backendH
#define r_backendH
#pragma once

#include "r_DStreams.h"
#include "r_constants_cache.h"
#include "r_backend_xform.h"
#include "fvf.h"

class ENGINE_API CBackend
{
public:
	// Dynamic geometry streams
	_VertexStream					Vertex;
	_IndexStream					Index;
	IDirect3DIndexBuffer9*			QuadIB;
	R_xforms						xforms;
private:
	// Render-targets
	IDirect3DSurface9*				pRT[4];
	IDirect3DSurface9*				pZB;

	// Vertices/Indices/etc
	IDirect3DVertexDeclaration9*	decl;
	IDirect3DVertexBuffer9*			vb;
	IDirect3DIndexBuffer9*			ib;
	u32								vb_stride;

	// Pixel/Vertex constants
	R_constants						constants;
	R_constant_table*				ctable;

	// Shaders/State
	IDirect3DStateBlock9*			state;
	IDirect3DPixelShader9*			ps;
	IDirect3DVertexShader9*			vs;
	u32								stencil_enable;
	u32								stencil_func;
	u32								stencil_ref;
	u32								stencil_mask;
	u32								stencil_writemask;
	u32								stencil_fail;
	u32								stencil_pass;
	u32								stencil_zfail;

	// Lists
	STextureList*					T;
	SMatrixList*					M;
	SConstantList*					C;

	// Lists-expanded
	CTexture*						textures	[8];
	CMatrix*						matrices	[8];

	void							Invalidate	()
	{
		pRT[0]						= NULL;
		pRT[1]						= NULL;
		pRT[2]						= NULL;
		pRT[3]						= NULL;
		pZB							= NULL;

		decl						= NULL;
		vb							= NULL;
		ib							= NULL;
		vb_stride					= 0;

		state						= NULL;
		ps							= NULL;
		vs							= NULL;
		ctable						= NULL;

		T							= NULL;
		M							= NULL;
		C							= NULL;

		textures[0]=textures[1]=textures[2]=textures[3]=textures[4]=textures[5]=textures[6]=textures[7]=NULL;
		matrices[0]=matrices[1]=matrices[2]=matrices[3]=matrices[4]=matrices[5]=matrices[6]=matrices[7]=NULL;
	}

public:
	struct _stats
	{
		u32								polys;
		u32								verts;
		u32								calls;
		u32								decl;
		u32								vs;
		u32								ps;
		u32								vb;
		u32								ib;
		u32								states;		// Number of times the shader-state changes
		u32								textures;		// Number of times the shader-tex changes
		u32								matrices;		// Number of times the shader-xform changes
		u32								constants;		// Number of times the shader-consts changes
		u32								xforms;
		u32								target_rt;
		u32								target_zb;
	}									stat;
public:
	IC	CTexture*					get_ActiveTexture	(u32 stage)
	{
		return textures	[stage];
	}

	// API
	IC	void						set_xform			(u32 ID, const Fmatrix& M);
	IC	void						set_xform_world		(const Fmatrix& M);
	IC	void						set_xform_view		(const Fmatrix& M);
	IC	void						set_xform_project	(const Fmatrix& M);

	IC	void						set_RT				(IDirect3DSurface9* RT, u32 ID=0);
	IC	void						set_ZB				(IDirect3DSurface9* ZB);
	IC	void						set_Textures		(STextureList* T);
	IC	void						set_Matrices		(SMatrixList* M);
	IC	void						set_Constants		(R_constant_table* C);
	IC	void						set_Element			(ShaderElement* S, u32	pass=0);
	IC	void						set_Shader			(Shader* S, u32 pass=0);

	IC	void						set_States			(IDirect3DStateBlock9* _state);
	IC  void						set_Format			(IDirect3DVertexDeclaration9* _decl);
	IC  void						set_PS				(IDirect3DPixelShader9* _ps);
	IC  void						set_VS				(IDirect3DVertexShader9* _vs);
	IC	void						set_Vertices		(IDirect3DVertexBuffer9* _vb, u32 _vb_stride);
	IC	void						set_Indices			(IDirect3DIndexBuffer9* _ib);
	IC  void						set_Geometry		(SGeometry* _geom);
	IC  void						set_Stencil			(BOOL _enable, u32 _func=D3DCMP_ALWAYS, u32 _ref=0x00, u32 _mask=0x00, u32 _writemask=0x00, u32 _fail=D3DSTENCILOP_KEEP, u32 _pass=D3DSTENCILOP_KEEP, u32 _zfail=D3DSTENCILOP_KEEP);

	// constants
	IC	R_constant*					get_c				(LPCSTR n)															{ if (ctable)	return ctable->get(n);else return 0;}

	IC	void						set_c				(R_constant* C, const Fmatrix& A)									{ if (C)		constants.set(C,A);					}
	IC	void						set_c				(R_constant* C, const Fvector4& A)									{ if (C)		constants.set(C,A);					}
	IC	void						set_c				(R_constant* C, float x, float y, float z, float w)					{ if (C)		constants.set(C,x,y,z,w);			}
	IC	void						set_ca				(R_constant* C, u32 e, const Fmatrix& A)							{ if (C)		constants.seta(C,e,A);				}
	IC	void						set_ca				(R_constant* C, u32 e, const Fvector4& A)							{ if (C)		constants.seta(C,e,A);				}
	IC	void						set_ca				(R_constant* C, u32 e, float x, float y, float z, float w)			{ if (C)		constants.seta(C,e,x,y,z,w);		}

	IC	void						set_c				(LPCSTR n, const Fmatrix& A)										{ if(ctable)	set_c	(ctable->get(n),A);			}
	IC	void						set_c				(LPCSTR n, const Fvector4& A)										{ if(ctable)	set_c	(ctable->get(n),A);			}
	IC	void						set_c				(LPCSTR n, float x, float y, float z, float w)						{ if(ctable)	set_c	(ctable->get(n),x,y,z,w);	}
	IC	void						set_ca				(LPCSTR n, u32 e, const Fmatrix& A)									{ if(ctable)	set_ca	(ctable->get(n),e,A);		}
	IC	void						set_ca				(LPCSTR n, u32 e, const Fvector4& A)								{ if(ctable)	set_ca	(ctable->get(n),e,A);		}
	IC	void						set_ca				(LPCSTR n, u32 e, float x, float y, float z, float w)				{ if(ctable)	set_ca	(ctable->get(n),e,x,y,z,w);	}

	IC	void						Render				(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC);
	IC	void						Render				(D3DPRIMITIVETYPE T, u32 startV, u32 PC);

	// Device create / destroy / frame signaling
	void							OnFrameBegin		();
	void							OnFrameEnd			();
	void							OnDeviceCreate		();
	void							OnDeviceDestroy		();

	// Debug render
	void dbg_Draw					(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, WORD* pIdx, int pcnt);
	void dbg_Draw					(D3DPRIMITIVETYPE T, FVF::L* pVerts, int pcnt);
	IC void dbg_DrawAABB			(Fvector& T, float sx, float sy, float sz, u32 C)						{	Fvector half_dim;	half_dim.set(sx,sy,sz); Fmatrix	TM;	TM.translate(T); dbg_DrawOBB(TM,half_dim,C);	}
	void dbg_DrawOBB				(Fmatrix& T, Fvector& half_dim, u32 C);
	IC void dbg_DrawTRI				(Fmatrix& T, Fvector* p, u32 C)											{	dbg_DrawTRI(T,p[0],p[1],p[2],C);	}
	void dbg_DrawTRI				(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, u32 C);
	void dbg_DrawLINE				(Fmatrix& T, Fvector& p1, Fvector& p2, u32 C);
	void dbg_DrawEllipse			(Fmatrix& T, u32 C);

	CBackend()						{	Invalidate(); };
};

extern ENGINE_API CBackend RCache;
#endif

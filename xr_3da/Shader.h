// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

#include "r_constants.h"
#include "tss_def.h"

typedef svector<string64,4>	sh_list;

class	ENGINE_API	CBlender_Compile;
class	ENGINE_API	IBlender;
class	ENGINE_API	CTexture;
class	ENGINE_API	CMatrix;
class	ENGINE_API	CConstant;
class	ENGINE_API	CRT;
class	ENGINE_API	CRTC;

#pragma pack(push,4)

struct	ENGINE_API		STextureList	: public svector<CTexture*,8>
{
	u32	dwReference;
};
struct	ENGINE_API		SMatrixList		: public svector<CMatrix*,8>
{
	u32	dwReference;
};
struct	ENGINE_API		SConstantList	: public svector<CConstant*,8>
{
	u32	dwReference;
};
struct	ENGINE_API		SVS
{
	u32								dwReference;

	IDirect3DVertexShader9*			vs;
	R_constant_table				constants;
};
struct	ENGINE_API		SPS
{
	u32								dwReference;

	IDirect3DPixelShader9*			ps;
	R_constant_table				constants;
};
struct	ENGINE_API		SState
{
	u32								dwReference;

	IDirect3DStateBlock9*			state;
	SimulatorStates					state_code;
};
struct	ENGINE_API		SDeclaration
{
	u32								dwReference;

	IDirect3DVertexDeclaration9*	dcl;		// in fact, may be a topper-level thing, but duplicated here for usability
	xr_vector<D3DVERTEXELEMENT9>	dcl_code;
};
struct	ENGINE_API		SGeometry
{
	u32								dwReference;

	IDirect3DVertexDeclaration9*	dcl;		// in fact, may be a topper-level thing, but duplicated here for usability
	IDirect3DVertexBuffer9*			vb;
	IDirect3DIndexBuffer9*			ib;
	u32								vb_stride;
};
struct	ENGINE_API		SPass
{
	IDirect3DStateBlock9*			state;		// Generic state, like Z-Buffering, samplers, etc
	IDirect3DPixelShader9*			ps;			// may be NULL = FFP, in that case "state" must contain TSS setup
	IDirect3DVertexShader9*			vs;			// may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used

	R_constant_table*				constants;

	STextureList*					T;
	SMatrixList*					M;
	SConstantList*					C;

	BOOL							equal		(SPass& P);
};
struct ENGINE_API		ShaderElement
{
public:
	struct SFlags
	{
		u32	iPriority	:	2;
		u32	bStrictB2F	:	1;
		u32	bLighting	:	1;
	};
public:
	ShaderElement			();
public:
	u32						dwReference;
	SFlags					Flags;
	svector<SPass*,4>		Passes;

	BOOL					equal	(ShaderElement& S);
	BOOL					equal	(ShaderElement* S);
};
struct ENGINE_API		Shader
{
public:
	u32						dwReference;
	ShaderElement*			E		[4];	// R1 - 0=lod0,		1=lod1, 2=lighting, 3=*undefined*
											// R2 - 0=deffer,	1=dsm,	2=psm,		3=ssm		(or special usage)

	BOOL					equal	(Shader& S);
	BOOL					equal	(Shader* S);
};

//////////////////////////////////////////////////////////////////////////
// auto references
//////////////////////////////////////////////////////////////////////////
struct ENGINE_API		ref_shader
{
	Shader*				_object;

	ref_shader()	: _object(NULL)	{}
	~ref_shader()					{ destroy(); }

	void			create			(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void			create			(IBlender*	B,	LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void			destroy			();

	IC void			operator	=	(Shader* _o)	{ destroy();	_object=_o;		}
	IC Shader*		operator	()	()				{ return _object;				}
};

struct ENGINE_API		ref_geom
{
	SGeometry*			_object;

	ref_geom()		: _object(NULL)	{}
	~ref_geom()						{ destroy(); }

	void 			create			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void			create			(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void			destroy			();

	u32				stride			()				{ VERIFY(_object); return _object->vb_stride;	}

	void			operator	=	(SGeometry* _o)	{ destroy();	_object=_o;		}
	SGeometry*		operator	()	()				{ return _object;				}
};

#pragma pack(pop)

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)

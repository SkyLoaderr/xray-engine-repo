// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

#include "r_constants.h"
#include "tss_def.h"
#include "xr_resource.h"

typedef svector<string64,4>	sh_list;

class	ENGINE_API		CBlender_Compile;
class	ENGINE_API		IBlender;
class	ENGINE_API		CTexture;
class	ENGINE_API		CMatrix;
class	ENGINE_API		CConstant;
class	ENGINE_API		CRT;
class	ENGINE_API		CRTC;

#pragma pack(push,4)

// Atomic resources
struct	ENGINE_API		SVS				: public xr_resorce									{
	IDirect3DVertexShader9*				vs;
	R_constant_table					constants;
};
struct	ENGINE_API		SPS				: public xr_resorce									{
	IDirect3DPixelShader9*				ps;
	R_constant_table					constants;
};
struct	ENGINE_API		SState			: public xr_resorce									{
	IDirect3DStateBlock9*				state;
	SimulatorStates						state_code;
};

// Non-atomic, ref-counted resources
struct	ENGINE_API		STextureList	: public xr_resorce, public svector<CTexture*,8>	{
	void				_release_		(STextureList * ptr);
};
struct	ENGINE_API		SMatrixList		: public xr_resorce, public svector<CMatrix*,8>		{
	void				_release_		(SMatrixList * ptr);
};
struct	ENGINE_API		SConstantList	: public xr_resorce, public svector<CConstant*,8>	{
	void				_release_		(SConstantList * ptr);
};
struct	ENGINE_API		SDeclaration	: public xr_resorce									{
	IDirect3DVertexDeclaration9*		dcl;		// in fact, may be a topper-level thing, but duplicated here for usability
	xr_vector<D3DVERTEXELEMENT9>		dcl_code;
	void				_release_		(SDeclaration * ptr);
};
struct	ENGINE_API		SGeometry		: public xr_resorce									{
	IDirect3DVertexDeclaration9*		dcl;		// in fact, may be a topper-level thing, but duplicated here for usability
	IDirect3DVertexBuffer9*				vb;
	IDirect3DIndexBuffer9*				ib;
	u32									vb_stride;
	void				_release_		(SGeometry * ptr);
};
struct	ENGINE_API		SPass			: public xr_resorce									{
	IDirect3DStateBlock9*				state;		// Generic state, like Z-Buffering, samplers, etc
	IDirect3DPixelShader9*				ps;			// may be NULL = FFP, in that case "state" must contain TSS setup
	IDirect3DVertexShader9*				vs;			// may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used

	R_constant_table*					constants;

	STextureList*						T;
	SMatrixList*						M;
	SConstantList*						C;

	void				_release_		(SPass * ptr);
	BOOL				equal			(SPass& P);
};
struct ENGINE_API		ShaderElement	: public xr_resorce									{
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
	SFlags				Flags;
	svector<SPass*,4>	Passes;

	BOOL				equal			(ShaderElement& S);
	BOOL				equal			(ShaderElement* S);
	void				_release_		(ShaderElement * ptr);
};
struct ENGINE_API		Shader			: public xr_resorce									{
public:
	ShaderElement*			E		[4];	// R1 - 0=lod0,		1=lod1, 2=lighting, 3=*undefined*
											// R2 - 0=deffer,	1=dsm,	2=psm,		3=ssm		(or special usage)

	BOOL				equal			(Shader& S);
	BOOL				equal			(Shader* S);
	void				_release_		(Shader* ptr);
};

//////////////////////////////////////////////////////////////////////////
// auto references + helper functors
//////////////////////////////////////////////////////////////////////////
struct ENGINE_API		ref_shader		: public xr_resorce_ptr<Shader>
{
	void				create			(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void				create			(IBlender*	B,	LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void				destroy			()		{ this_type(NULL).swap(*this);		}
};
struct ENGINE_API		ref_geom		: public xr_resorce_ptr<SGeometry>
{
	void 				create			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void				create			(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void				destroy			()		{ this_type(NULL).swap(*this);		}
	u32					stride			()		{ VERIFY(p_); return p_->vb_stride;	}
};

#pragma pack(pop)

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)

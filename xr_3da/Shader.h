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

class	ENGINE_API			CBlender_Compile;
class	ENGINE_API			IBlender;
class	ENGINE_API			CTexture;
class	ENGINE_API			CMatrix;
class	ENGINE_API			CConstant;
class	ENGINE_API			CRT;
class	ENGINE_API			CRTC;

#pragma pack(push,4)

// Atomic resources
struct	ENGINE_API		SVS				: public xr_resorce									{
	IDirect3DVertexShader9*				vs;
	R_constant_table					constants;
	void				_release_		(SVS * ptr);
};
typedef	resptr_core<SVS,resptr_base<SVS> >	
	ref_vs;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SPS				: public xr_resorce									{
	IDirect3DPixelShader9*				ps;
	R_constant_table					constants;
	void				_release_		(SPS * ptr);
};
typedef	resptr_core<SPS,resptr_base<SPS> >	
	ref_ps;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SState			: public xr_resorce									{
	IDirect3DStateBlock9*				state;
	SimulatorStates						state_code;
	void				_release_		(SState * ptr);
};
typedef	resptr_core<SState,resptr_base<SState> >	
	ref_state;

// Non-atomic, ref-counted resources
struct	ENGINE_API		STextureList	: public xr_resorce, public svector<CTexture*,8>	{
	void				_release_		(STextureList * ptr);
};
typedef	resptr_core<STextureList,resptr_base<STextureList> >	
	ref_texture_list;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SMatrixList		: public xr_resorce, public svector<CMatrix*,8>		{
	void				_release_		(SMatrixList * ptr);
};
typedef	resptr_core<SMatrixList,resptr_base<SMatrixList> >	
	ref_matrix_list;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SConstantList	: public xr_resorce, public svector<CConstant*,8>	{
	void				_release_		(SConstantList * ptr);
};
typedef	resptr_core<SConstantList,resptr_base<SConstantList> >	
	ref_constant_list;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SDeclaration	: public xr_resorce									{
	IDirect3DVertexDeclaration9*		dcl;		// in fact, may be a topper-level thing, but duplicated here for usability
	xr_vector<D3DVERTEXELEMENT9>		dcl_code;
	void				_release_		(SDeclaration * ptr);
};
typedef	resptr_core<SDeclaration,resptr_base<SDeclaration> >	
	ref_declaration;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SGeometry		: public xr_resorce									{
	IDirect3DVertexDeclaration9*		dcl;		// in fact, may be a topper-level thing, but duplicated here for usability
	IDirect3DVertexBuffer9*				vb;
	IDirect3DIndexBuffer9*				ib;
	u32									vb_stride;
	void				_release_		(SGeometry * ptr);
};
struct ENGINE_API		resptrcode_geom		: public resptr_base<SGeometry>
{
	void 				create			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void				create			(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void				destroy			()			{ _set(NULL);		}
	u32					stride			()	const	{ return _get()->vb_stride;	}
};
typedef	resptr_core<SGeometry,resptrcode_geom>	ref_geom;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SPass			: public xr_resorce									{
	ref_state							state;		// Generic state, like Z-Buffering, samplers, etc
	ref_ps								ps;			// may be NULL = FFP, in that case "state" must contain TSS setup
	ref_vs								vs;			// may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used
	R_constant_table*					constants;

	ref_texture_list					T;
	ref_matrix_list						M;
	ref_constant_list					C;

	void				_release_		(SPass * ptr);
	BOOL				equal			(SPass& P);
};
typedef	resptr_core<SPass,resptr_base<SPass> >	
	ref_pass;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API		ShaderElement	: public xr_resorce									{
public:
	struct SFlags
	{
		u32	iPriority	:	2;
		u32	bStrictB2F	:	1;
		u32	bLighting	:	1;
	};
public:
	SFlags				Flags;
	svector<ref_pass,4>	Passes;

						ShaderElement	();
	BOOL				equal			(ShaderElement& S);
	BOOL				equal			(ShaderElement* S);
	void				_release_		(ShaderElement * ptr);
};
typedef	resptr_core<ShaderElement,resptr_base<ShaderElement> >	
	ref_selement;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API		Shader			: public xr_resorce									{
public:
	ref_selement		E		[4];	// R1 - 0=lod0,		1=lod1, 2=lighting, 3=*undefined*
										// R2 - 0=deffer,	1=dsm,	2=psm,		3=ssm		(or special usage)

	BOOL				equal			(Shader& S);
	BOOL				equal			(Shader* S);
	void				_release_		(Shader* ptr);
};
struct ENGINE_API		resptrcode_shader	: public resptr_base<Shader>
{
	void				create			(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void				create			(IBlender*	B,	LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void				destroy			()	{ _set(NULL);		}
};
typedef	resptr_core<Shader,resptrcode_shader>	ref_shader;

#pragma pack(pop)

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)

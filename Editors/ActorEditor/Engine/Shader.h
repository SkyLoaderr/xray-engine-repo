// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SHADER__INCLUDED_)
#define SHADER__INCLUDED_
#pragma once

#include "r_constants.h"
#include "xr_resource.h"

#include "sh_atomic.h"
#include "sh_texture.h"
#include "sh_matrix.h"
#include "sh_constant.h"
#include "sh_rt.h"

typedef svector<string64,4>	sh_list;

class	ENGINE_API			CBlender_Compile;
class	ENGINE_API			IBlender;

#pragma pack(push,4)

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		STextureList	: public xr_resource, public svector<ref_texture,8>	{
						~STextureList	();
};
typedef	resptr_core<STextureList,resptr_base<STextureList> >								ref_texture_list;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SMatrixList		: public xr_resource, public svector<ref_matrix,4>	{
						~SMatrixList	();
};
typedef	resptr_core<SMatrixList,resptr_base<SMatrixList> >									ref_matrix_list;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SConstantList	: public xr_resource, public svector<ref_constant,4>	{
						~SConstantList	();
};
typedef	resptr_core<SConstantList,resptr_base<SConstantList> >								ref_constant_list;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SGeometry		: public xr_resource									{
	ref_declaration						dcl;
	IDirect3DVertexBuffer9*				vb;
	IDirect3DIndexBuffer9*				ib;
	u32									vb_stride;
						~SGeometry		();
};
struct ENGINE_API		resptrcode_geom	: public resptr_base<SGeometry>
{
	void 				create			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void				create			(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void				destroy			()			{ _set(NULL);		}
	u32					stride			()	const	{ return _get()->vb_stride;	}
};
typedef	resptr_core<SGeometry,resptrcode_geom>												ref_geom;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SPass			: public xr_resource									{
	ref_state							state;		// Generic state, like Z-Buffering, samplers, etc
	ref_ps								ps;			// may be NULL = FFP, in that case "state" must contain TSS setup
	ref_vs								vs;			// may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used
	ref_ctable							constants;	// may be NULL

	ref_texture_list					T;
	ref_matrix_list						M;
	ref_constant_list					C;

						~SPass			();
	BOOL				equal			(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C);
};
typedef	resptr_core<SPass,resptr_base<SPass> >												ref_pass;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API		ShaderElement	: public xr_resource									{
public:
	struct SFlags
	{
		u32	iPriority	:	2;
		u32	bStrictB2F	:	1;
		u32	bLighting	:	1;
	};
public:
	SFlags				Flags;
	svector<ref_pass,2>	Passes;

						ShaderElement	();
						~ShaderElement	();
	BOOL				equal			(ShaderElement& S);
	BOOL				equal			(ShaderElement* S);
};
typedef	resptr_core<ShaderElement,resptr_base<ShaderElement> >								ref_selement;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API		Shader			: public xr_resource									{
public:
	ref_selement		E		[4];	// R1 - 0=lod0,		1=lod1, 2=lighting, 3=_undefined_
										// R2 - 0=deffer,	1=dsm,	2=psm,		3=ssm		(or special usage)
						~Shader			();
	BOOL				equal			(Shader& S);
	BOOL				equal			(Shader* S);
};
struct ENGINE_API		resptrcode_shader	: public resptr_base<Shader>
{
	void				create			(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void				create			(IBlender*	B,	LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void				destroy			()	{ _set(NULL);		}
};
typedef	resptr_core<Shader,resptrcode_shader>												ref_shader;

#pragma pack(pop)

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)

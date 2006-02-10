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

typedef xr_vector<shared_str>	sh_list;
class	ENGINE_API				CBlender_Compile;
class	ENGINE_API				IBlender;
#define	SHADER_PASSES_MAX		2

#pragma pack(push,4)

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		STextureList	: public xr_resource_flagged, public xr_vector<std::pair<u32,ref_texture> >	{
						~STextureList	();

						IC BOOL		equal (const STextureList& base) const
						{
							if (size()!=base.size())			return FALSE;
							for (u32 cmp=0; cmp<size(); cmp++)	{
								if ((*this)[cmp].first	!=base[cmp].first)	return FALSE;
								if ((*this)[cmp].second	!=base[cmp].second)	return FALSE;
							}
							return TRUE;
						}
};
typedef	resptr_core<STextureList,resptr_base<STextureList> >								ref_texture_list;
//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SMatrixList		: public xr_resource_flagged, public svector<ref_matrix,4>		{
						~SMatrixList	();
};
typedef	resptr_core<SMatrixList,resptr_base<SMatrixList> >									ref_matrix_list;
//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SConstantList	: public xr_resource_flagged, public svector<ref_constant_obsolette,4>	{
						~SConstantList	();
};
typedef	resptr_core<SConstantList,resptr_base<SConstantList> >								ref_constant_list;

//////////////////////////////////////////////////////////////////////////
struct	ENGINE_API		SGeometry		: public xr_resource_flagged									{
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
struct	ENGINE_API		SPass			: public xr_resource_flagged									{
	ref_state							state;		// Generic state, like Z-Buffering, samplers, etc
	ref_ps								ps;			// may be NULL = FFP, in that case "state" must contain TSS setup
	ref_vs								vs;			// may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used
	ref_ctable							constants;	// may be NULL

	ref_texture_list					T;
	ref_constant_list					C;
#ifdef _EDITOR
	ref_matrix_list						M;
#endif

						~SPass			();
	BOOL				equal			(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C);
};
typedef	resptr_core<SPass,resptr_base<SPass> >												ref_pass;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API		ShaderElement	: public xr_resource_flagged									{
public:
	struct Sflags
	{
		u32	iPriority	:	2;
		u32	bStrictB2F	:	1;
		u32	bEmissive	:	1;
		u32	bDistort	:	1;
		u32	bWmark		:	1;
	};
public:
	Sflags								flags;
	svector<ref_pass,SHADER_PASSES_MAX>	passes;

						ShaderElement	();
						~ShaderElement	();
	BOOL				equal			(ShaderElement& S);
	BOOL				equal			(ShaderElement* S);
};
typedef	resptr_core<ShaderElement,resptr_base<ShaderElement> >								ref_selement;

//////////////////////////////////////////////////////////////////////////
struct ENGINE_API		Shader			: public xr_resource_flagged									{
public:
	ref_selement		E		[6];	// R1 - 0=norm_lod0(det),	1=norm_lod1(normal),	2=L_point,		3=L_spot,	4=L_for_models,	
										// R2 - 0=deffer,			1=norm_lod1(normal),	2=psm,			3=ssm,		4=dsm
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

enum	SE_R1				{
	SE_R1_NORMAL_HQ			= 0,	// high quality/detail
	SE_R1_NORMAL_LQ			= 1,	// normal or low quality
	SE_R1_LPOINT			= 2,	// add: point light
	SE_R1_LSPOT				= 3,	// add:	spot light
	SE_R1_LMODELS			= 4,	// lighting info for models or shadowing from models
};

#pragma pack(pop)

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)

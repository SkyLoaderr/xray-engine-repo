// FBasicVisual.h: interface for the FBasicVisual class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "vis_common.h"

#define VLOAD_NOVERTICES		(1<<0)
#define VLOAD_NOINDICES			(1<<1)
#define VLOAD_FORCESOFTWARE		(1<<2)

// The class itself
class	ENGINE_API				CSkeletonAnimated;
class	ENGINE_API				CSkeletonRigid;
class	ENGINE_API				CKinematics;
class	ENGINE_API				IParticleCustom;

struct	ENGINE_API				IRender_Mesh	
{
	// format
	ref_geom					geom;

	// verts
	IDirect3DVertexBuffer9*		pVertices;
	u32							vBase;
	u32							vCount;

	// indices
	IDirect3DIndexBuffer9*		pIndices;
	u32							iBase;
	u32							iCount;
	u32							dwPrimitives;

	IRender_Mesh()				{ pVertices=0; pIndices=0;					}
	virtual ~IRender_Mesh()		{ _RELEASE(pVertices); _RELEASE(pIndices);	}
};

// The class itself
class	ENGINE_API				IRender_Visual
{
public:
#ifdef _EDITOR
    ogf_desc					desc		;
#endif
#ifdef DEBUG
	shared_str					dbg_name	;
#endif
public:
	// Common data for rendering
	u32							Type		;				// visual's type
	vis_data					vis			;				// visibility-data
	ref_shader					shader		;				// pipe state, shared

	virtual void				Render					(float LOD)		{};		// LOD - Level Of Detail  [0..1], Ignored
	virtual void				Render_Fast				(float LOD)		{};		// LOD - Level Of Detail  [0..1], Ignored
	virtual void				Load					(const char* N, IReader *data, u32 dwFlags);
	virtual void				Release					();						// Shared memory release
	virtual void				Copy					(IRender_Visual* from);
	virtual void				Spawn					()				{};

	virtual	CKinematics*		dcast_PKinematics		()				{ return 0;	}
	virtual	CSkeletonRigid*		dcast_PSkeletonRigid	()				{ return 0;	}
	virtual	CSkeletonAnimated*	dcast_PSkeletonAnimated	()				{ return 0;	}
	virtual IParticleCustom*	dcast_ParticleCustom	()				{ return 0;	}

	IRender_Visual				();
	virtual ~IRender_Visual		();
};

#endif // !defined(AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_)

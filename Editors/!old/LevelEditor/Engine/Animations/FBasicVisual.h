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

// The class itself
class	ENGINE_API				IRender_Visual
{
public:
#ifdef _EDITOR
    ogf_desc					desc;
#endif
#ifdef DEBUG
	ref_str						dbg_name;
#endif
public:
	u32							Type;					// visual's type
	vis_data					vis;

	// Common data for rendering
	ref_shader					hShader;				// pipe state, shared
	ref_geom					hGeom;					// vertex shader, shared

	virtual void				Render					(float LOD)		{};		// LOD - Level Of Detail  [0..1], Ignored
	virtual void				Load					(const char* N, IReader *data, u32 dwFlags);
	virtual void				Release					();						// Shared memory release
	virtual void				Copy					(IRender_Visual* from);
	virtual void				Spawn					()				{};

	virtual	CKinematics*		dcast_PKinematics		()				{ return 0;	}
	virtual	CSkeletonRigid*		dcast_PSkeletonRigid	()				{ return 0;	}
	virtual	CSkeletonAnimated*	dcast_PSkeletonAnimated	()				{ return 0;	}
	virtual IParticleCustom*	dcast_ParticleCustom	()				{ return 0;	}

	IRender_Visual			();
	virtual ~IRender_Visual();
};

#endif // !defined(AFX_FBASICVISUAL_H__2DCBE980_AF27_11D3_B4E3_4854E82A090D__INCLUDED_)

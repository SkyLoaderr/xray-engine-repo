// SkeletonX.h: interface for the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SkeletonXH
#define SkeletonXH
#pragma once

#include "FVisual.h"
#include "FProgressiveFixedVisual.h"

// refs
class ENGINE_API CKinematics;

#pragma pack(push,4)
struct vertBoned1W	// (3+3+2+1)*4 = 9*4 = 36 bytes
{
	Fvector	P;
	Fvector	N;
	float	u,v;
	u32		matrix;
};
struct vertBoned2W	// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	u16		matrix0;
	u16		matrix1;
	Fvector	P0;
	Fvector	N0;
	Fvector	P1;
	Fvector	N1;
	float	w;
	float	u,v;
};
struct vertRender
{
	Fvector	P;
	Fvector	N;
	float	u,v;
};
#pragma pack(pop)

class ENGINE_API CSkeletonX
{
protected:
	enum					{ vertRenderFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1		};
	enum					{ RM_SKINNING_SOFT, RM_SINGLE, RM_SKINNING_1B, RM_SKINNING_2B	};

	CKinematics*			Parent;			// setted up by parent
	ref_smem<vertBoned1W>	Vertices1W;		// shared
	ref_smem<vertBoned2W>	Vertices2W;		// shared

	// soft-skinning only
	u32						cache_DiscardID;
	u32						cache_vCount;
	u32						cache_vOffset;

	// render-mode specifics
	u16						RenderMode;		
	u16						RMS_boneid;		// single-bone-rendering
	u16						RMS_bonecount;	// skinning, maximal bone ID
	
	void					_Copy			(CSkeletonX *V);
	void					_Render_soft	(ref_geom& hGeom,	u32 vCount,		u32 pCount);
	void					_Render			(ref_geom& hGeom,	u32 vCount,		u32 pCount);
	void					_Load			(const char* N,		IReader *data,	u32& dwVertCount);
	void					_Load_hw		(Fvisual& V);
public:
	virtual void			SetParent		(CKinematics* K) { Parent = K; }

	CSkeletonX()
	{
		Parent		= 0;
	}
};

class ENGINE_API CSkeletonX_ST	: public Fvisual, public CSkeletonX
{
private:
	typedef Fvisual	inherited;
public:
	virtual void			Render			(float LOD);
	virtual void			Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void			Copy			(IRender_Visual *pFrom);
	virtual void			Release			();
};

class ENGINE_API CSkeletonX_PM	: public FProgressiveFixedVisual, public CSkeletonX
{
private:
	typedef FProgressiveFixedVisual	inherited;
	u16*					indices;
public:
	CSkeletonX_PM()	: indices(0) {};
	
	virtual void			Render			(float LOD);
	virtual void			Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void			Copy			(IRender_Visual *pFrom);
	virtual void			Release			();
};

#endif // SkeletonXH

// SkeletonX.h: interface for the CSkeletonX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef		SkeletonXH_R
#define		SkeletonXH_R
#pragma		once

#include	"FVisual.h"
#include	"FProgressive.h"
#include	"SkeletonX.h"

class		CSkeletonX_ext	: public CSkeletonX	// shared code for SkeletonX derivates
{
protected:
	void					_Load_hw			(Fvisual& V,		void *		data);
	void					_CollectBoneFaces	(Fvisual* V,		u32 iBase,	u32 iCount);
	void					_FillVerticesHW1W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces);
	void					_FillVerticesHW2W	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16* indices, CBoneData::FacesVec& faces);
	void					_FillVertices		(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, Fvisual* V, u16 bone_id, u32 iBase, u32 iCount);

	BOOL					_PickBoneHW1W		(Fvector& normal, float& range, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces);
	BOOL					_PickBoneHW2W		(Fvector& normal, float& range, const Fvector& S, const Fvector& D, Fvisual* V, u16* indices, CBoneData::FacesVec& faces);
	BOOL					_PickBone			(Fvector& normal, float& range, const Fvector& S, const Fvector& D, Fvisual* V, u16 bone_id, u32 iBase, u32 iCount);
public:
};

class CSkeletonX_ST			: public Fvisual, public CSkeletonX_ext
{
private:
	typedef Fvisual			inherited1;
	typedef CSkeletonX_ext	inherited2;
public:
	virtual void			Render			(float LOD);
	virtual void			Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void			Copy			(IRender_Visual *pFrom);
	virtual void			Release			();
	virtual void			AfterLoad		(CKinematics* parent, u16 child_idx);
	virtual BOOL			PickBone		(Fvector& normal, float& dist, const Fvector& start, const Fvector& dir, u16 bone_id);
	virtual void			FillVertices	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id);
};

class CSkeletonX_PM			: public FProgressive, public CSkeletonX_ext
{
private:
	typedef FProgressive	inherited1;
	typedef CSkeletonX_ext	inherited2;
public:
	virtual void			Render			(float LOD);
	virtual void			Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void			Copy			(IRender_Visual *pFrom);
	virtual void			Release			();
	virtual void			AfterLoad		(CKinematics* parent, u16 child_idx);
	virtual BOOL			PickBone		(Fvector& normal, float& dist, const Fvector& start, const Fvector& dir, u16 bone_id);
	virtual void			FillVertices	(const Fmatrix& view, CSkeletonWallmark& wm, const Fvector& normal, float size, u16 bone_id);
};

#endif // SkeletonXH

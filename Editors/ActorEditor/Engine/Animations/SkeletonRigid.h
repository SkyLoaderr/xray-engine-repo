//---------------------------------------------------------------------------
#ifndef SkeletonRigidH
#define SkeletonRigidH

#include "SkeletonCustom.h"

class CBoneDataRigid: public CBoneData
{
public:
						CBoneDataRigid	(u16 ID):CBoneData(ID){}
	// Calculation                                                          
	virtual void		Calculate		(CSkeletonCustom* K, Fmatrix *Parent);
};

class CBoneInstanceRigid: public CBoneInstance
{
public:
	void				construct		();
};

class ENGINE_API	CSkeletonRigid	: public CSkeletonCustom
{
protected:
    virtual CBoneData*			CreateBoneData	(u16 ID){return xr_new<CBoneDataRigid>(ID);}
public:
	// Main functionality
	virtual void				Calculate		(BOOL bLight=FALSE);			// Recalculate skeleton (Light mode can be used to avoid interpolation)
};
IC CSkeletonRigid* PSkeletonRigid(IRender_Visual* V) { return dynamic_cast<CSkeletonRigid*>(V); }


//---------------------------------------------------------------------------
#endif

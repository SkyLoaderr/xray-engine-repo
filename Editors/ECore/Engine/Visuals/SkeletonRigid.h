//---------------------------------------------------------------------------
#ifndef SkeletonRigidH
#define SkeletonRigidH

#include "SkeletonCustom.h"

class ENGINE_API	CBoneDataRigid		: public CBoneData
{
public:
						CBoneDataRigid	(u16 ID):CBoneData(ID){}
	// Calculation                                                          
	virtual void		Calculate		(CKinematics* K, Fmatrix *Parent);
};

class ENGINE_API	CBoneInstanceRigid	: public CBoneInstance
{
public:
	void				construct		();
};

class ENGINE_API	CSkeletonRigid		: public CKinematics
{
protected:
    virtual CBoneData*			CreateBoneData	(u16 ID){return xr_new<CBoneDataRigid>(ID);}
public:
	// Main functionality
	virtual void				Calculate		(BOOL bForced=FALSE);
	virtual	CSkeletonRigid*		dcast_PSkeletonRigid	()				{ return this;	}
};
IC CSkeletonRigid* PSkeletonRigid(IRender_Visual* V)	{ return V?V->dcast_PSkeletonRigid():0; }

//---------------------------------------------------------------------------
#endif

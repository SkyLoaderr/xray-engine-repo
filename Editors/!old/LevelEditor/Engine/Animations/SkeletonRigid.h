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
	virtual u32			mem_usage		()
	{
		return			CBoneData::mem_usage()+sizeof(*this);
	}
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
	virtual void				CalculateBones			(BOOL bForced=FALSE);
	virtual	CSkeletonRigid*		dcast_PSkeletonRigid	()				{ return this;	}
	virtual u32					mem_usage			(bool bInstance)
	{
		u32 sz					= CKinematics::mem_usage(bInstance)+sizeof(*this);
		return sz;
	}
};
IC CSkeletonRigid* PSkeletonRigid(IRender_Visual* V)	{ return V?V->dcast_PSkeletonRigid():0; }

//---------------------------------------------------------------------------
#endif

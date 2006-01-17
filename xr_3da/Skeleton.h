// Skeleton.h: interface for the CSkeleton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKELETON_H__D325402D_E714_4F39_99AF_3517F8ABF892__INCLUDED_)
#define AFX_SKELETON_H__D325402D_E714_4F39_99AF_3517F8ABF892__INCLUDED_
#pragma once

//refs
class  CSkeletonBody;
class  CSkeletonMotions;
class  CMotion;
struct gePose;
struct geMotion;

class CSkeleton
{
public:
	CSkeletonMotions*	pSkeletonMotions;
	gePose				*pPose;

	vector<CMotion*>	BlendedMotions;
	CMotion*			pCurrentMotion;
public:
						CSkeleton			( char *fname );
	virtual				~CSkeleton			();
	void				OnMove				();

	void				PlayMotion			(const char* name, BOOL bLoop = false);
	void				PlayMotion			(int id, BOOL bLoop = false);
//	void				StopMotions			();

	BOOL 				Attach				( CSkeleton* pSkeleton, const char* SlaveBoneName, const char* MasterBoneName );
	void				Detach				( CSkeleton* pSkeleton );
};

#endif // !defined(AFX_SKELETON_H__D325402D_E714_4F39_99AF_3517F8ABF892__INCLUDED_)

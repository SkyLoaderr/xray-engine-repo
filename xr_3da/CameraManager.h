// CameraManager.h: interface for the CCameraManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "CameraDefs.h"

DEFINE_VECTOR				(CEffector*,EffectorVec,EffectorIt);

class ENGINE_API CCameraManager
{
	Fvector					vPosition;
	Fvector					vDirection;
	Fvector					vNormal;
	Fvector					vRight;

	EffectorVec				m_Effectors;

	float					fFov;
	float					fFar;
	float					fAspect;
	
	Fmatrix					unaffected_mView;
	Fvector					unaffected_vPosition;
	Fvector					unaffected_vDirection;
	Fvector					unaffected_vNormal;
	Fvector					unaffected_vRight;
	
	Fvector					affected_vPosition;
	Fvector					affected_vDirection;
	Fvector					affected_vNormal;
	Fvector					affected_vRight;
public:
	void					Dump				(void);
	CEffector*				AddEffector			(CEffector* ef);
	CEffector*				GetEffector			(EEffectorType type);
	void					RemoveEffector		(EEffectorType type);

	IC Fmatrix&				unaffected_View		()	{ return unaffected_mView;		}
	IC Fvector&				unaffected_Pos		()	{ return unaffected_vPosition;	}
	IC Fvector&				unaffected_Dir		()	{ return unaffected_vDirection;	}
	IC Fvector&				unaffected_Up		()	{ return unaffected_vNormal;	}
	IC Fvector&				unaffected_Right	()	{ return unaffected_vRight;		}
	IC void					unaffected_Matrix	(Fmatrix& M)	
	{	M.set(unaffected_vRight,unaffected_vNormal,unaffected_vDirection,unaffected_vPosition);	}

	IC Fvector&				affected_Pos		()	{ return affected_vPosition;	}
	IC Fvector&				affected_Dir		()	{ return affected_vDirection;	}
	IC Fvector&				affected_Up			()	{ return affected_vNormal;		}
	IC Fvector&				affected_Right		()	{ return affected_vRight;		}
	IC void					affected_Matrix		(Fmatrix& M)	
	{	M.set(affected_vRight,affected_vNormal,affected_vDirection,affected_vPosition);	}

	IC Fmatrix&				render_View			()	{ return Device.mView; }
	IC Fvector&				render_Pos			()	{ return vPosition;	}
	IC Fvector&				render_Dir			()	{ return vDirection;}
	IC Fvector&				render_Up			()	{ return vNormal;	}
	IC Fvector&				render_Right		()	{ return vRight;	}
	IC void					render_Matrix		(Fmatrix& M)	
	{	M.set(vRight,vNormal,vDirection,vPosition);	}

	void					Update				(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest);
	void					Update				(const CCameraBase* C);

	CCameraManager();
	~CCameraManager();
};

extern float psCamInert;
extern float psCamSlideInert;

#endif // !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)

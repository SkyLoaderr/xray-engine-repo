// CameraManager.h: interface for the CCameraManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "CameraDefs.h"

class ENGINE_API CCameraManager
{
	Fvector					vPosition;
	Fvector					vDirection;
	Fvector					vNormal;

	CEffector*				pEffector;

	float					fFov;
	float					fFar;
	
	Fmatrix					unaffected_mView;
	Fvector					unaffected_vPosition;
	Fvector					unaffected_vDirection;
	Fvector					unaffected_vNormal;
public:
	void					Dump				(void);
	void					SetEffector			(CEffector *pe);
	IC CEffector*			GetEffector			()	{ return pEffector;				}

	IC Fmatrix&				unaffected_View		()	{ return unaffected_mView;		}
	IC Fvector&				unaffected_Pos		()	{ return unaffected_vPosition;	}
	IC Fvector&				unaffected_Dir		()	{ return unaffected_vDirection;	}
	IC Fvector&				unaffected_Up		()	{ return unaffected_vNormal;	}

	void					Update				(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fFAR_Dest);
	void					Update				(const CCameraBase* C);

	CCameraManager();
	~CCameraManager();
};

extern float psCamInert;
extern float psCamSlideInert;

#endif // !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)

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
public:
	void					Dump				(void);
	void					SetEffector			(CEffector *pe);

	void					Update				(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fFAR_Dest);
	void					Update				(const CCameraBase* C);

	CCameraManager();
	~CCameraManager();
};

extern float psCamInert;
extern float psCamSlideInert;

#endif // !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)

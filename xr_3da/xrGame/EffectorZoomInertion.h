// EffectorZoomInertion.h: �������(�����������) ������ � ������ 
//						   �����������	
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"

class CEffectorZoomInertion : public CCameraEffector
{
	//����������� �������� "�����������" �������
	float	m_fFloatSpeed;
	float	m_fDispRadius;

	float	m_fEpsilon;
	Fvector	m_vCurrentPoint;
	Fvector	m_vTargetPoint;
	Fvector	m_vTargetVel;

	Fvector m_vOldCameraDir;

	u32		m_dwTimePassed;

	//��������� ��������� ���������
	float	m_fCameraMoveEpsilon;
	float	m_fDispMin;
	float	m_fSpeedMin;
	float	m_fZoomAimingDispK;
	float	m_fZoomAimingSpeedK;
	//����� ����� ������� �������� ������ ����������� ��������
	u32		m_dwDeltaTime;
public:
	CEffectorZoomInertion				();
	virtual ~CEffectorZoomInertion		();

			void	Load				();
			void	SetParams			(float disp);

	virtual	BOOL	Process				(Fvector &delta_p, Fvector &delta_d,
											Fvector &delta_n, float& fFov,
											float& fFar, float& fAspect);

};
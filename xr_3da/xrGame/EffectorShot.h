// EffectorShot.h: interface for the CEffectorShot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTORSHOT_H__B8CC6918_45D6_485D_8BB4_5A94CEEEFC2E__INCLUDED_)
#define AFX_EFFECTORSHOT_H__B8CC6918_45D6_485D_8BB4_5A94CEEEFC2E__INCLUDED_
#pragma once

#include "..\effector.h"

class CEffectorShot : public CEffector  
{
	Fvector	vDispersionDir;
	float	fAngleCurrent;
	float	fRelaxSpeed;
	float	fMaxAngle;
public:
	CEffectorShot			(float max_angle, float relax_time);
	virtual ~CEffectorShot	();
	virtual	BOOL	Process	(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n);

	void	Shot			(float angle);
};

#endif // !defined(AFX_EFFECTORSHOT_H__B8CC6918_45D6_485D_8BB4_5A94CEEEFC2E__INCLUDED_)

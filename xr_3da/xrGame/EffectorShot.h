// EffectorShot.h: interface for the CEffectorShot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTORSHOT_H__B8CC6918_45D6_485D_8BB4_5A94CEEEFC2E__INCLUDED_)
#define AFX_EFFECTORSHOT_H__B8CC6918_45D6_485D_8BB4_5A94CEEEFC2E__INCLUDED_
#pragma once

#include "..\effector.h"

class CEffectorShot : public CEffector  
{
	Fvector	vDirectionDiff;
	float	fTimeCurrent;
	float	fTimeTotal;
	float	fAngleCurrent;
	float	fAngleTotal;
public:
	CEffectorShot			(float relax_time, float angle);
	virtual ~CEffectorShot	();
	virtual	void	Process	(Fvector &p, Fvector &d, Fvector &n);

	void	Shot			();
};

#endif // !defined(AFX_EFFECTORSHOT_H__B8CC6918_45D6_485D_8BB4_5A94CEEEFC2E__INCLUDED_)

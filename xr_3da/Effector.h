// Effector.h: interface for the CEffector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTOR_H__E8300C00_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_)
#define AFX_EFFECTOR_H__E8300C00_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_

#pragma once

class ENGINE_API CEffector
{
protected:
	friend class	CCameraManager;
	float			fLifeTime;
public:
	CEffector		(float tm) {fLifeTime=tm;};
	virtual ~CEffector() {};
	virtual	void	Process(Fvector &p, Fvector &d, Fvector &n)	= 0;
};

#endif // !defined(AFX_EFFECTOR_H__E8300C00_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_)

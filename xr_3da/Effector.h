// Effector.h: interface for the CEffector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTOR_H__E8300C00_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_)
#define AFX_EFFECTOR_H__E8300C00_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_

#pragma once

#include "CameraDefs.h"

class ENGINE_API		CEffector
{
protected:
	EEffectorType		eType;
	BOOL				bAffected;
	
	friend class		CCameraManager;
	float				fLifeTime;
public:
						CEffector	(EEffectorType type, float tm, BOOL affected) {eType = type; fLifeTime=tm; bAffected=affected;};
	virtual				~CEffector	() {};
	IC EEffectorType	GetType		() {return eType;}
	IC BOOL				Affected	() {return bAffected;}
	IC float			LifeTime	() {return fLifeTime;}

	virtual	BOOL		Process		(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)= 0;
};

#endif // !defined(AFX_EFFECTOR_H__E8300C00_1C3E_11D4_B4E3_00C02610C34E__INCLUDED_)

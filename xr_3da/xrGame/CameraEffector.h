// CameraEffector.h: interface for the CCameraEffector class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../CameraDefs.h"

class CCameraEffector
{
protected:
	EEffectorType		eType;
	BOOL			bAffected;
	float			fLifeTime;
public:
	CCameraEffector						(EEffectorType type, float tm, BOOL affected) {eType = type; fLifeTime=tm; bAffected=affected;};
	virtual			~CCameraEffector	() {};
	IC EEffectorType	GetType			() {return eType;}
	IC BOOL				Affected		() {return bAffected;}
	IC float			LifeTime		() {return fLifeTime;}


	virtual	BOOL		Process			(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)= 0;
};
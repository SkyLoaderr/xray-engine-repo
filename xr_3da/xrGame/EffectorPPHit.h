#ifndef _EFFECTOR_PP_HIT_H
#define _EFFECTOR_PP_HIT_H
#pragma once

#include "..\effectorPP.h"

class CEffectorPPHit : public CEffectorPP
{
	float	fTime;
	float	fSpeed;
public:
			CEffectorPPHit		();
	virtual ~CEffectorPPHit		();
	virtual	BOOL	Process		(SPPInfo& pp);
};

#endif //_EFFECTOR_PP_HIT_H

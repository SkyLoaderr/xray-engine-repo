#pragma once

#include "../../effectorPP.h"
#include "../CameraEffector.h"
#include "../../cameramanager.h"

//////////////////////////////////////////////////////////////////////////
// Vampire Postprocess Effector
//////////////////////////////////////////////////////////////////////////
class CVampirePPEffector : public CEffectorPP {
	typedef CEffectorPP inherited;	

	//SPPInfo state;			// current state
	//float	m_total;		// total PP time
	//float	m_attack;		// attack time in percents	[0..1]
	//float	m_release;		// release time in percents	[0..1]

public:
					CVampirePPEffector		(const SPPInfo &ppi, float life_time);
	virtual	BOOL	Process					(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////
class CVampireCameraEffector : public CCameraEffector {
	typedef CCameraEffector inherited;	

	float	m_time_total;
	float	m_cam_dist;
	float	m_threshold_dist;
	Fvector	dangle_target;
	Fvector dangle_current;

public:
					CVampireCameraEffector	(float time, float dist, float threshold_dist);
	virtual	BOOL	Process					(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};




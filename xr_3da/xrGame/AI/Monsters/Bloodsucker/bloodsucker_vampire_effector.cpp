#include "stdafx.h"
#include "bloodsucker_vampire_effector.h"

//////////////////////////////////////////////////////////////////////////
// Vampire Postprocess Effector
//////////////////////////////////////////////////////////////////////////

CVampirePPEffector::CVampirePPEffector(const SPPInfo &ppi, float life_time) :
	inherited(EEffectorPPType(0), life_time)
{

}

BOOL CVampirePPEffector::Process(SPPInfo& pp)
{
	inherited::Process(pp);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////
#define DELTA_ANGLE_X	10 * PI / 180
#define DELTA_ANGLE_Y	DELTA_ANGLE_X
#define DELTA_ANGLE_Z	DELTA_ANGLE_X
#define ANGLE_SPEED		0.2f	

CVampireCameraEffector::CVampireCameraEffector(float time, float dist, float threshold_dist) :
	inherited(ECameraEffectorType(1), time, TRUE)
{
	fLifeTime			= time;
	m_time_total		= time;

	m_cam_dist			= dist;
	m_threshold_dist	= threshold_dist;

	dangle_target.set	(Random.randFs(DELTA_ANGLE_X),Random.randFs(DELTA_ANGLE_Y),Random.randFs(DELTA_ANGLE_Z));
	dangle_current.set	(0.f, 0.f, 0.f);
}

BOOL CVampireCameraEffector::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;

	// процент оставшегося времени
	float time_left_perc = fLifeTime / m_time_total;

	// Инициализация
	Fmatrix	Mdef;
	Mdef.identity		();
	Mdef.j.set			(n);
	Mdef.k.set			(d);
	Mdef.i.crossproduct	(n,d);
	Mdef.c.set			(p);

	
	//////////////////////////////////////////////////////////////////////////
	// using formula: y = k - 2*k*abs(x-1/2)   k - max distance
	float	cur_dist = m_cam_dist * (1 - 2*_abs((1-time_left_perc) - 0.5f));
	if (cur_dist > m_threshold_dist) cur_dist = m_threshold_dist;
	
	Mdef.c.mad(Mdef.k, cur_dist);

	if (angle_lerp(dangle_current.x, dangle_target.x, ANGLE_SPEED, Device.fTimeDelta)) {
		dangle_target.x = Random.randFs(DELTA_ANGLE_X);
	}
	
	if (angle_lerp(dangle_current.y, dangle_target.y, ANGLE_SPEED, Device.fTimeDelta)) {
		dangle_target.y = Random.randFs(DELTA_ANGLE_Y);
	}
	
	if (angle_lerp(dangle_current.z, dangle_target.z, ANGLE_SPEED, Device.fTimeDelta)) {
		dangle_target.z = Random.randFs(DELTA_ANGLE_Z);
	}

	if (time_left_perc > 0.5) {
		//fFar	= 100.f;
		//fAspect	= 1.f;
	} else {
		fFar	= 50.f;
		//fAspect	= 0.5f;
	}

	//////////////////////////////////////////////////////////////////////////

	// Установить углы смещения
	Fmatrix		R;
	R.setHPB	(dangle_current.x,dangle_current.y,dangle_current.z);

	Fmatrix		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);
	p.set		(mR.c);

	return TRUE;
}


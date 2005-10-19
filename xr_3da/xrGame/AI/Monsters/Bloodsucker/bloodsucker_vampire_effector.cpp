#include "stdafx.h"
#include "bloodsucker_vampire_effector.h"

//////////////////////////////////////////////////////////////////////////
// Vampire Postprocess Effector
//////////////////////////////////////////////////////////////////////////

CVampirePPEffector::CVampirePPEffector(const SPPInfo &ppi, float life_time) :
	inherited(EEffectorPPType(eCEHit), life_time)
{
	state		= ppi;
	m_total		= life_time;
}

#define TIME_ATTACK		0.2f
#define PERIODS			2			
#define RAD_TO_PERC(rad)	((rad - PI_DIV_2) / (PERIODS * PI_MUL_2))
#define PERC_TO_RAD(perc)	(perc * (PERIODS * PI_MUL_2) + PI_DIV_2)

BOOL CVampirePPEffector::Process(SPPInfo& pp)
{
	inherited::Process(pp);

	// amount of time passed in percents
	float time_past_perc = (m_total - fLifeTime) / m_total;
	
	float factor;
	if (time_past_perc < TIME_ATTACK) {
		factor = 0.75f * time_past_perc / TIME_ATTACK;
	} else if (time_past_perc > (1 - TIME_ATTACK)) {
		factor = 0.75f * (1-time_past_perc) / TIME_ATTACK;
	} else {	
		float time_past_sine_perc = (time_past_perc - TIME_ATTACK) * (1 / ( 1 - TIME_ATTACK + TIME_ATTACK));
		factor = 0.5f + 0.25f * _sin(PERC_TO_RAD(time_past_sine_perc));
	}
	
	clamp(factor,0.01f,1.0f);

	SPPInfo	def;

	pp.duality.h		+= def.duality.h		+ (state.duality.h			- def.duality.h)		* factor; 			
	pp.duality.v		+= def.duality.v		+ (state.duality.v			- def.duality.v)		* factor;
	pp.gray				+= def.gray				+ (state.gray				- def.gray)				* factor;
	pp.blur				+= def.blur				+ (state.blur				- def.blur)				* factor;
	pp.noise.intensity	+= def.noise.intensity	+ (state.noise.intensity	- def.noise.intensity)	* factor;
	pp.noise.grain		+= def.noise.grain		+ (state.noise.grain		- def.noise.grain)		* factor;
	pp.noise.fps		+= def.noise.fps		+ (state.noise.fps			- def.noise.fps)		* factor;	

	pp.color_base.set	(
		def.color_base.r	+ (state.color_base.r - def.color_base.r) * factor, 
		def.color_base.g	+ (state.color_base.g - def.color_base.g) * factor, 
		def.color_base.b	+ (state.color_base.b - def.color_base.b) * factor
		);

	pp.color_gray.set	(
		def.color_gray.r	+ (state.color_gray.r - def.color_gray.r) * factor, 
		def.color_gray.g	+ (state.color_gray.g - def.color_gray.g) * factor, 
		def.color_gray.b	+ (state.color_gray.b - def.color_gray.b) * factor
		);

	pp.color_add.set	(
		def.color_add.r	+ (state.color_add.r - def.color_add.r) * factor, 
		def.color_add.g	+ (state.color_add.g - def.color_add.g) * factor, 
		def.color_add.b	+ (state.color_add.b - def.color_add.b) * factor
	);


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
	inherited(ECameraEffectorType(1), time)
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

	// ������� ����������� �������
	float time_left_perc = fLifeTime / m_time_total;

	// �������������
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

	
	// check the time to return
	if (time_left_perc < 0.2f) {

		dangle_target.x = 0.f;
		dangle_target.y = 0.f;
		dangle_target.z = 0.f;

		angle_lerp(dangle_current.x, dangle_target.x, _abs(dangle_current.x / fLifeTime + 0.001f), Device.fTimeDelta);
		angle_lerp(dangle_current.y, dangle_target.y, _abs(dangle_current.y / fLifeTime + 0.001f), Device.fTimeDelta);
		angle_lerp(dangle_current.z, dangle_target.z, _abs(dangle_current.z / fLifeTime + 0.001f), Device.fTimeDelta);

	} else {
		
		if (angle_lerp(dangle_current.x, dangle_target.x, ANGLE_SPEED, Device.fTimeDelta)) {
			dangle_target.x = Random.randFs(DELTA_ANGLE_X);
		}

		if (angle_lerp(dangle_current.y, dangle_target.y, ANGLE_SPEED, Device.fTimeDelta)) {
			dangle_target.y = Random.randFs(DELTA_ANGLE_Y);
		}

		if (angle_lerp(dangle_current.z, dangle_target.z, ANGLE_SPEED, Device.fTimeDelta)) {
			dangle_target.z = Random.randFs(DELTA_ANGLE_Z);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	// ���������� ���� ��������
	Fmatrix		R;
	R.setHPB	(dangle_current.x,dangle_current.y,dangle_current.z);

	Fmatrix		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);
	p.set		(mR.c);

	return TRUE;
}


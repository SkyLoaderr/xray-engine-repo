#include "stdafx.h"
#include "explode_effector.h" 

CExplodeEffector::CExplodeEffector(float time, float amp, float periods, float power) 
		: CCameraEffector(ECameraEffectorType(EXPLODE_EFFECTOR_TYPE_ID), time, FALSE)
{
	total			= time;

	max_amp			= amp * power;
	period_number	= periods;
	this->power		= power;
}

BOOL CExplodeEffector::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	fLifeTime -= Device.fTimeDelta; if(fLifeTime<0) return FALSE;

	// ������� ����������� �������
	float time_left_perc = fLifeTime / total;

	// �������������
	Fmatrix	Mdef;
	Mdef.identity		();
	Mdef.j.set			(n);
	Mdef.k.set			(d);
	Mdef.i.crossproduct	(n,d);
	Mdef.c.set			(p);

	float period_all	= period_number * PI_MUL_2;		// ����. �������� �����
	float k				= 1 - time_left_perc + EPS_L + (1 - power);
	float cur_amp		= max_amp * (PI / 180) / (10 * k * k);

	Fvector dangle; 
	dangle.x = cur_amp/2	* _sin(period_all	* (1.0f - time_left_perc));
	dangle.y = cur_amp		* _cos(period_all/2 * (1.0f - time_left_perc));
	dangle.z = cur_amp/4	* _sin(period_all/4	* (1.0f - time_left_perc));

	// ���������� ���� ��������
	Fmatrix		R;
	R.setHPB	(dangle.x,dangle.y,dangle.z);

	Fmatrix		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);

	return TRUE;
}



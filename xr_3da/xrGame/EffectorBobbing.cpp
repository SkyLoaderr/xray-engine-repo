#include "stdafx.h"
#include "EffectorBobbing.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorBobbing::CEffectorBobbing	(float relax_time, float angle) : CEffector(cefBobbing,10000.f)
{
	fTimeCurrent	= -1;
	fTimeTotal		= relax_time;
	fAngleCurrent	= 0;
	fAngleTotal		= angle;
}

CEffectorBobbing::~CEffectorBobbing	()
{

}

void CEffectorBobbing::Shot		()
{
	Fvector	axis; axis.set		(0,0,1);
	vDirectionDiff.random_dir	();
	fTimeCurrent				= fTimeTotal;
	fAngleCurrent				= ::Random.randF(fAngleTotal/2,fAngleTotal);
}

void CEffectorBobbing::Process		(Fvector &p, Fvector &d, Fvector &n)
{
	fTimeCurrent	-= Device.fTimeDelta;
	if (fTimeCurrent<0)		return;

	float	angle	= fAngleCurrent*(fTimeCurrent/fTimeTotal);
	d.mad			(vDirectionDiff,tanf(angle));

	fLifeTime-=Device.fTimeDelta;
}

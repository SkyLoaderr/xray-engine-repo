// EffectorRecoil.cpp: implementation of the CEffectorRecoil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorRecoil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorRecoil::CEffectorRecoil(float power, float life_time) : CEffector(cefRecoil,life_time)
{
	fPower	= power;
	fTime	= Recoil_TIME;
}

CEffectorRecoil::~CEffectorRecoil()
{

}

void CEffectorRecoil::Process(Fvector &p, Fvector &d, Fvector &n)
{
	fTime-=Device.fTimeDelta;
	float dist = fPower*fTime/Recoil_TIME;
	p.direct(p,d,-dist);
	d.y += dist;
	d.z += dist;
	d.normalize();

	Fvector	right;
	right.crossproduct(n,d);
	n.crossproduct(d,right);
	n.normalize();

	fLifeTime-=Device.fTimeDelta;
}

#include "stdafx.h"
#include "EffectorPPHit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorPPHit::CEffectorPPHit() : CEffectorPP(cefppHit,10000.f)
{
	fTime			= 0;
	fSpeed			= 10.f;
}

CEffectorPPHit::~CEffectorPPHit	()
{
}

BOOL CEffectorPPHit::Process	(SPPInfo& pp)
{
	fTime			+= fSpeed*Device.fTimeDelta;
	//pp.duality.h	+= _sin(fTime);
	//pp.duality.v	+= _cos(fTime);
	return TRUE;
}

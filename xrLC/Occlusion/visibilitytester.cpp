#include "stdafx.h"
#include "visibilitytester.h"

void CVisibilityTest::Calculate(void)
{
	if (Count>0) {
		CHK_DX(Device()->SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity.d3d()));
		CHK_DX(Device()->ComputeSphereVisibility(
			(D3DVECTOR*)&(Center[0]),
			&(Radius[0]),
			Count,
			0,
			&(Result[0])));
	}
}

void CFrustumClipper::BuildFrustum()
{
	Planes[fcpLeft	].n.set(-1,0,-1);	Planes[fcpLeft].d = 0;
	Planes[fcpRight	].n.set( 1,0,-1);	Planes[fcpLeft].d = 0;

//	Planes[fcpBottom].n.set(-1,0,-1);	Planes[fcpLeft].d = 0;
//	Planes[fcpLeft	].n.set(-1,0,-1);	Planes[fcpLeft].d = 0;

//	Planes[fcpFar	].n.set(Device.vCameraDirection); Planes[fcpLeft].d = 30;
}
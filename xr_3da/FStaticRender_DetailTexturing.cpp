#include "stdafx.h"
#include "fstaticrender.h"

void CRender::calc_DetailTexturing()
{
	// Get axis of cylinder
	Fvector		vAxis	= Device.vCameraTop;
	vAxis.invert		();
	matDetailTexturing->xform.build_camera_dir(Device.vCameraPosition,vAxis,Device.vCameraDirection);

	//Fmatrix		M,m;
	//M.mapZXY	();				// ZXY, in camera space, so X=distance from plane
	//m.scale		(1,0,0);
}

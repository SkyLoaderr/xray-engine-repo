//#include "stdafx.h"
#ifndef __DCTRILISTCOLLIDER_INCLUDED__

#define __DCTRILISTCOLLIDER_INCLUDED__

class dcTriListCollider{

	dxGeom* Geometry;	// The geom object

	dxTriList* GeomData;	// Buffered pointer to classdata


public:

	dcTriListCollider(dxGeom* Geometry);

	~dcTriListCollider();



	int CollideBox(dxGeom* Box, int Flags, dContactGeom* Contact, int Stride);

	int CollideSphere(dxGeom* Sphere, int Flags, dContactGeom* Contact, int Stride);


};



#endif	//__DCTRILISTCOLLIDER_INCLUDED__
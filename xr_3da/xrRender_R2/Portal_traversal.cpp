#include "stdafx.h"

CPortalTraverser	PortalTraverser;

CPortalTraverser::CPortalTraverser	()
{
	i_marker			=	0xffffffff;
}
void CPortalTraverser::traverse			(IRender_Sector* start, CFrustum& F, Fvector& vBase, Fmatrix& mXFORM, u32 options)
{
	VERIFY				(start);
	i_marker			++;
	i_options			= options;
	i_vBase				= vBase;
	i_mXFORM			= mXFORM;
	i_start				= (CSector*)start;
	r_sectors.clear		();
	i_start->traverse	(F);
}

/*
// Render everything
{
Fvector	Tpos;
RImplementation.set_Frustum		(&F);
RImplementation.add_Glows		(Glows);
RImplementation.add_Lights		(Lights);
RImplementation.add_Geometry	(pRoot);

// 1 sorting-pass on objects
for (int s=0; s<int(Objects.size())-1; s++)
{
Fvector		C;
Objects[s+0]->Center(C);	float	D1 = Device.vCameraPosition.distance_to_sqr(C);
Objects[s+1]->Center(C);	float	D2 = Device.vCameraPosition.distance_to_sqr(C);
if (D2<D1)	std::swap(Objects[s+0],Objects[s+1]);
}

// Persistant models
xr_vector<CObject*>::iterator I=Objects.begin(), E=Objects.end();
for (; I!=E; I++) {
CObject* O = *I;
if (O->getVisible()) 
{
vis_data&	vis				= O->Visual	()->vis;
O->XFORM().transform_tiny	(Tpos, vis.sphere.P);
if (F.testSphere_dirty(Tpos,vis.sphere.R))	
{
RImplementation.set_Object	(O);
O->renderable_Render				();
RImplementation.set_Object	(0);
}
}
}

// Temporary models
{
for (int i=0; i<int(tempObjects.size()); i++) 
{
CTempObject* pV = tempObjects[i];
if (pV->Alive())
{
u32 planes		=	F.getMask();
vis_data&	vis	=	pV->Visual()->vis;
if (F.testSAABB(vis.sphere.P,vis.sphere.R,vis.box.min,vis.box.max,planes)!=fcvNone)
RImplementation.add_Geometry	(pV->Visual());
}
else
{
if (pV->IsAutomatic())
{
tempObjects.erase	(tempObjects.begin()+i);
xr_delete			(pV);
i--;
}
}
}
}
}

 */
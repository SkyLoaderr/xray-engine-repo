#include "stdafx.h"
#include "PhysicsShell.h"
#include "PHObject.h"
#include "PHWorld.h"
#include "PHInterpolation.h"
#include "PHShell.h"
#include "PHJoint.h"
#include "PHElement.h"
#include "PHSplitedShell.h"
#include "Physics.h"
#include "SpaceUtils.h"
void CPHSplitedShell::Collide()
{
	///////////////////////////////
	CollideStatic(dSpacedGeom());
	//near_callback(this,0,(dGeomID)dSpace(),ph_world->GetMeshGeom());
}

void CPHSplitedShell::get_spatial_params()
{
		spatialParsFromDGeom((dGeomID)m_space,spatial.center,AABB,spatial.radius);
		if(spatial.radius>m_max_AABBradius) spatial.radius=m_max_AABBradius;
}


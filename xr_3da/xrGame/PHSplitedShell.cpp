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

void CPHSplitedShell::Collide()
{

	///////////////////////////////
	CollideStatic(dSpacedGeom());
	//near_callback(this,0,(dGeomID)dSpace(),ph_world->GetMeshGeom());
}
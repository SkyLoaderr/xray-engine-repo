#include "stdafx.h"
#include "PHStaticGeomShell.h"
#include "SpaceUtils.h"
void CPHStaticGeomShell::get_spatial_params()
{
Fvector AABB;
spatialParsFromDGeom(dSpacedGeometry(),spatial.center,AABB,spatial.radius);
}

void CPHStaticGeomShell::Activate(const Fmatrix& form)
{
	build();
	setStaticForm(form);
	get_spatial_params();
	spatial_register();
}

void CPHStaticGeomShell::Deativate()
{
	spatial_unregister();
	destroy();
}

CPHStaticGeomShell::CPHStaticGeomShell()
{
		spatial.type|=STYPE_PHYSIC;
}
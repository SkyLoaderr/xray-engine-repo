#include "stdafx.h"
#include "PHGeometryOwner.h"


void CPHGeometryOwner::			build_Geom	(CODEGeom& geom)
{

	geom.build(m_mass_center);
	//geom.set_body(m_body);
	geom.set_material(ul_material);
	if(contact_callback)geom.set_contact_cb(contact_callback);
	if(object_contact_callback)geom.set_obj_contact_cb(object_contact_callback);
	if(m_phys_ref_object) geom.set_ref_object(m_phys_ref_object);
	if(m_group)
	{
		geom.add_to_space((dSpaceID)m_group);
	}
}

void CPHGeometryOwner::build_Geom(u16 i)
{
	CODEGeom& geom=*m_geoms[i];
	build_Geom(geom);
	geom.element_position()=i;
}

void CPHGeometryOwner::set_body(dBodyID body)
{
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_body(body);
}
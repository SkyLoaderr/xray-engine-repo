#include "stdafx.h"
#include "PHGeometryOwner.h"


CPHGeometryOwner::CPHGeometryOwner()
{
	b_builded=false;
}

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

void CPHGeometryOwner::build()
{
	if(b_builded) return;
	if(m_geoms.size()>1)
	{
		m_group=dSimpleSpaceCreate(0);
		dSpaceSetCleanup(m_group,0);
	}
	u16 geoms_size=u16(m_geoms.size());
	for(u16 i=0;i<geoms_size;++i) build_Geom(i);
	b_builded=true;
}
void CPHGeometryOwner::destroy()
{
	if(! b_builded) return;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i)
	{
		(*i)->destroy();
	}
	b_builded=false;
}
void CPHGeometryOwner::set_body(dBodyID body)
{
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_body(body);
}

Fvector CPHGeometryOwner::			get_mc_data	(){
	Fvector s;
	float pv;
	m_mass_center.set(0,0,0);
	m_volume=0.f;
	GEOM_I i_geom=m_geoms.begin(),e=m_geoms.end();
	for(;i_geom!=e;++i_geom)
	{
		pv=(*i_geom)->volume();
		s.mul((*i_geom)->local_center(),pv);
		m_volume+=pv;
		m_mass_center.add(s);
	}
	m_mass_center.mul(1.f/m_volume);
	return m_mass_center;
}

Fvector CPHGeometryOwner::			get_mc_geoms	(){
	////////////////////to be implemented
	Fvector mc;
	mc.set(0.f,0.f,0.f);
	return mc;
}

void CPHGeometryOwner::			calc_volume_data	()
{
	m_volume=0.f;
	GEOM_I i_geom=m_geoms.begin(),e=m_geoms.end();
	for(;i_geom!=e;++i_geom)
	{
		m_volume+=(*i_geom)->volume();
	}
}

void CPHGeometryOwner::SetMaterial(u16 m)
{
	ul_material=m;
	if(!b_builded) return;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_material(m);
}

void CPHGeometryOwner::SetPhObjectInGeomData(CPHObject* O)
{
	if(!b_builded) return;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_ph_object(O);
}

dGeomID CPHGeometryOwner::dSpacedGeometry()
{
	if(!b_builded) return 0;
	if(m_group) return (dGeomID)m_group;
	else return (*m_geoms.begin())->geometry_transform();
}
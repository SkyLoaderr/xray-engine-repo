#include "stdafx.h"
#include "Geometry.h"
#include "PHDynamicData.h"
#include "ExtendedGeom.h"
#include "dcylinder//dCylinder.h"
CODEGeom::CODEGeom()
{
	m_geom_transform=NULL;
	m_bone_id		=BI_NONE;
}

CODEGeom::~CODEGeom()
{
	if(m_geom_transform) destroy();
}

void CODEGeom::get_mass(dMass& m,const Fvector& ref_point, float density)
{
	get_mass(m);
	dMassAdjust(&m,density*volume());
	Fvector l;
	l.sub(local_center(),ref_point);
	dMassTranslate(&m,l.x,l.y,l.z);
}

void CODEGeom::get_mass(dMass& m,const Fvector& ref_point)
{
	get_mass(m);
	Fvector l;
	l.sub(local_center(),ref_point);
	dMassTranslate(&m,l.x,l.y,l.z);
}

void CODEGeom::add_self_mass(dMass& m,const Fvector& ref_point, float density)
{
	dMass self_mass;
	get_mass(self_mass,ref_point,density);
	dMassAdd(&m,&self_mass);
}

void CODEGeom::add_self_mass(dMass& m,const Fvector& ref_point)
{
	dMass self_mass;
	get_mass(self_mass,ref_point);
	dMassAdd(&m,&self_mass);
}

void CODEGeom::get_local_center_bt(Fvector& center)
{
	if(! m_geom_transform) return;
	if(!geom())		//geom is not transformed
	{
		center.set(0.f,0.f,0.f);
	}
	center.set(*((const Fvector*)dGeomGetPosition(geom())));
}
void CODEGeom::get_local_form_bt(Fmatrix& form)
{
	PHDynamicData::DMXPStoFMX(dGeomGetRotation(geom()),dGeomGetPosition(geom()),form);
}
void CODEGeom::get_global_center_bt(Fvector& center)
{
	center.set(*((const Fvector*)dGeomGetPosition(m_geom_transform)));
	dVector3 add;
	dMULTIPLY0_331 (add,dGeomGetRotation(m_geom_transform),dGeomGetPosition(geom()));
	center.x += add[0];
	center.y += add[1];
	center.z += add[2];
}
void CODEGeom::get_global_form_bt(Fmatrix& form)
{
	dMULTIPLY0_331 ((dReal*)(&form.c),dGeomGetRotation(m_geom_transform),dGeomGetPosition(geom()));
	form.c.add(*((const Fvector*)dGeomGetPosition(m_geom_transform)));
	dMULTIPLY3_333 ((dReal*)(&form),dGeomGetRotation(m_geom_transform),dGeomGetRotation(geom()));
	//PHDynamicData::DMXtoFMX((dReal*)(&form),form);
}

void CODEGeom::set_body(dBodyID body)
{
	if(m_geom_transform) dGeomSetBody(m_geom_transform,body);
}

void CODEGeom::add_to_space(dSpaceID space)
{
	if(m_geom_transform) dSpaceAdd(space,m_geom_transform);
}

void CODEGeom::set_material(u32 ul_material)
{
	if(!m_geom_transform) return;
	if(geom())
	{
		VERIFY(dGeomGetUserData(geom()));
		dGeomGetUserData(geom())->material=ul_material;
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomGetUserData(m_geom_transform)->material=ul_material;
	}
}

void CODEGeom::set_contact_cb(ContactCallbackFun* ccb)
{
	if(!m_geom_transform) return;
	if(geom())
	{
		VERIFY(dGeomGetUserData(geom()));
		dGeomUserDataSetContactCallback(geom(),ccb);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataSetContactCallback(m_geom_transform,ccb);
	}
}

void CODEGeom::set_obj_contact_cb(ObjectContactCallbackFun* occb)
{
	if(!m_geom_transform) return;
	if(geom())
	{
		VERIFY(dGeomGetUserData(geom()));
		dGeomUserDataSetObjectContactCallback(geom(),occb);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataSetObjectContactCallback(m_geom_transform,occb);
	}
}
void CODEGeom::set_ref_object(CPhysicsRefObject* ro)
{
	if(!m_geom_transform) return;
	if(geom())
	{
		VERIFY(dGeomGetUserData(geom()));
		dGeomUserDataSetPhysicsRefObject(geom(),ro);
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomUserDataSetPhysicsRefObject(m_geom_transform,ro);
	}
}

void CODEGeom::set_ph_object(CPHObject* o)
{
	if(!m_geom_transform) return;
	if(geom())
	{
		VERIFY(dGeomGetUserData(geom()));
		dGeomGetUserData(geom())->ph_object=o;
	}
	else
	{
		VERIFY(dGeomGetUserData(m_geom_transform));
		dGeomGetUserData(m_geom_transform)->ph_object=o;
	}
}
void CODEGeom::destroy()
{
	if(!m_geom_transform) return;
	if(geom())
	{
		dGeomDestroyUserData(geom());
		dGeomDestroy(geom());
		dGeomTransformSetGeom(m_geom_transform,0);
	}
	dGeomDestroyUserData(m_geom_transform);
	dGeomDestroy(m_geom_transform);
	m_geom_transform=NULL;
}

CBoxGeom::CBoxGeom(const Fobb& box)
{
	m_box=box;
}

void CBoxGeom::get_mass(dMass& m)
{
	Fvector& hside=m_box.m_halfsize;
	dMassSetBox(&m,1.f,hside.x*2.f,hside.y*2.f,hside.z*2.f);
	dMatrix3 DMatx;
	PHDynamicData::FMX33toDMX(m_box.m_rotate,DMatx);
	dMassRotate(&m,DMatx);
}

float CBoxGeom::volume()
{
	return m_box.m_halfsize.x*m_box.m_halfsize.y*m_box.m_halfsize.z*8.f;
}

float CBoxGeom::radius()
{
	return m_box.m_halfsize.x;
}
const Fvector& CBoxGeom::local_center()
{
	return m_box.m_translate;
}
void CBoxGeom::get_local_form(Fmatrix& form)
{
	form.i.set(m_box.m_rotate.i);
	form.j.set(m_box.m_rotate.j);
	form.k.set(m_box.m_rotate.k);
	form.c.set(m_box.m_translate);
}
void CBoxGeom::build(const Fvector& ref_point)
{
	dGeomID geom;
	dVector3 local_position={m_box.m_translate.x-ref_point.x,
							m_box.m_translate.y-ref_point.y,
							m_box.m_translate.z-ref_point.z
							};
	geom=dCreateBox(0,
					m_box.m_halfsize.x*2.f,
					m_box.m_halfsize.y*2.f,
					m_box.m_halfsize.z*2.f
					);
	dGeomSetPosition(geom,
					local_position[0],
					local_position[1],
					local_position[2]
					);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m_box.m_rotate,R);
	dGeomSetRotation(geom,R);
	m_geom_transform=dCreateGeomTransform(0);
	dGeomSetData(m_geom_transform,0);
	dGeomTransformSetGeom(m_geom_transform,geom);
	dGeomTransformSetInfo(m_geom_transform,1);
	dGeomCreateUserData(geom);
}

CSphereGeom::CSphereGeom(const Fsphere& sphere)
{
	m_sphere=sphere;
}
void CSphereGeom::get_mass(dMass& m)
{
	dMassSetSphere(&m,1.f,m_sphere.R);
}

float CSphereGeom::volume()
{
	return 4.f*M_PI*m_sphere.R*m_sphere.R*m_sphere.R/3.f;
}

float CSphereGeom::radius()
{
	return m_sphere.R;
}

const Fvector& CSphereGeom::local_center()
{
	return m_sphere.P;
}

void CSphereGeom::get_local_form(Fmatrix& form)
{
	form.identity();
	form.c.set(m_sphere.P);
}
void CSphereGeom::build(const Fvector& ref_point)
{
	dGeomID geom;
	dVector3 local_position={
		m_sphere.P.x-ref_point.x,
		m_sphere.P.y-ref_point.y,
		m_sphere.P.z-ref_point.z
	};
	geom=dCreateSphere(0,m_sphere.R);
	dGeomSetPosition(geom,local_position[0],local_position[1],local_position[2]);
	m_geom_transform=dCreateGeomTransform(0);
	dGeomSetData(m_geom_transform,0);
	dGeomTransformSetGeom(m_geom_transform,geom);
	dGeomTransformSetInfo(m_geom_transform,1);		
	dGeomCreateUserData(geom);
}

CCylinderGeom::CCylinderGeom(const Fcylinder& cyl)
{
	m_cylinder=cyl;
}
void CCylinderGeom::get_mass(dMass& m)
{
	dMassSetCylinder(&m,1.f,2,m_cylinder.m_radius,m_cylinder.m_height);
	dMatrix3 DMatx;
	Fmatrix33 m33;
	m33.j.set(m_cylinder.m_direction);
	Fvector::generate_orthonormal_basis(m33.j,m33.k,m33.i);
	PHDynamicData::FMX33toDMX(m33,DMatx);
	dMassRotate(&m,DMatx);
}

float CCylinderGeom::volume()
{
	return M_PI*m_cylinder.m_radius*m_cylinder.m_radius*m_cylinder.m_height;
}

float CCylinderGeom::radius()
{
	return m_cylinder.m_radius;
}

const Fvector& CCylinderGeom::local_center()
{
	return m_cylinder.m_center;
}

void CCylinderGeom::get_local_form(Fmatrix& form)
{
	form.j.set(m_cylinder.m_direction);
	Fvector::generate_orthonormal_basis(form.j,form.k,form.i);
	form.c.set(m_cylinder.m_center);
}
void CCylinderGeom::build(const Fvector& ref_point)
{
	dGeomID geom;
	dVector3 local_position={
	 m_cylinder.m_center.x-ref_point.x,
	 m_cylinder.m_center.y-ref_point.y,
	 m_cylinder.m_center.z-ref_point.z
	};
	geom=dCreateCylinder(
		0,
		m_cylinder.m_radius,
		m_cylinder.m_height
		);
	dGeomSetPosition(
			geom,
			local_position[0],
			local_position[1],
			local_position[2]
			);
		dMatrix3 R;
		Fmatrix33 m33;
		m33.j.set(m_cylinder.m_direction);
		Fvector::generate_orthonormal_basis(m33.j,m33.k,m33.i);
		PHDynamicData::FMX33toDMX(m33,R);
		dGeomSetRotation(geom,R);
		m_geom_transform=dCreateGeomTransform(0);
		dGeomSetData(m_geom_transform,0);
		dGeomTransformSetGeom(m_geom_transform,geom);
		dGeomTransformSetInfo(m_geom_transform,1);
		dGeomCreateUserData(geom);
}
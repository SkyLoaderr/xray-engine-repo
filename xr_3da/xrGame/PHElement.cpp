typedef void __stdcall BoneCallbackFun(CBoneInstance* B);
typedef  void __stdcall ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	 void __stdcall ObjectContactCallbackFun(bool& do_colide,dContact& c);
struct Fcylinder;
#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
///////////////////////////////////////////////////////////////
#pragma warning(disable:4995)
#include "..\ode\src\collision_kernel.h"
#include <..\ode\src\joint.h>
#include <..\ode\src\objects.h>

#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHShell.h"
#include "PHElement.h"



Shader* CPHElement::hWallmark=NULL;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Implementation for CPhysicsElement
void CPHElement::			add_Box		(const Fobb&		V){
	Fobb box;
	box=V;
	if(box.m_halfsize.x<0.005f) box.m_halfsize.x=0.005f;
	if(box.m_halfsize.y<0.005f) box.m_halfsize.y=0.005f;
	if(box.m_halfsize.z<0.005f) box.m_halfsize.z=0.005f;
	m_boxes_data.push_back(box);
}

void CPHElement::			create_Box	(const Fobb&		V){
	dGeomID geom,trans;

	dVector3 local_position={
		V.m_translate.x-m_mass_center.x,
			V.m_translate.y-m_mass_center.y,
			V.m_translate.z-m_mass_center.z};

		if(dDOT(local_position,local_position)>0.0001||
			m_spheras_data.size()+m_boxes_data.size()>1
			){
				geom=dCreateBox(0,
					V.m_halfsize.x*2.f,
					V.m_halfsize.y*2.f,
					V.m_halfsize.z*2.f);

				m_geoms.push_back(geom);
				dGeomSetPosition(geom,
					local_position[0],
					local_position[1],
					local_position[2]);
				dMatrix3 R;
				PHDynamicData::FMX33toDMX(V.m_rotate,R);
				dGeomSetRotation(geom,R);
				trans=dCreateGeomTransform(0);

				dGeomTransformSetGeom(trans,geom);
				dGeomSetBody(trans,m_body);
				m_trans.push_back(trans);
				/////////////////////////////////////////////////////////
				dGeomGroupAdd(m_group,trans);
				/////////////////////////////////////////////////////////
				dGeomTransformSetInfo(trans,1);
				dGeomCreateUserData(geom);
				dGeomGetUserData(geom)->material=ul_material;
				if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
				if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
			}
		else{
			geom=dCreateBox(0,
				V.m_halfsize.x*2.f,
				V.m_halfsize.y*2.f,
				V.m_halfsize.z*2.f);

			m_geoms.push_back(geom);


			dGeomSetPosition(geom,
				local_position[0],
				local_position[1],
				local_position[2]);

			dMatrix3 R;
			PHDynamicData::FMX33toDMX(V.m_rotate,R);
			dGeomSetRotation(geom,R);


			trans=dCreateGeomTransform(0);
			dGeomTransformSetInfo(trans,1);
			dGeomTransformSetGeom(trans,geom);
			dGeomSetBody(trans,m_body);
			m_trans.push_back(trans);

			dGeomCreateUserData(geom);
			dGeomGetUserData(geom)->material=ul_material;

			if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
			if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
		}


}

void CPHElement::			add_Sphere	(const Fsphere&	V){
	m_spheras_data.push_back(V);
}

void CPHElement::			create_Sphere	(const Fsphere&	V){
	dGeomID geom,trans;
	dVector3 local_position={
		V.P.x-m_mass_center.x,
			V.P.y-m_mass_center.y,
			V.P.z-m_mass_center.z};
		if(dDOT(local_position,local_position)>0.0001||
			m_spheras_data.size()+m_boxes_data.size()>1
			)
		{
			geom=dCreateSphere(0,V.R);
			m_geoms.push_back(geom);
			dGeomSetPosition(geom,local_position[0],local_position[1],local_position[2]);
			trans=dCreateGeomTransform(0);
			dGeomTransformSetGeom(trans,geom);
			dGeomSetBody(trans,m_body);
			m_trans.push_back(trans);
			dGeomGroupAdd(m_group,trans);
			dGeomTransformSetInfo(trans,1);		
			dGeomCreateUserData(geom);
			//dGeomGetUserData(geom)->material=GMLib.GetMaterialIdx("box_default");
			dGeomGetUserData(geom)->material=0;
			if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
			if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
		}
		else
		{
			geom=dCreateSphere(0,V.R);
			m_geoms.push_back(geom);
			dGeomSetPosition(geom,
				m_mass_center.x,
				m_mass_center.y,
				m_mass_center.z);
			dGeomSetBody(geom,m_body);
			dGeomCreateUserData(geom);
			//dGeomGetUserData(geom)->material=GMLib.GetMaterialIdx("box_default");
			dGeomGetUserData(geom)->material=0;
			if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
			if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
		}
};
void CPHElement::add_Cylinder(const Fcylinder& V)
{
	m_cylinders_data.push_back(V);
}

void CPHElement::create_Cylinder(const Fcylinder& V)
{
	dGeomID geom,trans;

	dVector3 local_position=
	{
		V.m_translate.x-m_mass_center.x,
			V.m_translate.y-m_mass_center.y,
			V.m_translate.z-m_mass_center.z
	};

	if(m_group){
		geom=dCreateCylinder
			(
			0,
			V.m_halflength*2.f,
			V.m_radius
			);

		m_geoms.push_back(geom);
		dGeomSetPosition(geom,
			local_position[0],
			local_position[1],
			local_position[2]);
		dMatrix3 R;
		PHDynamicData::FMX33toDMX(V.m_rotate,R);
		dGeomSetRotation(geom,R);
		trans=dCreateGeomTransform(0);

		dGeomTransformSetGeom(trans,geom);
		dGeomSetBody(trans,m_body);
		m_trans.push_back(trans);
		/////////////////////////////////////////////////////////
		dGeomGroupAdd(m_group,trans);
		/////////////////////////////////////////////////////////
		dGeomTransformSetInfo(trans,1);
		dGeomCreateUserData(geom);
		dGeomGetUserData(geom)->material=ul_material;
		if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
		if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);
	}
	else{
		geom=dCreateCylinder
			(
			0,
			V.m_halflength*2.f,
			V.m_radius
			);

		m_geoms.push_back(geom);


		dGeomSetPosition(geom,
			local_position[0],
			local_position[1],
			local_position[2]);

		dMatrix3 R;
		PHDynamicData::FMX33toDMX(V.m_rotate,R);
		dGeomSetRotation(geom,R);


		trans=dCreateGeomTransform(0);
		dGeomTransformSetInfo(trans,1);
		dGeomTransformSetGeom(trans,geom);
		dGeomSetBody(trans,m_body);
		m_trans.push_back(trans);

		dGeomCreateUserData(geom);
		dGeomGetUserData(geom)->material=ul_material;

		if(contact_callback)dGeomUserDataSetContactCallback(geom,contact_callback);
		if(object_contact_callback)dGeomUserDataSetObjectContactCallback(geom,object_contact_callback);

	}
}
void CPHElement::			build	(dSpaceID space){

	m_body=dBodyCreate(phWorld);
	m_saved_contacts=dJointGroupCreate (0);
	b_contacts_saved=false;
	dBodyDisable(m_body);
	//dBodySetFiniteRotationMode(m_body,1);
	//dBodySetFiniteRotationAxis(m_body,0,0,0);

	dBodySetMass(m_body,&m_mass);

	if(m_spheras_data.size()+m_boxes_data.size()>1)
		//m_group=dCreateGeomGroup(space);
		m_group=dCreateGeomGroup(0);

	Fvector mc=get_mc_data();
	//m_start=mc;

	m_inverse_local_transform.identity();
	m_inverse_local_transform.c.set(mc);
	m_inverse_local_transform.invert();
	dBodySetPosition(m_body,mc.x,mc.y,mc.z);
	///////////////////////////////////////////////////////////////////////////////////////
	xr_vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
		create_Box(*i_box);
	}
	///////////////////////////////////////////////////////////////////////////////////////
	xr_vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
		create_Sphere(*i_sphere);
	}
	///////////////////////////////////////////////////////////////////////////////////////
	xr_vector<Fcylinder>::iterator i_cylinder;
	for(i_cylinder=m_cylinders_data.begin();i_cylinder!=m_cylinders_data.end();i_cylinder++){
		create_Cylinder(*i_cylinder);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
}

void CPHElement::RunSimulation()
{
	if(push_untill)
		push_untill+=Device.dwTimeGlobal;

	if(m_phys_ref_object)
	{
		xr_vector<dGeomID>::iterator i;
		for(i=m_geoms.begin();i!=m_geoms.end();i++)
			dGeomUserDataSetPhysicsRefObject(*i,m_phys_ref_object);
	}

	if(m_group)
		dSpaceAdd(m_shell->GetSpace(),m_group);
	else
		if(m_boxes_data.size()==0)
			dSpaceAdd(m_shell->GetSpace(),*m_geoms.begin());
		else
			dSpaceAdd(m_shell->GetSpace(),*m_trans.begin());

	dBodyEnable(m_body);
}

void CPHElement::			destroy	(){
	m_attached_elements.clear();
	dJointGroupDestroy(m_saved_contacts);
	xr_vector<dGeomID>::iterator i;


	for(i=m_geoms.begin();i!=m_geoms.end();i++){
		dGeomDestroyUserData(*i);
		dGeomDestroy(*i);
	}
	for(i=m_trans.begin();i!=m_trans.end();i++){
		dGeomDestroyUserData(*i);


		//if(!attached)
		dGeomDestroy(*i);



		if(m_body && !attached)
		{
			dBodyDestroy(m_body);
			m_body=NULL;
		}




		if(m_group){
			dGeomDestroy(m_group);
			m_group=NULL;
		}
	}

	m_geoms.clear();
	m_trans.clear();
}

Fvector CPHElement::			get_mc_data	(){
	Fvector mc,s;
	float pv,volume=0.f;
	mc.set(0,0,0);
	xr_vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
		pv=(*i_box).m_halfsize.x*(*i_box).m_halfsize.y*(*i_box).m_halfsize.z*8;
		s.mul((*i_box).m_translate,pv);
		volume+=pv;
		mc.add(s);
	}
	xr_vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
		pv=(*i_sphere).R*(*i_sphere).R*(*i_sphere).R*4/3*M_PI;
		s.mul((*i_sphere).P,pv);
		volume+=pv;
		mc.add(s);
	}


	xr_vector<Fcylinder>::iterator i_cylider;
	for(i_cylider=m_cylinders_data.begin();i_cylider!=m_cylinders_data.end();i_cylider++){
		pv=M_PI*(*i_cylider).m_radius*(*i_cylider).m_radius*(*i_cylider).m_halflength*2.f;
		s.mul((*i_cylider).m_translate,pv);
		volume+=pv;
		mc.add(s);
	}

	m_volume=volume;
	mc.mul(1.f/volume);
	m_mass_center=mc;
	return mc;
}
Fvector CPHElement::			get_mc_geoms	(){
	////////////////////to be implemented
	Fvector mc;
	mc.set(0.f,0.f,0.f);
	return mc;
}
void CPHElement::set_BoxMass(const Fobb& box, float mass)
{

	dMassSetZero(&m_mass);

	m_mass_center.set(box.m_translate);
	const Fvector& hside=box.m_halfsize;
	dMassSetBox(&m_mass,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
	dMassAdjust(&m_mass,mass);
	dMatrix3 DMatx;
	PHDynamicData::FMX33toDMX(box.m_rotate,DMatx);
	dMassRotate(&m_mass,DMatx);



}

void CPHElement::calculate_it_data(const Fvector& mc,float mas){
	dMass m;
	dMassSetZero(&m_mass);
	xr_vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
		Fvector& hside=(*i_box).m_halfsize;
		Fvector& pos=(*i_box).m_translate;
		Fvector l;
		l.sub(pos,mc);
		dMassSetBox(&m,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
		dMassAdjust(&m,hside.x*hside.y*hside.z*8.f/m_volume*mas);
		dMatrix3 DMatx;
		PHDynamicData::FMX33toDMX((*i_box).m_rotate,DMatx);
		dMassRotate(&m,DMatx);
		dMassTranslate(&m,l.x,l.y,l.z);
		dMassAdd(&m_mass,&m);

	}

	xr_vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
		Fvector& pos=(*i_sphere).P;
		Fvector l;
		l.sub(pos,mc);
		dMassSetSphere(&m,mas/m_volume,(*i_sphere).R);
		dMassTranslate(&m,l.x,l.y,l.z);
		dMassAdd(&m_mass,&m);

	}

	xr_vector<Fcylinder>::iterator i_cylinder;
	for(i_cylinder=m_cylinders_data.begin();i_cylinder!=m_cylinders_data.end();i_cylinder++){
		Fvector& pos=(*i_cylinder).m_translate;
		Fvector l;
		l.sub(pos,mc);
		dMassSetCylinder(&m,mas/m_volume,2,(*i_cylinder).m_radius,2.f*(*i_cylinder).m_halflength);
		dMatrix3 DMatx;
		PHDynamicData::FMX33toDMX((*i_cylinder).m_rotate,DMatx);
		dMassRotate(&m,DMatx);
		dMassTranslate(&m,l.x,l.y,l.z);
		dMassAdd(&m_mass,&m);

	}
	//dMassSetCylinder()
}


void CPHElement::calculate_it_data_use_density(const Fvector& mc,float density){

	//	xr_vector<Fobb>::iterator i_box=m_boxes_data.begin();
	//	if(m_boxes_data.size()==1,m_spheras_data.size()==0){
	//	Fvector& hside=(*i_box).m_halfsize;

	//	dMassSetBox(&m_mass,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
	//	dMassAdjust(&m_mass,hside.x*hside.y*hside.z*8.f*density);

	//	return;
	//	}

	dMass m;
	dMassSetZero(&m_mass);
	xr_vector<Fobb>::iterator i_box;
	for(i_box=m_boxes_data.begin();i_box!=m_boxes_data.end();i_box++){
		Fvector& hside=(*i_box).m_halfsize;
		Fvector& pos=(*i_box).m_translate;
		Fvector l;
		l.sub(pos,mc);
		dMassSetBox(&m,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
		dMassAdjust(&m,hside.x*hside.y*hside.z*8.f*density);
		dMatrix3 DMatx;
		PHDynamicData::FMX33toDMX((*i_box).m_rotate,DMatx);
		dMassRotate(&m,DMatx);	
		dMassTranslate(&m,l.x,l.y,l.z);
		dMassAdd(&m_mass,&m);

	}

	xr_vector<Fsphere>::iterator i_sphere;
	for(i_sphere=m_spheras_data.begin();i_sphere!=m_spheras_data.end();i_sphere++){
		Fvector& pos=(*i_sphere).P;
		Fvector l;
		l.sub(pos,mc);
		dMassSetSphere(&m,1,(*i_sphere).R);
		dMassAdjust(&m,4.f/3.f*M_PI*(*i_sphere).R*(*i_sphere).R*(*i_sphere).R*density);
		dMassTranslate(&m,l.x,l.y,l.z);
		dMassAdd(&m_mass,&m);

	}

	xr_vector<Fcylinder>::iterator i_cylinder;
	for(i_cylinder=m_cylinders_data.begin();i_cylinder!=m_cylinders_data.end();i_cylinder++){
		Fvector& pos=(*i_cylinder).m_translate;
		Fvector l;
		l.sub(pos,mc);
		dMassSetCylinder(&m,1.f,2,(*i_cylinder).m_radius,(*i_cylinder).m_halflength*2.f);
		dMassAdjust(&m,M_PI*(*i_cylinder).m_radius*(*i_cylinder).m_radius*(*i_cylinder).m_halflength*2.f*density);
		dMatrix3 DMatx;
		PHDynamicData::FMX33toDMX((*i_box).m_rotate,DMatx);
		dMassRotate(&m,DMatx);	
		dMassTranslate(&m,l.x,l.y,l.z);
		dMassAdd(&m_mass,&m);

	}

}



void		CPHElement::	setDensity		(float M){

	//calculate_it_data(get_mc_data(),M);

	calculate_it_data_use_density(get_mc_data(),M);

}

void		CPHElement::	setMass		(float M){

	calculate_it_data(get_mc_data(),M);

	//calculate_it_data_use_density(get_mc_data(),M);

}

void		CPHElement::Start(){
	//mXFORM.set(m0);
	build(m_space);
	RunSimulation();

	//dBodySetPosition(m_body,m_m0.c.x,m_m0.c.y,m_m0.c.z);
	//Fmatrix33 m33;
	//m33.set(m_m0);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
	//dBodySetRotation(m_body,R);
}

void		CPHElement::Deactivate(){
	if(!bActive) return;
	if(push_untill)
	{
		push_untill-=Device.dwTimeGlobal;
		if(push_untill<=0)
			unset_Pushout();
	}
	destroy();
	bActive=false;
	bActivating=false;
}
void CPHElement::SetTransform(const Fmatrix &m0){
	Fvector mc;
	mc.set(m_mass_center);
	m0.transform_tiny(mc);
	dBodySetPosition(m_body,mc.x,mc.y+0.0f,mc.z);
	Fmatrix33 m33;
	m33.set(m0);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m33,R);
	dBodySetRotation(m_body,R);

}
CPHElement::~CPHElement	(){
	m_boxes_data.clear();
	m_spheras_data.clear();
	m_cylinders_data.clear();
}

void CPHElement::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){
	mXFORM.set(m0);
	Start();
	SetTransform(m0);
	//i=elements.begin();
	//m_body=(*i)->get_body();
	//m_inverse_local_transform.set((*i)->m_inverse_local_transform);
	//Fmatrix33 m33;
	//Fmatrix m,m1;
	//m1.set(m0);
	//m1.identity();
	//m1.invert();
	//m.mul(m1,m2);
	//m.mul(1.f/dt01);
	//m33.set(m);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
	dBodySetLinearVel(m_body,m2.c.x-m0.c.x,m2.c.y-m0.c.y,m2.c.z-m0.c.z);


	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


//////////////////////////////////////////////////////////////
//initializing values for disabling//////////////////////////
//////////////////////////////////////////////////////////////
	/*
	mean_w[0]=0.f;
	mean_w[1]=0.f;
	mean_w[2]=0.f;
	mean_v[0]=0.f;
	mean_v[1]=0.f;
	mean_v[2]=0.f;
	*/
	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);
	bActive=true;
}

void CPHElement::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){
	mXFORM.set(transform);
	Start();
	SetTransform(transform);
	//i=elements.begin();
	//m_body=(*i)->get_body();
	//m_inverse_local_transform.set((*i)->m_inverse_local_transform);
	//Fmatrix33 m33;
	//Fmatrix m,m1;
	//m1.set(m0);
	//m1.identity();
	//m1.invert();
	//m.mul(m1,m2);
	//m.mul(1.f/dt01);
	//m33.set(m);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
	dBodySetLinearVel(m_body,lin_vel.x,lin_vel.y,lin_vel.z);
	
	dBodySetAngularVel(m_body,ang_vel.x,ang_vel.y,ang_vel.z);


	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


	//////////////////////////////////////////////////////////////
	//initializing values for disabling//////////////////////////
	//////////////////////////////////////////////////////////////
	/*
	mean_w[0]=0.f;
	mean_w[1]=0.f;
	mean_w[2]=0.f;
	mean_v[0]=0.f;
	mean_v[1]=0.f;
	mean_v[2]=0.f;
	*/
	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);
	bActive=true;
}


void CPHElement::PhDataUpdate(dReal step){
	//m_body_interpolation.UpdatePositions();
	//m_body_interpolation.UpdateRotations();
	//return;

	///////////////skip body effecting update for attached elements////////////////
	if(attached) {
		if( !dBodyIsEnabled(m_body)) return;
		m_body_interpolation.UpdatePositions();
		m_body_interpolation.UpdateRotations();
		return;
	}

	///////////////skip for disabled elements////////////////////////////////////////////////////////////
	if( !dBodyIsEnabled(m_body)) {
		//	if(previous_p[0]!=dInfinity) previous_p[0]=dInfinity;//disable
		m_body_interpolation.UpdatePositions();
		m_body_interpolation.UpdateRotations();
		return;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////scale velocity///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	const dReal scale=1.01f;
	const dReal scalew=1.01f;
	const dReal* vel1= dBodyGetLinearVel(m_body);
	dBodySetLinearVel(m_body,vel1[0]/scale,vel1[1]/scale,vel1[2]/scale);
	const dReal* wvel1 = dBodyGetAngularVel(m_body);
	dBodySetAngularVel(m_body,wvel1[0]/scalew,wvel1[1]/scalew,wvel1[2]/scalew);



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////limit velocity & secure /////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static const dReal u = -0.1f;

	////////////////limit linear vel////////////////////////////////////////////////////////////////////////////////////////
	const dReal* pos = dBodyGetLinearVel(m_body);
	dReal mag;
	mag=_sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2]);
	if(!(mag>-dInfinity && mag<dInfinity)){
		dBodySetLinearVel(m_body,0.f,0.f,0.f);
		mag=0.f;
	}
	else if(mag>l_limit){
		dReal f=mag/l_limit;
		dBodySetLinearVel(m_body,pos[0]/f,pos[1]/f,pos[2]/f);
	}

	////////////////secure position///////////////////////////////////////////////////////////////////////////////////
	const dReal* position=dBodyGetPosition(m_body);
	if(!(position[0]<dInfinity && position[0]>-dInfinity &&
		position[1]<dInfinity && position[1]>-dInfinity &&
		position[2]<dInfinity && position[2]>-dInfinity)
		) 
		dBodySetPosition(m_body,m_safe_position[0]-m_safe_velocity[0]*fixed_step,
		m_safe_position[1]-m_safe_velocity[1]*fixed_step,
		m_safe_position[2]-m_safe_velocity[2]*fixed_step);
	else{
		Memory.mem_copy(m_safe_position,position,sizeof(dVector3));
		Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));
	}

	/////////////////limit & secure angular vel///////////////////////////////////////////////////////////////////////////////
	const dReal* rot = dBodyGetAngularVel(m_body);
	dReal w_mag=_sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
	if(!(w_mag>-dInfinity && w_mag<dInfinity)) 
		dBodySetAngularVel(m_body,0.f,0.f,0.f);
	else if(w_mag>w_limit){
		dReal f=w_mag/w_limit;
		dBodySetAngularVel(m_body,rot[0]/f,rot[1]/f,rot[2]/f);
	}

	////////////////secure rotation////////////////////////////////////////////////////////////////////////////////////////
	{
		const dReal* rotation=dBodyGetRotation(m_body);
		for(int i=0;i<12;i++)
		{
			if(!(rotation[i]>-dInfinity&&rotation[i]<dInfinity))
			{
				dMatrix3 m;
				dRSetIdentity(m);
				dBodySetRotation(m_body,m);
				break;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////disable///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ReEnable();
	Disabling();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////air resistance/////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//const dReal k_w=0.05f;
	//const dReal k_l=0.0002f;//1.8f;
	dBodyAddTorque(m_body,-rot[0]*k_w,-rot[1]*k_w,-rot[2]*k_w);
	dMass mass;
	dBodyGetMass(m_body,&mass);
	dReal l_air=mag*k_l;
	//if(mag*mag*k_l/fixed_step>mass.mass*mag) k_l=mass.mass/mag/fixed_step;
	if(mag*l_air/fixed_step>mass.mass*mag) l_air=mass.mass/fixed_step;
	//dBodyAddForce(m_body,-pos[0]*mag*k_l,-pos[1]*mag*k_l,-pos[2]*mag*k_l);			
	dBodyAddForce(m_body,-pos[0]*l_air,-pos[1]*l_air,-pos[2]*l_air);

	m_body_interpolation.UpdatePositions();
	m_body_interpolation.UpdateRotations();

}

void	CPHElement::Disabling(){
	//return;
	/////////////////////////////////////////////////////////////////////////////////////
	///////////////////disabling main body//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	{
		const dReal* torqu=dBodyGetTorque(m_body);
		const dReal* force=dBodyGetForce(m_body);
		dReal t_m =_sqrt(	torqu[0]*torqu[0]+
			torqu[1]*torqu[1]+
			torqu[2]*torqu[2]);
		dReal f_m=_sqrt(	force[0]*force[0]+
			force[1]*force[1]+
			force[2]*force[2]);
		if(t_m+f_m>0.000000){
			previous_p[0]=dInfinity;
			previous_dev=0;
			previous_v=0;
			dis_count_f=1;
			dis_count_f1=0;
			return;
		}
	}
	if(previous_p[0]==dInfinity&&ph_world->disable_count==0){
		const dReal* position=dBodyGetPosition(m_body);
		Memory.mem_copy(previous_p,position,sizeof(dVector3));
		Memory.mem_copy(previous_p1,position,sizeof(dVector3));
		const dReal* rotation=dBodyGetRotation(m_body);
		Memory.mem_copy(previous_r,rotation,sizeof(dMatrix3));
		Memory.mem_copy(previous_r1,rotation,sizeof(dMatrix3));
		previous_dev=0;
		previous_v=0;
		dis_count_f=1;
		dis_count_f1=0;
	}


	if(ph_world->disable_count==dis_frames){	
		if(previous_p[0]!=dInfinity){
			const dReal* current_p=dBodyGetPosition(m_body);
			dVector3 velocity={current_p[0]-previous_p[0],
				current_p[1]-previous_p[1],
				current_p[2]-previous_p[2]};
			dReal mag_v=_sqrt(
				velocity[0]*velocity[0]+
				velocity[1]*velocity[1]+
				velocity[2]*velocity[2]);
			mag_v/=dis_count_f;
			const dReal* current_r=dBodyGetRotation(m_body);
			dMatrix3 rotation_m;

			dMULTIPLYOP1_333(rotation_m,=,previous_r,current_r);

			dVector3 deviation_v={rotation_m[0]-1.f,
				rotation_m[5]-1.f,
				rotation_m[10]-1.f
			};

			dReal deviation =_sqrt(deviation_v[0]*deviation_v[0]+
				deviation_v[1]*deviation_v[1]+
				deviation_v[2]*deviation_v[2]);

			deviation/=dis_count_f;
			if(mag_v<0.005f* dis_frames && deviation<0.00005f*dis_frames)
				Disable();//dBodyDisable(m_body);//
			if((!(previous_dev<deviation)&&!(previous_v<mag_v))//
				) 
			{
				dis_count_f++;
				previous_dev=deviation;
				previous_v=mag_v;
				//return;
			}
			else{
				previous_dev=0;
				previous_v=0;
				dis_count_f=1;
				dis_count_f1=0;
				Memory.mem_copy(previous_p,current_p,sizeof(dVector3));
				Memory.mem_copy(previous_r,current_r,sizeof(dMatrix3));
				previous_p[0]=dInfinity;
			}

			{

				dVector3 velocity={current_p[0]-previous_p1[0],
					current_p[1]-previous_p1[1],
					current_p[2]-previous_p1[2]};
				dReal mag_v=_sqrt(
					velocity[0]*velocity[0]+
					velocity[1]*velocity[1]+
					velocity[2]*velocity[2]);
				mag_v/=dis_count_f;
				dMatrix3 rotation_m;
				dMULTIPLYOP1_333(rotation_m,=,previous_r1,current_r);

				dVector3 deviation_v={rotation_m[0]-1.f,
					rotation_m[5]-1.f,
					rotation_m[10]-1.f
				};

				dReal deviation =_sqrt(deviation_v[0]*deviation_v[0]+
					deviation_v[1]*deviation_v[1]+
					deviation_v[2]*deviation_v[2]);

				deviation/=dis_count_f;
				if(mag_v<0.16* dis_frames && deviation<0.06*dis_frames)
					dis_count_f1++;
				else{
					Memory.mem_copy(previous_p1,current_p,sizeof(dVector3));
					Memory.mem_copy(previous_r1,current_r,sizeof(dMatrix3));
				}

				if(dis_count_f1>10) dis_count_f*=10;

			}


		}

	}
	/////////////////////////////////////////////////////////////////

}

void CPHElement::ResetDisable(){
	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f	 =0;
	dis_count_f1 =0;
}

void CPHElement::Enable(){

	if(!bActive) return;
	if(dBodyIsEnabled(m_body)) return;
	ResetDisable();
	dBodyEnable(m_body);
}


void CPHElement::Disable(){
	//return;
	/*
	if(!b_contacts_saved){
	int num=dBodyGetNumJoints(m_body);
	for(int i=0;i<num;i++){
	dJointID joint=	dBodyGetJoint (m_body, i);
	if(dJointGetType (joint)==dJointTypeContact){
	dxJointContact* contact=(dxJointContact*) joint;
	dBodyID b1=dGeomGetBody(contact->contact.geom.g1);
	dBodyID b2=dGeomGetBody(contact->contact.geom.g2);
	if(b1==0 || b2==0){
	dJointID c = dJointCreateContact(phWorld, m_saved_contacts, &(contact->contact));
	dJointAttach(c, b1, b2);
	b_contacts_saved=true;
	}

	}

	}
	}

	*/
	if(!dBodyIsEnabled(m_body)) return;
	if(m_group)
		SaveContacts(ph_world->GetMeshGeom(),m_group,m_saved_contacts);
	else 
		SaveContacts(ph_world->GetMeshGeom(),m_trans[0],m_saved_contacts);

	xr_vector<CPHElement*>::iterator i;
	for(i=m_attached_elements.begin();i!=m_attached_elements.end();i++){


		if((*i)->m_group)
			SaveContacts(ph_world->GetMeshGeom(),(*i)->m_group,m_saved_contacts);
		else 
			SaveContacts(ph_world->GetMeshGeom(),(*i)->m_trans[0],m_saved_contacts);

	}

	dBodyDisable(m_body);
}


void CPHElement::ReEnable(){
	//if(b_contacts_saved && dBodyIsEnabled(m_body))
	//{
	dJointGroupEmpty(m_saved_contacts);
	b_contacts_saved=false;
	//}
}


void	CPHElement::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val){

	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	val/=fixed_step;
	Fvector body_pos;
	body_pos.sub(pos,m_inverse_local_transform.c);

	dBodyAddForceAtRelPos       (m_body, dir.x*val,dir.y*val,dir.z*val,body_pos.x, body_pos.y, body_pos.z);

	BodyCutForce(m_body);
}

void CPHElement::InterpolateGlobalTransform(Fmatrix* m){
	m_body_interpolation.InterpolateRotation(*m);
	m_body_interpolation.InterpolatePosition(m->c);
	m->mulB(m_inverse_local_transform);

}

void CPHElement::DynamicAttach(CPHElement* E)
{
	dVector3 p1;
	dMatrix3 R1;
	Memory.mem_copy(p1,dBodyGetPosition(m_body),sizeof(dVector3));
	const dReal* p2=dBodyGetPosition(E->m_body);
	Memory.mem_copy( R1,dBodyGetRotation(m_body),sizeof(dMatrix3));
	const dReal* R2=dBodyGetRotation(E->m_body);
	dVector3 pp={p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]};
	dMatrix3 RR;
	dMULTIPLY1_333(RR,R1,R2);
	dVector3 ppr;
	dMULTIPLY1_331(ppr,R1,pp);
	xr_vector<dGeomID>::iterator i;
	Fmatrix RfRf;
	PHDynamicData::DMXPStoFMX(RR,ppr,RfRf);
	E->m_inverse_local_transform.mulA(RfRf);
	//E->fixed_position.set(RfRf);
	for(i=E->m_trans.begin();i!=E->m_trans.end();i++){
		//	if(!m_group) {
		//		m_group=dCreateGeomGroup(0);
		//		dSpaceRemove (ph_world->GetSpace(), m_trans[0]);
		//		dGeomGroupAdd(m_group,m_trans[0]);
		//	}
		//	dSpaceRemove (ph_world->GetSpace(), *i);
		//	dGeomGroupAdd(m_group,*i);
		dGeomID geom=dGeomTransformGetGeom(*i);
		const dReal* pos=dGeomGetPosition(geom);
		const dReal* rot=dGeomGetRotation(geom);
		dMatrix3 rr;
		dMULTIPLY0_333(rr,RR,rot);

		dGeomSetRotation(geom,rr);
		dGeomSetPosition(geom,pos[0]+ppr[0],pos[1]+ppr[1],pos[2]+ppr[2]);

		dGeomSetBody(*i,m_body);
		dBodySetPosition(m_body,p1[0],p1[1],p1[2]);
		dBodySetRotation(m_body,R1);

	}
	for(i=E->m_geoms.begin();i!=E->m_geoms.end();i++)
		if(dGeomGetBody(*i)){
			dGeomID trans=dCreateGeomTransform(0);
			dGeomSetBody((*i),0);
			dGeomSetPosition((*i),pp[0],pp[1],pp[2]);
			dGeomSetRotation((*i),RR);
			dGeomTransformSetGeom(trans,(*i));
			dGeomTransformSetInfo(trans,1);
			dGeomSetBody(trans,m_body);
			dBodySetPosition(m_body,p1[0],p1[1],p1[2]);
			dBodySetRotation(m_body,R1);
			E->m_trans.push_back(trans);
		}



		dMass m1,m2;
		dBodyGetMass(E->m_body,&m1);
		dBodyGetMass(m_body,&m2);
		dMassAdd(&m1,&m2);
		dBodySetMass(m_body,&m1);
		dBodyDestroy(E->m_body);
		E->m_body=m_body;
		E->m_body_interpolation.SetBody(m_body);
		E->attached=true;
		m_attached_elements.push_back(E);
		//E->m_body;

}


void CPHElement::Activate(bool place_current_forms,bool disable){

	Start();
	if(place_current_forms)
	{
		SetTransform(mXFORM);
	}
	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


	//////////////////////////////////////////////////////////////
	//initializing values for disabling//////////////////////////
	//////////////////////////////////////////////////////////////

	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);


}

void CPHElement::Activate(const Fmatrix& start_from,bool disable){

	Start();
	//	if(place_current_forms)
	{
		Fmatrix globe;
		globe.mul(mXFORM,start_from);
		SetTransform(globe);
	}
	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


	//////////////////////////////////////////////////////////////
	//initializing values for disabling//////////////////////////
	//////////////////////////////////////////////////////////////

	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);


}


void CPHElement::CallBack(CBoneInstance* B){
	
	if(!bActive && !bActivating){
		mXFORM.set(B->mTransform);
		bActivating=true;
		bActive=true;
		m_start_time=Device.fTimeGlobal;

		if(!m_parent_element) m_shell->CreateSpace();
		build(m_space);

	
		Fmatrix global_transform;
		global_transform.set(m_shell->mXFORM);
		global_transform.mulB(mXFORM);
		SetTransform(global_transform);
		m_body_interpolation.SetBody(m_body);
		return;
	}
	Fmatrix parent;
	if(bActivating){

		if(!m_parent_element)
		{	
		m_shell->Activate();
		//parent.set(B->mTransform);
		//parent.invert();
		//m_inverse_local_transform.mulB(parent);
		}
	if(!m_shell->bActive) return;
	RunSimulation();

	
	//Fmatrix m,m1,m2;
	//m1.set(mXFORM);
	//m2.set(B->mTransform);
	
	//m1.invert();
	//m.mul(m1,m2);
	//float dt01=Device.fTimeGlobal-m_start_time;
	//m.mul(1.f/dt01);

	//Fmatrix33 m33;
	//m33.set(m);
	//dMatrix3 R;
	//PHDynamicData::FMX33toDMX(m33,R);
//	dBodySetLinearVel(m_body,m.c.x,m.c.y,m.c.z);
	PSGP.memCopy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	PSGP.memCopy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));


//////////////////////////////////////////////////////////////
//initializing values for disabling//////////////////////////
//////////////////////////////////////////////////////////////

	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f=0;


	bActivating=false;
	//previous_f[0]=dInfinity;
	return;
	}


	
//	if(!dBodyIsEnabled(m_body))
//	{
//		B->mTransform.set(mXFORM);
//		return;
//	}

	if(m_parent_element){
	InterpolateGlobalTransform(&mXFORM);
	
	parent.set(m_shell->mXFORM);
	parent.invert();
	mXFORM.mulA(parent);
	B->mTransform.set(mXFORM);
	}
	else{

		InterpolateGlobalTransform(&m_shell->mXFORM);
		mXFORM.identity();
		B->mTransform.set(mXFORM);
		//parent.set(B->mTransform);
		//parent.invert();
		//m_shell->mXFORM.mulB(parent);
		
	}

	if(push_untill)//temp_for_push_out||(!temp_for_push_out&&object_contact_callback)
		if(push_untill<Device.dwTimeGlobal) unset_Pushout();
}

void CPHElement::set_PhysicsRefObject(CPhysicsRefObject* ref_object)
{
	m_phys_ref_object=ref_object;
	if(!bActive)
	{
		return;
	}
	xr_vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
	{
		dGeomUserDataSetPhysicsRefObject(*i,ref_object);
	}
}


void CPHElement::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	object_contact_callback= callback;
	if(!bActive)
	{
		return;
	}
	xr_vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
	{
		dGeomUserDataSetObjectContactCallback(*i,callback);
	}
}

void CPHElement::set_ContactCallback(ContactCallbackFun* callback)
{
	contact_callback=callback;
	push_untill=0;
	if(!bActive)
	{
		return;
	}
	xr_vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
	{
		dGeomUserDataSetContactCallback(*i,callback);
	}
}

void CPHElement::SetPhObjectInGeomData(CPHObject* O)
{
	if(!bActive) return;
	xr_vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
				dGeomGetUserData(*i)->ph_object=O;
}



void CPHElement::SetMaterial(u32 m)
{
	ul_material=m;
	if(!bActive) return;
	xr_vector<dGeomID>::iterator i;
	for(i=m_geoms.begin();i!=m_geoms.end();i++)
					dGeomGetUserData(*i)->material=m;
}



void CPHElement::get_LinearVel(Fvector& velocity)
{
	if(!bActive)
	{
		velocity.set(0,0,0);
		return;
	}
	Memory.mem_copy(&velocity,dBodyGetLinearVel(m_body),sizeof(Fvector));
}




void CPHElement::set_PushOut(u32 time)
{
	temp_for_push_out=object_contact_callback;

	set_ObjectContactCallback(PushOutCallback);
	if(bActive) push_untill=Device.dwTimeGlobal+time;
	else		push_untill=time;

}

void CPHElement::unset_Pushout()
{
	object_contact_callback=temp_for_push_out;
	temp_for_push_out=NULL;
	set_ObjectContactCallback(object_contact_callback);
	push_untill=0;
}

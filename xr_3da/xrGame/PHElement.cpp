#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHFracture.h"
///////////////////////////////////////////////////////////////
#pragma warning(disable:4995)
#pragma warning(disable:4267)

#include "../ode/src/collision_kernel.h"

//for debug only
//#include <../ode\src\joint.h>
//struct dxSphere : public dxGeom {
//	dReal radius;		// sphere radius
//	dxSphere (dSpaceID space, dReal _radius);
//	void computeAABB();
//};
//#include <../ode\src\objects.h>
#pragma warning(default:4267)
#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHShell.h"
#include "PHElement.h"





/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Implementation for CPhysicsElement
void CPHElement::			add_Box		(const Fobb&		V)
{
	Fobb box;
	box=V;
	if(box.m_halfsize.x<0.005f) box.m_halfsize.x=0.005f;
	if(box.m_halfsize.y<0.005f) box.m_halfsize.y=0.005f;
	if(box.m_halfsize.z<0.005f) box.m_halfsize.z=0.005f;
	m_geoms.push_back(dynamic_cast<CODEGeom*>(xr_new<CBoxGeom>(box)));

}

void CPHElement::			build_Geom	(CODEGeom& geom)
{
		
		geom.build(m_mass_center);
		geom.set_body(m_body);
		geom.set_material(ul_material);
		if(contact_callback)geom.set_contact_cb(contact_callback);
		if(object_contact_callback)geom.set_obj_contact_cb(object_contact_callback);
		if(m_phys_ref_object) geom.set_ref_object(m_phys_ref_object);
		if(m_group)
		{
			geom.add_to_space((dSpaceID)m_group);
		}
}

void CPHElement::build_Geom(u16 i)
{
CODEGeom& geom=*m_geoms[i];
build_Geom(geom);
geom.element_position()=i;
}

void CPHElement::			add_Sphere	(const Fsphere&	V)
{
	m_geoms.push_back(dynamic_cast<CODEGeom*>(xr_new<CSphereGeom>(V)));
}

void CPHElement::add_Cylinder	(const Fcylinder& V)
{
	m_geoms.push_back(dynamic_cast<CODEGeom*>(xr_new<CCylinderGeom>(V)));
}

void CPHElement::			build	(){

	m_body=dBodyCreate(phWorld);
	m_saved_contacts=dJointGroupCreate (0);
	b_contacts_saved=false;
	dBodyDisable(m_body);
	//dBodySetFiniteRotationMode(m_body,1);
	//dBodySetFiniteRotationAxis(m_body,0,0,0);

	dBodySetMass(m_body,&m_mass);

	if(m_geoms.size()>1)
	{
		m_group=dSimpleSpaceCreate(0);
		dSpaceSetCleanup(m_group,0);
	}
	m_inverse_local_transform.identity();
	m_inverse_local_transform.c.set(m_mass_center);
	m_inverse_local_transform.invert();
	dBodySetPosition(m_body,m_mass_center.x,m_mass_center.y,m_mass_center.z);
	///////////////////////////////////////////////////////////////////////////////////////
	//GEOM_I i_geom=m_geoms.begin(),e=m_geoms.end();
	//for(;i_geom!=e;++i_geom) build_Geom(**i_geom);
	u16 geoms_size=u16(m_geoms.size());
	for(u16 i=0;i<geoms_size;++i) build_Geom(i);
}

void CPHElement::RunSimulation()
{
	if(push_untill)
		push_untill+=Device.dwTimeGlobal;

	if(m_group)
		dSpaceAdd(m_shell->GetSpace(),(dGeomID)m_group);
	else
		(*m_geoms.begin())->add_to_space(m_shell->GetSpace());
	dBodyEnable(m_body);
}

void CPHElement::destroy	()
{
	dJointGroupDestroy(m_saved_contacts);
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();


	for(;i!=e;++i)
	{
		(*i)->destroy();
	}

	if(m_body)
	{
		dBodyDestroy(m_body);
		m_body=NULL;
	}

	if(m_group){
		dGeomDestroy(m_group);
		m_group=NULL;
	}
}

Fvector CPHElement::			get_mc_data	(){
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

void CPHElement::			calc_volume_data	()
{
	m_volume=0.f;
	GEOM_I i_geom=m_geoms.begin(),e=m_geoms.end();
	for(;i_geom!=e;++i_geom)
	{
		m_volume+=(*i_geom)->volume();
	}
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

void CPHElement::calculate_it_data(const Fvector& mc,float mas)
{
	float density=mas/m_volume;
	calculate_it_data_use_density(mc,density);
}


void CPHElement::calculate_it_data_use_density(const Fvector& mc,float density)
{
	dMassSetZero(&m_mass);
	GEOM_I i_geom=m_geoms.begin(),e=m_geoms.end();
	for(;i_geom!=e;++i_geom)(*i_geom)->add_self_mass(m_mass,mc,density);
}
static float static_dencity;
void CPHElement::calc_it_fract_data_use_density(const Fvector& mc,float density)
{
	m_mass_center.set(mc);
	dMassSetZero(&m_mass);
	static_dencity=density;
	recursive_mass_summ(0,m_fratures_holder->m_fractures.begin());
}

dMass CPHElement::recursive_mass_summ(u16 start_geom,FRACTURE_I cur_fracture)
{
	dMass end_mass;
	dMassSetZero(&end_mass);
	GEOM_I i_geom=m_geoms.begin()+start_geom,	e=m_geoms.begin()+cur_fracture->m_start_geom_num;
	for(;i_geom!=e;++i_geom)(*i_geom)->add_self_mass(end_mass,m_mass_center,static_dencity);
	dMassAdd(&m_mass,&end_mass);
	start_geom=cur_fracture->m_start_geom_num;
	++cur_fracture;
	if(m_fratures_holder->m_fractures.end() != cur_fracture)
				cur_fracture->SetMassParts(m_mass,recursive_mass_summ(start_geom,cur_fracture));
	return end_mass;
}
void		CPHElement::	setDensity		(float M)
{
	calculate_it_data_use_density(get_mc_data(),M);
}

void		CPHElement::	setMass		(float M)
{

	calculate_it_data(get_mc_data(),M);
}

void		CPHElement::	setDensityMC		(float M,const Fvector& mass_center)
{
	m_mass_center.set(mass_center);
	calc_volume_data();
	calculate_it_data_use_density(mass_center,M);
}

void		CPHElement::	setMassMC		(float M,const Fvector& mass_center)
{
	m_mass_center.set(mass_center);
	calc_volume_data();
	calculate_it_data(mass_center,M);
}



void		CPHElement::Start()
{
	build();
	RunSimulation();
}

void		CPHElement::Deactivate()
{
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
	VERIFY2(_valid(m0),"invalid_form_in_set_transform");
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


CPHElement::~CPHElement	()
{
	Deactivate();
	GEOM_I i_geom=m_geoms.begin(),e=m_geoms.end();
	for(;i_geom!=e;++i_geom)xr_delete(*i_geom);
	m_geoms.clear();
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

void CPHElement::Update(){
	if(!bActive) return;
	if( !dBodyIsEnabled(m_body)) return;

	//		PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),
	//					  dBodyGetPosition(m_body),
	//					  mXFORM);

	m_body_interpolation.InterpolateRotation(mXFORM);	
	m_body_interpolation.InterpolatePosition(mXFORM.c);
	
	
	mXFORM.mulB(m_inverse_local_transform);
	
	if(push_untill)//temp_for_push_out||(!temp_for_push_out&&object_contact_callback)
		if(push_untill<Device.dwTimeGlobal) unset_Pushout();
	VERIFY2(_valid(mXFORM),"invalid position in update");
}


void CPHElement::PhDataUpdate(dReal step){

	if(! bActive)return;

	///////////////skip for disabled elements////////////////////////////////////////////////////////////
	if( !dBodyIsEnabled(m_body)) 
	{
		if(dInfinity != previous_p[0]) previous_p[0]=dInfinity;//disable
		return;
	}

//////////////////////////////////base pointers/////////////////////////////////////////////////
	const dReal* linear_velocity	=	dBodyGetLinearVel(m_body)	;
	const dReal* angular_velocity	=	dBodyGetAngularVel(m_body)	;

	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////scale velocity///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	dBodySetLinearVel(
						m_body,
						linear_velocity[0]			/m_l_scale		,
						linear_velocity[1]			/m_l_scale		,
						linear_velocity[2]			/m_l_scale
					 );
	dBodySetAngularVel(
						m_body,
						angular_velocity[0]			/m_w_scale		,
						angular_velocity[1]			/m_w_scale		,
						angular_velocity[2]			/m_w_scale
						);


///////////////////scale changes values directly so get base values after it/////////////////////////
/////////////////////////////base values////////////////////////////////////////////////////////////
	dReal linear_velocity_smag	=		dDOT(linear_velocity,linear_velocity);
	dReal linear_velocity_mag		=	_sqrt(linear_velocity_smag);
	
	dReal angular_velocity_smag	=	dDOT(angular_velocity,angular_velocity);
	dReal angular_velocity_mag	=	_sqrt(angular_velocity_smag);
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////limit velocity & secure /////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////limit linear vel////////////////////////////////////////////////////////////////////////////////////////
	

	if(!dV_valid(linear_velocity))
	{
		dBodySetLinearVel(m_body,0.f,0.f,0.f);
		linear_velocity_smag=0.f;
		linear_velocity_mag =0.f;
	}
	else if(linear_velocity_mag>m_l_limit)
	     {
			dReal f=linear_velocity_mag/m_l_limit;
			linear_velocity_mag=m_l_limit;
			linear_velocity_smag=m_l_limit*m_l_limit;
			dBodySetLinearVel(
				m_body,
				linear_velocity[0]/f,
				linear_velocity[1]/f,
				linear_velocity[2]/f
				);
		 }

	////////////////secure position///////////////////////////////////////////////////////////////////////////////////
	const dReal* position=dBodyGetPosition(m_body);
	if(!dV_valid(position)) 
	{
		dBodySetPosition(
			m_body,
			m_safe_position[0]-m_safe_velocity[0]*fixed_step,
			m_safe_position[1]-m_safe_velocity[1]*fixed_step,
			m_safe_position[2]-m_safe_velocity[2]*fixed_step
			);
	}
	else
	{
		Memory.mem_copy(m_safe_position,position,sizeof(dVector3));
		Memory.mem_copy(m_safe_velocity,dBodyGetLinearVel(m_body),sizeof(dVector3));
	}

	/////////////////limit & secure angular vel///////////////////////////////////////////////////////////////////////////////

	if(!dV_valid(angular_velocity)) 
		dBodySetAngularVel(m_body,0.f,0.f,0.f);
	else if(angular_velocity_mag>m_w_limit)
	{
		dReal f=angular_velocity_mag/m_w_limit;
		angular_velocity_mag=m_w_limit;
		angular_velocity_smag=m_w_limit*m_w_limit;
		dBodySetAngularVel(
							m_body,
							angular_velocity[0]/f,
							angular_velocity[1]/f,
							angular_velocity[2]/f);
	}

	////////////////secure rotation////////////////////////////////////////////////////////////////////////////////////////
	{
		const dReal* rotation=dBodyGetRotation(m_body);

		if(!dV_valid(rotation))
		{
			dMatrix3 m;
			dRSetIdentity(m);
			dBodySetRotation(m_body,m);

		}

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////disable///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//
	if(!dBodyIsEnabled(m_body)) return;
	ReEnable();
	Disabling();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////air resistance/////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(!fis_zero(k_w))
			dBodyAddTorque(
							m_body,
							-angular_velocity[0]*k_w,
							-angular_velocity[1]*k_w,
							-angular_velocity[2]*k_w
							);

	dMass mass;
	dBodyGetMass(m_body,&mass);
	dReal l_air=linear_velocity_mag*k_l;//force/velocity !!!
	if(l_air>mass.mass/fixed_step) l_air=mass.mass/fixed_step;//validate

	if(!fis_zero(l_air))
			dBodyAddForce(
							m_body,
							-linear_velocity[0]*l_air,
							-linear_velocity[1]*l_air,
							-linear_velocity[2]*l_air
							);

VERIFY2(dV_valid(dBodyGetPosition(m_body)),"invalid body position");
VERIFY2(dM_valid(dBodyGetRotation(m_body)),"invalid body rotation");
UpdateInterpolation				();

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
		if(dInfinity != previous_p[0]){
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
			if(mag_v<m_disl_param * dis_frames && deviation<m_disw_param*dis_frames)
				Disable();//dBodyDisable(m_body);//
			if((!(previous_dev<deviation)&&!(previous_v<mag_v))//
				) 
			{
				++dis_count_f;
				previous_dev=deviation;
				previous_v=mag_v;
				//return;
			}
			else{
				previous_dev=deviation;
				previous_v=mag_v;
				dis_count_f=1;
				dis_count_f1=0;
				Memory.mem_copy(previous_p,current_p,sizeof(dVector3));
				Memory.mem_copy(previous_r,current_r,sizeof(dMatrix3));
				//previous_p[0]=dInfinity;
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
				if(mag_v<0.32* dis_frames && deviation<0.24*dis_frames)//0.16,0.06
					++dis_count_f1;
				else{
					Memory.mem_copy(previous_p1,current_p,sizeof(dVector3));
					Memory.mem_copy(previous_r1,current_r,sizeof(dMatrix3));
				}

				if(dis_count_f1>5) 
				{
					dis_count_f*=10;//10
					dis_count_f1=0;
				}

			}


		}

	}
	/////////////////////////////////////////////////////////////////

}

void CPHElement::ResetDisable(){
	previous_p[0]=dInfinity;
	previous_r[0]=0.f;
	dis_count_f	 =1;
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
	for(int i=0;i<num;++i){
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
	FillInterpolation();
	if(m_group)
		SaveContacts(ph_world->GetMeshGeom(),m_group,m_saved_contacts);
	else 
		SaveContacts(ph_world->GetMeshGeom(),(*m_geoms.begin())->geometry_transform(),m_saved_contacts);

	dBodyDisable(m_body);
}


void CPHElement::ReEnable(){
	//if(b_contacts_saved && dBodyIsEnabled(m_body))
	//{
	dJointGroupEmpty(m_saved_contacts);
	b_contacts_saved=false;
	//}
}


void	CPHElement::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,u16 id)
{

	if(!bActive) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	val/=fixed_step;
	Fvector body_pos;
	body_pos.sub(pos,m_inverse_local_transform.c);
	
	Fvector impulse;
	impulse.set(dir);
	impulse.mul(val);
	dBodyAddForceAtRelPos(m_body, impulse.x,impulse.y,impulse.z,body_pos.x, body_pos.y, body_pos.z);
	if(m_fratures_holder)
	{
		impulse.add(*((Fvector*)dBodyGetPosition(m_body)));
		m_fratures_holder->AddImpact(impulse,body_pos,m_shell->BoneIdToRootGeom(id));
	}
	
	BodyCutForce(m_body,m_l_limit,m_w_limit);
}

void CPHElement::InterpolateGlobalTransform(Fmatrix* m){
	m_body_interpolation.InterpolateRotation(*m);
	m_body_interpolation.InterpolatePosition(m->c);
	m->mulB(m_inverse_local_transform);
	bUpdate=false;
}
void CPHElement::GetGlobalTransformDynamic(Fmatrix* m)
{
	PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),dBodyGetPosition(m_body),*m);
	m->mulB(m_inverse_local_transform);
}

void CPHElement::InterpolateGlobalPosition(Fvector* v){
	m_body_interpolation.InterpolatePosition(*v);
	//v->add(m_inverse_local_transform.c);

}

void CPHElement::GetGlobalPositionDynamic(Fvector* v)
{
	//Fmatrix m;

	//PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),dBodyGetPosition(m_body),m);
	//m.transform_tiny(*v,m_inverse_local_transform.c);

	//v->add(*((Fvector*)dBodyGetPosition(m_body)),tv);
	v->set((*(Fvector*)dBodyGetPosition(m_body)));
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
	if(bActive) return;
	bActive=true;
	bActivating=true;
	Start();
	//	if(place_current_forms)
	{
		Fmatrix globe;
		globe.mul(start_from,mXFORM);
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
		if(ph_world->GetSpace()->lock_count) return;
		mXFORM.set(B->mTransform);
		bActivating=true;
		bActive=true;
		m_start_time=Device.fTimeGlobal;

		if(!m_parent_element) 
			m_shell->CreateSpace();
		build();


		Fmatrix global_transform;
		global_transform.set(m_shell->mXFORM);
		global_transform.mulB(mXFORM);
		SetTransform(global_transform);
		m_body_interpolation.SetBody(m_body);
		return;
	}
	Fmatrix parent;
	if(bActivating){
	if(ph_world->GetSpace()->lock_count) return;
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

void CPHElement::CallBack1(CBoneInstance* B)
{
	Fmatrix parent;
	if(! bActive)return;
	if(bActivating)
	{
		//if(!dBodyIsEnabled(m_body))
		//	dBodyEnable(m_body);
		if(ph_world->GetSpace()->lock_count) return;
		mXFORM.set(B->mTransform);
		m_start_time=Device.fTimeGlobal;
		Fmatrix global_transform;
		global_transform.set(m_shell->mXFORM);
		//if(m_parent_element)
		global_transform.mulB(mXFORM);
		SetTransform(global_transform);
		FillInterpolation();
		bActivating=false;
		if(!m_parent_element) 
		{
			m_shell->m_object_in_root.set(mXFORM);
			m_shell->m_object_in_root.invert();
			m_shell->bActivating=false;
		}
		B->Callback_overwrite=TRUE;
		return;
	}

	if(push_untill)//temp_for_push_out||(!temp_for_push_out&&object_contact_callback)
		if(push_untill<Device.dwTimeGlobal) unset_Pushout();

	if(!m_parent_element)
	{
			m_shell->InterpolateGlobalTransform(&(m_shell->mXFORM));
	}
	
	if( !dBodyIsEnabled(m_body) && !bUpdate) return;

	{
		InterpolateGlobalTransform(&mXFORM);

		parent.set(m_shell->mXFORM);
		parent.invert();
		mXFORM.mulA(parent);
		B->mTransform.set(mXFORM);
	}
	//else
	//{

	//	InterpolateGlobalTransform(&m_shell->mXFORM);
	//	mXFORM.identity();
	//	B->mTransform.set(mXFORM);
		//parent.set(B->mTransform);
		//parent.invert();
		//m_shell->mXFORM.mulB(parent);

	//}


}

void CPHElement::set_PhysicsRefObject(CPhysicsRefObject* ref_object)
{
	m_phys_ref_object=ref_object;
	if(!bActive) return;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_ref_object(ref_object);
}


void CPHElement::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	object_contact_callback= callback;
	if(!bActive)return;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_obj_contact_cb(callback);
}

void CPHElement::set_ContactCallback(ContactCallbackFun* callback)
{
	contact_callback=callback;
	push_untill=0;
	if(!bActive)return;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_contact_cb(callback);
}

void CPHElement::SetPhObjectInGeomData(CPHObject* O)
{
	if(!bActive) return;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_ph_object(O);
}



void CPHElement::SetMaterial(u32 m)
{
	ul_material=m;
	if(!bActive) return;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i) (*i)->set_material(m);
}



void CPHElement::get_LinearVel(Fvector& velocity)
{
	if(!bActive||!dBodyIsEnabled(m_body))
	{
		velocity.set(0,0,0);
		return;
	}
	Memory.mem_copy(&velocity,dBodyGetLinearVel(m_body),sizeof(Fvector));
}
void CPHElement::get_AngularVel	(Fvector& velocity)
{
	if(!bActive)
	{
		velocity.set(0,0,0);
		return;
	}
	Memory.mem_copy(&velocity,dBodyGetAngularVel(m_body),sizeof(Fvector));
}

void CPHElement::set_LinearVel			  (const Fvector& velocity)
{
	if(!bActive) return;
	dBodySetLinearVel(m_body,velocity.x,velocity.y,velocity.z);
}
void CPHElement::set_AngularVel			  (const Fvector& velocity)
{
	if(!bActive) return;
	dBodySetAngularVel(m_body,velocity.x,velocity.y,velocity.z);
}

void CPHElement::set_PushOut(u32 time,PushOutCallbackFun* push_out)
{
	temp_for_push_out=object_contact_callback;

	set_ObjectContactCallback(push_out);
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

void CPHElement::add_Shape(const SBoneShape& shape,const Fmatrix& offset)
{
	switch(shape.type) {
	case SBoneShape::stBox	:
		{
			Fobb box=shape.box;
			Fmatrix m;
			m.set(offset);
			//Fmatrix position;
			//position.set(box.m_rotate);
			//position.c.set(box.m_translate);
			//position.mulA(offset);
			//box.m_rotate.set(position);
			//box.m_translate.set(position.c);
			box.transform(box,m);
			add_Box(box);
			break;
		}
	case SBoneShape::stSphere	:
		{
			Fsphere sphere=shape.sphere;
			offset.transform_tiny(sphere.P);
			add_Sphere(sphere);
			break;
		}


	case SBoneShape::stCylinder :
		{
			Fcylinder C=shape.cylinder;
			offset.transform_tiny(C.m_center);
			offset.transform_dir(C.m_direction);
			add_Cylinder(C);
			break;
		}


	case SBoneShape::stNone :
		break;
	default: NODEFAULT;
	}
}

void CPHElement::add_Shape(const SBoneShape& shape)
{
	switch(shape.type) {
	case SBoneShape::stBox	:
		{
			add_Box(shape.box);
			break;
		}
	case SBoneShape::stSphere	:
		{
			add_Sphere(shape.sphere);
			break;
		}


	case SBoneShape::stCylinder :
		{
			add_Cylinder(shape.cylinder);
			break;
		}

	case SBoneShape::stNone :
		break;
	default: NODEFAULT;
	}
}

#pragma todo(remake it using Geometry functions)

void CPHElement::add_Mass(const SBoneShape& shape,const Fmatrix& offset,const Fvector& mass_center,float mass,CPHFracture* fracture)
{

	dMass m;
	dMatrix3 DMatx;
	switch(shape.type) 
	{
	case SBoneShape::stBox	:
		{
			dMassSetBox(&m,1.f,shape.box.m_halfsize.x*2.f,shape.box.m_halfsize.y*2.f,shape.box.m_halfsize.z*2.f);
			dMassAdjust(&m,mass);
			Fmatrix box_transform;
			shape.box.xform_get(box_transform);
			PHDynamicData::FMX33toDMX(shape.box.m_rotate,DMatx);
			dMassRotate(&m,DMatx);
			dMassTranslate(&m,shape.box.m_translate.x-mass_center.x,shape.box.m_translate.y-mass_center.y,shape.box.m_translate.z-mass_center.z);
			break;
		}
	case SBoneShape::stSphere	:
		{
			shape.sphere;
			dMassSetSphere(&m,1.f,shape.sphere.R);
			dMassAdjust(&m,mass);
			dMassTranslate(&m,shape.sphere.P.x-mass_center.x,shape.sphere.P.y-mass_center.y,shape.sphere.P.z-mass_center.z);
			break;
		}


	case SBoneShape::stCylinder :
		{
			const Fvector& pos=shape.cylinder.m_center;
			Fvector l;
			l.sub(pos,mass_center);
			dMassSetCylinder(&m,1.f,2,shape.cylinder.m_radius,shape.cylinder.m_height);
			dMatrix3 DMatx;
			Fmatrix33 m33;
			m33.j.set(shape.cylinder.m_direction);
			Fvector::generate_orthonormal_basis(m33.j,m33.k,m33.i);
			PHDynamicData::FMX33toDMX(m33,DMatx);
			dMassRotate(&m,DMatx);
			dMassTranslate(&m,l.x,l.y,l.z);
			break;
		}

	case SBoneShape::stNone :
		break;
	default: NODEFAULT;
	}
	PHDynamicData::FMXtoDMX(offset,DMatx);
	dMassRotate(&m,DMatx);

	Fvector mc;
	offset.transform_tiny(mc,mass_center);
	//calculate _new mass_center 
	//new_mc=(m_mass_center*m_mass.mass+mc*mass)/(mass+m_mass.mass)
	Fvector tmp1;
	tmp1.set(m_mass_center);
	tmp1.mul(m_mass.mass);
	Fvector tmp2;
	tmp2.set(mc);
	tmp2.mul(mass);
	Fvector new_mc;
	new_mc.add(tmp1,tmp2);
	new_mc.mul(1.f/(mass+m_mass.mass));
	mc.sub(new_mc);
	dMassTranslate(&m,mc.x,mc.y,mc.z);
	m_mass_center.sub(new_mc);
	dMassTranslate(&m_mass,m_mass_center.x,m_mass_center.y,m_mass_center.z);
	if(m_fratures_holder)
	{
		m_fratures_holder->DistributeAdditionalMass(u16(m_geoms.size()-1),m);
	}
	if(fracture)
	{
		fracture->MassAddToSecond(m);
	}
	dMassAdd(&m_mass,&m);
	m_mass_center.set(new_mc);
}
float CPHElement::getRadius()
{
	if(!m_geoms.empty()) return m_geoms.back()->radius();
	else				  return 0.f;
}

void CPHElement::set_DynamicLimits(float l_limit,float w_limit)
{
	m_l_limit=l_limit;
	m_w_limit=w_limit;
}

void CPHElement::set_DynamicScales(float l_scale/* =default_l_scale */,float w_scale/* =default_w_scale */)
{
	m_l_scale=l_scale;
	m_w_scale=w_scale;
}

void CPHElement::set_DisableParams(float dis_l/* =default_disl */,float dis_w/* =default_disw */)
{
	m_disl_param=dis_l;
	m_disw_param=dis_w;
}




void CPHElement::get_Extensions(const Fvector& axis,float center_prg,float& lo_ext, float& hi_ext)
{
	lo_ext=dInfinity;hi_ext=-dInfinity;
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i)
	{
		float temp_lo_ext,temp_hi_ext;
		//GetTransformedGeometryExtensions((*i)->geometry_transform(),(float*)&axis,center_prg,&temp_lo_ext,&temp_hi_ext);
		(*i)->get_extensions_bt(axis,center_prg,temp_lo_ext,temp_hi_ext);
		if(lo_ext>temp_lo_ext)lo_ext=temp_lo_ext;
		if(hi_ext<temp_hi_ext)hi_ext=temp_hi_ext;
	}
	
}

const Fvector& CPHElement::mass_Center()
{
	return *((const Fvector*)dBodyGetPosition(m_body));
}

CPhysicsShell* CPHElement::PhysicsShell()
{
	return dynamic_cast<CPhysicsShell*>(m_shell);
}

dGeomID CPHElement::dSpacedGeometry()
{
	if(!bActive) return 0;
	if(m_group) return (dGeomID)m_group;
	else return (*m_geoms.begin())->geometry_transform();
}

void CPHElement::PassEndGeoms(u16 from,u16 to,CPHElement* dest)
{
	GEOM_I i_from=m_geoms.begin()+from,e=m_geoms.begin()+to;
	u16 shift=to-from;
	GEOM_I i=i_from;
	for(;i!=e;++i)
	{
		(*i)->remove_from_space(m_group);
		//(*i)->add_to_space(dest->m_group);
		//(*i)->set_body(dest->m_body);
		(*i)->set_body(0);
		u16& element_pos=(*i)->element_position();
		element_pos=element_pos-shift;
	}
	GEOM_I last=m_geoms.end();
	for(;i!=last;++i)
	{
		u16& element_pos=(*i)->element_position();
		element_pos=element_pos-shift;
	}

	dest->m_geoms.insert(dest->m_geoms.end(),i_from,e);
	m_geoms.erase(i_from,e);
}
void CPHElement::SplitProcess(ELEMENT_PAIR_VECTOR &new_elements)
{
	m_fratures_holder->SplitProcess(this,new_elements);
	if(!m_fratures_holder->m_fractures.size()) xr_delete(m_fratures_holder);
	
}


void CPHElement::CreateSimulBase()
{
	m_body=dBodyCreate(phWorld);
	m_saved_contacts=dJointGroupCreate (0);
	b_contacts_saved=false;
	dBodyDisable(m_body);
	if(m_geoms.size()>1)
	{
		m_group=dSimpleSpaceCreate(0);
		dSpaceSetCleanup(m_group,0);
	}
}
void CPHElement::ReAdjustMassPositions(const Fmatrix &shift_pivot,float density)
{
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i)
	{
		(*i)->move_local_basis(shift_pivot);
	}

	setDensity(density);
	dBodySetMass(m_body,&m_mass);
	m_inverse_local_transform.identity();
	m_inverse_local_transform.c.set(m_mass_center);
	m_inverse_local_transform.invert();
	//dBodySetPosition(m_body,m_mass_center.x,m_mass_center.y,m_mass_center.z);
}
void CPHElement::ResetMass(float density)
{
	Fvector tmp,shift_mc;

	tmp.set(m_mass_center);
	
	
	setDensity(density);
	dBodySetMass(m_body,&m_mass);
	m_inverse_local_transform.c.set(m_mass_center);
	m_inverse_local_transform.invert();
	shift_mc.sub(m_mass_center,tmp);
	tmp.set(*(Fvector *)dBodyGetPosition(m_body));
	tmp.add(shift_mc);
	//dBodySetPosition(m_body,tmp.x,tmp.y,tmp.z);

	bActivating = true;

	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i)
	{
		(*i)->set_position(m_mass_center);
	}
}
void CPHElement::ReInitDynamics(const Fmatrix &shift_pivot,float density)
{
	ReAdjustMassPositions(shift_pivot,density);
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i)
	{
		(*i)->set_position(m_mass_center);
		(*i)->set_body(m_body);
		//if(object_contact_callback)geom.set_obj_contact_cb(object_contact_callback);
		//if(m_phys_ref_object) geom.set_ref_object(m_phys_ref_object);
		if(m_group)
		{
			(*i)->add_to_space((dSpaceID)m_group);
		}
	}	
}

void CPHElement::PresetActive()
{
	if(bActive) return;
	
	CBoneInstance& B=m_shell->PKinematics()->LL_GetBoneInstance(m_SelfID);
	mXFORM.set(B.mTransform);
	m_start_time=Device.fTimeGlobal;
	Fmatrix global_transform;
	global_transform.set(m_shell->mXFORM);
	global_transform.mulB(mXFORM);
	SetTransform(global_transform);

	if(!m_parent_element) 
	{
		m_shell->m_object_in_root.set(mXFORM);
		m_shell->m_object_in_root.invert();

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
	FillInterpolation();
	bActive=true;
	RunSimulation();
	
}

bool CPHElement::isBreakable()
{
	return !!m_fratures_holder;
}
u16	CPHElement::setGeomFracturable(CPHFracture& fracture)
{
	if(!m_fratures_holder) m_fratures_holder=xr_new<CPHFracturesHolder>();
	return m_fratures_holder->AddFracture(fracture);
}

CPHFracture& CPHElement::Fracture(u16 num)
{
	R_ASSERT2(m_fratures_holder,"no fractures!");
	return m_fratures_holder->Fracture(num);
}
u16	CPHElement::numberOfGeoms()
{
	return (u16)m_geoms.size();
}
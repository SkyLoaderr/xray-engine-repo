#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHFracture.h"
#include "PHContactBodyEffector.h"
#include "MathUtils.h"
#include "PhysicsShellHolder.h"
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
CPHElement::CPHElement()																															//aux
{
	m_w_limit = default_w_limit;
	m_l_limit = default_l_limit;
	m_l_scale=default_l_scale;
	m_w_scale=default_w_scale;

	push_untill=0;

	temp_for_push_out=NULL;

	m_body=NULL;
	bActive=false;
	bActivating=false;
	m_parent_element=NULL;
	m_shell=NULL;


	k_w=default_k_w;
	k_l=default_k_l;//1.8f;
	m_fratures_holder=NULL;
	//b_enabled_onstep=false;
	//m_flags.set(flEnabledOnStep,FALSE);
	m_flags.assign(0);
}

void CPHElement::add_Box		(const Fobb&		V)
{
	CPHGeometryOwner::add_Box(V);
}


void CPHElement::add_Sphere	(const Fsphere&	V)
{
	CPHGeometryOwner::add_Sphere(V);
}

void CPHElement::add_Cylinder	(const Fcylinder& V)
{
	CPHGeometryOwner::add_Cylinder(V);
}

void CPHElement::			build	(){

	m_body=dBodyCreate(0);//phWorld
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
	dBodyDisable(m_body);
	//dBodySetFiniteRotationMode(m_body,1);
	//dBodySetFiniteRotationAxis(m_body,0,0,0);

	dBodySetMass(m_body,&m_mass);


	m_inverse_local_transform.identity();
	m_inverse_local_transform.c.set(m_mass_center);
	m_inverse_local_transform.invert();
	dBodySetPosition(m_body,m_mass_center.x,m_mass_center.y,m_mass_center.z);
	///////////////////////////////////////////////////////////////////////////////////////
	CPHGeometryOwner::build();
	set_body(m_body);
}

void CPHElement::RunSimulation()
{
	if(push_untill)
		push_untill+=Device.dwTimeGlobal;

	if(m_group)
		dSpaceAdd(m_shell->dSpace(),(dGeomID)m_group);
	else
		(*m_geoms.begin())->add_to_space(m_shell->dSpace());
	if(!m_body->world) 
	{
		//dWorldAddBody(phWorld, m_body);
		m_shell->Island().AddBody(m_body);
	}
	dBodyEnable(m_body);
}

void CPHElement::destroy	()
{
	//dJointGroupDestroy(m_saved_contacts);
	CPHGeometryOwner::destroy();
	if(m_body&&m_body->world)
	{
		m_shell->Island().RemoveBody(m_body);
		dBodyDestroy(m_body);
		m_body=NULL;
	}

	if(m_group){
		dGeomDestroy(m_group);
		m_group=NULL;
	}
}

void CPHElement::calculate_it_data(const Fvector& mc,float mas)
{
	float density=mas/m_volume;
	calculate_it_data_use_density(mc,density);
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
	CPHGeometryOwner::get_mc_vs_transform(mc,m0);
	dBodySetPosition(m_body,mc.x,mc.y,mc.z);
	Fmatrix33 m33;
	m33.set(m0);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m33,R);
	dBodySetRotation(m_body,R);
	dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));
	dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
}

void CPHElement::getQuaternion(Fquaternion& quaternion)
{
	if(!bActive) return;
	const float* q=dBodyGetQuaternion(m_body);
	quaternion.set(-q[0],q[1],q[2],q[3]);
}
void CPHElement::setQuaternion(const Fquaternion& quaternion)
{
	if(!bActive) return;
	dQuaternion q={-quaternion.w,quaternion.x,quaternion.y,quaternion.z};
	dBodySetQuaternion(m_body,q);
}
void CPHElement::GetGlobalPositionDynamic(Fvector* v)
{
	if(!bActive) return;
	v->set((*(Fvector*)dBodyGetPosition(m_body)));
}

void CPHElement::SetGlobalPositionDynamic(const Fvector& position)
{
	if(!bActive) return;
	dBodySetPosition(m_body,position.x,position.y,position.z);
}
CPHElement::~CPHElement	()
{
	Deactivate();
	DeleteFracturesHolder();

}

void CPHElement::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){
	mXFORM.set(transform);
	Start();
	SetTransform(transform);

	dBodySetLinearVel(m_body,lin_vel.x,lin_vel.y,lin_vel.z);

	dBodySetAngularVel(m_body,ang_vel.x,ang_vel.y,ang_vel.z);

	dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
	dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));

	m_body_interpolation.SetBody(m_body);

	if(disable) dBodyDisable(m_body);
	bActive=true;
	bActivating=true;
}
void CPHElement::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){

	Fvector lvel,avel;
	lvel.set(m2.c.x-m0.c.x,m2.c.y-m0.c.y,m2.c.z-m0.c.z);
	avel.set(0.f,0.f,0.f);
	Activate(m0,lvel,avel,disable);
}


void CPHElement::Activate(bool disable){

	Fvector lvel,avel;
	lvel.set(0.f,0.f,0.f);
	avel.set(0.f,0.f,0.f);
	Activate(mXFORM,lvel,avel,disable);

}

void CPHElement::Activate(const Fmatrix& start_from,bool disable){
	Fmatrix globe;
	globe.mul(start_from,mXFORM);

	Fvector lvel,avel;
	lvel.set(0.f,0.f,0.f);
	avel.set(0.f,0.f,0.f);
	Activate(globe,lvel,avel,disable);

}

void CPHElement::Update(){
	if(!bActive) return;
	if(bActivating) bActivating=false;
	if( !dBodyIsEnabled(m_body)&&!m_flags.test(flUpdate)/*!bUpdate*/) return;

	//		PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),
	//					  dBodyGetPosition(m_body),
	//					  mXFORM);

	InterpolateGlobalTransform(&mXFORM);

	if(push_untill)//temp_for_push_out||(!temp_for_push_out&&object_contact_callback)
		if(push_untill<Device.dwTimeGlobal) unset_Pushout();
	VERIFY2(_valid(mXFORM),"invalid position in update");
}

void CPHElement::PhTune(dReal step)
{
	if(!bActive) return;
	CPHContactBodyEffector* contact_effector=
		(CPHContactBodyEffector*) dBodyGetData(m_body);
	if(contact_effector)contact_effector->Apply();

}
void CPHElement::PhDataUpdate(dReal step){

	if(! bActive)return;

	///////////////skip for disabled elements////////////////////////////////////////////////////////////
	//b_enabled_onstep=!!dBodyIsEnabled(m_body);
	m_flags.set(flEnabledOnStep,!!dBodyIsEnabled(m_body));
	if(!m_flags.test(flEnabledOnStep)/*!b_enabled_onstep*/) return;
	


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
		dBodySetLinearVel(m_body,m_safe_velocity[0],m_safe_velocity[1],m_safe_velocity[2]);
		linear_velocity_smag=dDOT(m_safe_velocity,m_safe_velocity);
		linear_velocity_mag =dSqrt(linear_velocity_smag);

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
		dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));
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
		dVectorSet(m_safe_position,position);

		
	}

	/////////////////limit & secure angular vel///////////////////////////////////////////////////////////////////////////////

	if(!dV_valid(angular_velocity))
	{
		dBodySetAngularVel(m_body,0.f,0.f,0.f);

	}
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


		if(!dQ_valid(dBodyGetQuaternion(m_body)))
		{

			dBodySetQuaternion(m_body,m_safe_quaternion);

		}
		else dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////disable///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(dBodyIsEnabled(m_body)) Disabling();
	if(!dBodyIsEnabled(m_body)) return;

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
	VERIFY2(dV_valid(dBodyGetQuaternion(m_body)),"invalid body rotation");
	UpdateInterpolation				();

}

void CPHElement::Enable()	{
	if(!bActive) return;
	m_shell->EnableObject();
	if(dBodyIsEnabled(m_body)) return;
	dBodyEnable(m_body);
}

void CPHElement::Disable()	{

//	return;
	if(!dBodyIsEnabled(m_body)) return;
	FillInterpolation();
	//if(!b_contacts_saved)
	//{
	//	if(m_group)
	//		SaveContacts(ph_world->GetMeshGeom(),m_group,m_saved_contacts);
	//	else 
	//		SaveContacts(ph_world->GetMeshGeom(),(*m_geoms.begin())->geometry_transform(),m_saved_contacts);
	//	b_contacts_saved=true;
	//}
	//	dBodySetForce(m_body,0.f,0.f,0.f);
	//	dBodySetTorque(m_body,0.f,0.f,0.f);
	//	dBodySetLinearVel(m_body,0.f,0.f,0.f);
	//	dBodySetAngularVel(m_body,0.f,0.f,0.f);
	dBodyDisable(m_body);
}

void CPHElement::ReEnable(){

	//dJointGroupEmpty(m_saved_contacts);
	//b_contacts_saved=false;

}

void	CPHElement::Freeze()
{
	if(!m_body) return;
	//was_enabled_before_freeze=!!dBodyIsEnabled(m_body);
	m_flags.set(flWasEnabledBeforeFreeze,!!dBodyIsEnabled(m_body));
	dBodyDisable(m_body);
}
void	CPHElement::UnFreeze()
{
	if(!m_body) return;
	if(m_flags.test(flWasEnabledBeforeFreeze)/*was_enabled_before_freeze*/)dBodyEnable(m_body);
}
void	CPHElement::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,u16 id)
{

	if(!bActive||m_flags.test(flFixed)) return;
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
		///impulse.add(*((Fvector*)dBodyGetPosition(m_body)));
		m_fratures_holder->AddImpact(impulse,body_pos,m_shell->BoneIdToRootGeom(id));
	}

	BodyCutForce(m_body,m_l_limit,m_w_limit);
}
void CPHElement::applyImpact(const SPHImpact& I)
{
	Fvector pos;
	pos.add(m_inverse_local_transform.c,I.point);
	Fvector dir;
	dir.set(I.force);
	float val=I.force.magnitude();
	
	if(!fis_zero(val))
	{
		dir.mul(1.f/val);
		applyImpulseTrace(pos,dir,val,I.geom);
	}
	
}
void CPHElement::InterpolateGlobalTransform(Fmatrix* m){
	m_body_interpolation.InterpolateRotation(*m);
	m_body_interpolation.InterpolatePosition(m->c);
	m->mulB_43(m_inverse_local_transform);
	
	//bUpdate=false;
	m_flags.set(flUpdate,FALSE);
}
void CPHElement::GetGlobalTransformDynamic(Fmatrix* m)
{
	PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),dBodyGetPosition(m_body),*m);
	m->mulB(m_inverse_local_transform);
	//bUpdate=false;
	m_flags.set(flUpdate,FALSE);
}

void CPHElement::InterpolateGlobalPosition(Fvector* v){
	m_body_interpolation.InterpolatePosition(*v);
	//v->add(m_inverse_local_transform.c);

}




void CPHElement::build(bool disable){

	if(bActive) return;
	bActive=true;
	bActivating=true;

	build();
	//	if(place_current_forms)
	{

		SetTransform(mXFORM);
	}

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);


}

void CPHElement::RunSimulation(const Fmatrix& start_from)
{

	RunSimulation();
	//	if(place_current_forms)
	{
		Fmatrix globe;
		globe.mul(start_from,mXFORM);
		SetTransform(globe);
	}
	dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
	dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));

}



void CPHElement::StataticRootBonesCallBack(CBoneInstance* B)
{
	Fmatrix parent;
	if(! bActive)return;
	VERIFY(_valid(m_shell->mXFORM));
	VERIFY2(!fis_zero(DET((B->mTransform))),"Bones callback resive 0 matrix");

	if(bActivating)
	{
		//if(!dBodyIsEnabled(m_body))
		//	dBodyEnable(m_body);
		//if(ph_world->GetSpace()->lock_count) return;
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
		VERIFY2(!fis_zero(DET((B->mTransform))),"Bones callback returns 0 matrix");
		return;
	}

	if(push_untill)//temp_for_push_out||(!temp_for_push_out&&object_contact_callback)
		if(push_untill<Device.dwTimeGlobal) unset_Pushout();

	VERIFY2(!fis_zero(DET((B->mTransform))),"Bones callback returns 0 matrix");
	if( !m_shell->is_active() && !m_flags.test(flUpdate)/*!bUpdate*/ ) return;

	{
		InterpolateGlobalTransform(&mXFORM);
		parent.set(m_shell->mXFORM);
		parent.invert();
		mXFORM.mulA(parent);
		B->mTransform.set(mXFORM);
	}
	VERIFY2(!fis_zero(DET((B->mTransform))),"Bones callback returns 0 matrix");
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

void CPHElement::BonesCallBack(CBoneInstance* B)
{
	Fmatrix parent;
	if(! bActive)return;
	VERIFY(_valid(m_shell->mXFORM));
	VERIFY2(!fis_zero(DET((B->mTransform))),"Bones callback receive 0 matrix");
	if(bActivating)
	{
		//if(!dBodyIsEnabled(m_body))
		//	dBodyEnable(m_body);
		//if(ph_world->GetSpace()->lock_count) return;
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
		VERIFY2(!fis_zero(DET((B->mTransform))),"Bones callback returns 0 matrix");
		return;
	}

	if(push_untill)//temp_for_push_out||(!temp_for_push_out&&object_contact_callback)
		if(push_untill<Device.dwTimeGlobal) unset_Pushout();

	if( !m_shell->is_active() && !m_flags.test(flUpdate)/*!bUpdate*/) return;

	if(!m_parent_element)
	{
	
		m_shell->InterpolateGlobalTransform(&(m_shell->mXFORM));
		VERIFY(_valid(m_shell->mXFORM));
	}
	VERIFY2(!fis_zero(DET((B->mTransform))),"Bones callback returns 0 matrix");

	
	{
		InterpolateGlobalTransform(&mXFORM);
		parent.set(m_shell->mXFORM);
		parent.invert();
		mXFORM.mulA(parent);
		B->mTransform.set(mXFORM);
	}
	VERIFY2(!fis_zero(DET((B->mTransform))),"Bones callback returns 0 matrix");
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

void CPHElement::set_PhysicsRefObject(CPhysicsShellHolder* ref_object)
{
	CPHGeometryOwner::set_PhysicsRefObject(ref_object);
}


void CPHElement::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	CPHGeometryOwner::set_ObjectContactCallback(callback);
}

void CPHElement::set_ContactCallback(ContactCallbackFun* callback)
{
	push_untill=0;
	CPHGeometryOwner::set_ContactCallback(callback);
}

void CPHElement::SetMaterial(u16 m)
{
	CPHGeometryOwner::SetMaterial(m);
}

dMass*	CPHElement::getMassTensor()																						//aux
{
	return &m_mass;
}

void	CPHElement::setInertia(const dMass& M)
{
	m_mass=M;
	if(!bActive||m_flags.test(flFixed))return;
	dBodySetMass(m_body,&M);
}

void	CPHElement::addInertia(const dMass& M)
{
	dMassAdd(&m_mass,&M);
	if(!bActive)return;
	dBodySetMass(m_body,&m_mass);
}
void CPHElement::get_LinearVel(Fvector& velocity)
{
	if(!bActive||!dBodyIsEnabled(m_body))
	{
		velocity.set(0,0,0);
		return;
	}
	dVectorSet((dReal*)&velocity,dBodyGetLinearVel(m_body));
	
}
void CPHElement::get_AngularVel	(Fvector& velocity)
{
	if(!bActive||!dBodyIsEnabled(m_body))
	{
		velocity.set(0,0,0);
		return;
	}
	dVectorSet((dReal*)&velocity,dBodyGetAngularVel(m_body));
}

void CPHElement::set_LinearVel			  (const Fvector& velocity)
{
	if(!bActive||m_flags.test(flFixed)) return;
	dBodySetLinearVel(m_body,velocity.x,velocity.y,velocity.z);
	dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));
}
void CPHElement::set_AngularVel			  (const Fvector& velocity)
{
	if(!bActive||m_flags.test(flFixed)) return;
	dBodySetAngularVel(m_body,velocity.x,velocity.y,velocity.z);
}

void	CPHElement::getForce(Fvector& force)
{
	if(!bActive) return;
	force.set(*(Fvector*)dBodyGetForce(m_body));
}
void	CPHElement::getTorque(Fvector& torque)
{
	if(!bActive) return;
	torque.set(*(Fvector*)dBodyGetTorque(m_body));
}
void	CPHElement::setForce(const Fvector& force)
{
	if(!bActive||m_flags.test(flFixed)) return;
	dBodySetForce(m_body,force.x,force.y,force.z);
}
void	CPHElement::setTorque(const Fvector& torque)
{
	if(!bActive||m_flags.test(flFixed)) return;
	dBodySetTorque(m_body,torque.x,torque.y,torque.z);
}

void	CPHElement::applyForce(const Fvector& dir, float val)															//aux
{
	applyForce				(dir.x*val,dir.y*val,dir.z*val);
}
void	CPHElement::applyForce(float x,float y,float z)																//called anywhere ph state influent
{
	if(!bActive)return;//hack??
	if(m_flags.test(flFixed)) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject();
	dBodyAddForce(m_body,x,y,z);
}

void	CPHElement::applyImpulse(const Fvector& dir, float val)//aux
{														
	applyForce(dir.x*val/fixed_step,dir.y*val/fixed_step,dir.z*val/fixed_step);
}

void CPHElement::set_PushOut(u32 time,ObjectContactCallbackFun* push_out)
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
	CPHGeometryOwner::add_Shape(shape,offset);
}

void CPHElement::add_Shape(const SBoneShape& shape)
{
	CPHGeometryOwner::add_Shape(shape);
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
			dMassAdjust(&m,mass);
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
	R_ASSERT2(dMass_valide(&m),"bad bone mass params");
	dMassAdd(&m_mass,&m);
	R_ASSERT2(dMass_valide(&m),"bad result mass params");
	m_mass_center.set(new_mc);
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

void CPHElement::calculate_it_data_use_density(const Fvector& mc,float density)
{
	dMassSetZero(&m_mass);
	GEOM_I i_geom=m_geoms.begin(),e=m_geoms.end();
	for(;i_geom!=e;++i_geom)(*i_geom)->add_self_mass(m_mass,mc,density);
}

float CPHElement::getRadius()
{
	return CPHGeometryOwner::getRadius();
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

void	CPHElement::set_DisableParams				(const SAllDDOParams& params)
{
	CPHDisablingFull::set_DisableParams(params);
}




void CPHElement::get_Extensions(const Fvector& axis,float center_prg,float& lo_ext, float& hi_ext)
{
	CPHGeometryOwner::get_Extensions(axis,center_prg,lo_ext,hi_ext);
}

const Fvector& CPHElement::mass_Center()
{
	return *((const Fvector*)dBodyGetPosition(m_body));
}

CPhysicsShell* CPHElement::PhysicsShell()
{
	return dynamic_cast<CPhysicsShell*>(m_shell);
}

CPHShell* CPHElement::PHShell()
{
	return (m_shell);
}
void	CPHElement::SetShell(CPHShell* p)
{
	if(!m_body||!m_shell)
		{
			m_shell=p;
			return;
		}
	if(m_shell!=p)
	{
		m_shell->Island().RemoveBody(m_body);
		p->Island().AddBody(m_body);
		m_shell=p;
	}

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
void CPHElement::DeleteFracturesHolder()
{
	xr_delete(m_fratures_holder);
}

void CPHElement::CreateSimulBase()
{
	m_body=dBodyCreate(0);
	m_shell->Island().AddBody(m_body);
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
	dBodyDisable(m_body);
	CPHGeometryOwner::CreateSimulBase();
}
void CPHElement::ReAdjustMassPositions(const Fmatrix &shift_pivot,float density)
{

	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i)
	{
		(*i)->move_local_basis(shift_pivot);
	}
	if(m_shell->PKinematics())
	{
		float mass;
		get_mc_kinematics(m_shell->PKinematics(),m_mass_center,mass);
		calculate_it_data(m_mass_center,mass);
	}
	else
	{
	
		setDensity(density);
	}

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

	CPHGeometryOwner::setPosition(m_mass_center);
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
	dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
	dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));

	//////////////////////////////////////////////////////////////
	//initializing values for disabling//////////////////////////
	//////////////////////////////////////////////////////////////
	VERIFY(dBodyStateValide(m_body));
	m_body_interpolation.SetBody(m_body);
	FillInterpolation();
	bActive=true;
	RunSimulation();
	VERIFY(dBodyStateValide(m_body));
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
	return CPHGeometryOwner::numberOfGeoms();
}


void CPHElement::cv2bone_Xfrom(const Fquaternion& q,const Fvector& pos, Fmatrix& xform)
{
	VERIFY2(_valid(q)&&_valid(pos),"cv2bone_Xfrom receive wrong data");
	xform.rotation(q);
	xform.c.set(pos);
	xform.mulB(m_inverse_local_transform);
	VERIFY2(_valid(xform),"cv2bone_Xfrom returns wrong data");
}
void CPHElement::cv2obj_Xfrom(const Fquaternion& q,const Fvector& pos, Fmatrix& xform)
{
	
	cv2bone_Xfrom(q,pos,xform);
	xform.mulB(m_shell->m_object_in_root);
	VERIFY2(_valid(xform),"cv2obj_Xfrom returns wrong data");
}

void CPHElement::set_ApplyByGravity(bool flag)
{
	if(!bActive||m_flags.test(flFixed)) return;
	dBodySetGravityMode(m_body,flag);
}
void	CPHElement::Fix()
{
	m_flags.set(flFixed,TRUE);
	FixBody(m_body);
}
void CPHElement::applyGravityAccel				(const Fvector& accel)
{
	if(m_flags.test(flFixed)) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject();
	Fvector val;
	val.set(accel);
	val.mul(m_mass.mass);
	//ApplyGravityAccel(m_body,(const dReal*)(&accel));
	applyForce(val.x,val.y,val.z);
}
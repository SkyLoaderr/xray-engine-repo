/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHShellSplitter.h"
#include "PHFracture.h"
#include "PHJointDestroyInfo.h"
#include "SpaceUtils.h"
#include "MathUtils.h"
#include "PhysicsShellHolder.h"
//#pragma warning(disable:4995)
//#pragma warning(disable:4267)
//#include "../ode/src/collision_kernel.h"
//#pragma warning(default:4995)
//#pragma warning(default:4267)
///////////////////////////////////////////////////////////////
///#pragma warning(disable:4995)

//#include <../ode/src/joint.h>
//#include <../ode/src/objects.h>

//#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"
#include "PHElement.h"
#include "PHShell.h"

CPHShell::~CPHShell				()							
{
	if(bActive) Deactivate();

	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();elements.end()!=i;++i)
		xr_delete(*i);
	elements.clear();

	xr_vector<CPHJoint*>::iterator j;
	for(j=joints.begin();joints.end()!=j;++j)
		xr_delete(*j);
	joints.clear();
	if(m_spliter_holder)xr_delete(m_spliter_holder);
}
CPHShell::CPHShell()
{
	bActive=false;
	bActivating=false;
	m_space=NULL;
	m_pKinematics=NULL;
	m_spliter_holder=NULL;
	m_object_in_root.identity();
}

void CPHShell::EnableObject()
{
	CPHObject::activate();
	if(m_spliter_holder)m_spliter_holder->Activate();
}
void CPHShell::DisableObject()
{
	InterpolateGlobalTransform(&mXFORM);
	CPHObject::deactivate();
	if(m_spliter_holder)m_spliter_holder->Deactivate();
}
void CPHShell::Disable()
{
	ELEMENT_I i,e;
	i=elements.begin(); e=elements.end();
	DisableObject();
	for( ;i!=e;++i)
	{
		(*i)->Disable();
	}
}
void CPHShell::ReanableObject()
{
	//if(b_contacts_saved) dJointGroupEmpty(m_saved_contacts);
	//b_contacts_saved=false;

}

void CPHShell::vis_update_activate()
{
	CPhysicsShellHolder* ref_object=(*elements.begin())->PhysicsRefObject();
	if(ref_object)ref_object->processing_activate();
}

void CPHShell::vis_update_deactivate()
{
		CPhysicsShellHolder* ref_object=(*elements.begin())->PhysicsRefObject();
		if(ref_object)ref_object->processing_deactivate();
}
void CPHShell::setDensity(float M)
{
	ELEMENT_I i;
	//float volume=0.f;
	//for(i=elements.begin();elements.end() != i;++i)	volume+=(*i)->get_volume();

	for(i=elements.begin();elements.end() != i;++i)
		(*i)->setDensity(M);
}


void CPHShell::setMass(float M){
	ELEMENT_I i;
	float volume=0.f;
	for(i=elements.begin();elements.end() != i;++i)	volume+=(*i)->get_volume();

	for(i=elements.begin();elements.end() != i;++i)
		(*i)->setMass(
		(*i)->get_volume()/volume*M
		);
}

void CPHShell::setMass1(float M){
	ELEMENT_I i;


	for(i=elements.begin();elements.end() != i;++i)
		(*i)->setMass(
		M/elements.size()
		);
}
float CPHShell::getMass()
{
	float m=0.f;

	ELEMENT_I i;

	for(i=elements.begin();elements.end() != i;++i)	m+=(*i)->getMass();

	return m;
}

void  CPHShell::get_spatial_params()
{
	spatialParsFromDGeom((dGeomID)m_space,spatial.center,AABB,spatial.radius);
}

float CPHShell::getVolume()
{
	float v=0.f;

	ELEMENT_I i;

	for(i=elements.begin();elements.end() != i;++i)	v+=(*i)->getVolume();

	return v;
}

float	CPHShell::getDensity()
{
	return getMass()/getVolume();
}




void CPHShell::PhDataUpdate(dReal step){

	ELEMENT_I i=elements.begin(),e=elements.end();
	bool disable=true;
	for(; e!=i ;++i)
	{
		(*i)->PhDataUpdate(step);
		dBodyID body=(*i)->get_body();
		if(body&&disable&&dBodyIsEnabled(body))disable=false;
	}
	if(disable) DisableObject();
	else		ReanableObject();
}



void CPHShell::PhTune(dReal step){
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(; e!=i ;++i)
		(*i)->PhTune(step);
}

void CPHShell::Update(){
	if(!bActive) return;
	if(bActivating) bActivating=false;
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
		(*i)->Update();

	mXFORM.set((*elements.begin())->mXFORM);
	VERIFY2(_valid(mXFORM),"invalid position in update");
}

void	CPHShell::Freeze()
{
	CPHObject::Freeze();
}
void	CPHShell::UnFreeze()
{
	CPHObject::UnFreeze();
}
void	CPHShell::FreezeContent()
{
	
	CPHObject::FreezeContent();
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(; e!=i ;++i)
		(*i)->Freeze();
	
}
void	CPHShell::UnFreezeContent()
{
	CPHObject::UnFreezeContent();
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(; e!=i ;++i)
		(*i)->UnFreeze();
	
}
void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val){
	if(!bActive) return;
	(*elements.begin())->applyImpulseTrace		( pos,  dir,  val, 0);
	EnableObject();
}

void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const u16 id){
	if(!bActive) return;
	VERIFY(m_pKinematics);
	CBoneInstance& instance=m_pKinematics->LL_GetBoneInstance				(id);
	if(!instance.Callback_Param) return;
	((CPhysicsElement*)instance.Callback_Param)->applyImpulseTrace		( pos,  dir,  val, id);
	EnableObject();
}

CPhysicsElement* CPHShell::get_Element		(const ref_str & bone_name)
{
	VERIFY(m_pKinematics);
	return get_Element(m_pKinematics->LL_BoneID(bone_name));
}
CPhysicsElement* CPHShell::get_Element		(LPCSTR bone_name)
{
		return get_Element((const ref_str&)(bone_name));
}
CPhysicsElement* CPHShell::get_ElementByStoreOrder(u16 num)
{
	R_ASSERT2(num<elements.size(),"argument is out of range");
	return dynamic_cast<CPhysicsElement*>(elements[num]);
}

CPHSynchronize*	CPHShell::get_ElementSync			  (u16 element)
{
	return dynamic_cast<CPHSynchronize*>(elements[element]);
}

CPhysicsElement* CPHShell::get_Element(u16 bone_id)
{
	if(m_pKinematics&& bActive)
	{
		CBoneInstance& instance=m_pKinematics->LL_GetBoneInstance				(bone_id);
		if(instance.Callback==BonesCallback||instance.Callback==StataticRootBonesCallBack)
		{
			return (CPhysicsElement*) instance.Callback_Param;
		}
	}

	ELEMENT_I i=elements.begin(),e=elements.end();
	for(; e!=i ;++i)
		if((*i)->m_SelfID==bone_id)
			return (CPhysicsElement*)(*i);
	return NULL;
}

CPhysicsJoint* CPHShell::get_Joint(u16 bone_id)
{
	JOINT_I i= joints.begin(),e=joints.end();
	for(;e!=i;i++)
		if((*i)->BoneID()==bone_id)
			return (CPhysicsJoint*)(*i);
	return NULL;
}
CPhysicsJoint* CPHShell::get_Joint(const ref_str &bone_name)
{
	VERIFY(m_pKinematics);
	return get_Joint(m_pKinematics->LL_BoneID(bone_name));
}

CPhysicsJoint* CPHShell::get_Joint(LPCSTR bone_name)
{
	return get_Joint((const ref_str&)bone_name);
}

CPhysicsJoint* CPHShell::get_JointByStoreOrder		(u16 num)
{
	return (CPhysicsJoint*) joints[num];
}

u16			CPHShell::get_JointsNumber				()
{
	return u16(joints.size());
}
void __stdcall CPHShell:: BonesCallback				(CBoneInstance* B){
	///CPHElement*	E			= dynamic_cast<CPHElement*>	(static_cast<CPhysicsBase*>(B->Callback_Param));

	CPHElement*	E			= static_cast<CPHElement*>(B->Callback_Param);
	E->BonesCallBack(B);
}


void __stdcall CPHShell::StataticRootBonesCallBack			(CBoneInstance* B){
	///CPHElement*	E			= dynamic_cast<CPHElement*>	(static_cast<CPhysicsBase*>(B->Callback_Param));

	CPHElement*	E			= static_cast<CPHElement*>(B->Callback_Param);
	E->StataticRootBonesCallBack(B);
}


void CPHShell::SetTransform	(const Fmatrix& m0){

	ELEMENT_I i=elements.begin();
	for( ;elements.end() != i; ++i)
	{
		(*i)->SetTransform(m0);
	}
	spatial_move();
}


void CPHShell::Enable()
{
	if(!bActive)
		return;

	ELEMENT_I i,e;
	i=elements.begin(); e=elements.end();
	//if(dBodyIsEnabled((*i)->get_body())) return;
	for( ;i!=e;++i)
		(*i)->Enable();
	EnableObject();
}

void CPHShell::set_PhysicsRefObject	 (CPhysicsShellHolder* ref_object)
{
	

 	if(elements.empty()) return;
	if((*elements.begin())->PhysicsRefObject()==ref_object) return;
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
	{
		(*i)->set_PhysicsRefObject(ref_object);
	}



}



void CPHShell::set_ContactCallback(ContactCallbackFun* callback)
{
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
		(*i)->set_ContactCallback(callback);
}


void CPHShell::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
		(*i)->set_ObjectContactCallback(callback);
}

void CPHShell::SetPhObjectInElements()
{
	if(!bActive) return;
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i )
		(*i)->SetPhObjectInGeomData((CPHObject*)this);
}

void CPHShell::SetMaterial(LPCSTR m)
{
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
	{
		(*i)->SetMaterial(m);
	}
}

void CPHShell::SetMaterial(u16 m)
{
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
	{
		(*i)->SetMaterial(m);
	}
}

void CPHShell::get_LinearVel(Fvector& velocity)
{

	(*elements.begin())->get_LinearVel(velocity);
}

void CPHShell::get_AngularVel(Fvector& velocity)
{

	(*elements.begin())->get_AngularVel(velocity);
}

void	CPHShell::set_LinearVel(const Fvector& velocity)
{
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(;i!=e;i++) (*i)->set_LinearVel(velocity);
}

void	CPHShell::set_AngularVel(const Fvector& velocity)
{
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(;i!=e;i++) (*i)->set_AngularVel(velocity);
}

void CPHShell::set_PushOut(u32 time,ObjectContactCallbackFun* push_out)
{
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
	{
		(*i)->set_PushOut(time,push_out);
	}
}


void CPHShell::SmoothElementsInertia(float k)
{
	dMass m_avrg;
	dReal krc=1.f-k;
	dMassSetZero(&m_avrg);
	ELEMENT_I i;
	for(i=elements.begin();elements.end() != i;++i)
	{

		dMassAdd(&m_avrg,(*i)->getMassTensor());

	}
	int n = (int)elements.size();
	m_avrg.mass/=n*k;
	for(int j=0;j<4*3;++j) m_avrg.I[j]/=n*k;

	for(i=elements.begin();elements.end() != i;++i)
	{
		dVector3 tmp;
		dMass* m=(*i)->getMassTensor();
		dVectorSet(tmp,m->c);

		m->mass*=krc;
		for(int j=0;j<4*3;++j) m->I[j]*=krc;
		dMassAdd(m,&m_avrg);

		dVectorSet(m->c,tmp);
	}
}

void CPHShell::setEquelInertiaForEls(const dMass& M)
{
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(;i!=e;++i)
	{
		(*i)->setInertia(M);
	}
}

void CPHShell::addEquelInertiaToEls(const dMass& M)
{
	ELEMENT_I i=elements.begin(),e=elements.end();
	for(;i!=e;++i)
	{
		(*i)->addInertia(M);
	}
}
static BONE_P_MAP* spGetingMap=NULL;
void CPHShell::build_FromKinematics(CKinematics* K,BONE_P_MAP* p_geting_map)
{
	m_pKinematics			=K;
	spGetingMap				=p_geting_map;
	//CBoneData& bone_data	= m_pKinematics->LL_GetData(0);
	if(!m_spliter_holder) m_spliter_holder=xr_new<CPHShellSplitterHolder>(this);
	AddElementRecursive(0,m_pKinematics->LL_GetBoneRoot(),Fidentity,0);
	R_ASSERT2((*elements.begin())->numberOfGeoms(),"No physics shapes was assigned for model or no shapes in main root bone!!!");
	SetCallbacks(BonesCallback);
	if(m_spliter_holder->isEmpty())xr_delete(m_spliter_holder);
}

void CPHShell::preBuild_FromKinematics(CKinematics* K,BONE_P_MAP* p_geting_map)
{
	m_pKinematics			=K;
	spGetingMap				=p_geting_map;
	//CBoneData& bone_data	= m_pKinematics->LL_GetData(0);
	if(!m_spliter_holder) m_spliter_holder=xr_new<CPHShellSplitterHolder>(this);
	AddElementRecursive(0,m_pKinematics->LL_GetBoneRoot(),Fidentity,0);
	R_ASSERT2((*elements.begin())->numberOfGeoms(),"No physics shapes was assigned for model or no shapes in main root bone!!!");
	if(m_spliter_holder->isEmpty())xr_delete(m_spliter_holder);
}

void CPHShell::AddElementRecursive(CPhysicsElement* root_e, u16 id,Fmatrix global_parent,u16 element_number)
{

	//CBoneInstance& B	= m_pKinematics->LL_GetBoneInstance(u16(id));
	CBoneData& bone_data= m_pKinematics->LL_GetData(u16(id));
	SJointIKData& joint_data=bone_data.IK_data;
	Fmatrix fm_position;
	fm_position.set(bone_data.bind_transform);
	fm_position.mulA(global_parent);
	Flags64 mask;
	mask.assign(m_pKinematics->LL_GetBonesVisible());
	if(!mask.is(1ui64<<(u64)id))
	{
		for (vecBonesIt it=bone_data.children.begin(); bone_data.children.end() != it; ++it)
			AddElementRecursive		(root_e,(*it)->SelfID,fm_position,element_number);
		return;
	}
	
	CPhysicsElement* E  = 0;
	CPhysicsJoint*   J	= 0;
	bool	breakable=joint_data.ik_flags.test(SJointIKData::flBreakable)	&& 
		root_e															&&
		!(
		SBoneShape::stNone==bone_data.shape.type&&
		joint_data.type==jtRigid
		)				
		;
	bool element_added=false;//set true when if elemen created and added by this call
	u16	 splitter_position=0;
	u16 fracture_num=u16(-1);

	if(SBoneShape::stNone!=bone_data.shape.type || !root_e)	//
	{	

		if(joint_data.type==jtRigid && root_e ) //
		{

			Fmatrix vs_root_position;
			vs_root_position.set(root_e->mXFORM);
			vs_root_position.invert();
			vs_root_position.mulB(fm_position);

			E=root_e;
			if(breakable)
			{
				CPHFracture fracture;
				fracture.m_bone_id					=id;
				R_ASSERT2(id<64,"ower 64 bones in breacable are not supported");
				fracture.m_start_geom_num			=E->numberOfGeoms();
				fracture.m_end_geom_num				=u16(-1);
				fracture.m_start_el_num				=u16(elements.size());
				fracture.m_start_jt_num				=u16(joints.size());	 
				fracture.MassSetFirst				(*(E->getMassTensor()));
				fracture.m_pos_in_element			.set(vs_root_position.c);
				VERIFY								(u16(-1)!=fracture.m_start_geom_num);
				fracture.m_break_force				=joint_data.break_force;
				fracture.m_break_torque				=joint_data.break_torque;
				root_e->add_Shape(bone_data.shape,vs_root_position);
				root_e->add_Mass(bone_data.shape,vs_root_position,bone_data.center_of_mass,bone_data.mass,&fracture);

				fracture_num=E->setGeomFracturable(fracture);
			}
			else
			{
				root_e->add_Shape(bone_data.shape,vs_root_position);
				root_e->add_Mass(bone_data.shape,vs_root_position,bone_data.center_of_mass,bone_data.mass);
			}


			//B.Callback_Param=root_e;
			//B.Callback=NULL;

		}
		else										//
		{
			E	= P_create_Element();
			E->m_SelfID=id;
			E->mXFORM.set		(fm_position);
			E->SetMaterial		(bone_data.game_mtl_idx);
			//Fvector mc;
			//fm_position.transform_tiny(mc,bone_data.center_of_mass);
			E->set_ParentElement(root_e);
			///B.set_callback(BonesCallback1,E);
			E->add_Shape(bone_data.shape);
			E->setMassMC(bone_data.mass,bone_data.center_of_mass);
			element_number=u16(elements.size());
			add_Element(E);
			element_added=true;


			if(root_e)
			{
				switch(joint_data.type) 
				{
				case jtCloth: 
					{
						J= P_create_Joint(CPhysicsJoint::ball,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						break;
					}
				case jtJoint:
					{
						bool	eqx=!!fsimilar(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y),
							eqy=!!fsimilar(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y),
							eqz=!!fsimilar(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y);

						if(eqx)
						{
							if(eqy)
							{
								J= P_create_Joint(CPhysicsJoint::hinge,root_e,E);
								J->SetAnchorVsSecondElement	(0,0,0);
								J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
								J->SetAxisDirVsSecondElement (0.f,0.f,1.f,0);
								if(joint_data.limits[2].limit.y-joint_data.limits[2].limit.x<M_PI*2.f)
								{
									J->SetLimits(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y,0);
									J->SetAxisSDfactors(joint_data.limits[2].spring_factor,joint_data.limits[2].damping_factor,0);
								}
								break;
							}
							if(eqz)
							{
								J= P_create_Joint(CPhysicsJoint::hinge,root_e,E);
								J->SetAnchorVsSecondElement	(0,0,0);
								J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
								J->SetAxisDirVsSecondElement(0,1,0,0);
								if(joint_data.limits[1].limit.y-joint_data.limits[1].limit.x<M_PI*2.f)
								{
									J->SetLimits(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y,0);
									J->SetAxisSDfactors(joint_data.limits[1].spring_factor,joint_data.limits[1].damping_factor,0);
								}
								break;
							}
							J= P_create_Joint(CPhysicsJoint::full_control,root_e,E);
							J->SetAnchorVsSecondElement	(0,0,0);
							J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
							J->SetAxisDirVsSecondElement(0.f,0.f,1.f,0);//2-0
							//0-1
							J->SetAxisDirVsSecondElement(0.f,1.f,0.f,2);//1-2

							if(joint_data.limits[2].limit.y-joint_data.limits[2].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y,0);
								J->SetAxisSDfactors(joint_data.limits[2].spring_factor,joint_data.limits[2].damping_factor,0);
							}
							if(joint_data.limits[0].limit.y-joint_data.limits[0].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y,1);
								J->SetAxisSDfactors(joint_data.limits[0].spring_factor,joint_data.limits[0].damping_factor,1);
							}

							if(joint_data.limits[1].limit.y-joint_data.limits[1].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y,2);
								J->SetAxisSDfactors(joint_data.limits[1].spring_factor,joint_data.limits[1].damping_factor,2);
							}

							break;

						}

						if(eqy)
						{
							if(eqz)
							{
								J= P_create_Joint(CPhysicsJoint::hinge,root_e,E);
								J->SetAnchorVsSecondElement	(0,0,0);
								J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
								J->SetAxisDirVsSecondElement(1,0,0,0);
								if(joint_data.limits[0].limit.y-joint_data.limits[0].limit.x<M_PI*2.f)
								{
									J->SetLimits(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y,0);
									J->SetAxisSDfactors(joint_data.limits[0].spring_factor,joint_data.limits[0].damping_factor,0);
								}
								break;
							}

							J= P_create_Joint(CPhysicsJoint::full_control,root_e,E);
							J->SetAnchorVsSecondElement	(0,0,0);
							J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
							J->SetAxisDirVsSecondElement(0.f,0.f,1.f,0);//2-0
							//1-1
							J->SetAxisDirVsSecondElement(1.f,0.f,0.f,2);//0-2
							if(joint_data.limits[2].limit.y-joint_data.limits[2].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y,0);
								J->SetAxisSDfactors(joint_data.limits[2].spring_factor,joint_data.limits[2].damping_factor,0);
							}
							if(joint_data.limits[0].limit.y-joint_data.limits[0].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y,2);
								J->SetAxisSDfactors(joint_data.limits[0].spring_factor,joint_data.limits[0].damping_factor,2);
							}

							if(joint_data.limits[1].limit.y-joint_data.limits[1].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y,1);
								J->SetAxisSDfactors(joint_data.limits[1].spring_factor,joint_data.limits[1].damping_factor,1);
							}
							break;
						}

						if(eqz)
						{
							J= P_create_Joint(CPhysicsJoint::full_control,root_e,E);
							J->SetAnchorVsSecondElement	(0,0,0);
							J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
							J->SetAxisDirVsSecondElement(1.f,0.f,0.f,0);//0-0
							//2-1
							J->SetAxisDirVsSecondElement(0.f,1.f,0.f,2);//1-2
							if(joint_data.limits[2].limit.y-joint_data.limits[2].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y,1);
								J->SetAxisSDfactors(joint_data.limits[2].spring_factor,joint_data.limits[2].damping_factor,1);
							}
							if(joint_data.limits[0].limit.y-joint_data.limits[0].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y,0);
								J->SetAxisSDfactors(joint_data.limits[0].spring_factor,joint_data.limits[0].damping_factor,0);
							}

							if(joint_data.limits[1].limit.y-joint_data.limits[1].limit.x<M_PI*2.f)
							{
								J->SetLimits(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y,2);
								J->SetAxisSDfactors(joint_data.limits[1].spring_factor,joint_data.limits[1].damping_factor,2);
							}
							break;
						}
						J= P_create_Joint(CPhysicsJoint::full_control,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDirVsSecondElement(0.f,0.f,1.f,0);//2-0
						//0-1
						J->SetAxisDirVsSecondElement(0.f,1.f,0.f,2);//1-2
						if(joint_data.limits[2].limit.y-joint_data.limits[2].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y,0);
							J->SetAxisSDfactors(joint_data.limits[2].spring_factor,joint_data.limits[2].damping_factor,0);
						}
						if(joint_data.limits[0].limit.y-joint_data.limits[0].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y,1);
							J->SetAxisSDfactors(joint_data.limits[0].spring_factor,joint_data.limits[0].damping_factor,1);
						}

						if(joint_data.limits[1].limit.y-joint_data.limits[1].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y,2);
							J->SetAxisSDfactors(joint_data.limits[1].spring_factor,joint_data.limits[1].damping_factor,2);
						}

						break;
					}
				case jtWheel:
					{
						J= P_create_Joint(CPhysicsJoint::hinge2,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDirVsSecondElement(1,0,0,0);
						J->SetAxisDirVsSecondElement(0,0,1,1);
						if(joint_data.limits[0].limit.y-joint_data.limits[0].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y,0);	
							J->SetAxisSDfactors(joint_data.limits[0].spring_factor,joint_data.limits[0].damping_factor,0);
						}
						break;
					}

				case jtNone: break;

				default: NODEFAULT;
				}
				if(J)
				{
					
					J->SetForceAndVelocity(0.f);//joint_data.friction
					SetJointRootGeom(root_e,J);
					J->SetBoneID(id);
					add_Joint	(J);
					if(breakable)
					{
						setEndJointSplitter();
						J->SetBreakable(joint_data.break_force,joint_data.break_torque);
					}
				}
			}
			if(m_spliter_holder)
			{
				splitter_position=u16(m_spliter_holder->m_splitters.size());
			}
		}	
	}
	else
	{	
		//B.set_callback(0,root_e);
		E=root_e;
	}

	CODEGeom* added_geom	=	E->last_geom();
	if(added_geom)	added_geom->set_bone_id(id);

	if(m_spliter_holder&&E->has_geoms())
	{
		m_spliter_holder->AddToGeomMap(mk_pair(id,E->last_geom())); 
	}

	if(spGetingMap)
	{
		const BONE_P_PAIR_IT c_iter= spGetingMap->find(id);
		if(spGetingMap->end()!=c_iter)
		{
			c_iter->second.joint=J;
			c_iter->second.element=E;
		}
	}


	/////////////////////////////////////////////////////////////////////////////////////
	for (vecBonesIt it=bone_data.children.begin(); bone_data.children.end() != it; ++it)
		AddElementRecursive		(E,(*it)->SelfID,fm_position,element_number);
	/////////////////////////////////////////////////////////////////////////////////////
	if(breakable)
	{
		if(joint_data.type==jtRigid)
		{
			CPHFracture& fracture=E->Fracture(fracture_num);
			fracture.m_bone_id			=id;
			fracture.m_end_geom_num		=E->numberOfGeoms();
			fracture.m_end_el_num		=u16(elements.size());//just after this el = current+1
			fracture.m_end_jt_num		=u16(joints.size());	 //current+1

		}
		else
		{
			if(J)
			{
				J->JointDestroyInfo()->m_end_element=u16(elements.size());
				J->JointDestroyInfo()->m_end_joint=u16(joints.size());
			}
		}
	}

	if(element_added&&E->isBreakable())setElementSplitter(element_number,splitter_position);

}

void CPHShell::ResetCallbacks(u16 id,Flags64 &mask)
{
	ResetCallbacksRecursive(id,u16(-1),mask);
}

void CPHShell::ResetCallbacksRecursive(u16 id,u16 element,Flags64 &mask)
{

	//if(elements.size()==element)	return;
	CBoneInstance& B	= m_pKinematics->LL_GetBoneInstance(u16(id));
	CBoneData& bone_data= m_pKinematics->LL_GetData(u16(id));
	SJointIKData& joint_data=bone_data.IK_data;

	if(mask.is(1ui64<<(u64)id))
	{

		if(bone_data.shape.type==SBoneShape::stNone||joint_data.type==jtRigid&& element!=u16(-1))
		{

			B.set_callback(0,dynamic_cast<CPhysicsElement*>(elements[element]));
		}
		else
		{

			element++;
			R_ASSERT2(element<elements.size(),"Out of elements!!");
			//if(elements.size()==element)	return;
			CPhysicsElement* E=dynamic_cast<CPhysicsElement*>(elements[element]);
			B.set_callback(BonesCallback,E);
			B.Callback_overwrite=TRUE;
		}
	}
	for (vecBonesIt it=bone_data.children.begin(); it!=bone_data.children.end(); ++it)
		ResetCallbacksRecursive((*it)->SelfID,element,mask);
}

void CPHShell::EnabledCallbacks(BOOL val)
{
	if (val){	
		SetCallbacks(BonesCallback);
		// set callback owervrite in used bones
		ELEMENT_I i,e;
		i=elements.begin(); e=elements.end();
		for( ;i!=e;++i){
			CBoneInstance& B	= m_pKinematics->LL_GetBoneInstance((*i)->m_SelfID);
			B.Callback_overwrite= TRUE;
		}
	}else		ZeroCallbacks();
}

static u16 element_position_in_set_calbacks=u16(-1);
static BoneCallbackFun* bones_callback;//temp ror SetCallbacksRecursive
void CPHShell::SetCallbacks(BoneCallbackFun* callback)
{
	element_position_in_set_calbacks=u16(-1);
	bones_callback=callback;
	SetCallbacksRecursive(m_pKinematics->LL_GetBoneRoot(),element_position_in_set_calbacks);
}

void CPHShell::SetCallbacksRecursive(u16 id,u16 element)
{
	//if(elements.size()==element)	return;
	CBoneInstance& B	= m_pKinematics->LL_GetBoneInstance(u16(id));
	CBoneData& bone_data= m_pKinematics->LL_GetData(u16(id));
	SJointIKData& joint_data=bone_data.IK_data;
	Flags64 mask;
	mask.assign(m_pKinematics->LL_GetBonesVisible());
	if(mask.is(1ui64<<(u64)id))
	{
		if((bone_data.shape.type==SBoneShape::stNone||joint_data.type==jtRigid)	&& element!=u16(-1)){
			B.set_callback(0,dynamic_cast<CPhysicsElement*>(elements[element]));
		}else{
			element_position_in_set_calbacks++;
			element=element_position_in_set_calbacks;
			R_ASSERT2(element<elements.size(),"Out of elements!!");
			//if(elements.size()==element)	return;
			CPhysicsElement* E=dynamic_cast<CPhysicsElement*>(elements[element]);
			B.set_callback(bones_callback,E);
			//B.Callback_overwrite=TRUE;
		}
	}

	for (vecBonesIt it=bone_data.children.begin(); it!=bone_data.children.end(); ++it)
		SetCallbacksRecursive((*it)->SelfID,element);
}

void CPHShell::ZeroCallbacks()
{
	if (m_pKinematics) ZeroCallbacksRecursive(m_pKinematics->LL_GetBoneRoot());
}
void CPHShell::ZeroCallbacksRecursive(u16 id)
{
	CBoneInstance& B	= m_pKinematics->LL_GetBoneInstance(u16(id));
	CBoneData& bone_data= m_pKinematics->LL_GetData(u16(id));
	B.set_callback(0,0);
	B.Callback_overwrite=FALSE;
	for (vecBonesIt it=bone_data.children.begin(); bone_data.children.end() != it; ++it)
		ZeroCallbacksRecursive		((*it)->SelfID);

}
void CPHShell::set_DynamicLimits(float l_limit,float w_limit)
{
	ELEMENT_I i,e;
	i=elements.begin(); e=elements.end();
	for( ;i!=e;++i)
		(*i)->set_DynamicLimits(l_limit,w_limit);
}

void CPHShell::set_DynamicScales(float l_scale/* =default_l_scale */,float w_scale/* =default_w_scale */)
{
	ELEMENT_I i,e;
	i=elements.begin(); e=elements.end();
	for( ;i!=e;++i)
		(*i)->set_DynamicScales(l_scale,w_scale);
}

void CPHShell::set_DisableParams(const SAllDDOParams& params)
{
	ELEMENT_I i,e;
	i=elements.begin(); e=elements.end();
	for( ;i!=e;++i)
		(*i)->set_DisableParams(params);
}

void CPHShell::UpdateRoot()
{

	ELEMENT_I i=elements.begin();
	if( (!(*i)->bActive) || (*i)->bActivating ) return;

	(*i)->InterpolateGlobalTransform(&mXFORM);

}

void CPHShell::InterpolateGlobalTransform(Fmatrix* m)
{
	
	if(!CPHObject::is_active()&&!CPHObject::NetInterpolation()) return;
	(*elements.begin())->InterpolateGlobalTransform(m);
	m->mulB_43	(m_object_in_root);
	//mXFORM.set(*m);
	//m->c.add(m_object_in_root);
}

void CPHShell::GetGlobalTransformDynamic(Fmatrix* m)
{
	(*elements.begin())->GetGlobalTransformDynamic(m);
	m->mulB(m_object_in_root);
}
void CPHShell::InterpolateGlobalPosition(Fvector* v)
{
	(*elements.begin())->InterpolateGlobalPosition(v);
	v->add(m_object_in_root.c);
}

void CPHShell::GetGlobalPositionDynamic(Fvector* v)
{
	(*elements.begin())->GetGlobalPositionDynamic(v);
}


void CPHShell::ObjectToRootForm(const Fmatrix& form)
{
	Fmatrix M;
	M.mul(m_object_in_root,(*elements.begin())->InverceLocalForm());
	M.invert();
	mXFORM.mul(form,M);
	
}
CPhysicsElement* CPHShell::NearestToPoint(const Fvector& point)
{
	ELEMENT_I i,e;
	i=elements.begin(); e=elements.end();
	float min_distance	=dInfinity;
	CPHElement* nearest_element=NULL;
	for( ;i!=e;++i)
	{
		Fvector tmp;
		float	distance;
		(*i)->GetGlobalPositionDynamic(&tmp);
		tmp.sub(point);
		distance=tmp.magnitude();
		if(distance<min_distance)
		{
			min_distance=distance;
			nearest_element=*i;
		}
	}
	return nearest_element;
}

void CPHShell::PassEndElements(u16 from,u16 to,CPHShell *dest)
{

	ELEMENT_I i_from=elements.begin()+from,e=elements.begin()+to;
	if(from!=to)
	{
		if(!dest->elements.empty())	(*i_from)->set_ParentElement(dest->elements.back());
		else						(*i_from)->set_ParentElement(NULL);
	}
	for(ELEMENT_I i=i_from;i!=e;++i)
	{
		dGeomID spaced_geom=(*i)->dSpacedGeometry();
		if(spaced_geom)//for active elems
		{
			dSpaceRemove (m_space,spaced_geom );
			dSpaceAdd(dest->m_space,spaced_geom);
		}
		VERIFY(_valid(dest->mXFORM));
		(*i)->SetShell(dest);
	}
	dest->elements.insert(dest->elements.end(),i_from,e);
	elements.erase(i_from,e);		

}

void CPHShell::PassEndJoints(u16 from,u16 to,CPHShell *dest)
{
	JOINT_I i_from=joints.begin()+from,e=joints.begin()+to;
	JOINT_I i=i_from;
	for(;i!=e;i++)
	{
		(*i)->SetShell(dest);
	}
	dest->joints.insert(dest->joints.end(),i_from,e);
	joints.erase(i_from,e);
}

void CPHShell::DeleteElement(u16 element)
{
	elements[element]->Deactivate();
	xr_delete(elements[element]);
	elements.erase(elements.begin()+element);
}
void CPHShell::DeleteJoint(u16 joint)
{
	joints[joint]->Deactivate();
	xr_delete(joints[joint]);
	joints.erase(joints.begin()+joint);
}

void CPHShell::setEndElementSplitter()
{

	if(!elements.back()->FracturesHolder())//adding fracture for element supposed before adding splitter. Need only one splitter for an element
		AddSplitter(CPHShellSplitter::splElement,u16(elements.size()-1),u16(joints.size()-1));
}

void CPHShell::setElementSplitter(u16 element_number,u16 splitter_position)
{
	AddSplitter(CPHShellSplitter::splElement,element_number,element_number-1,splitter_position);
}
void CPHShell::AddSplitter(CPHShellSplitter::EType type,u16 element,u16 joint)
{
	if(!m_spliter_holder) m_spliter_holder=xr_new<CPHShellSplitterHolder>(this);
	m_spliter_holder->AddSplitter(type,element,joint);
}

void CPHShell::AddSplitter(CPHShellSplitter::EType type,u16 element,u16 joint,u16 position)
{
	if(!m_spliter_holder) m_spliter_holder=xr_new<CPHShellSplitterHolder>(this);
	m_spliter_holder->AddSplitter(type,element,joint,position);
}
void CPHShell::setEndJointSplitter()
{
	if(!joints.back()->JointDestroyInfo())//setting joint breacable supposed before adding splitter. Need only one splitter for a joint
		AddSplitter(CPHShellSplitter::splJoint,u16(elements.size()-1),u16(joints.size()-1));
}

bool CPHShell::isBreakable()
{
	return !!m_spliter_holder;
}

bool CPHShell::isFractured()
{
	return(m_spliter_holder&&m_spliter_holder->Breaked());
}

void CPHShell::SplitProcess(PHSHELL_PAIR_VECTOR &out_shels)
{
	if(! m_spliter_holder) return;
	m_spliter_holder->SplitProcess(out_shels);
	if(!m_spliter_holder->m_splitters.size()) xr_delete(m_spliter_holder);
}

u16 CPHShell::BoneIdToRootGeom(u16 id)
{
	if(! m_spliter_holder)return u16(-1);
	return m_spliter_holder->FindRootGeom(id);
}

void CPHShell::SetJointRootGeom(CPhysicsElement* root_e,CPhysicsJoint* J)
{
	CPHElement* e=dynamic_cast<CPHElement*>(root_e);
	CPHJoint*	j=dynamic_cast<CPHJoint*>(J);
	R_ASSERT(e);
	R_ASSERT(j);
	CPHFracturesHolder* f_holder=e->FracturesHolder();
	if(!f_holder) return;
	j->RootGeom()=e->Geom(f_holder->LastFracture().m_start_geom_num);
}

void CPHShell::set_ApplyByGravity(bool flag)
{
	ELEMENT_I i,e;
	i=elements.begin(); e=elements.end();
	for( ;i!=e;++i)
		(*i)->set_ApplyByGravity(flag);
}

void CPHShell::applyGravityAccel(const Fvector& accel)
{
	ELEMENT_I i,e;
	Fvector a;
	a.set(accel);
	a.mul((float)elements.size());
	i=elements.begin(); e=elements.end();
	for( ;i!=e;++i)
		(*i)->applyGravityAccel(a);
	EnableObject();
}

void CPHShell::PlaceBindToElForms()
{
	Flags64 mask;
	mask.assign(m_pKinematics->LL_GetBonesVisible());
	PlaceBindToElFormsRecursive(Fidentity,m_pKinematics->LL_GetBoneRoot(),0,mask);
}

void CPHShell::PlaceBindToElFormsRecursive(Fmatrix parent,u16 id,u16 element,Flags64 &mask)
{
	
	
	CBoneData& bone_data= m_pKinematics->LL_GetData(u16(id));
	SJointIKData& joint_data=bone_data.IK_data;

	if(mask.is(1ui64<<(u64)id))
	{

		if(bone_data.shape.type==SBoneShape::stNone||joint_data.type==jtRigid&& element!=u16(-1))
		{

			
		}
		else
		{

			element++;
			R_ASSERT2(element<elements.size(),"Out of elements!!");
			//if(elements.size()==element)	return;
			CPHElement* E=(elements[element]);
			E->mXFORM.mul(parent,bone_data.bind_transform);
			
		}
	}
	for (vecBonesIt it=bone_data.children.begin(); it!=bone_data.children.end(); ++it)
		PlaceBindToElFormsRecursive(mXFORM,(*it)->SelfID,element,mask);

}

void CPHShell::BonesBindCalculate(u16 id_from)
{
	BonesBindCalculateRecursive(Fidentity,0);
}
void CPHShell::BonesBindCalculateRecursive(Fmatrix parent,u16 id)
{

	CBoneInstance& bone_instance=m_pKinematics->LL_GetBoneInstance(id);
	CBoneData& bone_data= m_pKinematics->LL_GetData(u16(id));

	bone_instance.mTransform.mul(parent,bone_data.bind_transform);
	
	for (vecBonesIt it=bone_data.children.begin(); it!=bone_data.children.end(); ++it)
		BonesBindCalculateRecursive(bone_instance.mTransform,(*it)->SelfID);
}

void CPHShell::AddTracedGeom				(u16 element/*=0*/,u16 geom/*=0*/)
{
	m_traced_geoms.add(elements[element]->Geom(geom));
	CPHObject::SetRayMotions();
}

void CPHShell::SetPrefereExactIntegration()
{
	CPHObject::SetPrefereExactIntegration();
}
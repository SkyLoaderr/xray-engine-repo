/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"

///////////////////////////////////////////////////////////////
///#pragma warning(disable:4995)
//#include "..\ode\src\collision_kernel.h"
//#include <..\ode\src\joint.h>
//#include <..\ode\src\objects.h>

//#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHElement.h"
#include "PHShell.h"

void CPHShell::setDensity(float M){
	xr_vector<CPHElement*>::iterator i;
	//float volume=0.f;
	//for(i=elements.begin();i!=elements.end();i++)	volume+=(*i)->get_volume();

	for(i=elements.begin();i!=elements.end();i++)
		(*i)->setDensity(M);
}


void CPHShell::setMass(float M){
	xr_vector<CPHElement*>::iterator i;
	float volume=0.f;
	for(i=elements.begin();i!=elements.end();i++)	volume+=(*i)->get_volume();

	for(i=elements.begin();i!=elements.end();i++)
		(*i)->setMass(
		(*i)->get_volume()/volume*M
		);
}

void CPHShell::setMass1(float M){
	xr_vector<CPHElement*>::iterator i;


	for(i=elements.begin();i!=elements.end();i++)
		(*i)->setMass(
		M/elements.size()
		);
}
float CPHShell::getMass()
{
	float m=0.f;

	xr_vector<CPHElement*>::iterator i;

	for(i=elements.begin();i!=elements.end();i++)	m+=(*i)->getMass();

	return m;
}
void CPHShell::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){
	if(bActive)
		return;
	CPHObject::Activate();

	xr_vector<CPHElement*>::iterator i;

	mXFORM.set(m0);
	m_space=dSimpleSpaceCreate(ph_world->GetSpace());
	//dSpaceSetCleanup (m_space, 0);
	for(i=elements.begin();i!=elements.end();i++){
		//(*i)->Start();
		//(*i)->SetTransform(m0);
		(*i)->Activate(m0,dt01, m2, disable);
	}
	//SetPhObjectInElements();
	bActive=true;
}



void CPHShell::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){
	if(bActive)
		return;
	CPHObject::Activate();

	xr_vector<CPHElement*>::iterator i;

	mXFORM.set(transform);
	m_space=dSimpleSpaceCreate(ph_world->GetSpace());
	//dSpaceSetCleanup (m_space, 0);
	for(i=elements.begin();i!=elements.end();i++){
		//(*i)->Start();
		//(*i)->SetTransform(m0);
		(*i)->Activate(transform,lin_vel, ang_vel);
	}
	//SetPhObjectInElements();
	bActive=true;
}






void CPHShell::Deactivate(){
	if(!bActive)
		return;
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->Deactivate();

	xr_vector<CPHJoint*>::iterator j;
	for(j=joints.begin();j!=joints.end();j++)
		(*j)->Deactivate();

	CPHObject::Deactivate();
	if(m_space) {
		dSpaceDestroy(m_space);
		m_space=NULL;
	}
	bActive=false;
	bActivating=false;
}

void CPHShell::PhDataUpdate(dReal step){

	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->PhDataUpdate(step);
}



void CPHShell::PhTune(dReal step){
}

void CPHShell::Update(){
	if(!bActive) return;
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->Update();
	mXFORM.set((*elements.begin())->mXFORM);
	R_ASSERT2(_valid(mXFORM),"invalid position in update");
}


void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val){
	if(!bActive) return;
	(*elements.begin())->applyImpulseTrace		( pos,  dir,  val);
}

void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const s16 element){
	if(!bActive) return;
	VERIFY(m_pKinematics);
	CBoneInstance& instance=m_pKinematics->LL_GetInstance				(element);
	if(!instance.Callback_Param) return;
	((CPHElement*)instance.Callback_Param)->applyImpulseTrace		( pos,  dir,  val);

}

CPhysicsElement* CPHShell::get_Element(LPCSTR bone_name)
{
	VERIFY(m_pKinematics);

	
	CBoneInstance& instance=m_pKinematics->LL_GetInstance				(m_pKinematics->LL_BoneID(bone_name));

	return (CPhysicsElement*) (instance.Callback_Param);
}

CPhysicsElement* CPHShell::get_ElementByStoreOrder(u16 num)
{
	R_ASSERT2(num<elements.size(),"argument is out of range");
	return dynamic_cast<CPhysicsElement*>(elements[num]);
}

CPhysicsElement* CPHShell::get_Element(s16 bone_id)
{
	VERIFY(m_pKinematics);
	CBoneInstance& instance=m_pKinematics->LL_GetInstance				(bone_id);
	return (CPhysicsElement*) instance.Callback_Param;
}

void __stdcall CPHShell:: BonesCallback				(CBoneInstance* B){
	///CPHElement*	E			= dynamic_cast<CPHElement*>	(static_cast<CPhysicsBase*>(B->Callback_Param));
	CPHElement*	E			= static_cast<CPHElement*>(B->Callback_Param);

	E->CallBack(B);
}

void __stdcall CPHShell:: BonesCallback1				(CBoneInstance* B){
	//CPHElement*	E			= dynamic_cast<CPHElement*>	(static_cast<CPhysicsBase*>(B->Callback_Param));
	CPHElement*	E			= static_cast<CPHElement*>(B->Callback_Param);
	E->CallBack1(B);
}


void CPHShell::Activate(bool place_current_forms,bool disable)
{ 
	if(bActive)
		return;
		CPHObject::Activate();
		if(!m_space) m_space=dSimpleSpaceCreate(ph_world->GetSpace());

		{		
		xr_vector<CPHElement*>::iterator i;
		if(place_current_forms)
		for(i=elements.begin();i!=elements.end();i++)	{
															(*i)->Activate(mXFORM,disable);
														}
		}
		
		{
		xr_vector<CPHJoint*>::iterator i;
		for(i=joints.begin();i!=joints.end();i++) (*i)->Activate();
		}	
	bActive=true;
	bActivating=true;
}

void CPHShell::SetTransform(Fmatrix m){
	Fmatrix init;
	xr_vector<CPHElement*>::iterator i=elements.begin();
	(*i)->InterpolateGlobalTransform(&init);
	init.invert();
	Fmatrix add;
	add.mul(init,m);
	(*i)->SetTransform(m);
	i++;
	Fmatrix element_transform;
	for( ;i!=elements.end(); i++)
	{
		(*i)->InterpolateGlobalTransform(&element_transform);
		element_transform.mulA(add);
		(*i)->SetTransform(element_transform);
	}


}


void CPHShell::Enable()
{
	if(!bActive)
		return;

	xr_vector<CPHElement*>::iterator i,e;
	i=elements.begin(); e=elements.end();
	if(dBodyIsEnabled((*i)->get_body())) return;
	for( ;i!=e;i++)
		(*i)->Enable();
}

void CPHShell::set_PhysicsRefObject	 (CPhysicsRefObject* ref_object)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->set_PhysicsRefObject(ref_object);
	}
//if(dynamic_cast<CCar*>(ref_object))
//{
//ref_object=ref_object;
//}
	
	
	
}



void CPHShell::set_ContactCallback(ContactCallbackFun* callback)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->set_ContactCallback(callback);
}


void CPHShell::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->set_ObjectContactCallback(callback);
}

void CPHShell::SetPhObjectInElements()
{
	if(!bActive) return;
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++ )
		(*i)->SetPhObjectInGeomData((CPHObject*)this);
}

void CPHShell::SetMaterial(LPCSTR m)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->SetMaterial(m);
	}
}

void CPHShell::SetMaterial(u32 m)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
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

void CPHShell::set_PushOut(u32 time,PushOutCallbackFun* push_out)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->set_PushOut(time,push_out);
	}
}


void CPHShell::SmoothElementsInertia(float k)
{
	dMass m_avrg;
	dReal krc=1.f-k;
	dMassSetZero(&m_avrg);
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{

		dMassAdd(&m_avrg,(*i)->GetMass());

	}
	int n = (int)elements.size();
	m_avrg.mass/=n*k;
	for(int j=0;j<4*3;j++) m_avrg.I[j]/=n*k;

	for(i=elements.begin();i!=elements.end();i++)
	{
		dVector3 tmp;
		dMass* m=(*i)->GetMass();
		Memory.mem_copy(tmp,m->c,sizeof(dVector3));

		m->mass*=krc;
		for(int j=0;j<4*3;j++) m->I[j]*=krc;
		dMassAdd(m,&m_avrg);

		Memory.mem_copy(m->c,tmp,sizeof(dVector3));
	}
}

static BONE_P_MAP* spGetingMap=NULL;
void CPHShell::build_FromKinematics(CKinematics* K,BONE_P_MAP* p_geting_map)
{
	m_pKinematics=K;
	spGetingMap=p_geting_map;
	CBoneData& bone_data= m_pKinematics->LL_GetData(0);
	AddElementRecursive(0,m_pKinematics->LL_BoneRoot(),bone_data);
	
	//Activate(true);

}
void CPHShell::AddElementRecursive(CPhysicsElement* root_e, int id,const CBoneData& parent_data)
{

	CBoneInstance& B	= m_pKinematics->LL_GetInstance(u16(id));
	CBoneData& bone_data= m_pKinematics->LL_GetData(u16(id));


	CPhysicsElement* E  = 0;
	CPhysicsJoint*   J	= 0;
	if(bone_data.shape.type!=SBoneShape::stNone || !root_e)	//
	{														
		Fmatrix fm_position;
		fm_position.setXYZi(bone_data.bind_xyz);
		fm_position.c.set(bone_data.bind_translate);
		if(bone_data.IK_data.type==jtRigid && root_e ) //
		{
			Fmatrix inv_root;
			inv_root.set(root_e->mXFORM);
			inv_root.invert();
			fm_position.mulA(inv_root);
			root_e->add_Shape(bone_data.shape,fm_position);
			root_e->add_Mass(bone_data.shape,fm_position,bone_data.center_of_mass,bone_data.mass);
			B.Callback_Param=root_e;
			B.Callback=NULL;
			E=root_e;
		}
		else										//
		{
			E	= P_create_Element();
			E->m_SelfID=id;
			E->mXFORM.set		(fm_position);
			E->SetMaterial(bone_data.game_mtl);
			//Fvector mc;
			//fm_position.transform_tiny(mc,bone_data.center_of_mass);
			E->set_ParentElement(root_e);
			B.set_callback(BonesCallback1,E);
			E->add_Shape(bone_data.shape);
			E->setMassMC(bone_data.mass,bone_data.center_of_mass);
			add_Element(E);
			
			SJointIKData& joint_data=bone_data.IK_data;
			
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
				case jtWheelXZ:
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
				case jtWheelXY:
					{
						J= P_create_Joint(CPhysicsJoint::hinge2,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDirVsSecondElement(1,0,0,0);
						J->SetAxisDirVsSecondElement(0,1,0,1);
						if(joint_data.limits[0].limit.y-joint_data.limits[0].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y,0);	
							J->SetAxisSDfactors(joint_data.limits[0].spring_factor,joint_data.limits[0].damping_factor,0);
						}
						break;
					}
				case jtWheelYX:
					{
						J= P_create_Joint(CPhysicsJoint::hinge2,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDirVsSecondElement(0,1,0,0);
						J->SetAxisDirVsSecondElement(1,0,0,1);
						if(joint_data.limits[1].limit.y-joint_data.limits[1].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y,0);	
							J->SetAxisSDfactors(joint_data.limits[1].spring_factor,joint_data.limits[1].damping_factor,0);
						}
						break;
					}
				case jtWheelYZ:
					{
						J= P_create_Joint(CPhysicsJoint::hinge2,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDirVsSecondElement(0,1,0,0);
						J->SetAxisDirVsSecondElement(0,0,1,1);
						if(joint_data.limits[1].limit.y-joint_data.limits[1].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y,0);	
							J->SetAxisSDfactors(joint_data.limits[1].spring_factor,joint_data.limits[1].damping_factor,0);
						}
						break;
					}
				case jtWheelZX:
					{
						J= P_create_Joint(CPhysicsJoint::hinge2,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDirVsSecondElement(0,0,1,0);
						J->SetAxisDirVsSecondElement(1,0,0,1);
						if(joint_data.limits[2].limit.y-joint_data.limits[2].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y,0);	
							J->SetAxisSDfactors(joint_data.limits[2].spring_factor,joint_data.limits[2].damping_factor,0);
						}
						break;
					}
				case jtWheelZY:
					{
						J= P_create_Joint(CPhysicsJoint::hinge2,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDirVsSecondElement(0,0,1,0);
						J->SetAxisDirVsSecondElement(0,1,0,1);
						if(joint_data.limits[2].limit.y-joint_data.limits[2].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y,0);	
							J->SetAxisSDfactors(joint_data.limits[2].spring_factor,joint_data.limits[2].damping_factor,0);
						}
						break;
					}

				default: NODEFAULT;
				}

			}
			add_Joint	(J);
		}

	}
	else
	{	
		B.set_callback(0,root_e);
		E=root_e;
	}

	if(spGetingMap)
	{
		const BONE_P_PAIR_IT c_iter= spGetingMap->find(id);
		if(c_iter!=spGetingMap->end())
		{
			c_iter->second.joint=J;
			c_iter->second.element=E;
		}
	}

	for (vecBonesIt it=bone_data.children.begin(); it!=bone_data.children.end(); it++)
		AddElementRecursive		(E,(*it)->SelfID,bone_data);

}

  
void CPHShell::ZeroCallbacks()
{
ZeroCallbacksRecursive(m_pKinematics->LL_BoneRoot());
}
void CPHShell::ZeroCallbacksRecursive(int id)
{

	CBoneInstance& B	= m_pKinematics->LL_GetInstance(u16(id));
	CBoneData& bone_data= m_pKinematics->LL_GetData(u16(id));
		B.set_callback(0,0);
	for (vecBonesIt it=bone_data.children.begin(); it!=bone_data.children.end(); it++)
		ZeroCallbacksRecursive		((*it)->SelfID);

}
void CPHShell::set_DynamicLimits(float l_limit,float w_limit)
{
	xr_vector<CPHElement*>::iterator i,e;
	i=elements.begin(); e=elements.end();
	for( ;i!=e;i++)
		(*i)->set_DynamicLimits(l_limit,w_limit);
}

void CPHShell::set_DynamicScales(float l_scale/* =default_l_scale */,float w_scale/* =default_w_scale */)
{
	xr_vector<CPHElement*>::iterator i,e;
	i=elements.begin(); e=elements.end();
	for( ;i!=e;i++)
		(*i)->set_DynamicScales(l_scale,w_scale);
}

void CPHShell::set_DisableParams(float dis_l/* =default_disl */,float dis_w/* =default_disw */)
{
	xr_vector<CPHElement*>::iterator i,e;
	i=elements.begin(); e=elements.end();
	for( ;i!=e;i++)
		(*i)->set_DisableParams(dis_l,dis_w);
}

void CPHShell::UpdateRoot()
{

xr_vector<CPHElement*>::iterator i=elements.begin();
if( (!(*i)->bActive) || (*i)->bActivating ) return;

(*i)->InterpolateGlobalTransform(&mXFORM);

}

void CPHShell::InterpolateGlobalTransform(Fmatrix* m)
{
(*elements.begin())->InterpolateGlobalTransform(m);
m->mulB(m_object_in_root);
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

CPhysicsElement* CPHShell::NearestToPoint(const Fvector& point)
{
	xr_vector<CPHElement*>::iterator i,e;
	i=elements.begin(); e=elements.end();
	float min_distance	=dInfinity;
	CPHElement* nearest_element=NULL;
	for( ;i!=e;i++)
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
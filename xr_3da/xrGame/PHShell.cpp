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
	m_ident=ph_world->AddObject(this);

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
	m_ident=ph_world->AddObject(this);

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

	ph_world->RemoveObject(m_ident);
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

CPhysicsElement* CPHShell::get_Element(s16 bone_id)
{
	VERIFY(m_pKinematics);
	CBoneInstance& instance=m_pKinematics->LL_GetInstance				(bone_id);
	return (CPhysicsElement*) instance.Callback_Param;
}

void __stdcall CPHShell:: BonesCallback				(CBoneInstance* B){
	CPHElement*	E			= dynamic_cast<CPHElement*>	(static_cast<CObject*>(B->Callback_Param));

	E->CallBack(B);
}



void CPHShell::Activate(bool place_current_forms,bool disable){
	if(bActive)
		return;

		m_ident=ph_world->AddObject(this);
		{
		xr_vector<CPHElement*>::iterator i;
		if(place_current_forms)
		for(i=elements.begin();i!=elements.end();i++)	{
														//(*i)->Start();
														//(*i)->SetTransform(m0);
														(*i)->Activate(mXFORM,disable);
														}
		}

		{
		
		xr_vector<CPHJoint*>::iterator i;
		for(i=joints.begin();i!=joints.end();i++) (*i)->Activate();
		}
	//SetPhObjectInElements();/////////////////////////////////////////////////////////////////////
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
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->Enable();
}

void CPHShell::set_PhysicsRefObject	 (CPhysicsRefObject* ref_object)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->set_PhysicsRefObject(ref_object);
	}
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

void CPHShell::build_FromKinematics(CKinematics* K)
{
	m_pKinematics=K;
	AddElementRecursive(0,m_pKinematics->LL_BoneRoot());
	

}
void CPHShell::AddElementRecursive(CPhysicsElement* root_e, int id)
{

	CBoneInstance& B	= m_pKinematics->LL_GetInstance(id);
	CBoneData& bone_data= m_pKinematics->LL_GetData(id);
	CPhysicsElement* E  = 0;
	CPhysicsJoint* J	= 0;
	if(bone_data.shape.type!=SBoneShape::stNone)	//для BD.shape==stNone нет ни елемента ни колижена
	{
		Fmatrix fm_position;
		fm_position.setHPB(bone_data.bind_hpb.x,bone_data.bind_hpb.y,bone_data.bind_hpb.z);
		fm_position.c.set(bone_data.bind_translate);

		if(bone_data.IK_data.type==jtRigid && root_e!=0) //нет элемента-колижен добавляется к root
		{
			Fmatrix inv_root;
			inv_root.set(root_e->mXFORM);
			inv_root.invert();
			fm_position.mulA(inv_root);
			root_e->add_Shape(bone_data.shape,fm_position);
			E=root_e;
		}
		else										//создать елемент и джоинт к root
		{
			E	= P_create_Element();
			E->mXFORM.set		(fm_position);
			E->SetMaterial(bone_data.game_mtl);
			//Fvector mc;
			//fm_position.transform_tiny(mc,bone_data.center_of_mass);

			E->set_ParentElement(root_e);
			B.set_callback(GetBonesCallback(),E);
			E->add_Shape(bone_data.shape);
			E->setMassMC(bone_data.mass,bone_data.center_of_mass);
			add_Element(E);
			SJointIKData& joint_data=bone_data.IK_data;
			if(root_e)
			switch(joint_data.type) {
		case jtCloth: 
			{
				J= P_create_Joint(CPhysicsJoint::ball,root_e,E);
				J->SetAnchorVsSecondElement	(0,0,0);
				J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
				break;
			}
		case jtJoint:
			{
				bool	eqx=joint_data.limits[0].limit.x==joint_data.limits[0].limit.y,
						eqy=joint_data.limits[1].limit.x==joint_data.limits[1].limit.y,
						eqz=joint_data.limits[2].limit.x==joint_data.limits[2].limit.y;

				if(eqx)
				{
					if(eqy)
					{
						J= P_create_Joint(CPhysicsJoint::hinge,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDir (fm_position.k,0);
						if(joint_data.limits[2].limit.y-joint_data.limits[2].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[2].limit.x,joint_data.limits[2].limit.y,2);
							J->SetAxisSDfactors(joint_data.limits[2].spring_factor,joint_data.limits[2].damping_factor,2);
						}
						break;
					}
					if(eqz)
					{
						J= P_create_Joint(CPhysicsJoint::hinge,root_e,E);
						J->SetAnchorVsSecondElement	(0,0,0);
						J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
						J->SetAxisDir(fm_position.j,0);
						if(joint_data.limits[1].limit.y-joint_data.limits[1].limit.x<M_PI*2.f)
						{
							J->SetLimits(joint_data.limits[1].limit.x,joint_data.limits[1].limit.y,0);
							J->SetAxisSDfactors(joint_data.limits[1].spring_factor,joint_data.limits[1].damping_factor,1);
						}
						break;
					}
				}

				if(eqy&&eqz)
				{
					J= P_create_Joint(CPhysicsJoint::hinge,root_e,E);
					J->SetAnchorVsSecondElement	(0,0,0);
					J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
					J->SetAxisDir(fm_position.i,0);
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
				J->SetAxisDir(fm_position.i,0);
				J->SetAxisDir(fm_position.j,1);
				J->SetAxisDir(fm_position.k,2);
				for(int i=0;i<3;i++)
					if(joint_data.limits[i].limit.y-joint_data.limits[i].limit.x<M_PI*2.f)
					{
						J->SetLimits(joint_data.limits[i].limit.x,joint_data.limits[i].limit.y,i);
						J->SetAxisSDfactors(joint_data.limits[i].spring_factor,joint_data.limits[i].damping_factor,i);
					}
				break;
			}
		case jtWheel:
			{
				J= P_create_Joint(CPhysicsJoint::hinge2,root_e,E);//доделать
				J->SetAnchorVsSecondElement	(0,0,0);
				J->SetJointSDfactors(joint_data.spring_factor,joint_data.damping_factor);
				J->SetAxisDir(fm_position.i,0);
				J->SetAxisDir(fm_position.k,1);
				if(joint_data.limits[0].limit.y-joint_data.limits[0].limit.x<M_PI*2.f)
				{
					J->SetLimits(joint_data.limits[0].limit.x,joint_data.limits[0].limit.y,0);	
					J->SetAxisSDfactors(joint_data.limits[0].spring_factor,joint_data.limits[0].damping_factor,0);
				}
				break;
			}
		default: NODEFAULT;
			}
			add_Joint	(J);
		}

	}

	for (vecBonesIt it=bone_data.children.begin(); it!=bone_data.children.end(); it++)
		AddElementRecursive		(E,(*it)->SelfID);
	/*



	E->mXFORM.set		(K->LL_GetTransform(id));
	Fobb bb			=	K->LL_GetBox(id);

	E->add_Box			(bb);
	E->setMass			(10.f);

	B.set_callback		(m_pPhysicsShell->GetBonesCallback(),E);
	m_pPhysicsShell->add_Element	(E);
	if( !(m_type==epotFreeChain && root_e==0) )
	{		
	CPhysicsJoint* J= P_create_Joint(CPhysicsJoint::full_control,root_e,E);
	J->SetAnchorVsSecondElement	(0,0,0);
	J->SetAxisDirVsSecondElement	(1,0,0,0);
	J->SetAxisDirVsSecondElement	(0,1,0,2);
	J->SetLimits				(-M_PI/2,M_PI/2,0);
	J->SetLimits				(-M_PI/2,M_PI/2,1);
	J->SetLimits				(-M_PI/2,M_PI/2,2);
	m_pPhysicsShell->add_Joint	(J);	
	}


	for (vecBonesIt it=BD.children.begin(); it!=BD.children.end(); it++){
	AddElementRecursive		(E,(*it)->SelfID);

	}
	*/
}
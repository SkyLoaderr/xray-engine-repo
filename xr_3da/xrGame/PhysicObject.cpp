#include "stdafx.h"
#include "physicobject.h"

CPhysicObject::CPhysicObject(void) {
	m_type = epotBox;
	m_mass = 10.f;
}

CPhysicObject::~CPhysicObject(void) {
}

BOOL CPhysicObject::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeObjectPhysic		*po	= dynamic_cast<CSE_ALifeObjectPhysic*>(e);
	R_ASSERT				(po);
	cNameVisual_set			(po->get_visual());
	inherited::net_Spawn	(DC);

	m_type = EPOType(po->type);
	m_mass = po->mass;

	xr_delete(cfModel);
	switch(m_type) {
		case epotBox:			cfModel = xr_new<CCF_Rigid>(this);		break;
		case epotFixedChain:	
		case epotSkeleton:		cfModel = xr_new<CCF_Skeleton>(this);	break;
		default: NODEFAULT; 
	}
	pCreator->ObjectSpace.Object_Register(this);
	cfModel->OnMove();

	switch(m_type) {
		case epotBox:				break;
		case epotFixedChain:
			R_ASSERT				(pVisual&&PKinematics(pVisual));
			PKinematics(pVisual)->PlayCycle("idle");
			PKinematics(pVisual)->Calculate();
			break;
		case epotSkeleton:
			R_ASSERT				(pVisual&&PKinematics(pVisual));
			PKinematics(pVisual)->PlayCycle("death_init");
			PKinematics(pVisual)->Calculate();
			break;
		default: NODEFAULT; 
	}
	CreateBody				(po->fixed_bone);

	setVisible(true);
	setEnabled(true);

	return TRUE;
}

void CPhysicObject::UpdateCL	()
{
	inherited::UpdateCL		();
	if(m_pPhysicsShell){
		if(m_type==epotBox) m_pPhysicsShell->Update();
		
		
		mRotate.i.set(m_pPhysicsShell->mXFORM.i);
		mRotate.j.set(m_pPhysicsShell->mXFORM.j);
		mRotate.k.set(m_pPhysicsShell->mXFORM.k);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		UpdateTransform();
	}
}

void CPhysicObject::AddElement(CPhysicsElement* root_e, int id)
{
	CKinematics* K		= PKinematics(pVisual);

	CPhysicsElement* E	= P_create_Element();
	CBoneInstance& B	= K->LL_GetInstance(id);
	E->mXFORM.set		(K->LL_GetTransform(id));
	Fobb bb			= K->LL_GetBox(id);


	if(bb.m_halfsize.magnitude()<0.05f)
	{
		bb.m_halfsize.add(0.05f);

	}
	E->add_Box			(bb);
	E->setMass			(10.f);
	E->set_ParentElement(root_e);
	B.set_callback		(m_pPhysicsShell->GetBonesCallback(),E);
	m_pPhysicsShell->add_Element	(E);
	if( !(m_type==epotFreeChain && root_e==0) )
	{		
		CPhysicsJoint* J= P_create_Joint(CPhysicsJoint::full_control,root_e,E);
		J->SetAnchorVsSecondElement	(0,0,0);
		J->SetAxisVsSecondElement	(1,0,0,0);
		J->SetAxisVsSecondElement	(0,1,0,2);
		J->SetLimits				(-M_PI/2,M_PI/2,0);
		J->SetLimits				(-M_PI/2,M_PI/2,1);
		J->SetLimits				(-M_PI/2,M_PI/2,2);
		m_pPhysicsShell->add_Joint	(J);	
	}

	CBoneData& BD		= K->LL_GetData(id);
	for (vecBonesIt it=BD.children.begin(); it!=BD.children.end(); it++){
		AddElement		(E,(*it)->SelfID);
	}
}

void CPhysicObject::CreateBody(LPCSTR fixed_bone) {
	m_pPhysicsShell		= P_create_Shell();
	switch(m_type) {
		case epotBox : {
			Fobb obb; Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); obb.m_rotate.identity();
			CPhysicsElement* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
			m_pPhysicsShell->add_Element(E);
			m_pPhysicsShell->setMass(m_mass);
			if(!H_Parent())
				m_pPhysicsShell->Activate(svXFORM(),0,svXFORM());
			m_pPhysicsShell->mDesired.identity();
			m_pPhysicsShell->fDesiredStrength = 0.f;
		} break;
		case epotFixedChain : 
		case epotFreeChain  :
			{
			m_pPhysicsShell->set_Kinematics(PKinematics(pVisual));
			AddElement(0,PKinematics(pVisual)->LL_BoneRoot());
			m_pPhysicsShell->setMass1(m_mass);
		} break;
		case   epotSkeleton: CreateSkeleton(fixed_bone); break;

		default : {
		} break;
	
	}
	m_pPhysicsShell->mXFORM.set(svTransform);
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	//m_pPhysicsShell->SetAirResistance(0.002f, 0.3f);
}


void CPhysicObject::Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse)
{
	if(m_pPhysicsShell){
		switch(m_type) {
		case epotBox :
			inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
		break;
		case epotFixedChain :
		case epotFreeChain  :
		case epotSkeleton   :
			m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
		break;
		default : NODEFAULT;
		}
	}
}

void CPhysicObject::CreateSkeleton(LPCSTR fixed_bone)
{
	if (!pVisual) return;

	int iFBone = PKinematics(pVisual)->LL_BoneID(fixed_bone);

	Fmatrix ident;
	float density=100.f;
	float hinge_force=0.f;
	float hinge_force1=0.f;
	float hinge_vel=0.f;
	//u32 material=0;
	//LPCSTR material="actor";
	LPCSTR material="materials\\skel1";
	ident.identity();

	Fmatrix m1;
	m1.set(ident);
	m1._11=0.f;
	m1._12=1.f;
	m1._21=-1.f;
	m1._22=0.f;


	Fmatrix m2;
	m2.set(ident);
	m2._11=-1.f;
	m2._33=-1.f;

	Fmatrix m3;
	m3.set(ident);
	m3._11=-1.f;
	m3._22=-1.f;

	Fmatrix m4;
	m4.set(ident);
	m4._22=-1.f;
	m4._33=-1.f;

	Fmatrix m5;
	m5.set(ident);

	Fmatrix m6;
	m6.set(m1);
	m6._12=-1.f;
	m6._21=1.f;

	//create shell
	CKinematics* M		= PKinematics(pVisual);			VERIFY(M);
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->set_Kinematics(M);
	CPhysicsJoint*		joint;
	//get bone instance
	int id=M->LL_BoneID("bip01_pelvis");

	

	CBoneInstance& instance=M->LL_GetInstance				(id);

	//create root element
	CPhysicsElement* element=P_create_Element				();
	element->mXFORM.set(m1);
	instance.set_callback(m_pPhysicsShell->GetBonesCallback(),element);

	element->add_Box(M->LL_GetBox(id));
	//Fsphere sphere;
	//sphere.P.set(0,0,0);
	//sphere.R=0.3f;
	//pelvis->add_Sphere(sphere);
	element->setDensity(density);
	m_pPhysicsShell->add_Element(element);
	element->SetMaterial("materials\\skel1");

	if(id==iFBone)
	{
	joint=P_create_Joint(CPhysicsJoint::ball,0,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	m_pPhysicsShell->add_Joint(joint);
	}

	CPhysicsElement* parent=element;
	CPhysicsElement* root=parent;

	//spine

	id=M->LL_BoneID("bip01_spine");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1.f/4.f,M_PI*1.f/2.f,0);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	//Fquaternion k;
	//k.get_axis_angle
	//Fmatrix m;

	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}
	//parent=element;

	id=M->LL_BoneID("bip01_spine1");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/4.f,0);//
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(-M_PI/8.f,M_PI/8.f,1);
	joint->SetForceAndVelocity(hinge_force);
	joint->SetForceAndVelocity(hinge_force*10,0.f,2);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	CPhysicsElement* root1=parent;
	id=M->LL_BoneID("bip01_neck");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	/*
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,0);
	*/

	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetAxisVsSecondElement(0,0,1,1);
	joint->SetAxisVsSecondElement(1,0,0,0);

	joint->SetLimits(-M_PI/5.f,M_PI/5.f,0);
	joint->SetLimits(0.f,0.f,1);
	joint->SetLimits(-M_PI/3.f,M_PI/3.f,2);

	//joint->SetLimits(0.1f,0.f,0);
	//joint->SetLimits(0.f,0.f,1);
	//joint->SetLimits(0.f,0.f,2);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	id=M->LL_BoneID("bip01_head");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*5);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/3.f,M_PI/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);

	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=root1;

	id=M->LL_BoneID("bip01_l_clavicle");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	//	const Fobb& box=M->LL_GetBox(id);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);//box.m_halfsize.y box.m_halfsize.x*2.f
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	//parent=element;

	id=M->LL_BoneID("bip01_l_upperarm");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetLimits(-M_PI/3.f,M_PI/2.2f,0);
	//joint->SetLimits(0.f,0.f,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	id=M->LL_BoneID("bip01_l_forearm");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*3.f/4.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_l_hand");
	element=P_create_Element				();
	element->mXFORM.set(m3);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*20.f);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/3.f,M_PI*1/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=root1;
	id=M->LL_BoneID("bip01_r_clavicle");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/3.f,M_PI/4.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	//parent=element;
	id=M->LL_BoneID("bip01_r_upperarm");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetLimits(-M_PI/2.2f,M_PI/3.f,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	id=M->LL_BoneID("bip01_r_forearm");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*3.f/4.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	id=M->LL_BoneID("bip01_r_hand");
	element=P_create_Element				();
	element->mXFORM.set(m5);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*20.f);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/3.f,M_PI*1/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=root;
	id=M->LL_BoneID("bip01_r_thigh");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI*1.f/6.f,M_PI*1.f/4.f,2);
	joint->SetLimits(0.f,0.f,1);
	//joint->SetLimits(0,M_PI*1/3.5f,0);
	joint->SetLimits(-M_PI*1/8.f,0,0);
	joint->SetForceAndVelocity(hinge_force*2.f);
	joint->SetForceAndVelocity(hinge_force*2.f,hinge_vel*3.f,2);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	id=M->LL_BoneID("bip01_r_calf");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*2/3.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	id=M->LL_BoneID("bip01_r_foot");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*5.f);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/6.f,M_PI*1/6.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=root;
	id=M->LL_BoneID("bip01_l_thigh");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,0,1,0);
	joint->SetAxisVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI*1.f/6.f,M_PI*1.f/4.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetLimits(0,M_PI*1/8.f,0);
	joint->SetForceAndVelocity(hinge_force*2.f);
	joint->SetForceAndVelocity(hinge_force*2.f,hinge_vel*3.f,2);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	id=M->LL_BoneID("bip01_l_calf");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*2/3.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	parent=element;
	id=M->LL_BoneID("bip01_l_foot");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*20.f);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/6.f,M_PI*1/6.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");
	if(id==iFBone)
	{
		joint=P_create_Joint(CPhysicsJoint::ball,0,element);
		joint->SetAnchorVsSecondElement(0,0,0);
		m_pPhysicsShell->add_Joint(joint);
	}

	//set shell start position
	Fmatrix m;
	m.set(mRotate);

	m.c.set(vPosition);
	//Movement.GetDeathPosition(m.c);
	//m.c.y-=0.4f;
	//m_pPhysicsShell->setMass1(1000.f);
	m_pPhysicsShell->mXFORM.set(m);
//	m_pPhysicsShell->SetAirResistance(0.0014f,
//	1.5f);

}

void CPhysicObject::net_Export(NET_Packet& P)
{
	R_ASSERT						(Local());
}

void CPhysicObject::net_Import(NET_Packet& P)
{
	R_ASSERT						(Remote());
}

#include "stdafx.h"
#pragma hdrstop

#include "actor.h"

void CActor::create_Skeleton(){
	Fmatrix ident;
	float density=100.f*skel_density_factor;
	float hinge_force=5.f*hinge_force_factor;
	float hinge_force1=5.f*hinge_force_factor2;
	//u32 material=0;
	LPCSTR material="actor";
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
	m_phSkeleton		= P_create_Shell();
	m_phSkeleton->set_Kinematics(M);
	CPhysicsJoint*		joint;
	//get bone instance
	int id=M->LL_BoneID("bip01_pelvis");
	CBoneInstance& instance=M->LL_GetInstance				(id);

	//create root element
	CPhysicsElement* element=P_create_Element				();
	element->mXFORM.set(m1);
	instance.set_callback(m_phSkeleton->GetBonesCallback(),element);

	element->add_Box(M->LL_GetBox(id));
	//Fsphere sphere;
	//sphere.P.set(0,0,0);
	//sphere.R=0.3f;
	//pelvis->add_Sphere(sphere);
	element->setDensity(density);
	m_phSkeleton->add_Element(element);
	element->SetMaterial("materials\\skel1");
	CPhysicsElement* parent=element;
	CPhysicsElement* root=parent;

	//spine

	id=M->LL_BoneID("bip01_spine");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1.f/4.f,M_PI*1.f/2.f,0);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);
	//Fquaternion k;
	//k.get_axis_angle
	//Fmatrix m;


	//parent=element;

	id=M->LL_BoneID("bip01_spine1");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(1,0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/4.f,0);//
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(-M_PI/8.f,M_PI/8.f,1);
	joint->SetForceAndVelocity(hinge_force);
	joint->SetForceAndVelocity(hinge_force*10,1.5f,2);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	CPhysicsElement* root1=parent;
	id=M->LL_BoneID("bip01_neck");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	/*
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,0);
	*/

	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,2);
	joint->SetAxisDirVsSecondElement(0,0,1,1);
	joint->SetAxisDirVsSecondElement(1,0,0,0);

	joint->SetLimits(-M_PI/5.f,M_PI/5.f,0);
	joint->SetLimits(0.f,0.f,1);
	joint->SetLimits(-M_PI/3.f,M_PI/3.f,2);

	//joint->SetLimits(0.1f,0.f,0);
	//joint->SetLimits(0.f,0.f,1);
	//joint->SetLimits(0.f,0.f,2);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_head");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*5);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/3.f,M_PI/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=root1;

	id=M->LL_BoneID("bip01_l_clavicle");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	//	const Fobb& box=M->LL_GetBox(id);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);//box.m_halfsize.y box.m_halfsize.x*2.f
	joint->SetAxisDirVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	//parent=element;

	id=M->LL_BoneID("bip01_l_upperarm");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,0,1,0);
	joint->SetLimits(-M_PI/3.f,M_PI/2.2f,0);
	//joint->SetLimits(0.f,0.f,0);
	joint->SetAxisDirVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_l_forearm");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*3.f/4.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_l_hand");
	element=P_create_Element				();
	element->mXFORM.set(m3);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*20.f);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/3.f,M_PI*1/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);


	parent=root1;
	id=M->LL_BoneID("bip01_r_clavicle");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/3.f,M_PI/4.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	//parent=element;
	id=M->LL_BoneID("bip01_r_upperarm");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,0,1,0);
	joint->SetLimits(-M_PI/2.2f,M_PI/3.f,0);
	joint->SetAxisDirVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_r_forearm");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*3.f/4.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	id=M->LL_BoneID("bip01_r_hand");
	element=P_create_Element				();
	element->mXFORM.set(m5);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*20.f);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/3.f,M_PI*1/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);

	parent=root;
	id=M->LL_BoneID("bip01_r_thigh");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,0,1,0);
	joint->SetAxisDirVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI*1.f/6.f,M_PI*1.f/4.f,2);
	joint->SetLimits(0.f,0.f,1);
	//joint->SetLimits(0,M_PI*1/3.5f,0);
	joint->SetLimits(-M_PI*1/8.f,0,0);
	joint->SetForceAndVelocity(hinge_force*2.f);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_r_calf");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*2/3.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_r_foot");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*5.f);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/6.f,M_PI*1/6.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=root;
	id=M->LL_BoneID("bip01_l_thigh");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::full_control,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,0,1,0);
	joint->SetAxisDirVsSecondElement(0,1,0,2);
	joint->SetLimits(-M_PI*1.f/6.f,M_PI*1.f/4.f,2);
	joint->SetLimits(0.f,0.f,1);
	joint->SetLimits(0,M_PI*1/8.f,0);
	joint->SetForceAndVelocity(hinge_force*2.f);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_l_calf");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*2/3.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_l_foot");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density*20.f);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/6.f,M_PI*1/6.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial("materials\\skel1");


	//set shell start position
	Fmatrix m;
	m.set(mRotate);
	m.c.set(vPosition);
	//ph_Movement.GetDeathPosition(m.c);
	//m.c.y-=0.4f;
	m_phSkeleton->mXFORM.set(m);
	m_phSkeleton->SetAirResistance(0.002f*skel_airr_lin_factor,
		0.3f*skel_airr_ang_factor);

}



void CActor::create_Skeleton1(){
	Fmatrix ident;
	float density=100.f;
	float hinge_force=5.f*hinge_force_factor;
	//u32 material=0;
	LPCSTR material="actor";
	ident.identity();

	Fmatrix m1;
	m1.set(ident);


	Fmatrix m2;
	/*
	m2._11=	0.80434370f;
	m2._12=	-0.59369904f;
	m2._13=	0.00000000f;

	m2._14=	0.00000000f;

	m2._21=	0.59369904f;
	m2._22=	0.80434370f;
	m2._23=	0.00000000f;

	m2._24=	0.00000000f;

	m2._31=	0.00000000f;
	m2._32=	0.00000000f;
	m2._33=	1.0000000f;

	m2._34=	0.00000000f;
	*/

	m2._11=	0.0f;
	m2._12=	-1.0f;
	m2._13=	0.00000000f;

	m2._14=	0.00000000f;

	m2._21=	1.0f;
	m2._22=	0.0f;
	m2._23=	0.00000000f;

	m2._24=	0.00000000f;

	m2._31=	0.00000000f;
	m2._32=	0.00000000f;
	m2._33=	1.0000000f;

	m2._34=	0.00000000f;



	//create shell
	CKinematics* M		= PKinematics(pVisual);			VERIFY(M);
	m_phSkeleton		= P_create_Shell();
	CPhysicsJoint*		joint;
	//get bone instance
	int id=M->LL_BoneID("bone01");
	CBoneInstance& instance=M->LL_GetInstance				(id);

	//create root element
	CPhysicsElement* element=P_create_Element				();
	element->mXFORM.set(m1);
	instance.set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	//Fsphere sphere;
	//sphere.P.set(0,0,0);
	//sphere.R=0.3f;
	//pelvis->add_Sphere(sphere);
	element->setDensity(density);
	m_phSkeleton->add_Element(element);
	element->SetMaterial(material);

	CPhysicsElement* parent=element;


	//spine
	id=M->LL_BoneID("bone02");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_phSkeleton->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setDensity(density);
	element->set_ParentElement(parent);
	m_phSkeleton->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisDirVsSecondElement(0,0,1,0);
	joint->SetLimits(0,M_PI*1.f/2.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_phSkeleton->add_Joint(joint);
	element->SetMaterial(material);
	//Fquaternion k;
	//k.get_axis_angle
	//Fmatrix m;







	//set shell start position
	Fmatrix m;
	m.set(mRotate);
	ph_Movement.GetDeathPosition(m.c);
	m_phSkeleton->mXFORM.set(m);

}

////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "..\\..\\..\\xr_trims.h"

CAI_Stalker::CAI_Stalker			()
{
	m_tStateList.clear				();
	while (m_tStateStack.size())
		m_tStateStack.pop			();
	
	m_tMovementDirection			= eMovementDirectionForward;
	m_tDesirableDirection			= eMovementDirectionForward;
	m_tLookType						= eLookTypePatrol;
	m_tBodyState					= eBodyStateStand;
	m_dwDirectionStartTime			= 0;
	m_dwAnimationSwitchInterval		= 500;
	r_torso_speed					= PI_MUL_2;
	r_head_speed					= 3*PI_DIV_2;
	
	m_dwTimeToChange				= 0;
	m_dwHitTime						= 0;

	m_dwActionRefreshRate			= 1000;
	m_fAttackSuccessProbability		= .7f;
	m_dwNoFireTime					= 0;

	m_tSavedEnemy					= 0;
	m_tSavedEnemyPosition.set		(0,0,0);
	m_dwLostEnemyTime				= 0;
	m_tpSavedEnemyNode				= 0;
	m_dwSavedEnemyNodeID			= u32(-1);
	m_tpWeaponToTake				= 0;
	m_bActionStarted				= false;
	
//	m_fAccuracy						= 100.f;
//	m_fIntelligence					= 100.f;
	
	m_pPhysicsShell					= NULL;
	m_saved_impulse					= 0.f;
}

CAI_Stalker::~CAI_Stalker			()
{
//	Msg								("FSM report for %s :",cName());
//	for (int i=0; i<(int)m_tStateStack.size(); i++)
//		Msg							("%3d %6d",m_tStateList[i].eState,m_tStateList[i].dwTime);
//	Msg								("Total updates : %d",m_dwUpdateCount);
	xr_delete						(Weapons);
	xr_delete						(m_pPhysicsShell);
}

// when soldier is dead
void CAI_Stalker::Die				()
{
	vfAddStateToList				(m_eCurrentState = eStalkerStateDie);

	Fvector	dir;
	AI_Path.Direction				(dir);
	SelectAnimation					(clTransform.k,dir,AI_Path.fSpeed);
}

void CAI_Stalker::Load				(LPCSTR section)
{ 
	setEnabled						(false);
	inherited::Load					(section);
	m_tSelectorFreeHunting.Load		(section);
	m_tSelectorReload.Load			(section);
	m_tSelectorRetreat.Load			(section);
	// visibility
	m_dwMovementIdleTime			= pSettings->ReadINT(section,"MovementIdleTime");
	m_fMaxInvisibleSpeed			= pSettings->ReadFLOAT(section,"MaxInvisibleSpeed");
	m_fMaxViewableSpeed				= pSettings->ReadFLOAT(section,"MaxViewableSpeed");
	m_fMovementSpeedWeight			= pSettings->ReadFLOAT(section,"MovementSpeedWeight");
	m_fDistanceWeight				= pSettings->ReadFLOAT(section,"DistanceWeight");
	m_fSpeedWeight					= pSettings->ReadFLOAT(section,"SpeedWeight");
	m_fCrouchVisibilityMultiplier	= pSettings->ReadFLOAT(section,"CrouchVisibilityMultiplier");
	m_fLieVisibilityMultiplier		= pSettings->ReadFLOAT(section,"LieVisibilityMultiplier");
	m_fVisibilityThreshold			= pSettings->ReadFLOAT(section,"VisibilityThreshold");
	m_fLateralMultiplier			= pSettings->ReadFLOAT(section,"LateralMultiplier");
	m_fShadowWeight					= pSettings->ReadFLOAT(section,"ShadowWeight");

	m_fCrouchFactor					= pSettings->ReadFLOAT(section,"CrouchFactor");
	m_fWalkFactor					= pSettings->ReadFLOAT(section,"WalkFactor");
	m_fRunFactor					= pSettings->ReadFLOAT(section,"RunFactor");

	//fire
	m_dwFireRandomMin  				= pSettings->ReadINT(section,"FireRandomMin");
	m_dwFireRandomMax  				= pSettings->ReadINT(section,"FireRandomMax");
	m_dwNoFireTimeMin  				= pSettings->ReadINT(section,"NoFireTimeMin");
	m_dwNoFireTimeMax  				= pSettings->ReadINT(section,"NoFireTimeMax");
	m_fMinMissDistance 				= pSettings->ReadFLOAT(section,"MinMissDistance");
	m_fMinMissFactor   				= pSettings->ReadFLOAT(section,"MinMissFactor");
	m_fMaxMissDistance 				= pSettings->ReadFLOAT(section,"MaxMissDistance");
	m_fMaxMissFactor				= pSettings->ReadFLOAT(section,"MaxMissFactor");

	eye_fov							= pSettings->ReadFLOAT(section,"eye_fov");
	eye_range						= pSettings->ReadFLOAT(section,"eye_range");

	m_tpaTerrain.clear				();
	LPCSTR							S = pSettings->ReadSTRING(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT						(((N % (LOCATION_TYPE_COUNT + 2)) == 0) && (N));
	STerrainPlace					tTerrainPlace;
	tTerrainPlace.tMask.resize		(LOCATION_TYPE_COUNT);
	m_tpaTerrain.reserve			(32);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; j++, i++)
			tTerrainPlace.tMask[j] = _LOCATION_ID(atoi(_GetItem(S,i,I)));
		tTerrainPlace.dwMinTime		= atoi(_GetItem(S,i++,I))*1000;
		tTerrainPlace.dwMaxTime		= atoi(_GetItem(S,i++,I))*1000;
		m_tpaTerrain.push_back(tTerrainPlace);
	}
	m_fGoingSpeed					= pSettings->ReadFLOAT	(section, "going_speed");

	// physics
	skel_density_factor = pSettings->ReadFLOAT(section,"ph_skeleton_mass_factor");
	skel_airr_lin_factor=pSettings->ReadFLOAT(section,"ph_skeleton_airr_lin_factor");
	skel_airr_ang_factor=pSettings->ReadFLOAT(section,"ph_skeleton_airr_ang_factor");
	hinge_force_factor  =pSettings->ReadFLOAT(section,"ph_skeleton_hinger_factor");
	hinge_force_factor1 =pSettings->ReadFLOAT(section,"ph_skeleton_hinger_factor1");
	skel_ddelay			=pSettings->ReadINT(section,"ph_skeleton_ddelay");
	hinge_force_factor2 =pSettings->ReadFLOAT(section,"ph_skeleton_hinger_factor2");
	hinge_vel			=pSettings->ReadFLOAT(section,"ph_skeleton_hinge_vel");
	skel_fatal_impulse_factor=pSettings->ReadFLOAT(section,"ph_skel_fatal_impulse_factor");

	// Msg	("! stalker size: %d",sizeof(*this));
}

BOOL CAI_Stalker::net_Spawn			(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return						(FALSE);
	xrSE_Human						*tpHuman = (xrSE_Human*)(DC);
	R_ASSERT						(tpHuman);
	cNameVisual_set					(tpHuman->caModel);
	
	fHealth							= tpHuman->fHealth;
	m_tCurGP						= tpHuman->m_tGraphID;
	m_tNextGP						= tpHuman->m_tNextGraphID;
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_tNextGP						= m_tCurGP = getAI().m_tpaCrossTable[AI_NodeID].tGraphIndex;
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	m_tStateStack.push				(m_eCurrentState = eStalkerStateAccomplishingTask);
	vfAddStateToList				(m_eCurrentState);

	CStalkerAnimations::Load		(PKinematics(pVisual));
	vfAssignBones					(pSettings,cNameSect());

	setEnabled						(true);
	return							(TRUE);
}

void CAI_Stalker::net_Export		(NET_Packet& P)
{
	R_ASSERT						(Local());

	// export last known packet
	R_ASSERT						(!NET.empty());
	net_update& N					= NET.back();
	P.w_u32							(N.dwTimeStamp);
	P.w_u8							(0);
	P.w_vec3						(N.p_pos);
	P.w_angle8						(N.o_model);
	P.w_angle8						(N.o_torso.yaw);
	P.w_angle8						(N.o_torso.pitch);
	P.w_float						(N.fHealth);
	
	P.w								(&m_tNextGP,				sizeof(m_tNextGP));
	P.w								(&m_tCurGP,					sizeof(m_tCurGP));
	P.w								(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.w								(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	
	float							f1;
	if (m_tCurGP != u16(-1)) {
		f1								= vPosition.distance_to		(getAI().m_tpaGraph[m_tCurGP].tLocalPoint);
		P.w								(&f1,						sizeof(f1));
		f1								= vPosition.distance_to		(getAI().m_tpaGraph[m_tNextGP].tLocalPoint);
		P.w								(&f1,						sizeof(f1));
	}
	else {
		f1								= 0;
		P.w								(&f1,						sizeof(f1));
		P.w								(&f1,						sizeof(f1));
	}
}

void CAI_Stalker::net_Import		(NET_Packet& P)
{
	R_ASSERT						(Remote());
	net_update						N;

	u8 flags;
	P.r_u32							(N.dwTimeStamp);
	P.r_u8							(flags);
	P.r_vec3						(N.p_pos);
	P.r_angle8						(N.o_model);
	P.r_angle8						(N.o_torso.yaw);
	P.r_angle8						(N.o_torso.pitch);
	P.r_float						(N.fHealth);

	P.r								(&m_tNextGP,		sizeof(m_tNextGP));
	P.r								(&m_tCurGP,			sizeof(m_tCurGP));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back				(N);
		NET_WasInterpolating		= TRUE;
	}

	setVisible						(TRUE);
	setEnabled						(TRUE);
}

void CAI_Stalker::Exec_Movement		(float dt)
{
	AI_Path.Calculate				(this,vPosition,vPosition,m_fCurSpeed,dt);
}

void CAI_Stalker::CreateSkeleton()
{
	if (!pVisual)
		return;
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
	element->setMass(density);
	m_pPhysicsShell->add_Element(element);
	element->SetMaterial("materials\\skel1");
	CPhysicsElement* parent=element;
	CPhysicsElement* root=parent;

	//spine
	
	id=M->LL_BoneID("bip01_spine");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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
	
	
	//parent=element;
	
	id=M->LL_BoneID("bip01_spine1");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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
	joint->SetForceAndVelocity(hinge_force*10,1.5f,2);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);

	parent=element;
	CPhysicsElement* root1=parent;
	id=M->LL_BoneID("bip01_neck");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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

	parent=element;
	id=M->LL_BoneID("bip01_head");
	element=P_create_Element				();
	element->mXFORM.set(m1);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*5);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/3.f,M_PI/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);

	parent=root1;
	
	id=M->LL_BoneID("bip01_l_clavicle");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
//	const Fobb& box=M->LL_GetBox(id);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);//box.m_halfsize.y box.m_halfsize.x*2.f
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/4.f,M_PI/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);

	//parent=element;
	
	id=M->LL_BoneID("bip01_l_upperarm");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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

	parent=element;
	id=M->LL_BoneID("bip01_l_forearm");
	element=P_create_Element				();
	element->mXFORM.set(m2);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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
	element->setMass(density*20.f);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/3.f,M_PI*1/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);


	parent=root1;
	id=M->LL_BoneID("bip01_r_clavicle");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(1,0,0,0);
	joint->SetLimits(-M_PI/3.f,M_PI/4.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);

	//parent=element;
	id=M->LL_BoneID("bip01_r_upperarm");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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

	parent=element;
	id=M->LL_BoneID("bip01_r_forearm");
	element=P_create_Element				();
	element->mXFORM.set(m4);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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
	id=M->LL_BoneID("bip01_r_hand");
	element=P_create_Element				();
	element->mXFORM.set(m5);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*20.f);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/3.f,M_PI*1/3.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial(material);

	parent=root;
	id=M->LL_BoneID("bip01_r_thigh");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_r_calf");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*2/3.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_r_foot");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*5.f);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/6.f,M_PI*1/6.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=root;
	id=M->LL_BoneID("bip01_l_thigh");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
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
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

	parent=element;
	id=M->LL_BoneID("bip01_l_calf");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::hinge,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*2/3.f,0,0);
	joint->SetForceAndVelocity(hinge_force1,hinge_vel);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");

 	parent=element;
	id=M->LL_BoneID("bip01_l_foot");
	element=P_create_Element				();
	element->mXFORM.set(m6);
	(M->LL_GetInstance(id)).set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	element->add_Box(M->LL_GetBox(id));
	element->setMass(density*20.f);
	element->set_ParentElement(parent);
	m_pPhysicsShell->add_Element(element);
	joint=P_create_Joint(CPhysicsJoint::welding,parent,element);
	joint->SetAnchorVsSecondElement(0,0,0);
	joint->SetAxisVsSecondElement(0,1,0,0);
	joint->SetLimits(-M_PI*1/6.f,M_PI*1/6.f,0);
	joint->SetForceAndVelocity(hinge_force);
	m_pPhysicsShell->add_Joint(joint);
	element->SetMaterial("materials\\skel1");


	//set shell start position
	Fmatrix m;
	m.set(mRotate);
	m.c.set(vPosition);
	//ph_Movement.GetDeathPosition(m.c);
	//m.c.y-=0.4f;
	m_pPhysicsShell->mXFORM.set(m);
	m_pPhysicsShell->SetAirResistance(0.002f*skel_airr_lin_factor,
								   0.3f*skel_airr_ang_factor);
	
}

void CAI_Stalker::UpdateCL(){

	inherited::UpdateCL();
	if(m_pPhysicsShell)
		clTransform.set(m_pPhysicsShell->mXFORM);
	else
		if (!g_Alive())
			CreateSkeleton();
}

void CAI_Stalker::Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse){
	
	
	if(!m_pPhysicsShell){
		inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
		m_saved_impulse=impulse*skel_fatal_impulse_factor;
		m_saved_element=element;
		m_saved_hit_dir.set(dir);
		m_saved_hit_position.set(p_in_object_space);
	}
	else {
		if (!g_Alive()) {
			if(m_pPhysicsShell) 
				m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
				//m_pPhysicsShell->applyImpulseTrace(position_in_bone_space,dir,impulse);
			else{
				m_saved_hit_dir.set(dir);
				m_saved_impulse=impulse*skel_fatal_impulse_factor;
				m_saved_element=element;
				m_saved_hit_position.set(p_in_object_space);
			}
		}
	}
}

void CAI_Stalker::Update	( u32 DT )
{
	// Queue shrink
	u32	dwTimeCL	= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();
	
	// Queue setup
	float dt			= float(DT)/1000.f;
	if (dt > 3)
		return;
	
	if (Remote())		{
	} else {
		// here is monster AI call
		m_fTimeUpdateDelta				= dt;
		Device.Statistic.AI_Think.Begin	();
		Think							();
		m_dwLastUpdateTime = Level().timeServer();
		Device.Statistic.AI_Think.End	();
		//
		Engine.Sheduler.Slice			();

		// Look and action streams
		if (fHealth>0) {
			Exec_Look				(dt);
			Exec_Movement			(dt);
			Exec_Visibility			();
			
			//////////////////////////////////////
			Fvector C; float R;
			//////////////////////////////////////
			// Ñ Îëåñÿ - ÏÈÂÎ!!!! (Äèìå :-))))
			// Movement.GetBoundingSphere	(C,R);
			//////////////////////////////////////
			svCenter(C);
			R = Radius();
			//////////////////////////////////////
			feel_touch_update		(C,R);

			net_update				uNext;
			uNext.dwTimeStamp		= Level().timeServer();
			uNext.o_model			= r_torso_current.yaw;
			uNext.o_torso			= r_current;
			uNext.p_pos				= vPosition;
			uNext.fHealth			= fHealth;
			NET.push_back			(uNext);
		}
		else 
		{
			Exec_Physics			(dt);
			if (bfExecMovement()) 
			{
				Exec_Movement		(dt);
				net_update			uNext;
				uNext.dwTimeStamp	= Level().timeServer();
				uNext.o_model		= r_torso_current.yaw;
				uNext.o_torso		= r_current;
				uNext.p_pos			= vPosition;
				uNext.fHealth		= fHealth;
				NET.push_back		(uNext);
			}
			else {
				net_update			uNext;
				uNext.dwTimeStamp	= Level().timeServer();
				uNext.o_model		= r_torso_current.yaw;
				uNext.o_torso		= r_current;
				uNext.p_pos			= vPosition;
				uNext.fHealth		= fHealth;
				NET.push_back		(uNext);
			}
		}
		Exec_Action				(dt);
	}

	// weapons
	if (Weapons)		Weapons->Update		(dt,false);
	
	// *** general stuff
	inherited::inherited::Update	(DT);
	
//	inherited::Update	(DT);

	if(m_pPhysicsShell){

	if(m_pPhysicsShell->bActive && m_saved_impulse!=0.f)
		{
			m_pPhysicsShell->applyImpulseTrace(m_saved_hit_position,m_saved_hit_dir,m_saved_impulse*1.5f,m_saved_element);
			m_saved_impulse=0.f;
		}
		mRotate.set(m_pPhysicsShell->mXFORM);
		mRotate.c.set(0,0,0);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		if(skel_ddelay==0)
		{
		m_pPhysicsShell->set_JointResistance(5.f*hinge_force_factor1);
		
		}
		skel_ddelay--;

		mRotate.set(m_pPhysicsShell->mXFORM);
		mRotate.c.set(0,0,0);
		UpdateTransform					();
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		svTransform.set(m_pPhysicsShell->mXFORM);
		
	//	CKinematics* M		= PKinematics(pVisual);			VERIFY(M);
	//	int id=M->LL_BoneID("bip01_pelvis");
	//	CBoneInstance& instance=M->LL_GetInstance				(id);
	//	instance.mTransform.set(m_pPhysicsShell->mXFORM);
			
	}
}
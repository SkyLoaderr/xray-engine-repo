#include "stdafx.h"
#include "physicobject.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"
#include "level.h"



CPhysicObject::CPhysicObject(void) 
{
	m_type = epotBox;
	m_mass = 10.f;

}
CPhysicObject::~CPhysicObject(void)
{

}




BOOL CPhysicObject::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeObjectPhysic	*po	= smart_cast<CSE_ALifeObjectPhysic*>(e);
	R_ASSERT				(po);
	m_type					= EPOType(po->type);
	m_mass					= po->mass;
	inherited::net_Spawn	(DC);
	xr_delete(collidable.model);
	switch(m_type) {
		case epotBox:			collidable.model = xr_new<CCF_Rigid>(this);		break;
		case epotFixedChain:
		case epotFreeChain :
		case epotSkeleton  :	collidable.model = xr_new<CCF_Skeleton>(this);	break;

		default: NODEFAULT; 
	}

	CPHSkeleton::Spawn(e);
	setVisible(true);
	setEnabled(true);
	return TRUE;
}

void	CPhysicObject::SpawnInitPhysics	(CSE_Abstract* D)
{
	CreatePhysicsShell(D);

	if(Visual()&&smart_cast<CKinematics*>(Visual()))
	{
//		CSE_PHSkeleton	*po	= smart_cast<CSE_PHSkeleton*>(D);
		CSkeletonAnimated*	pSkeletonAnimated=NULL;
		R_ASSERT			(Visual()&&smart_cast<CKinematics*>(Visual()));
		pSkeletonAnimated	=smart_cast<CSkeletonAnimated*>(Visual());
		if(pSkeletonAnimated)
		{
			CSE_Visual					*visual = smart_cast<CSE_Visual*>(D);
			R_ASSERT					(visual);
			R_ASSERT2					(*visual->startup_animation,"no startup animation");
			pSkeletonAnimated->PlayCycle(*visual->startup_animation);
		}
		smart_cast<CKinematics*>(Visual())->CalculateBones	();
	}
}
void CPhysicObject::net_Destroy()
{
	CPHSkeleton::RespawnInit();
	inherited::net_Destroy();
}

void CPhysicObject::net_Save(NET_Packet& P)
{
	inherited::net_Save(P);
	CPHSkeleton::SaveNetState(P);
}
void CPhysicObject::CreatePhysicsShell(CSE_Abstract* e)
{
	CSE_ALifeObjectPhysic	*po	= smart_cast<CSE_ALifeObjectPhysic*>(e);
	CreateBody(po);
}

void CPhysicObject::CreateSkeleton(CSE_ALifeObjectPhysic* po)
{
	if(m_pPhysicsShell) return;
	if(!Visual()) return;
	LPCSTR	fixed_bones=*po->fixed_bones;
	m_pPhysicsShell=P_build_Shell(this,!po->_flags.test(CSE_PHSkeleton::flActive),fixed_bones);
}

void CPhysicObject::Load(LPCSTR section)
{
	inherited::Load(section);
	CPHSkeleton::Load(section);
}


void CPhysicObject::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);
	CPHSkeleton::Update(dt);
}
void CPhysicObject::UpdateCL()
{
	inherited::UpdateCL();
	PHObjectPositionUpdate();
}
void CPhysicObject::PHObjectPositionUpdate	()
{
	
	if(m_pPhysicsShell)
	{


		if(m_type==epotBox) 
		{
			m_pPhysicsShell->Update();
			XFORM().set			(m_pPhysicsShell->mXFORM);
		}
		else
			m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
	}


}

void CPhysicObject::AddElement(CPhysicsElement* root_e, int id)
{
	CKinematics* K		= smart_cast<CKinematics*>(Visual());

	CPhysicsElement* E	= P_create_Element();
	CBoneInstance& B	= K->LL_GetBoneInstance(u16(id));
	E->mXFORM.set		(K->LL_GetTransform(u16(id)));
	Fobb bb			= K->LL_GetBox(u16(id));


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
		J->SetAxisDirVsSecondElement	(1,0,0,0);
		J->SetAxisDirVsSecondElement	(0,1,0,2);
		J->SetLimits				(-M_PI/2,M_PI/2,0);
		J->SetLimits				(-M_PI/2,M_PI/2,1);
		J->SetLimits				(-M_PI/2,M_PI/2,2);
		m_pPhysicsShell->add_Joint	(J);	
	}

	CBoneData& BD		= K->LL_GetData(u16(id));
	for (vecBonesIt it=BD.children.begin(); BD.children.end() != it; ++it){
		AddElement		(E,(*it)->SelfID);
	}
}


void CPhysicObject::CreateBody(CSE_ALifeObjectPhysic* po) {

	if(m_pPhysicsShell) return;
	CKinematics* pKinematics=smart_cast<CKinematics*>(Visual());
	switch(m_type) {
		case epotBox : {
			m_pPhysicsShell=P_build_SimpleShell(this,m_mass,!po->_flags.test(CSE_ALifeObjectPhysic::flActive));
					   } break;
		case epotFixedChain : 
		case epotFreeChain  :
			{	
				m_pPhysicsShell		= P_create_Shell();
				m_pPhysicsShell->set_Kinematics(pKinematics);
				AddElement(0,pKinematics->LL_GetBoneRoot());
				m_pPhysicsShell->setMass1(m_mass);
			} break;

		case   epotSkeleton: 
			{
				//pKinematics->LL_SetBoneRoot(0);
				CreateSkeleton(po);
			}break;

		default : {
				  } break;

	}

	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	if(pKinematics)
	{

		SAllDDOParams disable_params;
		disable_params.Load(pKinematics->LL_UserData());
		m_pPhysicsShell->set_DisableParams(disable_params);
	}
	//m_pPhysicsShell->SetAirResistance(0.002f, 0.3f);


}






BOOL CPhysicObject::net_SaveRelevant()
{
	return TRUE;//!m_flags.test(CSE_ALifeObjectPhysic::flSpawnCopy);
}


BOOL CPhysicObject::UsedAI_Locations()
{
	return					(FALSE);
}



void CPhysicObject::InitServerObject(CSE_Abstract * D)
{
	CPHSkeleton::InitServerObject(D);
	CSE_ALifeObjectPhysic		*l_tpALifePhysicObject = smart_cast<CSE_ALifeObjectPhysic*>(D);
	if(!l_tpALifePhysicObject)return;
	l_tpALifePhysicObject->type			= u32(m_type);
}


//////////////////////////////////////////////////////////////////////////
/*
DEFINE_MAP_PRED	(LPCSTR,	CPhysicsJoint*,	JOINT_P_MAP,	JOINT_P_PAIR_IT,	pred_str);

JOINT_P_MAP			*l_tpJointMap = xr_new<JOINT_P_MAP>();

l_tpJointMap->insert(mk_pair(bone_name,joint*));
JOINT_P_PAIR_IT		I = l_tpJointMap->find(bone_name);
if (l_tpJointMap->end()!=I){
//bone_name is found and is an pair_iterator
(*I).second
}

JOINT_P_PAIR_IT		I = l_tpJointMap->begin();
JOINT_P_PAIR_IT		E = l_tpJointMap->end();
for ( ; I != E; ++I) {
(*I).second->joint_method();
Msg("%s",(*I).first);
}

*/

//////////////////////////////////////////////////////////////////////////

	

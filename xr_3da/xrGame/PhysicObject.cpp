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
	LPTSTR start_anim=po->startup_animation;
	xr_delete(collidable.model);
	switch(m_type) {
		case epotBox:			collidable.model = xr_new<CCF_Rigid>(this);		break;
		case epotFixedChain:	
		case epotSkeleton:		collidable.model = xr_new<CCF_Skeleton>(this);	break;
		default: NODEFAULT; 
	}

	switch(m_type) {
		case epotBox:				break;
		case epotFixedChain:
			R_ASSERT				(Visual()&&PKinematics(Visual()));
			PKinematics(Visual())->PlayCycle(start_anim);
			PKinematics(Visual())->Calculate();
			break;
		case epotSkeleton:
			R_ASSERT				(Visual()&&PKinematics(Visual()));
			PKinematics(Visual())->PlayCycle(start_anim);
			PKinematics(Visual())->Calculate();
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
	if(m_pPhysicsShell)
	{
		if(m_type==epotBox) m_pPhysicsShell->Update();
		XFORM().set			(m_pPhysicsShell->mXFORM);
	}
}

void CPhysicObject::AddElement(CPhysicsElement* root_e, int id)
{
	CKinematics* K		= PKinematics(Visual());

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
		J->SetAxisDirVsSecondElement	(1,0,0,0);
		J->SetAxisDirVsSecondElement	(0,1,0,2);
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
				m_pPhysicsShell->Activate(XFORM(),0,XFORM());
			m_pPhysicsShell->mDesired.identity();
			m_pPhysicsShell->fDesiredStrength = 0.f;
		} break;
		case epotFixedChain : 
		case epotFreeChain  :
			{
			m_pPhysicsShell->set_Kinematics(PKinematics(Visual()));
			AddElement(0,PKinematics(Visual())->LL_BoneRoot());
			m_pPhysicsShell->setMass1(m_mass);
		} break;
		case   epotSkeleton: CreateSkeleton(fixed_bone); break;

		default : {
		} break;
	
	}
	m_pPhysicsShell->mXFORM.set(XFORM());
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
	if (!Visual()) return;
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(PKinematics(Visual()));

	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->Activate(true);
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

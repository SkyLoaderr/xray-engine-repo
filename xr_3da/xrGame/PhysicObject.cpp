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
	xrSE_PhysicObject* po	= (xrSE_PhysicObject*)(DC);
	R_ASSERT				(po);
	cNameVisual_set			(po->visual_name);
	inherited::net_Spawn	(DC);

	m_type = EPOType(po->type);
	m_mass = po->mass;

	R_ASSERT(!cfModel);
	switch(m_type) {
		case epotBox : cfModel = xr_new<CCF_Rigid>(this); break;
		default : cfModel = xr_new<CCF_Skeleton>(this);
	}
	pCreator->ObjectSpace.Object_Register(this);
	cfModel->OnMove();

	R_ASSERT				(pVisual&&PKinematics(pVisual));
	PKinematics(pVisual)->PlayCycle("idle");
	PKinematics(pVisual)->Calculate();
	CreateBody				();

	setVisible(true);
	setEnabled(true);

	return TRUE;
}

void CPhysicObject::UpdateCL	()
{
	inherited::UpdateCL		();
	if(m_pPhysicsShell){
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

void CPhysicObject::CreateBody() {
	m_pPhysicsShell		= P_create_Shell();
	switch(m_type) {
		case epotBox : {
			Fobb obb; Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); obb.m_rotate.identity();
			CPhysicsElement* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
			m_pPhysicsShell->add_Element(E);
			m_pPhysicsShell->setMass(m_mass);
			if(!H_Parent())m_pPhysicsShell->Activate(svXFORM(),0,svXFORM());
			m_pPhysicsShell->mDesired.identity();
			m_pPhysicsShell->fDesiredStrength = 0.f;
		} break;
		case epotFixedChain : {
			m_pPhysicsShell->set_Kinematics(PKinematics(pVisual));
			AddElement(0,PKinematics(pVisual)->LL_BoneRoot());
			m_pPhysicsShell->setMass1(m_mass);
		} break;
		default : {
		} break;
	}
	m_pPhysicsShell->mXFORM.set(svTransform);
	m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
	//m_pPhysicsShell->SetAirResistance(0.002f, 0.3f);
}


void CPhysicObject::Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse)
{
	if(m_pPhysicsShell) m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
}

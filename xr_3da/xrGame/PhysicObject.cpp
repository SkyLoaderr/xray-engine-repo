#include "stdafx.h"
#include "physicobject.h"

CPhysicObject::CPhysicObject(void) {
	m_type = epotBox;
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

	CreateBody				();

	setVisible(true);
	setEnabled(true);

	return TRUE;
}

void CPhysicObject::AddElement(CPhysicsElement* root_e, int id)
{
	CKinematics* K		= PKinematics(pVisual);

	CPhysicsElement* E	= P_create_Element();
	CBoneInstance& B	= K->LL_GetInstance(id);
	E->mXFORM.set		(K->LL_GetTransform(id));
	Fobb& bb			= K->LL_GetBox(id);
	bb.m_halfsize.add	(0.05f);
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
			m_pPhysicsShell->setDensity(2000.f);
			if(!H_Parent())m_pPhysicsShell->Activate(svXFORM(),0,svXFORM());
			m_pPhysicsShell->mDesired.identity();
			m_pPhysicsShell->fDesiredStrength = 0.f;
		} break;
		default : {
			m_pPhysicsShell->set_Kinematics(PKinematics(pVisual));
			AddElement			(0,PKinematics(pVisual)->LL_BoneRoot());
		} break;
	}
	m_pPhysicsShell->mXFORM.set(svTransform);
	m_pPhysicsShell->SetAirResistance(0.002f, 0.3f);
}

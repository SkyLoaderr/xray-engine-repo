///////////////////////////////////////////////////////////////
// PDA.cpp
// PDA - пейджер сталкеров и солдат
///////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "pda.h"
#include "hudmanager.h"
#include "artifact.h"

CPda::CPda(void) 
{
	m_fRadius = 200.0f;
}

CPda::~CPda(void) 
{
}

BOOL CPda::net_Spawn(LPVOID DC) 
{
	BOOL res = inherited::net_Spawn(DC);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");

	if (m_pPhysicsShell==0) 
	{
		// Physics (Box)
		Fobb obb; 
		Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); 
		obb.m_rotate.identity();
		
		// Physics (Elements)
		CPhysicsElement* E = P_create_Element(); 
		R_ASSERT(E); 
		E->add_Box(obb);
		// Physics (Shell)
		m_pPhysicsShell = P_create_Shell(); 
		R_ASSERT(m_pPhysicsShell);
		m_pPhysicsShell->add_Element(E);
		m_pPhysicsShell->setDensity(2000.f);
		
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity();
		m_pPhysicsShell->fDesiredStrength = 0.f;
	}

	setVisible(true);
	setEnabled(true);

	return res;
}

void CPda::Load(LPCSTR section) 
{
	inherited::Load(section);

	//m_fRadius = pSettings->r_float(section,"radius");
	m_fRadius = 10.f;
	
}

void CPda::net_Destroy() 
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	
	inherited::net_Destroy();
}

void CPda::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);

	if(!H_Parent()) return;
	Position().set(H_Parent()->Position());

	
	Fvector	P; 
	P.set(H_Parent()->Position());
	
	//обновить список дос€гаемых PDA
	feel_touch_update(P,m_fRadius);
}

void CPda::UpdateCL() 
{
	inherited::UpdateCL();

	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}

void CPda::feel_touch_new(CObject* O) 
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);

	if(pInvOwner && pInvOwner->GetPDA()) 
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"new PDA detected");
		m_PDAList.push_back(pInvOwner->GetPDA());
	}
}

void CPda::feel_touch_delete(CObject* O) 
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);

	if(pInvOwner && pInvOwner->GetPDA()) 
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"a PDA left radius of sight");
		m_PDAList.erase(std::find(m_PDAList.begin(), 
									m_PDAList.end(), 
									pInvOwner->GetPDA()));
	}
}

BOOL CPda::feel_touch_contact(CObject* O) 
{
	CInventoryOwner* pInvOwner = dynamic_cast<CInventoryOwner*>(O);

	if(pInvOwner && pInvOwner->GetPDA()) 
		return TRUE;
	else
		return FALSE;
}


void CPda::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
	setVisible					(false);
	setEnabled					(false);
}

void CPda::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);

	//NET_Packet			P;
	//u_EventGen			(P,GE_DESTROY,ID());
	//u_EventSend			(P);
}


void CPda::renderable_Render() 
{
	if(getVisible() && !H_Parent()) 
	{
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}

//отправка сообщени€ другому владельцу PDA
void CPda::SendMessage(u32 pda_num, EPDAMsg msg)
{
	//найти InventoryOwner с нужным номером в списке
	u32 i=0;
	for(PDA_LIST_it it = m_PDAList.begin();
		i<=pda_num && it!=m_PDAList.end(); 
		i++, it++){}

	if(it == m_PDAList.end()) return;
	
	CObject* pObject = dynamic_cast<CObject*>(*it);
	pObject;
}

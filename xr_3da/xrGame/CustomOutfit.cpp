///////////////////////////////////////////////////////////////
// CustomOutfit.cpp
// CustomOutfit - защитные костюмы для персонажей
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "customoutfit.h"
#include "PhysicsShell.h"

CCustomOutfit::CCustomOutfit()
{
	m_slot = OUTFIT_SLOT;
}

CCustomOutfit::~CCustomOutfit() 
{
}


BOOL CCustomOutfit::net_Spawn(LPVOID DC) 
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

void CCustomOutfit::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_fOutfitBurn = pSettings->r_float(section,"burn_immunity");
	m_fOutfitStrike = pSettings->r_float(section,"strike_immunity");
	m_fOutfitShock = pSettings->r_float(section,"shock_immunity");
	m_fOutfitWound = pSettings->r_float(section,"wound_immunity");
	m_fOutfitRadiation = pSettings->r_float(section,"radiation_immunity");
	m_fOutfitTelepatic = pSettings->r_float(section,"telepatic_immunity");
}

void CCustomOutfit::net_Destroy() 
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	
	inherited::net_Destroy();
}

void CCustomOutfit::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CCustomOutfit::UpdateCL() 
{
	inherited::UpdateCL();

	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}


void CCustomOutfit::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();

	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CCustomOutfit::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();

	setVisible(true);
	setEnabled(true);
	
	CObject* E = dynamic_cast<CObject*>(H_Parent()); 
	R_ASSERT(E);
	
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);

	if(m_pPhysicsShell) 
	{
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM()); l_p1.c.add(l_up); l_up.mul(1.2f);
		l_p2.set(XFORM()); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
		Position().set(XFORM().c);
	}
}

void CCustomOutfit::renderable_Render() 
{
	if(getVisible() && !H_Parent()) 
	{
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}
#include "stdafx.h"
#include "weaponammo.h"
#include "PhysicsShell.h"

CCartridge::CCartridge() {
	m_ammoSect = NULL;
	m_kDist = m_kDisp = m_kHit = m_kImpulse = m_kPierce = 1.f;
	m_tracer = true;
	m_buckShot = 1;
	m_impair = 1.f;
}

void CCartridge::Load(LPCSTR section) {
	m_ammoSect = section;
	m_kDist = pSettings->r_float(section, "k_dist");
	m_kDisp = pSettings->r_float(section, "k_disp");
	m_kHit = pSettings->r_float(section, "k_hit");
	m_kImpulse = pSettings->r_float(section, "k_impulse");
	m_kPierce = pSettings->r_float(section, "k_pierce");
	m_tracer = !!pSettings->r_bool(section, "tracer");
	m_buckShot = pSettings->r_s32(section, "buck_shot");
	m_impair = pSettings->r_float(section, "impair");
}

CWeaponAmmo::CWeaponAmmo(void) {
	m_weight = .2f;
	m_belt = true;
}

CWeaponAmmo::~CWeaponAmmo(void) {}

void CWeaponAmmo::Load(LPCSTR section) {
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	m_kDist = pSettings->r_float(section, "k_dist");
	m_kDisp = pSettings->r_float(section, "k_disp");
	m_kHit = pSettings->r_float(section, "k_hit");
	m_kImpulse = pSettings->r_float(section, "k_impulse");
	m_kPierce = pSettings->r_float(section, "k_pierce");
	m_tracer = !!pSettings->r_bool(section, "tracer");
	m_buckShot = pSettings->r_s32(section, "buck_shot");
	m_impair = pSettings->r_float(section, "impair");

	m_boxSize = (u16)pSettings->r_s32(section, "box_size");
	m_boxCurr = m_boxSize;
}

BOOL CWeaponAmmo::net_Spawn(LPVOID DC) {
	BOOL bResult = inherited::net_Spawn	(DC);
	CSE_Abstract					*e		= (CSE_Abstract*)(DC);
	CSE_ALifeItemAmmo					*l_pW	= dynamic_cast<CSE_ALifeItemAmmo*>(e);
	m_boxCurr = l_pW->a_elapsed;
	R_ASSERT(m_boxCurr <= m_boxSize);

	setVisible(true);
	setEnabled(true);
	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");

	if (0==m_pPhysicsShell) {
		// Physics (Box)
		Fobb obb; Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); obb.m_rotate.identity();
		// Physics (Elements)
		CPhysicsElement* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
		// Physics (Shell)
		m_pPhysicsShell = P_create_Shell(); R_ASSERT(m_pPhysicsShell);
		m_pPhysicsShell->add_Element(E);
		m_pPhysicsShell->setDensity(2000.f);
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity();
		m_pPhysicsShell->fDesiredStrength = 0.f;
	}

	return bResult;
}

void CWeaponAmmo::net_Destroy() {
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	inherited::net_Destroy();
}

void CWeaponAmmo::OnH_B_Chield() {
	inherited::OnH_B_Chield		();
	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CWeaponAmmo::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	if(!Useful()) {
		NET_Packet P;
		u_EventGen(P,GE_DESTROY,ID());
		u_EventSend(P);
		return;
	}
	setVisible(true);
	setEnabled(true);
	CObject* E = dynamic_cast<CObject*>(H_Parent()); R_ASSERT(E);
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);
	if(m_pPhysicsShell) {
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

const char* CWeaponAmmo::Name() {
	if(m_name) strcpy(m_tmpName, m_name); else m_tmpName[0] = 0;
	char l_tmp[20]; sprintf(l_tmp, " %d/%d", m_boxCurr, m_boxSize);
	strcpy(&m_tmpName[strlen(m_tmpName)], l_tmp);
	return m_tmpName;
}

const char* CWeaponAmmo::NameShort() {
	if(m_nameShort) strcpy(m_tmpName, m_nameShort); else m_tmpName[0] = 0;
	return m_tmpName;
}

bool CWeaponAmmo::Useful() {
	// Если IItem еще не полностью использованый, вернуть true
	return !!m_boxCurr;
}

s32 CWeaponAmmo::Sort(PIItem pIItem) {
	// Если нужно разместить IItem после this - вернуть 1, если
	// перед - -1. Если пофиг то 0.
	CWeaponAmmo *l_pA = dynamic_cast<CWeaponAmmo*>(pIItem);
	if(!l_pA) return 0;
	if(strcmp(cNameSect(), l_pA->cNameSect())) return 0;
	if(m_boxCurr <= l_pA->m_boxCurr) return 1;
	else return -1;
}

bool CWeaponAmmo::Merge(PIItem pIItem) {
	// Если удалось слить вместе вернуть true
	if(m_boxCurr == m_boxSize) return false;
	CWeaponAmmo *l_pA = dynamic_cast<CWeaponAmmo*>(pIItem);
	if(!l_pA) return false;
	if(strcmp(cNameSect(), l_pA->cNameSect())) return false;
	u16 l_free = m_boxSize - m_boxCurr;
	m_boxCurr = m_boxCurr + (l_free < l_pA->m_boxCurr ? l_free : l_pA->m_boxCurr);
	l_pA->m_boxCurr = l_pA->m_boxCurr - (l_free < l_pA->m_boxCurr ? l_free : l_pA->m_boxCurr);
	return true;
}

bool CWeaponAmmo::Get(CCartridge &cartridge) {
	if(!m_boxCurr) return false;
	cartridge.m_ammoSect = cNameSect();
	cartridge.m_kDist = m_kDist;
	cartridge.m_kDisp = m_kDisp;
	cartridge.m_kHit = m_kHit;
	cartridge.m_kImpulse = m_kImpulse;
	cartridge.m_kPierce = m_kPierce;
	cartridge.m_tracer = m_tracer;
	cartridge.m_buckShot = m_buckShot;
	cartridge.m_impair = m_impair;
	m_boxCurr--;
	return true;
}

void CWeaponAmmo::renderable_Render() {
	if(getVisible() && !H_Parent()) {
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}

void CWeaponAmmo::UpdateCL() {
	inherited::UpdateCL();
	if(getVisible() && m_pPhysicsShell) {
		m_pPhysicsShell->Update	();
		XFORM().set			(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}


void CWeaponAmmo::net_Export(NET_Packet& P) {
	//inherited::net_Export(P);
	P.w_u16(m_boxCurr);
}

void CWeaponAmmo::net_Import(NET_Packet& P) {
	inherited::net_Import(P);
	P.r_u16(m_boxCurr);
}


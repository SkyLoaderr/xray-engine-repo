#include "stdafx.h"
#include "weaponammo.h"
#include "PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
#include "Actor_Flags.h"

CCartridge::CCartridge() 
{
	m_ammoSect = NULL;
	m_kDist = m_kDisp = m_kHit = m_kImpulse = m_kPierce = 1.f;
	m_tracer = true;
	m_buckShot = 1;
	m_impair = 1.f;
}

void CCartridge::Load(LPCSTR section) 
{
	m_ammoSect = section;
	m_kDist = pSettings->r_float(section, "k_dist");
	m_kDisp = pSettings->r_float(section, "k_disp");
	m_kHit = pSettings->r_float(section, "k_hit");
	m_kImpulse = pSettings->r_float(section, "k_impulse");
	m_kPierce = pSettings->r_float(section, "k_pierce");
	m_tracer = !!pSettings->r_bool(section, "tracer");
	m_buckShot = pSettings->r_s32(section, "buck_shot");
	m_impair = pSettings->r_float(section, "impair");
	fWallmarkSize = pSettings->r_float(section, "wm_size");
	R_ASSERT(fWallmarkSize>0);
}

CWeaponAmmo::CWeaponAmmo(void) 
{
	m_weight = .2f;
	m_belt = true;
}

CWeaponAmmo::~CWeaponAmmo(void)
{
}

void CWeaponAmmo::Load(LPCSTR section) 
{
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
	fWallmarkSize = pSettings->r_float(section,"wm_size");
	R_ASSERT(fWallmarkSize>0);

	m_boxSize = (u16)pSettings->r_s32(section, "box_size");
	m_boxCurr = m_boxSize;
}

BOOL CWeaponAmmo::net_Spawn(LPVOID DC) 
{
	BOOL bResult = inherited::net_Spawn	(DC);
	CSE_Abstract					*e		= (CSE_Abstract*)(DC);
	CSE_ALifeItemAmmo					*l_pW	= dynamic_cast<CSE_ALifeItemAmmo*>(e);
	m_boxCurr = l_pW->a_elapsed;
	R_ASSERT(m_boxCurr <= m_boxSize);

	return bResult;
}

void CWeaponAmmo::net_Destroy() 
{
	inherited::net_Destroy();
}

void CWeaponAmmo::OnH_B_Chield() 
{
	inherited::OnH_B_Chield		();
}

void CWeaponAmmo::OnH_B_Independent() 
{
	if(!Useful()) {
		NET_Packet		P;
		u_EventGen		(P,GE_DESTROY,ID());
		Msg				("ge_destroy: [%d] - %s",ID(),*cName());
		if (Local())
			u_EventSend	(P);
		m_ready_to_destroy	= true;
	}
	inherited::OnH_B_Independent();
}

const char* CWeaponAmmo::Name() 
{
	if(m_name) strcpy(m_tmpName, m_name); else m_tmpName[0] = 0;
	char l_tmp[20]; sprintf(l_tmp, " %d/%d", m_boxCurr, m_boxSize);
	strcpy(&m_tmpName[xr_strlen(m_tmpName)], l_tmp);
	return m_tmpName;
}

const char* CWeaponAmmo::NameShort() 
{
	if(m_nameShort) strcpy(m_tmpName, m_nameShort); else m_tmpName[0] = 0;
	return m_tmpName;
}

bool CWeaponAmmo::Useful() const
{
	// ���� IItem ��� �� ��������� �������������, ������� true
	return !!m_boxCurr;
}

s32 CWeaponAmmo::Sort(PIItem pIItem) 
{
	// ���� ����� ���������� IItem ����� this - ������� 1, ����
	// ����� - -1. ���� ����� �� 0.
	CWeaponAmmo *l_pA = dynamic_cast<CWeaponAmmo*>(pIItem);
	if(!l_pA) return 0;
	if(xr_strcmp(cNameSect(), l_pA->cNameSect())) return 0;
	if(m_boxCurr <= l_pA->m_boxCurr) return 1;
	else return -1;
}

bool CWeaponAmmo::Merge(PIItem pIItem) 
{
	// ���� ������� ����� ������ ������� true
	if(m_boxCurr == m_boxSize) return false;
	CWeaponAmmo *l_pA = dynamic_cast<CWeaponAmmo*>(pIItem);
	if(!l_pA) return false;
	if(xr_strcmp(cNameSect(), l_pA->cNameSect())) return false;
	u16 l_free = m_boxSize - m_boxCurr;
	m_boxCurr = m_boxCurr + (l_free < l_pA->m_boxCurr ? l_free : l_pA->m_boxCurr);
	l_pA->m_boxCurr = l_pA->m_boxCurr - (l_free < l_pA->m_boxCurr ? l_free : l_pA->m_boxCurr);
	return true;
}

bool CWeaponAmmo::Get(CCartridge &cartridge) 
{
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
	cartridge.fWallmarkSize = fWallmarkSize;
	if (!psActorFlags.test(AF_UNLIMITEDAMMO))
		--m_boxCurr;
	return true;
}

void CWeaponAmmo::renderable_Render() 
{
	if(!m_ready_to_destroy)
		inherited::renderable_Render();
}

void CWeaponAmmo::UpdateCL() 
{
	inherited::UpdateCL	();
	make_Interpolation	();
}

void CWeaponAmmo::net_Export(NET_Packet& P) 
{
	inherited::net_Export(P);
	
	P.w_u16(m_boxCurr);
}

void CWeaponAmmo::net_Import(NET_Packet& P) 
{
	inherited::net_Import(P);

	P.r_u16(m_boxCurr);
}
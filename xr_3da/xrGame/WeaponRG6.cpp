#include "stdafx.h"
#include "WeaponRG6.h"
#include "entity.h"
#include "explosiveRocket.h"
#include "level.h"

CWeaponRG6::~CWeaponRG6()
{
}

void CWeaponRG6::Load(LPCSTR section)
{
	inheritedRL::Load(section);
	inheritedSG::Load(section);
}

void CWeaponRG6::FireStart ()
{

	if(STATE == eIdle	&& getRocketCount() ) 
	{
		inheritedSG::FireStart ();
	
		Fvector p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);

		CEntity* E = smart_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams (this, p1,d);

		Fmatrix launch_matrix;
		launch_matrix.identity();
		launch_matrix.k.set(d);
		Fvector::generate_orthonormal_basis(launch_matrix.k,
											launch_matrix.j, launch_matrix.i);
		launch_matrix.c.set(p1);

		d.normalize();
		d.mul(m_fLaunchSpeed);

		CRocketLauncher::LaunchRocket(launch_matrix, d, zero_vel);

		CExplosiveRocket* pGrenade = smart_cast<CExplosiveRocket*>(getCurrentRocket());
		VERIFY(pGrenade);
		pGrenade->SetCurrentParentID(H_Parent()->ID());

		if (OnServer())
		{
			NET_Packet P;
			u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
			P.w_u16(u16(/*m_pRocket->ID()*/getCurrentRocket()->ID()));
			u_EventSend(P);
		}
		dropCurrentRocket();
	}
}

u8 CWeaponRG6::AddCartridge		(u8 cnt)
{
	u8 t = inheritedSG::AddCartridge(cnt);
	u8 k = cnt-t;
	shared_str fake_grenade_name = pSettings->r_string(*m_pAmmo->cNameSect(), "fake_grenade_name");
	while(k){
		--k;
		inheritedRL::SpawnRocket(*fake_grenade_name, this);
	}
	return k;
}

void CWeaponRG6::OnEvent(NET_Packet& P, u16 type) 
{
	inheritedSG::OnEvent(P,type);

	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			inheritedRL::AttachRocket(id, this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			P.r_u16(id);
			inheritedRL::DetachRocket(id);
		} break;
	}
}


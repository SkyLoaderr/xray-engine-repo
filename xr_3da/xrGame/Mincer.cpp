#include "stdafx.h"
#include "mincer.h"
#include "hudmanager.h"
#include "xrmessages.h"
#include "level.h"
#include "CustomZone.h"
CMincer::CMincer(void) 
{
}

CMincer::~CMincer(void) 
{
}
void CMincer::SwitchZoneState(EZoneState new_state)
{
	if(new_state==eZoneStateBlowout)
	{
		xr_set<CObject*>::iterator it=m_inZone.begin(),e=m_inZone.end();
		for(;e!=it;++it)
		{
			CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>(*it);
			Telekinesis().activate(GO, 0.1f, m_fTeleHeight, 100000);

		}
	}

	if(m_eZoneState==eZoneStateBlowout && new_state!=eZoneStateBlowout)
	{
		Telekinesis().deactivate();
	}
	inherited::SwitchZoneState(new_state);
}
void CMincer::Load (LPCSTR section)
{
	inherited::Load(section);
	//pSettings->r_fvector3(section,whirlwind_center);
}

BOOL CMincer::net_Spawn(LPVOID DC)
{
	BOOL result=inherited::net_Spawn(DC);
	Fvector C;
	Center(C);
	C.y+=5.f;
	m_telekinetics.SetCenter(C);
	return result;
}
#ifdef DEBUG
void CMincer::OnRender()
{
	Fmatrix M;M.identity();M.scale(0.2f,0.2f,0.2f);
	//Center(M.c);
	if(!m_telekinetics.is_active()) return;
	M.c.set(m_telekinetics.Center());
	RCache.dbg_DrawEllipse(M, 0xffffffff);
}
#endif
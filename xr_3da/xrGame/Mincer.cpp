#include "stdafx.h"
#include "mincer.h"
#include "hudmanager.h"
#include "xrmessages.h"
#include "level.h"
#include "CustomZone.h"
#include "PHDestroyable.h"
CMincer::CMincer(void) 
{
}

CMincer::~CMincer(void) 
{
}
void CMincer::SwitchZoneState(EZoneState new_state)
{
	if(m_eZoneState!=eZoneStateBlowout && new_state==eZoneStateBlowout)
	{
		xr_set<CObject*>::iterator it=m_inZone.begin(),e=m_inZone.end();
		for(;e!=it;++it)
		{
			CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>(*it);
			Telekinesis().activate(GO,m_fThrowInImpulse, m_fTeleHeight, 100000);

		}
	}

	if(m_eZoneState==eZoneStateBlowout && new_state!=eZoneStateBlowout)
	{
		Telekinesis().clear_deactivate();
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
	C.y+=m_fTeleHeight;
	m_telekinetics.SetCenter(C);
	m_telekinetics.SetOwnerObject(smart_cast<CGameObject*>(this));
	return result;
}
void CMincer::net_Destroy()
{
	inherited::net_Destroy();
	m_telekinetics.clear_impacts();
}
void CMincer::feel_touch_new				(CObject* O)
{
	inherited::feel_touch_new(O);
	if(m_eZoneState==eZoneStateBlowout && 
			(
			m_dwBlowoutExplosionTime>(u32)m_iStateTime
				)
		)
	{
		CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>(O);
		Telekinesis().activate(GO, m_fThrowInImpulse, m_fTeleHeight, 100000);
	}
}

void CMincer:: AffectThrow	(CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist)
{
	inherited::AffectThrow(GO,throw_in_dir,dist);
}

bool CMincer::BlowoutState	()
{
	bool ret=inherited::BlowoutState	();
	if(m_dwBlowoutExplosionTime<(u32)m_iPreviousStateTime ||
		m_dwBlowoutExplosionTime>=(u32)m_iStateTime) return ret;
	Telekinesis().deactivate();
	return ret;
}
void CMincer ::ThrowInCenter(Fvector& C)
{
	C.set(m_telekinetics.Center());
}


void CMincer ::Center	(Fvector& C) const
{
	C.set(Position());
}

void CMincer ::OnEvent(NET_Packet& P,u16 type)
{

	switch(type)
	{

	case GE_OWNERSHIP_TAKE:
		u16 id=P.r_u16();
		Fvector dir;float impulse;
		m_telekinetics.draw_out_impact(dir,impulse);
		CObject* obj=Level().Objects.net_Find(id);
		if(obj->SUB_CLS_ID ==CLSID_ARTEFACT) break;
		if (OnServer())
		{
			NET_Packet	l_P;
			u_EventGen	(l_P,GE_HIT, id);
			l_P.w_u16	(u16(id));
			l_P.w_u16	(ID());
			l_P.w_dir	(dir);
			l_P.w_float	(0.f);
			l_P.w_s16	(0/*(s16)BI_NONE*/);
			Fvector		position_in_bone_space={0.f,0.f,0.f};
			l_P.w_vec3	(position_in_bone_space);
			l_P.w_float	(impulse);
			l_P.w_u16	(ALife::eHitTypeStrike);
			u_EventSend	(l_P);
	/////////////////////////////////////////////////////////
			obj->H_SetParent(NULL);
			return;
		};
	}
		inherited::OnEvent(P,type);
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
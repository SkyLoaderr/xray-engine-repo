#include "stdafx.h"
#include "PHDestroyable.h"
#include "mincer.h"
#include "hudmanager.h"
#include "xrmessages.h"
#include "level.h"
#include "CustomZone.h"
#include "PHDestroyable.h"
#include "clsid_game.h"
#include "entity_alive.h"
#include "PHDestroyableNotificate.h"

CMincer::CMincer(void) 
{
	m_fActorBlowoutRadiusPercent=0.5f;
}

CMincer::~CMincer(void) 
{
}
void CMincer::OnStateSwitch(EZoneState new_state)
{
	if(m_eZoneState!=eZoneStateBlowout && new_state==eZoneStateBlowout)
	{
		OBJECT_INFO_VEC_IT it;
		for(it = m_ObjectInfoMap.begin(); m_ObjectInfoMap.end() != it; ++it) 
		{
			CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>((*it).object);
			if (GO)					Telekinesis().activate(GO,m_fThrowInImpulse, m_fTeleHeight, 100000);
		}
	}

	if(m_eZoneState==eZoneStateBlowout && new_state!=eZoneStateBlowout)
	{
		Telekinesis().clear_deactivate();
	}
	inherited::OnStateSwitch(new_state);
}

void CMincer::Load (LPCSTR section)
{
	inherited::Load(section);
	
	m_telekinetics.set_destroing_particles(shared_str(pSettings->r_string(section,"tearing_particles")));
	m_telekinetics.set_throw_power(pSettings->r_float(section,"throw_out_impulse"));
	m_torn_particles=pSettings->r_string(section,"torn_particles");
	m_tearing_sound.create(TRUE,pSettings->r_string(section,"body_tearing_sound"));
	m_fActorBlowoutRadiusPercent=pSettings->r_float(section,"actor_blowout_radius_percent");

	//pSettings->r_fvector3(section,whirlwind_center);
}

BOOL CMincer::net_Spawn(CSE_Abstract* DC)
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
	if( m_eZoneState==eZoneStateBlowout && (m_dwBlowoutExplosionTime>(u32)m_iStateTime) )
	{
		CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>(O);
		Telekinesis().activate(GO, m_fThrowInImpulse, m_fTeleHeight, 100000);
	}
}
BOOL	CMincer::feel_touch_contact				(CObject* O)
{
	return inherited::feel_touch_contact(O)&&smart_cast<CPhysicsShellHolder *>(O);
}
void CMincer:: AffectThrow	(SZoneObjectInfo* O, CPhysicsShellHolder* GO,const Fvector& throw_in_dir,float dist)
{
	inherited::AffectThrow(O,GO,throw_in_dir,dist);
}

bool CMincer::BlowoutState	()
{
	bool ret=inherited::BlowoutState	();

	//xr_set<CObject*>::iterator it=m_inZone.begin(),e=m_inZone.end();
	//for(;e!=it;++it)
	//{
	//	CEntityAlive * EA = smart_cast<CEntityAlive *>(*it);
	//	if(!EA)continue;
	//	CPhysicsShellHolder * GO = smart_cast<CPhysicsShellHolder *>(*it);
	//	Telekinesis().activate(GO,m_fThrowInImpulse, m_fTeleHeight, 100000);

	//}

	if(m_dwBlowoutExplosionTime<(u32)m_iPreviousStateTime ||
		m_dwBlowoutExplosionTime>=(u32)m_iStateTime) return ret;
	Telekinesis().deactivate();
	return ret;
}
void CMincer ::ThrowInCenter(Fvector& C)
{
	C.set(m_telekinetics.Center());
	C.y=Position().y;
}


void CMincer ::Center	(Fvector& C) const
{
	C.set(Position());
}


void CMincer::OnOwnershipTake(u16 id)
{
	
	//if(obj->CLS_ID ==CLSID_ARTEFACT)
	{
		inherited::OnOwnershipTake(id);
		return;
	}


}
void CMincer::NotificateDestroy			(CPHDestroyableNotificate *dn)
{
	Fvector dir;
	float impulse;
	//if(!m_telekinetics.has_impacts()) return;

	//CObject* obj=Level().Objects.net_Find(id);
	CPhysicsShellHolder* obj=dn->PPhysicsShellHolder();
	m_telekinetics.draw_out_impact(dir,impulse);
	CParticlesPlayer* PP = smart_cast<CParticlesPlayer*>(obj);
	if(PP && *m_torn_particles)
	{
		PP->StartParticles(m_torn_particles,Fvector().set(0,1,0),ID());
	}
	m_tearing_sound.play_at_pos(this,m_telekinetics.Center());
	if (OnServer())
	{
		NET_Packet	l_P;
		u_EventGen	(l_P,GE_HIT, obj->ID());
		l_P.w_u16	(ID());
		l_P.w_u16	(ID());
		l_P.w_dir	(Fvector().set(1.f,0.f,1.f));//dir
		l_P.w_float	(0.f);
		l_P.w_s16	(0/*(s16)BI_NONE*/);
		Fvector		position_in_bone_space={0.f,0.f,0.f};
		l_P.w_vec3	(position_in_bone_space);
		l_P.w_float	(impulse);
		l_P.w_u16	(ALife::eHitTypeExplosion);
		u_EventSend	(l_P);
		/////////////////////////////////////////////////////////
	//	obj->H_SetParent(NULL);
		return;
	};
}
void CMincer::AffectPullAlife(CEntityAlive* EA,const Fvector& throw_in_dir,float dist)
{
	float power=Power(dist);
	//Fvector dir;
	//dir.random_dir(throw_in_dir,2.f*M_PI);
	if(EA->CLS_ID!=CLSID_OBJECT_ACTOR)
	{
		
		if (OnServer())
		{
			NET_Packet	l_P;
			u_EventGen	(l_P,GE_HIT, EA->ID());
			l_P.w_u16	(ID());
			l_P.w_u16	(ID());
			l_P.w_dir	(throw_in_dir);
			l_P.w_float	(power);
			l_P.w_s16	(0/*(s16)BI_NONE*/);
			l_P.w_vec3	(Fvector().set(0,0,0));
			l_P.w_float	(0);
			l_P.w_u16	((u16)m_eHitTypeBlowout);
			u_EventSend	(l_P);
		}
	}
	inherited::AffectPullAlife(EA,throw_in_dir,dist);

}

bool CMincer::CheckAffectField(CPhysicsShellHolder* GO,float dist_to_radius)
{
	if(GO->CLS_ID!=CLSID_OBJECT_ACTOR)
		return inherited::CheckAffectField(GO,dist_to_radius);
	else
	{
		return dist_to_radius>m_fActorBlowoutRadiusPercent;
	}
}

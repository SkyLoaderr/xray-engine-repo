#include "stdafx.h"
#include "phantom.h"
#include "../../NET_utils.h"
#include "../../level.h"
#include "../../xrServer_Objects_ALife_Monsters.h"

CPhantom::CPhantom()
{
	fDHeading			= 0;
	fSpeed				= 4.f;
	fASpeed				= 1.7f;
	vHPB.set			(0,0,0);
	fGoalChangeTime		= 0.f;
	fGoalChangeDelta	= 4.f;
	vGoalDir.set		(0,0,0);
	vVarGoal.set		(0,0,0);
	vCurrentDir.set		(0,0,1);
}

CPhantom::~CPhantom()
{
}

//---------------------------------------------------------------------
void CPhantom::Load( LPCSTR section )
{
	inherited::Load		(section);
	//////////////////////////////////////////////////////////////////////////
	ISpatial* self		= smart_cast<ISpatial*> (this);
	if (self) {
		self->spatial.type &=~STYPE_VISIBLEFORAI;
		self->spatial.type &=~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

	LPCSTR snd_name			= 0;
	m_state_data[stBirth].particles	= pSettings->r_string(section,"particles_birth");
	snd_name						= pSettings->r_string(section,"sound_birth");
	if (snd_name&&snd_name[0])		m_state_data[stBirth].sound.create(TRUE,snd_name);

	m_state_data[stFly].particles	= pSettings->r_string(section,"particles_fly");
	snd_name						= pSettings->r_string(section,"sound_fly");
	if (snd_name&&snd_name[0])		m_state_data[stFly].sound.create(TRUE,snd_name);

	m_state_data[stDeath].particles	= pSettings->r_string(section,"particles_death");
	snd_name						= pSettings->r_string(section,"sound_death");
	if (snd_name&&snd_name[0])		m_state_data[stDeath].sound.create(TRUE,snd_name);
}
BOOL CPhantom::net_Spawn(CSE_Abstract* DC)
{
	CSE_ALifeCreaturePhantom*	OBJ	= smart_cast<CSE_ALifeCreaturePhantom*>(DC); VERIFY(OBJ);
	
	// select visual at first
	LPCSTR visuals	= pSettings->r_string(cNameSect(),"visuals");
	u32 cnt			= _GetItemCount(visuals);
	string256 tmp;
	OBJ->set_visual	(_GetItem(visuals,Random.randI(cnt),tmp));

	if (!inherited::net_Spawn(DC)) return FALSE;
	
	setVisible		(TRUE);
	setEnabled		(TRUE);

	m_enemy			= Level().CurrentEntity();
	VERIFY			(m_enemy);

	m_State			= stIdle;
	m_fly_particles	= 0;
	fEntityHealth	= 0.001f;

	XFORM().k.sub	(m_enemy->Position(),Position()).normalize();
	XFORM().j.set	(0,1,0);
	XFORM().i.crossproduct	(XFORM().j,XFORM().k);

	XFORM().getHPB	(vHPB);

	SwitchToState	(stBirth);

//. temp
	// set animation
	CSkeletonAnimated *skeleton_animated = smart_cast<CSkeletonAnimated*>(Visual());
	skeleton_animated->PlayCycle("fly_0");

	return	TRUE;
}
void CPhantom::net_Destroy	()
{
	inherited::net_Destroy	();

}
//---------------------------------------------------------------------
void CPhantom::SwitchToState(EState new_state)
{
	if (new_state!=m_State){
		if (m_fly_particles)CParticlesObject::Destroy(m_fly_particles);
		switch (new_state){
		case stBirth:	
			UpdateEvent.bind(this,&CPhantom::OnBirthState);	
			PlayParticles	(m_state_data[stBirth].particles.c_str(),TRUE);
		break;
		case stFly:		
			UpdateEvent.bind(this,&CPhantom::OnFlyState);	
			m_fly_particles	= PlayParticles(m_state_data[stFly].particles.c_str(),FALSE);
		break;
		case stDeath:	
			UpdateEvent.bind(this,&CPhantom::OnDeathState);	
			PlayParticles	(m_state_data[stDeath].particles.c_str(),TRUE);
		break;
		}
		m_State				= new_state;
	}
}
void CPhantom::OnBirthState	()
{
	SwitchToState			(stFly);
}
void CPhantom::OnFlyState()
{
	if (g_Alive()){
		Fvector vE,vP;
		m_enemy->Center		(vE);
		Center				(vP);
		if (vP.distance_to_sqr(vE)<_sqr(Radius())){
			// hit enemy
			PsyHit			(m_enemy,1);
			// destroy
			Hit				(1000.f,Fvector().set(0,0,1),this,u16(-1),Fvector().set(0,0,0),0.f,ALife::eHitTypeFireWound);
		}else{
			UpdatePosition	(m_enemy->Position());
			// update particles
			if (m_fly_particles){		
				Fvector		vel;
				vel.sub		(m_enemy->Position(),Position()).normalize_safe().mul(fSpeed);
				Fmatrix	xform		= XFORM_center();
				m_fly_particles->UpdateParent(xform,vel);
			}
		}
	}
}
void CPhantom::OnDeathState()
{
	DestroyObject	();
	SwitchToState	(stIdle);
}
//---------------------------------------------------------------------


void CPhantom::shedule_Update(u32 DT)
{
	spatial.type &=~STYPE_VISIBLEFORAI;

	inherited::shedule_Update(DT);
}

void CPhantom::UpdateCL()
{
	inherited::UpdateCL();

	if (!UpdateEvent.empty()) UpdateEvent();
}
//---------------------------------------------------------------------
void CPhantom::Hit	(float P, Fvector &dir, CObject* who, s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	fEntityHealth	= -1.f;
	inherited::Hit	(P,dir,who,element,p_in_object_space,impulse/100.f, hit_type);
}
//---------------------------------------------------------------------
void CPhantom::Die	(CObject* who)
{
	inherited::Die	(who);
	SwitchToState	(stDeath);
};

Fmatrix	CPhantom::XFORM_center()
{
	Fvector			center;
	Center			(center);
	Fmatrix	xform	= XFORM();
	return			xform.translate_over(center);
}

CParticlesObject* CPhantom::PlayParticles(const shared_str& name, BOOL bAutoRemove)
{
	CParticlesObject* ps = xr_new<CParticlesObject>(name.c_str(),bAutoRemove);
	Fmatrix	xform		= XFORM_center();
	ps->UpdateParent	(xform, zero_vel);
	ps->Play			();
	return bAutoRemove?0:ps;
}

//---------------------------------------------------------------------
void CPhantom::UpdatePosition(const Fvector& tgt_pos) 
{
	Fvector& vDirection	= XFORM().k;
	float			tgt_h,tgt_p;
	Fvector			tgt_dir,cur_dir;
	tgt_dir.sub		(tgt_pos,Position());
	tgt_dir.getHP	(tgt_h,tgt_p);

	angle_lerp		(vHPB.x,tgt_h,fASpeed,Device.fTimeDelta);
	angle_lerp		(vHPB.y,tgt_p,fASpeed,Device.fTimeDelta);

	cur_dir.setHP	(vHPB.x,vHPB.y);

	Fvector prev_pos=Position();
	XFORM().setHPB	(vHPB.x,0,0);
	Position().mad	(prev_pos,cur_dir,fSpeed*Device.fTimeDelta);
}

void CPhantom::PsyHit(const CObject *object, float value) 
{
	NET_Packet		P;

	u_EventGen		(P,GE_HIT, object->ID());				// 
	P.w_u16			(ID());									// own
	P.w_u16			(ID());									// own
	P.w_dir			(Fvector().set(0.f,1.f,0.f));			// direction
	P.w_float		(value);								// hit value	
	P.w_s16			(BI_NONE);								// bone
	P.w_vec3		(Fvector().set(0.f,0.f,0.f));			
	P.w_float		(0.f);									
	P.w_u16			(u16(ALife::eHitTypeTelepatic));
	u_EventSend		(P);
}

//---------------------------------------------------------------------
// Core events
void CPhantom::net_Export	(NET_Packet& P)					// export to server
{
	// export 
	R_ASSERT			(Local());

	u8					flags = 0;
	P.w_float_q16		(g_Health(),-500,1000);

	P.w_float			(0);
	P.w_u32				(0);
	P.w_u32				(0);

	P.w_u32				(Device.dwTimeGlobal);
	P.w_u8				(flags);

	float				yaw, pitch, bank;
	XFORM().getHPB		(yaw,pitch,bank);
	P.w_angle8			(yaw);
	P.w_angle8			(yaw);
	P.w_angle8			(pitch);
	P.w_angle8			(0);
	P.w_u8				(u8(g_Team()));
	P.w_u8				(u8(g_Squad()));
	P.w_u8				(u8(g_Group()));
}

void CPhantom::net_Import	(NET_Packet& P)
{
	// import
	R_ASSERT			(Remote());

	u8					flags;

	float health;
	P.r_float_q16		(health,-500,1000);
	fEntityHealth		= health;

	float fDummy;
	u32 dwDummy;
	P.r_float			(fDummy);
	P.r_u32				(dwDummy);
	P.r_u32				(dwDummy);

	P.r_u32				(dwDummy);
	P.r_u8				(flags);

	float				yaw, pitch, bank = 0, roll = 0;

	P.r_angle8			(yaw);
	P.r_angle8			(yaw);
	P.r_angle8			(pitch);
	P.r_angle8			(roll);

	id_Team				= P.r_u8();
	id_Squad			= P.r_u8();
	id_Group			= P.r_u8();

	XFORM().setHPB		(yaw,pitch,bank);
}


#include "stdafx.h"
#include "phantom.h"
#include "../../NET_utils.h"
#include "../../level.h"
#include "../../xrServer_Objects_ALife_Monsters.h"
#include "../../../motion.h"

CPhantom::CPhantom()
{
	fSpeed				= 4.f;
	fASpeed				= 1.7f;
	vHP.set				(0,0);
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

	m_state_data[stAttack].particles= pSettings->r_string(section,"particles_attack");
	snd_name						= pSettings->r_string(section,"sound_attack");
	if (snd_name&&snd_name[0])		m_state_data[stAttack].sound.create(TRUE,snd_name);

	m_state_data[stDeath].particles	= pSettings->r_string(section,"particles_death");
	snd_name						= pSettings->r_string(section,"sound_death");
	if (snd_name&&snd_name[0])		m_state_data[stDeath].sound.create(TRUE,snd_name);
}
BOOL CPhantom::net_Spawn(CSE_Abstract* DC)
{
	CSE_ALifeCreaturePhantom*	OBJ	= smart_cast<CSE_ALifeCreaturePhantom*>(DC); VERIFY(OBJ);
	
	// select visual at first
	LPCSTR vis_name = OBJ->get_visual();
	if (!(vis_name&&vis_name[0])){
		LPCSTR visuals	= pSettings->r_string(cNameSect(),"visuals");
		u32 cnt			= _GetItemCount(visuals);
		string256 tmp;
		OBJ->set_visual	(_GetItem(visuals,Random.randI(cnt),tmp));
		// inform server
		NET_Packet		P;
		u_EventGen		(P, GE_CHANGE_VISUAL, OBJ->ID);
		P.w_stringZ		(tmp);
		u_EventSend		(P);
	}

	m_State			= stBirth; // initial state (changed on load method in inherited::)

	// inherited
	if (!inherited::net_Spawn(DC)) return FALSE;
	
	m_enemy			= Level().CurrentEntity();
	VERIFY			(m_enemy);

	// default init 
	m_fly_particles	= 0;
	fEntityHealth	= 0.001f;

	// orientate to enemy
	XFORM().k.sub	(m_enemy->Position(),Position()).normalize();
	XFORM().j.set	(0,1,0);
	XFORM().i.crossproduct	(XFORM().j,XFORM().k);
	XFORM().k.getHP	(vHP.x,vHP.y);

	// set animation
	CSkeletonAnimated *K			= smart_cast<CSkeletonAnimated*>(Visual());
	m_state_data[stBirth].motion	= K->ID_Cycle("birth_0");	
	m_state_data[stFly].motion		= K->ID_Cycle("fly_0");
	m_state_data[stAttack].motion	= K->ID_Cycle("death_0");	
	m_state_data[stDeath].motion	= K->ID_Cycle("death_0");	

	VERIFY(K->LL_GetMotionDef(m_state_data[stBirth].motion)->flags&esmStopAtEnd);
	VERIFY(K->LL_GetMotionDef(m_state_data[stAttack].motion)->flags&esmStopAtEnd);
	VERIFY(K->LL_GetMotionDef(m_state_data[stDeath].motion)->flags&esmStopAtEnd);

	// set state
	EState new_state= m_State;
	m_State			= stInvalid;
	SwitchToState	(new_state);

	setVisible		(m_State>stIdle?TRUE:FALSE);
	setEnabled		(TRUE);

	return	TRUE;
}
void CPhantom::net_Destroy	()
{
	inherited::net_Destroy	();

}

//---------------------------------------------------------------------
// Animation Callbacks
void __stdcall CPhantom::animation_end_callback(CBlend* B)
{
	CPhantom *phantom				= (CPhantom*)B->CallbackParam;
	switch (phantom->m_State){
	case stBirth: phantom->SwitchToState(stFly);	break;
	case stAttack:phantom->SwitchToState(stDeath);	break;
	case stDeath: phantom->SwitchToState(stIdle);	break;
	}
}
//---------------------------------------------------------------------
void CPhantom::SwitchToState(EState new_state)
{
	if (new_state!=m_State){
		CSkeletonAnimated *K	= smart_cast<CSkeletonAnimated*>(Visual());
		Fmatrix	xform			= XFORM_center	();
		UpdateEvent				= 0;
		// after event
		switch (m_State){
		case stBirth:		break;
		case stFly:{
			// stop fly effects
			CParticlesObject::Destroy		(m_fly_particles);
			m_state_data[stFly].sound.stop	();
				   }break;
		case stAttack:		break;
		case stDeath:		break;
		case stIdle:		break;
		}
		// before event
		switch (new_state){
		case stBirth:{
			SStateData& sdata	= m_state_data[new_state];
			PlayParticles		(sdata.particles.c_str(),TRUE,xform);
			sdata.sound.play_at_pos(0,xform.c);
			K->PlayCycle		(sdata.motion, TRUE, animation_end_callback, this);
		}break;
		case stFly:{
			SStateData& sdata	= m_state_data[new_state];
			UpdateEvent.bind	(this,&CPhantom::OnFlyState);	
			m_fly_particles		= PlayParticles(sdata.particles.c_str(),FALSE,xform);
			sdata.sound.play_at_pos(0,xform.c,sm_Looped);
			K->PlayCycle		(sdata.motion);
		}break;
		case stAttack:{
			SStateData& sdata	= m_state_data[new_state];
			PlayParticles		(sdata.particles.c_str(),FALSE,xform);
			sdata.sound.play_at_pos(0,xform.c);
			K->PlayCycle		(sdata.motion, TRUE, animation_end_callback, this);
		}break;
		case stDeath:{
			SStateData& sdata	= m_state_data[new_state];
			PlayParticles		(sdata.particles.c_str(),TRUE,xform);
			sdata.sound.play_at_pos_unlimited(0,xform.c);
			K->PlayCycle		(sdata.motion, TRUE, animation_end_callback, this);
		}break;
		case stIdle:{
			VERIFY(0==m_fly_particles && 0==m_state_data[stFly].sound.feedback);
			DestroyObject	();
		}break;
		}
		m_State				= new_state;
	}
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
			SwitchToState	(stAttack);
		}else{
			UpdatePosition	(m_enemy->Position());
			Fmatrix	xform	= XFORM_center();
			// update particles
			if (m_fly_particles){		
				Fvector		vel;
				vel.sub		(m_enemy->Position(),Position()).normalize_safe().mul(fSpeed);
				m_fly_particles->UpdateParent(xform,vel);
			}
			// update sound
			if (m_state_data[stFly].sound.feedback) m_state_data[stFly].sound.set_position(xform.c);
		}
	}
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

CParticlesObject* CPhantom::PlayParticles(const shared_str& name, BOOL bAutoRemove, const Fmatrix& xform)
{
	CParticlesObject* ps = xr_new<CParticlesObject>(name.c_str(),bAutoRemove);
	ps->UpdateParent	(xform, zero_vel);
	ps->Play			();
	return bAutoRemove?0:ps;
}

//---------------------------------------------------------------------
void CPhantom::UpdatePosition(const Fvector& tgt_pos) 
{
	float			tgt_h,tgt_p;
	Fvector			tgt_dir,cur_dir;
	tgt_dir.sub		(tgt_pos,Position());
	tgt_dir.getHP	(tgt_h,tgt_p);

	angle_lerp		(vHP.x,tgt_h,fASpeed,Device.fTimeDelta);
	angle_lerp		(vHP.y,tgt_p,fASpeed,Device.fTimeDelta);

	cur_dir.setHP	(vHP.x,vHP.y);

	Fvector prev_pos=Position();
	XFORM().rotateY (-vHP.x);
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
void CPhantom::save(NET_Packet &output_packet)
{
	output_packet.w_s32	(s32(m_State));
}
void CPhantom::load(IReader &input_packet)
{
	m_State				= EState(input_packet.r_s32());
}
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


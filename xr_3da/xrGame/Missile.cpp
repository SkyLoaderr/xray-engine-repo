#include "stdafx.h"
#include "missile.h"
#include "WeaponHUD.h"
#include "PhysicsShell.h"
#include "actor.h"
#include "../CameraBase.h"
#include "xrserver_objects_alife.h"
#include "ActorEffector.h"
#include "level.h"
#include "xr_level_controller.h"

#define PLAYING_ANIM_TIME 10000

CMissile::CMissile(void) 
{
	m_offset.identity	();
	///////////////////////////////////
	m_throw_direction.set(0.0f, 1.0f, 0.0f);
	m_throw_matrix.identity();
	///////////////////////////////////
}

CMissile::~CMissile(void) 
{
	HUD_SOUND::DestroySound(sndPlaying);
}

void CMissile::reinit		()
{
	inherited::reinit	();
	m_state				= MS_HIDDEN;
	m_throw				= false;
	m_fThrowForce		= 0;
	m_dwDestroyTime		= 0xffffffff;
	m_bPending			= false;
	m_fake_missile		= NULL;
	hud_mode			= FALSE;
}

void CMissile::Load(LPCSTR section) 
{
	inherited::Load		(section);

	m_fMinForce			= pSettings->r_float(section,"force_min");
	m_fMaxForce			= pSettings->r_float(section,"force_max");
	m_fForceGrowSpeed	= pSettings->r_float(section,"force_grow_speed");

	m_dwDestroyTimeMax	= pSettings->r_u32(section,"destroy_time");
	
	Fvector	position_offset, angle_offset;
	position_offset		= pSettings->r_fvector3(section,"position_offset");
	angle_offset		= pSettings->r_fvector3(section,"angle_offset");
	
	m_offset.setHPB			(VPUSH(angle_offset));
	m_offset.translate_over	(position_offset);

	m_vThrowPoint		= pSettings->r_fvector3(section,"throw_point");
	m_vThrowDir			= pSettings->r_fvector3(section,"throw_dir");
	m_vHudThrowPoint	= pSettings->r_fvector3(*hud_sect,"throw_point");
	m_vHudThrowDir		= pSettings->r_fvector3(*hud_sect,"throw_dir");

	//загрузить анимации HUD-а
	m_sAnimShow			= pSettings->r_string(*hud_sect, "anim_show");
	m_sAnimHide			= pSettings->r_string(*hud_sect, "anim_hide");
	m_sAnimIdle			= pSettings->r_string(*hud_sect, "anim_idle");
	m_sAnimPlaying		= pSettings->r_string(*hud_sect, "anim_playing");
	m_sAnimThrowBegin	= pSettings->r_string(*hud_sect, "anim_throw_begin");
	m_sAnimThrowIdle	= pSettings->r_string(*hud_sect, "anim_throw_idle");
	m_sAnimThrowAct		= pSettings->r_string(*hud_sect, "anim_throw_act");
	m_sAnimThrowEnd		= pSettings->r_string(*hud_sect, "anim_throw_end");

	if(pSettings->line_exist(section,"snd_playing"))
		HUD_SOUND::LoadSound(section,"snd_playing",sndPlaying,TRUE);

	m_animation_slot	= pSettings->r_u32(section,"animation_slot");
}

BOOL CMissile::net_Spawn(LPVOID DC) 
{
	BOOL l_res = inherited::net_Spawn(DC);

	CSE_Abstract						*abstract = (CSE_Abstract*)DC;
	if (abstract->ID_Parent==0xffff) {
		m_pPhysicsShell->Activate	(XFORM(),0,XFORM(),true);

	}

	dwXF_Frame					= 0xffffffff;

	///////////////////////////////////
	m_throw_direction.set(0.0f, 1.0f, 0.0f);
	m_throw_matrix.identity();
	///////////////////////////////////

	return l_res;
}

void CMissile::net_Destroy() 
{
	inherited::net_Destroy();
	m_fake_missile = 0;
}

void CMissile::OnActiveItem		()
{
	inherited::OnActiveItem();
	m_state	= MS_IDLE;
	if (m_pHUD) m_pHUD->Show();
}
void CMissile::OnHiddenItem		()
{
	inherited::OnHiddenItem();
	if (m_pHUD) m_pHUD->Hide();
	m_state = MS_HIDDEN;
}


void CMissile::OnH_B_Chield() 
{
	inherited::OnH_B_Chield		();
}

void CMissile::spawn_fake_missile()
{
	if (OnClient()) return;
//#ifdef _DEBUG
//	//Msg					("%6d : Spawning fake missile for object %s",Level().timeServer(),*cName());
//#endif
//	CSE_Abstract		*D	= F_entity_Create(*cNameSect());
//	R_ASSERT			(D);
//	CSE_ALifeDynamicObject				*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
//	R_ASSERT							(l_tpALifeDynamicObject);
//	l_tpALifeDynamicObject->m_tNodeID	= level_vertex_id();
//	// Fill
//	strcpy				(D->s_name,*cNameSect());
//	strcpy				(D->s_name_replace,"");
//	D->s_gameid			=	u8(GameID());
//	D->s_RP				=	0xff;
//	D->ID				=	0xffff;
//	D->ID_Parent		=	(u16)ID();
//	D->ID_Phantom		=	0xffff;
//	D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
//	D->RespawnTime		=	0;
//	// Send
//	NET_Packet			P;
//	D->Spawn_Write		(P,TRUE);
//	Level().Send		(P,net_flags(TRUE));
//	// Destroy
//	F_entity_Destroy	(D);
	Level().spawn_item	(*cNameSect(),Position(),level_vertex_id(),ID());
}

void CMissile::OnH_A_Chield() 
{
	inherited::OnH_A_Chield();

	if(!m_fake_missile && !dynamic_cast<CMissile*>(H_Parent())) 
		spawn_fake_missile	();
}

void CMissile::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();

	m_pHUD->Hide();

	if(!m_dwDestroyTime && Local()) 
	{
		NET_Packet			P;
		u_EventGen			(P,GE_DESTROY,ID());
//		Msg					("ge_destroy: [%d] - %s",ID(),*cName());
		u_EventSend			(P);
		return;
	}
}

void CMissile::UpdateCL() 
{
	inherited::UpdateCL();

	if(State() == MS_IDLE && m_dwStateTime > PLAYING_ANIM_TIME) 
		OnStateSwitch(MS_PLAYING);
	
	if(State() == MS_READY) 
	{
		if(m_throw) 
			SwitchState(MS_THROW);
		else 
		{
			CActor	*actor = dynamic_cast<CActor*>(H_Parent());
			if (actor) {
				m_fThrowForce		+= (m_fForceGrowSpeed * Device.dwTimeDelta) * .001f;
				if (m_fThrowForce > m_fMaxForce)
					m_fThrowForce = m_fMaxForce;
			}
		}
	}
	
	
	if(!H_Parent() && getVisible() && m_pPhysicsShell) 
	{
		if(m_dwDestroyTime <= Device.dwTimeDelta) 
		{
			m_dwDestroyTime = 0xffffffff;
			R_ASSERT(!m_pInventory);
			Destroy();
			return;
		}
		if (m_dwDestroyTime < 0xffffffff) m_dwDestroyTime -= Device.dwTimeDelta;
	} 
}

void CMissile::StartIdleAnim()
{
	m_pHUD->animDisplay(m_pHUD->animGet(*m_sAnimIdle), TRUE);
}

u32 CMissile::State(u32 state) 
{
	m_state = state;
	
	switch(State()) 
	{
	case MS_SHOWING:
        {
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimShow), FALSE, this);
			
			//Msg("------------------");
			//Msg("Missile show begin");
		} break;
	case MS_IDLE:
		{
			m_bPending = false;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimIdle));
			
			//Msg("Missile idle begin");
		} break;
	case MS_HIDING:
		{
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimHide), true, this);

			//Msg("Missile hide begin");
		} break;
	case MS_HIDDEN:
		{
			m_bPending = false;
			setVisible(false);
			setEnabled(false);
		
			//Msg("Missile hidden");
		} break;
	case MS_THREATEN:
		{
			m_bPending = true;
			m_fThrowForce = m_fMinForce;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowBegin), true, this);

			//Msg("Missile throw begin");
		} break;
	case MS_READY:
		{
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowIdle), true, this);

			//Msg("Missile throw idle");
		} break;
	case MS_THROW:
		{
			m_bPending = true;
			m_throw = false;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowAct), true, this);

			//Msg("Missile throw act");
		} break;
	case MS_END:
		{
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowEnd), true, this);

			//Msg("Missile throw finish");
		} break;
	case MS_PLAYING:
		{
			PlaySound(sndPlaying,Position());
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimPlaying), true, this);

			//Msg("Missile playing");
		} break;
	}
	return State();
}

void CMissile::OnStateSwitch	(u32 S)
{
	State(S);
	inherited::OnStateSwitch(S);
}


void CMissile::OnAnimationEnd() 
{
	switch(State()) 
	{
	case MS_HIDING:
		{
			setVisible(FALSE);
			OnStateSwitch(MS_HIDDEN);
		} break;
	case MS_SHOWING:
		{
			//Msg("Missile show end");

			setVisible(TRUE);
			OnStateSwitch(MS_IDLE);
		} break;
	case MS_THREATEN:
		{
			if(m_throw) 
				SwitchState(MS_THROW); 
			else 
				OnStateSwitch(MS_READY);
		} break;
	case MS_THROW:
		{
			Throw();
			OnStateSwitch(MS_END);
		} break;
	case MS_END:
		{
			OnStateSwitch(MS_SHOWING);
		} break;
	case MS_PLAYING:
		{
			OnStateSwitch(MS_IDLE);
		} break;
	}
}


void CMissile::UpdatePosition(const Fmatrix& trans)
{
//	Position().set	(trans.c);
//	XFORM().mul		(trans,m_offset);
	XFORM().mul		(trans,m_offset);
}

void CMissile::UpdateXForm()
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame			= Device.dwFrame;

		if (0==H_Parent())	return;

		// Get access to entity and its visual
		CEntityAlive*		E		= dynamic_cast<CEntityAlive*>(H_Parent());
        
		if(!E)				return	;

		R_ASSERT			(E);
		CKinematics*		V		= PKinematics	(E->Visual());
		VERIFY				(V);

		// Get matrices
		int					boneL,boneR,boneR2;
		E->g_WeaponBones	(boneL,boneR,boneR2);

		boneL = boneR2;

		V->CalculateBones	();
		Fmatrix& mL			= V->LL_GetTransform(u16(boneL));
		Fmatrix& mR			= V->LL_GetTransform(u16(boneR));

		// Calculate
		Fmatrix				mRes;
		Fvector				R,D,N;
		D.sub				(mL.c,mR.c);	D.normalize_safe();
		R.crossproduct		(mR.j,D);		R.normalize_safe();
		N.crossproduct		(D,R);			N.normalize_safe();
		mRes.set			(R,N,D,mR.c);
		mRes.mulA_43		(E->XFORM());
		UpdatePosition		(mRes);
	}
}

void CMissile::renderable_Render() 
{
	inherited::renderable_Render();
}

void CMissile::Show() 
{
	SwitchState(MS_SHOWING);
}

void CMissile::Hide() 
{
	//SwitchState(MS_HIDING);
	SwitchState(MS_HIDDEN);
}

void CMissile::setup_throw_params()
{
	CActor* pActor = dynamic_cast<CActor*>(H_Parent());
	if(pActor)// && pActor->HUDview())
	{
//		UpdateFP();
//		pActor->EffectorManager().affected_Matrix(trans);
		Fmatrix trans;
		trans.identity();
		Fvector FirePos, FireDir;
		pActor->g_fireParams(this, FirePos, FireDir);
		trans.k.set(FireDir);
		Fvector::generate_orthonormal_basis(trans.k, trans.j,trans.i);
		trans.c.set(FirePos);
		//trans.c.mad(FireDir, 1.f);
		m_throw_matrix.set(trans);
		m_throw_direction.set(trans.k);
	}
	else
	{
		m_throw_direction.set	(H_Parent()->XFORM().k);
		m_throw_matrix			= XFORM();
	};
}

void CMissile::Throw() 
{
	VERIFY								(dynamic_cast<CEntity*>(H_Parent()));
	
	//Fvector								throw_point, throw_direction;
	//entity->g_fireParams				(throw_point,throw_direction);
	setup_throw_params					();
	
	m_fake_missile->m_throw_direction	= m_throw_direction;
	m_fake_missile->m_throw_matrix		= m_throw_matrix;
	
	m_fake_missile->m_fThrowForce		= m_fThrowForce; 
	m_fThrowForce						= m_fMinForce;
	
	if (Local()) {
		NET_Packet						P;
		u_EventGen						(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16							(u16(m_fake_missile->ID()));
		u_EventSend						(P);
	}

}

void CMissile::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent		(P,type);
	u16						id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CMissile		*missile = dynamic_cast<CMissile*>(Level().Objects.net_Find(id));			
			m_fake_missile	= missile;
			missile->H_SetParent(this);
			break;
		} 
		case GE_OWNERSHIP_REJECT : {
			P.r_u16			(id);
			if (m_fake_missile && (id == m_fake_missile->ID()))
				m_fake_missile	= NULL;
			CMissile		*missile = dynamic_cast<CMissile*>(Level().Objects.net_Find(id));
			if (!missile)
				break;
			missile->H_SetParent(0);
			break;
		}
	}
}

void CMissile::Destroy() 
{
	NET_Packet			P;
	u_EventGen			(P,GE_DESTROY,ID());
//	Msg					("ge_destroy: [%d] - %s",ID(),*cName());
	if (Local()) u_EventSend			(P);
}

bool CMissile::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	switch(cmd) 
	{
	case kWPN_FIRE:
		{
        	if(flags&CMD_START) 
			{
				m_throw = false;
				if(State() == MS_IDLE) 
					SwitchState(MS_THREATEN);
				else if(State() == MS_READY)
				{
					m_throw = true; 
				}

			} 
			else if(State() == MS_READY || State() == MS_THREATEN
				    || State() == MS_IDLE) 
			{
				m_throw = true; 
				if(State() == MS_READY) SwitchState(MS_THROW);
			}
		}
        return true;
	}
	return false;
}

void  CMissile::UpdateFP()
{
	if (0==H_Parent())	return;

    if (Device.dwFrame!=dwFP_Frame) 
	{
		dwFP_Frame = Device.dwFrame;

		UpdateXForm			();
		
		if (hud_mode && !IsHidden())
		{
			// 1st person view - skeletoned
			CKinematics* V			= PKinematics(m_pHUD->Visual());
			V->CalculateBones		();

			// fire point&direction
			Fmatrix& parent			= m_pHUD->Transform	();
			m_throw_direction.set	(parent.k);
		} 
		else 
		{
			// 3rd person
			Fmatrix& parent			= H_Parent()->XFORM();

			m_throw_direction.set	(m_vThrowDir);
			parent.transform_dir	(m_throw_direction);
		}
	}
}

void CMissile::activate_physic_shell()
{
	if (!dynamic_cast<CMissile*>(H_Parent())) {
		inherited::activate_physic_shell();
		return;
	}
	
	Fvector l_vel;
	l_vel.set(m_throw_direction);
	l_vel.normalize_safe();
	l_vel.mul(m_fThrowForce);

	Fvector a_vel;
	float fi,teta,r;
	fi=	 ::Random.randF(0.f,2.f*M_PI);
	teta=::Random.randF(0.f,M_PI);
	r=	 ::Random.randF(2.f*M_PI,3.f*M_PI);
	float rxy=r*_sin(teta);
	a_vel.set(rxy*_cos(fi),rxy*_sin(fi),r*_cos(teta));

	XFORM().set(m_throw_matrix);
	m_pPhysicsShell->Activate	(m_throw_matrix, l_vel, a_vel);
	m_pPhysicsShell->AddTracedGeom();
	PKinematics(Visual())->CalculateBones();
}

void CMissile::create_physic_shell	()
{
	//create_box2sphere_physic_shell();
	CInventoryItem::CreatePhysicsShell();
}

void CMissile::setup_physic_shell	()
{
	// do not delete!!!
}
#include "stdafx.h"
#include "missile.h"
#include "WeaponHUD.h"
#include "PhysicsShell.h"
#include "effectorshot.h"
#include "actor.h"
#include "../CameraBase.h"


#define PLAYING_ANIM_TIME 10000

CMissile::CMissile(void) 
{
	m_pHUD				= xr_new<CWeaponHUD>();
	m_offset.identity	();
}

CMissile::~CMissile(void) 
{
	xr_delete			(m_pHUD);
}

void CMissile::reinit		()
{
	m_state				= MS_HIDDEN;
	m_throw				= false;
	m_fThrowForce				= 0;
	m_fMinForce			= 20.f;
	m_fMaxForce			= 200.f;
	m_fForceGrowSpeed	= 50.f;
	m_dwDestroyTime		= 0xffffffff;
	m_pInventory		= 0;
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

	//��������� �������� HUD-�
	m_sAnimShow			= pSettings->r_string(*hud_sect, "anim_show");
	m_sAnimHide			= pSettings->r_string(*hud_sect, "anim_hide");
	m_sAnimIdle			= pSettings->r_string(*hud_sect, "anim_idle");
	m_sAnimPlaying		= pSettings->r_string(*hud_sect, "anim_playing");
	m_sAnimThrowBegin	= pSettings->r_string(*hud_sect, "anim_throw_begin");
	m_sAnimThrowIdle	= pSettings->r_string(*hud_sect, "anim_throw_idle");
	m_sAnimThrowAct		= pSettings->r_string(*hud_sect, "anim_throw_act");
	m_sAnimThrowEnd		= pSettings->r_string(*hud_sect, "anim_throw_end");

}

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

BOOL CMissile::net_Spawn(LPVOID DC) 
{
#pragma todo("Dima to Yura : find out why m_pInventory != NULL on net_spawn after net_destroy")
	R_ASSERT(!m_pInventory);
	//m_pInventory = 0;
	BOOL l_res = inherited::net_Spawn(DC);

	CSkeletonRigid* V = PSkeletonRigid(Visual());
	R_ASSERT(V);


	dwXF_Frame					= 0xffffffff;

	setVisible					(true);
	setEnabled					(true);

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		
		Fvector ax;
		float	radius;
		CHOOSE_MAX(	obb.m_halfsize.x,ax.set(obb.m_rotate.i) ; ax.mul(obb.m_halfsize.x); radius=_min(obb.m_halfsize.y,obb.m_halfsize.z) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.y,ax.set(obb.m_rotate.j) ; ax.mul(obb.m_halfsize.y); radius=_min(obb.m_halfsize.x,obb.m_halfsize.z) ;obb.m_halfsize.x/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.z,ax.set(obb.m_rotate.k) ; ax.mul(obb.m_halfsize.z); radius=_min(obb.m_halfsize.y,obb.m_halfsize.x) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.x/=2.f
					)
		//radius*=1.4142f;
		Fsphere sphere1,sphere2;
		sphere1.P.add						(obb.m_translate,ax);
		sphere1.R							=radius*1.4142f;

		sphere2.P.sub						(obb.m_translate,ax);
		sphere2.R							=radius/2.f;

		E->add_Box							(obb);
		E->add_Sphere						(sphere1);
		E->add_Sphere						(sphere2);
	
		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setDensity			(2000.f);
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate			(XFORM(),0,XFORM(),true);
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
		m_pPhysicsShell->SetAirResistance();
	}
	return l_res;
}

void CMissile::net_Destroy() 
{
	//R_ASSERT(!m_pInventory);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	inherited::net_Destroy();
	m_pInventory = 0;
}
void CMissile::OnH_B_Chield() 
{
	inherited::OnH_B_Chield		();
	
	setVisible					(false);
	setEnabled					(false);

	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CMissile::spawn_fake_missile()
{
#ifdef DEBUG
	Msg					("Spawning fake missile for object %s",cName());
#endif
	CSE_Abstract		*D	= F_entity_Create(cNameSect());
	R_ASSERT			(D);
	CSE_ALifeDynamicObject				*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
	R_ASSERT							(l_tpALifeDynamicObject);
	l_tpALifeDynamicObject->m_tNodeID	= level_vertex_id();
	// Fill
	strcpy				(D->s_name,cNameSect());
	strcpy				(D->s_name_replace,"");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	(u16)ID();
	D->ID_Phantom		=	0xffff;
	D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);
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
		u_EventSend			(P);
		return;
	}
	
	//xr_delete(m_pHUD);
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	XFORM().set(E->XFORM());
	
	if(m_pPhysicsShell) 
	{
		/*Fvector		D;
		D			= m_throw_direction;
		D.normalize	();

		Fvector		l_fw; 
		Fvector		l_up; 

		l_fw.set	(D);

		l_up.set	(XFORM().j); l_up.mul(2.f);
		
		Fmatrix		l_p1, l_p2;
		l_p1.set	(XFORM()); l_p1.c.add(l_up); l_up.mul(1.2f);
		l_p2.set	(XFORM()); l_p2.c.add(l_up); l_fw.mul(1.f+m_fThrowForce); l_p2.c.add(l_fw);
		
		Fvector		l_vel,a_vel;
		float		fi,teta,r;
		l_vel.add	(l_up,l_fw);
		fi			= ::Random.randF(0.f,2.f*M_PI);
		teta		= ::Random.randF(0.f,M_PI);
		r			= ::Random.randF(2.f*M_PI,3.f*M_PI);
		float		rxy = r*_sin(teta);
		a_vel.set	(rxy*_cos(fi),rxy*_sin(fi),r*_cos(teta));

		m_pPhysicsShell->Activate(l_p1, l_vel, a_vel);
		
		XFORM().set	(l_p1);
				

//		}
//		else {
//			Fvector						linear_velocity = m_throw_direction;
//			VERIFY						(linear_velocity.square_magnitude() > EPS_L);
//			linear_velocity.normalize	();
//			linear_velocity.mul			(m_fThrowForce);
//			m_pPhysicsShell->Activate	(H_Parent()->XFORM(), linear_velocity, zero_vel);
//			XFORM().set					(m_pPhysicsShell->mXFORM);
//		}
*/

		Fvector vel;
		vel.set(m_throw_direction);
		vel.normalize();
		vel.mul(m_fThrowForce);
		XFORM().c.set(m_throw_point);
		m_pPhysicsShell->Activate(XFORM(), vel, zero_vel);
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
		if(m_dwDestroyTime < 0xffffffff) m_dwDestroyTime -= Device.dwTimeDelta;
		m_pPhysicsShell->Update	();
		XFORM().set	(m_pPhysicsShell->mXFORM);
		Position().set(m_pPhysicsShell->mXFORM.c);
	} 
	else if(H_Parent()) XFORM().set(H_Parent()->XFORM());
}

u32 CMissile::State() 
{
	return m_state;
}

u32 CMissile::State(u32 state) 
{
	m_state = state;
	
	switch(State()) 
	{
	case MS_SHOWING:
        {
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimShow), true, this);
		} break;
	case MS_IDLE:
		{
			m_bPending = false;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimIdle));
		} break;
	case MS_HIDING:
		{
			m_bPending = true;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimHide), true, this);
		} break;
	case MS_HIDDEN:
		{
			m_bPending = false;
			setVisible(false);
			setEnabled(false);
		} break;
	case MS_THREATEN:
		{
			m_fThrowForce = m_fMinForce;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowBegin), true, this);
		} break;
	case MS_READY:
		{
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowIdle), true, this);
		} break;
	case MS_THROW:
		{
			m_bPending = true;
			m_throw = false;
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowAct), true, this);
		} break;
	case MS_END:
		{
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimThrowEnd), true, this);
		} break;
	case MS_PLAYING:
		{
			m_pHUD->animPlay(m_pHUD->animGet(*m_sAnimPlaying), true, this);
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
			setVisible(TRUE);
			OnStateSwitch(MS_IDLE);
		} break;
	case MS_THREATEN:
		{
			if(m_throw) SwitchState(MS_THROW); else OnStateSwitch(MS_READY);
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
	if (Device.dwFrame!=dwXF_Frame){
		dwXF_Frame = Device.dwFrame;

		if (0==H_Parent())	return;

		// Get access to entity and its visual
		CEntityAlive*	E		= dynamic_cast<CEntityAlive*>(H_Parent());

		if(!E) return;
		R_ASSERT		(E);
		CKinematics*	V		= PKinematics	(E->Visual());
		VERIFY			(V);

		// Get matrices
		int				boneL,boneR,boneR2;
		E->g_WeaponBones(boneL,boneR,boneR2);

//		if ((STATE == eReload) || (!E->g_Alive()))
			boneL = boneR2;
		V->Calculate	();
		Fmatrix& mL		= V->LL_GetTransform(u16(boneL));
		Fmatrix& mR		= V->LL_GetTransform(u16(boneR));
		// Calculate
		Fmatrix			mRes;
		Fvector			R,D,N;
		D.sub			(mL.c,mR.c);	D.normalize_safe();
		R.crossproduct	(mR.j,D);		R.normalize_safe();
		N.crossproduct	(D,R);			N.normalize_safe();
		mRes.set		(R,N,D,mR.c);
		mRes.mulA_43	(E->XFORM());
		UpdatePosition	(mRes);
		UpdateHudPosition	();
	}
}

void CMissile::renderable_Render() 
{
	UpdateXForm();
	UpdateFP();

	inherited::renderable_Render();
}

void CMissile::Show() 
{
	SwitchState(MS_SHOWING);
}

void CMissile::Hide() 
{
	SwitchState(MS_HIDING);
}

void CMissile::Throw() 
{
	CEntity								*entity = dynamic_cast<CEntity*>(H_Parent());
	VERIFY								(entity);
	
	//Fvector								throw_point, throw_direction;
	//entity->g_fireParams				(throw_point,throw_direction);

	m_fake_missile->m_throw_point		= m_throw_point;
	m_fake_missile->m_throw_direction	= m_throw_direction;
	
	m_fake_missile->m_fThrowForce				= m_fThrowForce; 
	m_fThrowForce								= m_fMinForce;
	
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
				if(State() == MS_IDLE) SwitchState(MS_THREATEN);
			} 
			else if(State() == MS_READY || State() == MS_THREATEN) 
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
			V->Calculate			();

			// fire point&direction
			Fmatrix& fire_mat		= V->LL_GetTransform(u16(m_pHUD->iFireBone));
			Fmatrix& parent			= m_pHUD->Transform	();
			Fvector& fp				= m_vHudThrowPoint;

			fire_mat.transform_tiny	(m_throw_point,fp);
			parent.transform_tiny	(m_throw_point);
			
			//m_throw_direction.set	(0.f,0.f,1.f);
			m_throw_direction.set	(m_vHudThrowDir);
			parent.transform_dir	(m_throw_direction);
		} 
		else 
		{
			// 3rd person
			Fmatrix& parent			= H_Parent()->XFORM();
			Fvector& fp				= m_vThrowPoint;

			parent.transform_tiny	(m_throw_point,fp);
			
			//m_throw_direction.set	(0.f,0.f,1.f);
			m_throw_direction.set	(m_vThrowDir);
			parent.transform_dir	(m_throw_direction);
		}
	}
}

#include "stdafx.h"
#include "helicopter.h"
#include "xrserver_objects_alife.h"
#include "PhysicsShell.h"
#include "CustomRocket.h"
#include "ExplosiveRocket.h"
#include "level.h"
#include "HudManager.h"
#include "script_game_object.h"
#include "../LightAnimLibrary.h"
#include "HelicopterMovementManager.h"
#include "clsid_game.h"
#include "../skeletonanimated.h"

CHelicopter::CHelicopter()
{
	init();
	m_pParticle		= NULL;
	m_light_render	= NULL;
	m_lanim			= NULL;

	ISpatial*		self				=	smart_cast<ISpatial*> (this);
	if (self)		self->spatial.type &=~STYPE_VISIBLEFORAI;;	

	m_movMngr		= xr_new<CHelicopterMovManager>();
}

CHelicopter::~CHelicopter()
{
//	xr_delete(m_pParticle);
	xr_delete		(m_movMngr);
}

void CHelicopter::setState(CHelicopter::EHeliState s)
{
	m_curState = s;
	
	LPCSTR str;
	switch(m_curState) {
	case CHelicopter::eIdleState:
		str = "eIdleState";
		break;
	case CHelicopter::eMovingByPatrolZonePath:
		str = "eMovingByPatrolZonePath";
		break;
	case CHelicopter::eInitiateHunt:
		str = "eInitiateHunt";
		break;
	case CHelicopter::eInitiateHunt2:
		str = "eInitiateHunt2";
		break;
	case CHelicopter::eMovingByAttackTraj:
		str = "eMovingByAttackTraj";
		break;
	case CHelicopter::eInitiatePatrolZone:
		str = "eInitiatePatrolZone";
		break;

	case CHelicopter::eWaitForStart:
		str = "eWaitForStart";
		break;
		
	case CHelicopter::eWaitBetweenPatrol:
		str = "eWaitBetweenPatrol";
		break;
			
	case CHelicopter::eInitiateWaitBetweenPatrol:
		str = "eInitiateWaitBetweenPatrol";
		break;
			
	case CHelicopter::eMovingToWaitPoint:
		str = "eMovingToWaitPoint";
		break;

	case CHelicopter::eInitiateGoToPoint:
		str = "eInitiateGoToPoint";
		break;

	case CHelicopter::eDead:
		str = "eDead";
		break;
	case CHelicopter::eMovingToPoint:
		str = "eMovingToPoint";
		break;

	case CHelicopter::eInitiatePatrolByPath:
		str = "eInitiatePatrolByPath";
		break;

	case CHelicopter::eMovingByPatrolPath:
		str = "eMovingByPatrolPath";
		break;

	default:
		str = "unknown";
		break;
	};

	Msg("---CHelicopter::state[%d]==(%s)", Device.dwTimeGlobal, str);
}


//CAI_ObjectLocation
void CHelicopter::init()
{
	m_data.m_destEnemy					= NULL;
	m_cur_x_rot					= 0.0f;
	m_cur_y_rot					= 0.0f;
	m_tgt_x_rot					= 0.0f;
	m_tgt_y_rot					= 0.0f;
	m_bind_x_rot				= 0.f;
	m_bind_y_rot				= 0.f;
	m_allow_fire				= FALSE;
	m_use_rocket_on_attack		= TRUE;
	m_use_mgun_on_attack		= TRUE;	
	m_syncronize_rocket			= TRUE;
	m_min_rocket_dist			= 20.0f;
	m_max_rocket_dist			= 200.0f;
	m_time_between_rocket_attack = 0;
	m_last_rocket_attack		= Device.dwTimeGlobal;

	SetfHealth(100.0f);

	m_data.m_stayPoint.set(0.0f, 0.0f, 0.0f);
}

void CHelicopter::reinit()
{
	inherited::reinit	();
}


//CGameObject
void CHelicopter::Load(LPCSTR section)
{
	inherited::Load			(section);
	CShootingObject::Load	(section);
	CRocketLauncher::Load	(section);

////////////////////////////////////
	m_on_point_range_dist		= 10.0f;
	m_data.m_time_last_patrol_end		= 0.0f;
	m_data.m_time_last_patrol_start	= 0.0f;

	m_data.m_hunt_dist				= 20.0f;
	m_data.m_hunt_time				= 5.0f;
	
	m_data.m_startDir.set				(1.0f,0.0f,2.0f);
	m_smoke_particle					= pSettings->r_string(section,"smoke_particle");
	m_explode_particle					= pSettings->r_string(section,"explode_particle");
	m_angularSpeedPitch					= pSettings->r_float(section,"path_angular_sp_pitch");
	m_angularSpeedHeading				= pSettings->r_float(section,"path_angular_sp_heading");
	m_LinearAcc_fw						= pSettings->r_float(section,"path_linear_acc_fw");
	m_LinearAcc_bk						= pSettings->r_float(section,"path_linear_acc_bk");

	m_model_angSpeedBank					= pSettings->r_float(section,"model_angular_sp_bank");
	m_model_angSpeedPitch					= pSettings->r_float(section,"model_angular_sp_pitch");
	m_data.m_pitch_k					= pSettings->r_float(section,"model_pitch_koef");
	m_data.m_bank_k					= pSettings->r_float(section,"model_bank_koef");

	m_data.m_baseAltitude				= pSettings->r_float(section,"altitude");
	m_data.m_attackAltitude			= pSettings->r_float(section,"attack_altitude");
	m_data.m_basePatrolSpeed			= pSettings->r_float(section,"velocity");
	m_data.m_baseAttackSpeed			= pSettings->r_float(section,"attack_velocity");
	m_data.m_alt_korridor				= pSettings->r_float(section,"altitude_korridor");
	m_data.m_maxKeyDist				= pSettings->r_float(section,"max_key_dist");

	m_data.m_time_delay_between_patrol	= pSettings->r_float(section,"time_delay_between_patrol");
	m_data.m_time_patrol_period		= pSettings->r_float(section,"time_patrol_period");
	m_data.m_time_delay_before_start	= pSettings->r_float(section,"time_delay_before_start");

	m_death_ang_vel						= pSettings->r_fvector3(section,"death_angular_vel");
	m_death_lin_vel_k					= pSettings->r_float(section,"death_lin_vel_koeff");


	m_data.m_wrk_altitude = m_data.m_baseAltitude;
	m_maxLinearSpeed = m_data.m_basePatrolSpeed;
////////////////////////////////////
	m_movMngr->load (section);

	m_sAmmoType = pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load(*m_sAmmoType);

	m_sRocketSection						= pSettings->r_string	(section,"rocket_class");

	CHitImmunity::LoadImmunities(section);

	m_use_rocket_on_attack				= !!pSettings->r_bool(section,"use_rocket");
	m_use_mgun_on_attack				= !!pSettings->r_bool(section,"use_mgun");
	m_min_rocket_dist					= pSettings->r_float(section,"min_rocket_attack_dist");
	m_max_rocket_dist					= pSettings->r_float(section,"max_rocket_attack_dist");
	m_min_mgun_dist						= pSettings->r_float(section,"min_mgun_attack_dist");
	m_max_mgun_dist						= pSettings->r_float(section,"max_mgun_attack_dist");
	m_time_between_rocket_attack		= pSettings->r_u32(section,"time_between_rocket_attack");
	m_syncronize_rocket					= !!pSettings->r_bool(section,"syncronize_rocket");
	
	shared_str expl_snd					= pSettings->r_string	(section,"explode_sound");
	m_explodeSound.create(TRUE,*expl_snd);

//lighting
	m_light_range=pSettings->r_float(section,"light_range");
	m_light_brightness = pSettings->r_float(section,"light_brightness");

	m_light_color			= pSettings->r_fcolor(section,"light_color");
	m_light_color.a			= 1.f;
	m_light_color.mul_rgb	(m_light_brightness);
	m_l_anim				= pSettings->r_string	(section,"light_color_animmator");
	m_lanim					= LALib.FindItem(*m_l_anim);


}

void CHelicopter::reload(LPCSTR section)
{
	inherited::reload	(section);
}

void CollisionCallbackAlife(bool& do_colide,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{
	do_colide=false;
}
BOOL CHelicopter::net_Spawn(LPVOID	DC)
{
	SetfHealth(100.0f);
	setState(CHelicopter::eIdleState);
	if (!inherited::net_Spawn(DC))
		return			(FALSE);
	CPHSkeleton::Spawn((CSE_Abstract*)(DC));
	for(u32 i=0; i<4; ++i)
		CRocketLauncher::SpawnRocket(*m_sRocketSection, smart_cast<CGameObject*>(this/*H_Parent()*/));

	// assigning m_animator here
	CSE_Abstract		*abstract	=(CSE_Abstract*)(DC);
	CSE_ALifeHelicopter	*heli		= smart_cast<CSE_ALifeHelicopter*>(abstract);
	VERIFY				(heli);

	R_ASSERT						(Visual()&&smart_cast<CKinematics*>(Visual()));
	CKinematics* K					= smart_cast<CKinematics*>(Visual());
	CInifile* pUserData				= K->LL_UserData();

	m_rotate_x_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_x_bone"));
	m_rotate_y_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_y_bone"));
	m_fire_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_fire_bone"));
	m_death_bones_to_hide = pUserData->r_string("on_death_mode","scale_bone");
	m_left_rocket_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","left_rocket_bone"));
	m_right_rocket_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","right_rocket_bone"));

	m_smoke_bone 			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","smoke_bone"));
	m_light_bone 			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","light_bone"));
	LPCSTR s = pUserData->r_string("helicopter_definition","hit_section");

	if( pUserData->section_exist(s) ){
		int lc = pUserData->line_count(s);
		LPCSTR name;
		LPCSTR value;
		s16 boneID;
		for (int i=0 ;i<lc; ++i){
			pUserData->r_line( s, i, &name, &value);
			boneID	=K->LL_BoneID(name);
			m_hitBones.insert( std::make_pair(boneID, (float)atof(value)) );
		}
	}
	
	CBoneInstance& biX		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_x_bone);	
	biX.set_callback		(BoneMGunCallbackX,this);
	CBoneInstance& biY		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_y_bone);	
	biY.set_callback		(BoneMGunCallbackY,this);
	CBoneData& bdX			= K->LL_GetData(m_rotate_x_bone); VERIFY(bdX.IK_data.type==jtJoint);
	m_lim_x_rot.set			(bdX.IK_data.limits[0].limit.x,bdX.IK_data.limits[0].limit.y);
	CBoneData& bdY			= K->LL_GetData(m_rotate_y_bone); VERIFY(bdY.IK_data.type==jtJoint);
	m_lim_y_rot.set			(bdY.IK_data.limits[1].limit.x,bdY.IK_data.limits[1].limit.y);

	xr_vector<Fmatrix> matrices;
	K->LL_GetBindTransform	(matrices);
	m_i_bind_x_xform.invert	(matrices[m_rotate_x_bone]);
	m_i_bind_y_xform.invert	(matrices[m_rotate_y_bone]);
	m_bind_x_rot			= matrices[m_rotate_x_bone].k.getP();
	m_bind_y_rot			= matrices[m_rotate_y_bone].k.getH();
	m_bind_x.set			(matrices[m_rotate_x_bone].c);
	m_bind_y.set			(matrices[m_rotate_y_bone].c);
	
	CSkeletonAnimated	*A	= smart_cast<CSkeletonAnimated*>(Visual());
	if (A) {
		A->PlayCycle		(*heli->startup_animation);
		A->CalculateBones	();
	}
	m_engineSound.create(TRUE,*heli->engine_sound);
	m_engineSound.play_at_pos(0,XFORM().c,sm_Looped);
	
	CShootingObject::Light_Create();


	setVisible			(true);
	setEnabled			(true);



	Fbox b = Level().ObjectSpace.GetBoundingVolume();
	if(!b.contains( XFORM().c ) ){
		if( (b.min.y > XFORM().c.y) || (b.max.y < XFORM().c.y) )
			XFORM().c.y = (b.max.y-b.min.y)/2.0f;
	};
	if(!b.contains( XFORM().c ) ){
		Debug.fatal("helicopter spawned out of level bounding box");
	};
	b.getcenter(XFORM().c);

	m_data.m_stayPoint			= XFORM().c;
/////////////////////////////	
	m_data.m_desiredXFORM	= XFORM();
	m_data.m_desiredP		= XFORM().c;
	XFORM().getXYZ(m_data.m_desiredR);
	float bbb;
	XFORM().getHPB(m_currPathH, m_currPathP, bbb);

	m_currP					= m_data.m_desiredP;
	m_currR					= m_data.m_desiredR;

	m_data.m_to_point		= m_currP;
	m_curLinearSpeed		= 0.0f;
	m_curLinearAcc			= m_LinearAcc_fw;
	
	XFORM().getHPB(m_currBodyH, m_currBodyP, m_currBodyB);

    m_movMngr->init			(this);

//lighting
	m_light_render			= ::Render->light_create();
	m_light_render->set_shadow  (false);
	m_light_render->set_type	(IRender_Light::POINT);
	m_light_render->set_range	(m_light_range);
	m_light_render->set_color	(m_light_color);

	if(g_Alive())processing_activate();
	return				(TRUE);
}

void CHelicopter::net_Destroy()
{
	inherited::net_Destroy();
	CShootingObject::Light_Destroy();
	CPHSkeleton::RespawnInit();
	m_engineSound.stop();
//	m_pParticle->Stop();
	xr_delete(m_pParticle);
	::Render->light_destroy	(m_light_render);

}

void	CHelicopter::SpawnInitPhysics	(CSE_Abstract	*D)	
{
	
	PPhysicsShell()=P_build_Shell	(this,false);
	if(g_Alive())
	{
		PPhysicsShell()->EnabledCallbacks(FALSE);
		PPhysicsShell()->set_ObjectContactCallback(CollisionCallbackAlife);
		PPhysicsShell()->Disable();
	}
}

void	CHelicopter::net_Save			(NET_Packet& P)	
{
	inherited::net_Save(P);
	CPHSkeleton::SaveNetState(P);
}

BOOL	CHelicopter::net_SaveRelevant	()
{
	return BOOL(PPhysicsShell()!=NULL);
}

void CHelicopter::net_Export(NET_Packet &P)
{
}

void CHelicopter::net_Import (NET_Packet &P)
{
}

void CHelicopter::renderable_Render()
{
	inherited::renderable_Render();
}

void CHelicopter::UpdateCL()
{
	inherited::UpdateCL	();
	if(PPhysicsShell() && (state() == CHelicopter::eDead) ){
		PPhysicsShell()->InterpolateGlobalTransform(&XFORM());

		CKinematics* K		= smart_cast<CKinematics*>(Visual());
		K->CalculateBones	();
		//smoke
		m_particleXFORM	= K->LL_GetTransform(m_smoke_bone);
		m_particleXFORM.mulA(XFORM());
		UpdateHeliParticles();
		return;
	}
	else
	{
		PPhysicsShell()->SetTransform(XFORM());
	}
//	m_movMngr.getPathPosition (Level().timeServer()/1000.0f,Device.fTimeDelta, XFORM() );

///////////////////////////////////////////////////////////////////////////
	float td = Device.fTimeDelta;
//	float lt = Level().timeServer()/1000.0f;

//	Fvector stub;
//	m_movMngr.getPathPosition (lt ,m_data.m_desiredP, stub);
	m_data.m_desiredP = m_data.m_to_point;

	float dist = m_currP.distance_to(m_data.m_desiredP);

	Fvector dir, pathDir;
	dir.sub(m_data.m_desiredP,m_currP);
	dir.normalize_safe();
	pathDir = dir;
	float desired_H, desired_P;
	dir.getHP(desired_H, desired_P);
	

	angle_lerp	(m_currPathH, desired_H, m_angularSpeedHeading, td);
	angle_lerp	(m_currPathP, desired_P, m_angularSpeedPitch, td);
	
	dir.setHP(m_currPathH, m_currPathP);


	//position
	float dDist = m_curLinearSpeed*td+(m_curLinearAcc*td*td)/2.0f;
	float deltaDist = dDist-dist;
	int incFlag = 0;
	if(!fis_zero(dist,EPS_L)&&deltaDist>2.0f)
		incFlag = -1;
	if(!fis_zero(dist,EPS_L)&&deltaDist<-2.0f)
		incFlag = 1;

	if(angle_difference(m_currPathH,desired_H)>PI_DIV_2)
		incFlag = -1;

	switch(incFlag){
		case 0://no acceleration
			m_curLinearAcc = 0.0f;
		break;
		case 1://increase linear speed
			m_curLinearAcc = m_LinearAcc_fw;
		break;
		case -1://decrease linear speed
			m_curLinearAcc = -m_LinearAcc_bk;
		break;
		default:
			NODEFAULT;
	};
	float vp = m_curLinearSpeed*td+(m_curLinearAcc*td*td)/2.0f;
	m_currP.mad	(dir, vp);
	m_curLinearSpeed += m_curLinearAcc*td;
	
	clamp(m_curLinearSpeed,0.0f,m_maxLinearSpeed);

	m_currBodyH = m_currPathH;

	float needBodyP = -m_data.m_pitch_k*m_curLinearSpeed;
	if(incFlag==-1) needBodyP*=-1;
	angle_lerp	(m_currBodyP, needBodyP, m_model_angSpeedPitch, Device.fTimeDelta);


	float sign;
	Fvector cp;
	cp.crossproduct (pathDir,dir);
	(cp.y>0.0)?sign=1.0f:sign=-1.0f;
	float ang_diff = angle_difference (m_currPathH, desired_H);
	
	float needBodyB = -ang_diff*sign*m_data.m_bank_k*m_curLinearSpeed;
	angle_lerp	(m_currBodyB, needBodyB, m_model_angSpeedBank, Device.fTimeDelta);
	

	XFORM().setHPB(m_currBodyH,m_currBodyP,m_currBodyB);

	XFORM().translate_over(m_currP);
//PPhysicsShell()->setXForn()	
/*
	HUD().pFontSmall->SetColor(color_rgba(0xff,0xff,0xff,0xff));
	HUD().pFontSmall->OutSet	(120,630);
	HUD().pFontSmall->OutNext("Linear Velocity:         [%3.5f]",m_curLinearSpeed);
	HUD().pFontSmall->OutNext("Linear Acceleration:     [%3.2f]",m_curLinearAcc);
	HUD().pFontSmall->OutNext("Lag:                     [%3.2f]",deltaDist);
*/
///////////////////////////////////////////////////////////////////////////
	m_engineSound.set_position(XFORM().c);

	//weapon
	MGunUpdateFire();

	CKinematics* K		= smart_cast<CKinematics*>(Visual());
	K->CalculateBones	();

	//smoke
	m_particleXFORM	= K->LL_GetTransform(m_smoke_bone);
	m_particleXFORM.mulA(XFORM());
	UpdateHeliParticles();

	if( m_curState==CHelicopter::eMovingByAttackTraj ){

		m_fire_bone_xform	= K->LL_GetTransform(m_fire_bone);

		m_fire_bone_xform.mulA(XFORM());
		m_fire_pos.set(0,0,0); 
		m_fire_bone_xform.transform_tiny(m_fire_pos);
		m_fire_dir.set(0,0,1);
		m_fire_bone_xform.transform_dir(m_fire_dir);

		m_left_rocket_bone_xform	= K->LL_GetTransform(m_left_rocket_bone);
		m_left_rocket_bone_xform.mulA(XFORM());
		m_left_rocket_bone_xform.c.y += 1.0f;
		//.fake
		m_right_rocket_bone_xform	= K->LL_GetTransform(m_right_rocket_bone);
		m_right_rocket_bone_xform.mulA(XFORM());
		m_right_rocket_bone_xform.c.y += 1.0f;
		//.fake

		updateMGunDir();

	}else{
		m_tgt_x_rot = 0.0f;
		m_tgt_y_rot = 0.0f;
	};

	// lerp angle
	angle_lerp	(m_cur_x_rot, m_tgt_x_rot, PI, Device.fTimeDelta);
	angle_lerp	(m_cur_y_rot, m_tgt_y_rot, PI, Device.fTimeDelta);


	if( m_curState==CHelicopter::eMovingByAttackTraj ){
		m_data.m_destEnemy->Center(m_data.m_destEnemyPos);
		updateMGunDir();

		if(m_allow_fire){
			
			float d = XFORM().c.distance_to_xz(m_data.m_destEnemyPos);
			
			if( (d > m_min_mgun_dist) && 
				(d < m_max_mgun_dist) ){
					MGunFireStart();
				}

			
			if( (d > m_min_rocket_dist) && 
				(d < m_max_rocket_dist) &&
				(Device.dwTimeGlobal-m_last_rocket_attack > m_time_between_rocket_attack)) {
				if(m_syncronize_rocket)	{
					startRocket(1);
					startRocket(2);
				}else{
					if(m_last_launched_rocket==1)
						startRocket(2);
					else
						startRocket(1);
				}
				m_last_rocket_attack = Device.dwTimeGlobal;
			}

		}else{
				MGunFireEnd();
		}

		if( m_data.m_destEnemy->getDestroy() )	{
			m_data.m_destEnemy = NULL;
			setState(CHelicopter::eInitiatePatrolZone);
		};

	}else
		MGunFireEnd();

}

void CHelicopter::shedule_Update(u32 time_delta)
{
	if (!getEnabled())	return;
	inherited::shedule_Update	(time_delta);
	CPHSkeleton::Update(time_delta);
/*
	if ( (state() != CHelicopter::eDead) && (GetfHealth() < 30.0f) )
		PrepareDie();

	if ( (state() != CHelicopter::eDead) && (GetfHealth() <= 0.0f) )
		Die();
*/
	if(state() != CHelicopter::eDead){
		float dist = GetDistanceToDestPosition();
		if(dist < m_on_point_range_dist  ){
			NET_Packet P;
			P.write_start();
			P.w_float(dist);
			P.w_vec3(m_currP);
			s16 curr_idx = -1;
			if(m_movMngr->m_currPatrolVertex)
				curr_idx = (s16)m_movMngr->m_currPatrolVertex->vertex_id();
			
			P.w_s16(curr_idx);
			lua_game_object()->OnEventRaised(CHelicopter::EV_ON_POINT,P);
		}

		m_movMngr->shedule_Update (time_delta);


	if(getRocketCount()<4)
		for(u32 i=getRocketCount(); i<4; ++i) {
			CRocketLauncher::SpawnRocket(*m_sRocketSection, this);
		}
	}
}

void CHelicopter::Hit(	float P, 
						Fvector &dir, 
						CObject* who, 
						s16 element, 
						Fvector position_in_bone_space, 
						float impulse,  
						ALife::EHitType hit_type/* = ALife::eHitTypeWound*/)
{
	inherited::Hit(P,dir,who,element,position_in_bone_space,impulse,hit_type);
if(state() == CHelicopter::eDead ) return;

if(who==this)
	return;

	bonesIt It = m_hitBones.find(element);
	if(It != m_hitBones.end() && hit_type==ALife::eHitTypeFireWound) {
		float curHealth = GetfHealth();
		curHealth -= P*It->second*10.0f;
		SetfHealth(curHealth);

		Log("----Helicopter::PilotHit(). health=",curHealth);

	}else {
		float hit_power		= P/100.f;
		hit_power			*= m_HitTypeK[hit_type];

		SetfHealth(GetfHealth()-hit_power);
		float h= GetfHealth();
		Log("----Helicopter::Hit(). health=",h);
		Log("----------------------------------------");
	};
	
/*	if ( GetfHealth() <= 0.0f ) {
		Die();
		return;
	}
*/
	if (who){
		switch (who->SUB_CLS_ID){
			case CLSID_OBJECT_ACTOR: {
//				doHunt(who);
//				doHunt2(who, 5, 5);
//				goPatrolByPatrolPath("heli_way_1");

			NET_Packet P_;
			P_.write_start();
			P_.w_float(P);
			P_.w_float(impulse);
			P_.w_u32(hit_type);
			P_.w_stringZ(*who->cName());

			lua_game_object()->OnEventRaised(CHelicopter::EV_ON_HIT,P_);

		}break;
		default:
			break;
		}
	}

}
void CHelicopter::PHHit(float P,Fvector &dir,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	if(!g_Alive())inherited::PHHit(P,dir,element,p_in_object_space,impulse,hit_type);
}
void CHelicopter::doHunt(CObject* dest)
{
	VERIFY(this != dest);

	if( !isOnAttack()  ){
		m_data.m_destEnemy		= dest;
		dest->Center	(m_data.m_destEnemyPos);
		
		setState(CHelicopter::eInitiateHunt);
	}
}

void CHelicopter::doHunt2(CObject* dest, float dist, float time)
{
	if( !isOnAttack() ){

		m_data.m_destEnemy		= dest;
		dest->Center	(m_data.m_destEnemyPos);

		m_data.m_hunt_dist = dist;
		m_data.m_hunt_time = time;
		setState(CHelicopter::eInitiateHunt2);
	}
}

void CHelicopter::OnEvent(	NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CRocketLauncher::AttachRocket(id, this);
								 } break;
		case GE_OWNERSHIP_REJECT : {
			P.r_u16(id);
			CRocketLauncher::DetachRocket(id);
								   } break;
	}
}

void CHelicopter::Die()
{
	if ( state() == CHelicopter::eDead )
		return;
	CKinematics* K		= smart_cast<CKinematics*>(Visual());
	if(true /*!PPhysicsShell()*/){
		string256						I;
		LPCSTR bone;
		
		u16 bone_id;
		for (u32 i=0, n=_GetItemCount(*m_death_bones_to_hide); i<n; ++i){
			bone = _GetItem(*m_death_bones_to_hide,i,I);
			bone_id		= K->LL_BoneID	(bone);
			K->LL_SetBoneVisible(bone_id,FALSE,TRUE);
		}

		///PPhysicsShell()=P_build_Shell	(this,false);
		PPhysicsShell()->EnabledCallbacks(TRUE);
		PPhysicsShell()->set_ObjectContactCallback(NULL);
	}
	Fvector lin_vel;

	Fvector prev_pos = PositionStack.front().vPosition;
	lin_vel.sub(XFORM().c,prev_pos);
	lin_vel.div((Device.dwTimeGlobal-PositionStack.front().dwTime)/1000.0f);

	lin_vel.mul(m_death_lin_vel_k);
	PPhysicsShell()->set_LinearVel(lin_vel);
	PPhysicsShell()->set_AngularVel(m_death_ang_vel);
	PPhysicsShell()->Enable();
	K->CalculateBones_Invalidate();
	K->CalculateBones();
	setState(CHelicopter::eDead);
	m_engineSound.stop();
	processing_deactivate();
}

void CHelicopter::gotoStayPoint(float time, Fvector* pos)
{
	if( !fis_zero(time) )
		m_data.m_time_delay_between_patrol = time;
		

	if(pos)
		m_data.m_stayPoint = *pos;
	
	setState(CHelicopter::eInitiateWaitBetweenPatrol);
}

void CHelicopter::goPatrol(float time)
{
	if( !fis_zero(time) )
		m_data.m_time_patrol_period = time;

	setState(CHelicopter::eInitiatePatrolZone);
}

void CHelicopter::goToPoint(Fvector* to, Fvector* via, float time)
{
	m_data.m_to_point = *to;
	m_data.m_via_point = *via;
	m_data.m_wait_in_point = time;

	if(time>61.0f)
		m_data.m_wait_in_point = 60.0f;

	setState(CHelicopter::eInitiateGoToPoint);
}

float CHelicopter::getLastPointTime	()
{
	return m_movMngr->EndTime();

}

void CHelicopter::goPatrolByPatrolPath (LPCSTR path_name, int start_idx)
{
	m_data.m_patrol_begin_idx = start_idx;
	m_data.m_patrol_path_name = path_name;
	setState(CHelicopter::eInitiatePatrolByPath);
}

void CHelicopter::SetDestPosition (Fvector* pos)
{
	Collide::rq_result		cR;
	Fvector down_dir;
	Fvector point = *pos;

	down_dir.set(0.0f, -1.0f, 0.0f);
	Fbox boundingVolume = Level().ObjectSpace.GetBoundingVolume();

	point.y = boundingVolume.max.y+EPS_L;

	Level().ObjectSpace.RayPick(point, down_dir, boundingVolume.max.y-boundingVolume.min.y+1.0f, Collide::rqtStatic, cR);
	
	point.y = point.y-cR.range;

	point.y += m_data.m_wrk_altitude;

	m_data.m_to_point = point;
}

float CHelicopter::GetCurrAltitude()
{
	return m_data.m_wrk_altitude;
}

void CHelicopter::SetCurrAltitude(float a)
{
	m_data.m_wrk_altitude = a;
}

float CHelicopter::GetCurrVelocity()
{
	return m_curLinearSpeed;
}

void CHelicopter::SetCurrVelocity(float v)
{
	m_maxLinearSpeed = v;
}

void CHelicopter::SetEnemy(CScriptGameObject* e)
{
	m_data.m_destEnemy = e->object();
}

float CHelicopter::GetDistanceToDestPosition()
{
	return m_data.m_desiredP.distance_to(m_currP);
}

float CHelicopter::GetRealAltitude()
{
	Collide::rq_result		cR;
	Fvector down_dir;

	down_dir.set(0.0f, -1.0f, 0.0f);


	Level().ObjectSpace.RayPick(XFORM().c, down_dir, 1000.0f, Collide::rqtStatic, cR);
	
	return cR.range;
}

void CHelicopter::PrepareDie ()
{
	if(m_pParticle)return;
	m_pParticle = xr_new<CParticlesObject>(*m_smoke_particle,false);

	Fvector zero_vector;
	zero_vector.set(0.f,0.f,0.f);
	m_pParticle->UpdateParent(m_particleXFORM, zero_vector );
	m_pParticle->Play();
}

void CHelicopter::UpdateHeliParticles	()
{
	if (m_pParticle){
		
		Fvector vel;

		Fvector last_pos = PositionStack.back().vPosition;
		vel.sub(Position(), last_pos);
		vel.mul(5.0f);

		m_pParticle->UpdateParent(m_particleXFORM, vel );
	}
//lighting
	if(m_light_render->get_active()){
		Fmatrix xf;
		Fmatrix& M = smart_cast<CKinematics*>(Visual())->LL_GetTransform(u16(m_light_bone));
		xf.mul		(XFORM(),M);
		VERIFY(!fis_zero(DET(xf)));

		m_light_render->set_rotation	(xf.k,xf.i);
		m_light_render->set_position	(xf.c);

		if (m_lanim)
		{
			int frame;
			u32 clr					= m_lanim->CalculateBGR(Device.fTimeGlobal,frame); // возвращает в формате BGR
			Fcolor					fclr;
			fclr.set				((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb			(m_light_brightness/255.f);
			m_light_render->set_color	(fclr);
		}

	}
}
void CHelicopter::Explode ()
{
	if(m_pParticle){
		m_pParticle->Stop();
		xr_delete(m_pParticle);
	}
	m_pParticle = xr_new<CParticlesObject>(*m_explode_particle,false);

	Fvector zero_vector;
	zero_vector.set(0.f,0.f,0.f);
	m_pParticle->UpdateParent(m_particleXFORM, zero_vector );

	m_explodeSound.play_at_pos(0,XFORM().c);
	m_explodeSound.set_position(XFORM().c);

	m_pParticle->Play();
}

float CHelicopter::GetfHealth() const 
{ 
	float f = CEntity::GetfHealth(); 
	return f;
}

float CHelicopter::SetfHealth(float value) {
	CEntity::SetfHealth(value);
	return value;
}

void CHelicopter::TurnLighting(bool bOn)
{
	m_light_render->set_active						(bOn);

}
#include "stdafx.h"
#include "helicopter.h"
#include "xrserver_objects_alife.h"
#include "PhysicsShell.h"
#include "CustomRocket.h"
#include "ExplosiveRocket.h"
#include "level.h"

CHelicopter::CHelicopter()
{
	init();
}

CHelicopter::~CHelicopter()
{
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

	case CHelicopter::eDead:
		str = "eDead";
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
	m_destEnemy					= 0;
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

#ifdef MOV_MANAGER_OLD
	m_time_delay_before_start	= 1000;//1 sec
	m_time_patrol_period		= 60000;//1 min
	m_time_delay_between_patrol	= 60000;//1 min
	m_velocity					= 25.0f;
	m_altitude					= 30.0f;
	m_attack_altitude			= m_altitude;
	m_korridor					= 10.0f;

	m_x_level_bound				= 0.0f;
	m_z_level_bound				= 0.0f;
	m_time_last_patrol_start	= 0;
	m_time_last_patrol_end		= 0;

	m_movementMngr.init(this);

#endif

	setState(CHelicopter::eIdleState);
	SetfHealth(100.0f);

	m_stayPos.set(0.0f, 0.0f, 0.0f);


	m_HitTypeK.resize(ALife::eHitTypeMax);

	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeK[i] = 1.0f;

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
	
#ifdef MOV_MANAGER_NEW
	m_movMngr.load (section);
#endif

	m_sAmmoType = pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load(*m_sAmmoType);

	m_sRocketSection						= pSettings->r_string	(section,"rocket_class");

	m_HitTypeK[ALife::eHitTypeBurn]			= pSettings->r_float(section,"burn_immunity");
	m_HitTypeK[ALife::eHitTypeStrike]		= pSettings->r_float(section,"strike_immunity");
	m_HitTypeK[ALife::eHitTypeShock]		= pSettings->r_float(section,"shock_immunity");
	m_HitTypeK[ALife::eHitTypeWound]		= pSettings->r_float(section,"wound_immunity");
	m_HitTypeK[ALife::eHitTypeRadiation]	= pSettings->r_float(section,"radiation_immunity");
	m_HitTypeK[ALife::eHitTypeTelepatic]	= pSettings->r_float(section,"telepatic_immunity");
	m_HitTypeK[ALife::eHitTypeChemicalBurn] = pSettings->r_float(section,"chemical_burn_immunity");
	m_HitTypeK[ALife::eHitTypeFireWound]	= pSettings->r_float(section,"fire_wound_immunity");
	m_HitTypeK[ALife::eHitTypeExplosion]	= pSettings->r_float(section,"explosion_immunity");

#ifdef MOV_MANAGER_OLD
	m_time_delay_before_start				= pSettings->r_u32(section,"time_delay_before_start")*1000;
	m_time_patrol_period					= pSettings->r_u32(section,"time_patrol_period")*1000;
	m_time_delay_between_patrol				= pSettings->r_u32(section,"time_delay_between_patrol")*1000;
	m_velocity								= pSettings->r_float(section,"velocity");
	m_altitude								= pSettings->r_float(section,"altitude");
	m_attack_altitude						= pSettings->r_float(section,"attack_altitude");
	m_korridor								= pSettings->r_float(section,"alt_korridor");
	m_x_level_bound							= pSettings->r_float(section,"x_level_bound");
	m_z_level_bound							= pSettings->r_float(section,"z_level_bound");
#endif

	m_use_rocket_on_attack					= pSettings->r_bool(section,"use_rocket");
	m_use_mgun_on_attack					= pSettings->r_bool(section,"use_mgun");
	m_min_rocket_dist						= pSettings->r_float(section,"min_rocket_attack_dist");
	m_max_rocket_dist						= pSettings->r_float(section,"max_rocket_attack_dist");
	m_time_between_rocket_attack			= pSettings->r_u32(section,"time_between_rocket_attack");
	m_syncronize_rocket						= (bool)pSettings->r_bool(section,"syncronize_rocket");

}

void CHelicopter::reload(LPCSTR section)
{
	inherited::reload	(section);
}

BOOL CHelicopter::net_Spawn(LPVOID	DC)
{
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

//	PPhysicsShell()=P_build_Shell	(this,true);

	CRocketLauncher::SpawnRocket(*m_sRocketSection, dynamic_cast<CGameObject*>(this/*H_Parent()*/));
	CRocketLauncher::SpawnRocket(*m_sRocketSection, dynamic_cast<CGameObject*>(this/*H_Parent()*/));
	CRocketLauncher::SpawnRocket(*m_sRocketSection, dynamic_cast<CGameObject*>(this/*H_Parent()*/));
	CRocketLauncher::SpawnRocket(*m_sRocketSection, dynamic_cast<CGameObject*>(this/*H_Parent()*/));

	// assigning m_animator here
	CSE_Abstract		*abstract	=(CSE_Abstract*)(DC);
	CSE_ALifeHelicopter	*heli		= dynamic_cast<CSE_ALifeHelicopter*>(abstract);
	VERIFY				(heli);

	R_ASSERT						(Visual()&&PKinematics(Visual()));
	CKinematics* K					= PKinematics(Visual());
	CInifile* pUserData				= K->LL_UserData();

	m_rotate_x_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_x_bone"));
	m_rotate_y_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_y_bone"));
	m_fire_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_fire_bone"));

	m_left_rocket_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","left_rocket_bone"));
	m_right_rocket_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","right_rocket_bone"));

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
	
	CBoneInstance& biX		= PKinematics(Visual())->LL_GetBoneInstance(m_rotate_x_bone);	
	biX.set_callback		(BoneMGunCallbackX,this);
	CBoneInstance& biY		= PKinematics(Visual())->LL_GetBoneInstance(m_rotate_y_bone);	
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
	
	CSkeletonAnimated	*A	= PSkeletonAnimated(Visual());
	if (A) {
		A->PlayCycle	(*heli->startup_animation);
		A->Calculate	();
	}
	m_engineSound.create(TRUE,*heli->engine_sound);
	m_engineSound.play_at_pos(0,XFORM().c,sm_Looped);

	CShootingObject::Light_Create();


	setVisible			(true);
	setEnabled			(true);

	setState			(eWaitForStart);
	m_stayPos			= XFORM().c;

#ifdef MOV_MANAGER_NEW
	m_movMngr.init		(XFORM());
#endif

	return				(TRUE);
}

void CHelicopter::net_Destroy()
{
	inherited::net_Destroy();
	CShootingObject::Light_Destroy();
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
	
#ifdef MOV_MANAGER_OLD
	m_movementMngr.onFrame( XFORM(),Device.fTimeDelta );
#endif

#ifdef MOV_MANAGER_NEW
	m_movMngr.getPathPosition (Level().timeServer()/1000.0f,Device.fTimeDelta, XFORM() );
#endif
/*	if( PPhysicsShell()&&(GetfHealth() < 99.97f) ){
		PPhysicsShell()->InterpolateGlobalTransform(&XFORM());
		return;
	}else{
		PPhysicsShell()->SetTransform(XFORM());
		PPhysicsShell()->Disable();
	}*/


	m_engineSound.set_position(XFORM().c);

	//weapon
	CKinematics* K	= PKinematics(Visual());
	K->Calculate	();

	MGunUpdateFire();

	if( m_curState==CHelicopter::eMovingByAttackTraj ){
		m_fire_bone_xform	= K->LL_GetTransform(m_fire_bone);

		m_fire_bone_xform.mulA(XFORM());
		m_fire_pos.set(0,0,0); 
		m_fire_bone_xform.transform_tiny(m_fire_pos);
		m_fire_dir.set(0,0,1); 
		m_fire_bone_xform.transform_dir(m_fire_dir);


		m_left_rocket_bone_xform	= K->LL_GetTransform(m_left_rocket_bone);
		m_left_rocket_bone_xform.mulA(XFORM());
		//	m_left_rocket_bone_xform.c.y += 1.0f;
		//.fake
		m_right_rocket_bone_xform	= K->LL_GetTransform(m_right_rocket_bone);
		m_right_rocket_bone_xform.mulA(XFORM());
		//	m_right_rocket_bone_xform.c.y += 1.0f;
		//.fake

		updateMGunDir();

	}else{
		m_tgt_x_rot = 0.0f;
		m_tgt_y_rot = 0.0f;
	};

	// lerp angle
	angle_lerp	(m_cur_x_rot, m_tgt_x_rot, PI, Device.fTimeDelta);
	angle_lerp	(m_cur_y_rot, m_tgt_y_rot, PI, Device.fTimeDelta);
}

void CHelicopter::shedule_Update(u32 time_delta)
{
	if (!getEnabled())	return;
	inherited::shedule_Update	(time_delta);
	
//	if( GetfHealth() >= 0.0f ){
#ifdef MOV_MANAGER_OLD
	m_movementMngr.shedule_Update(time_delta);
#endif

#ifdef MOV_MANAGER_NEW
	m_movMngr.shedule_Update (time_delta, this);
#endif
//	};

//	if ( GetfHealth() <= 0.0f && !PPhysicsShell() )
//		Die();

//	if( m_curState==CHelicopter::eWaitBetweenPatrol){
//		SetfHealth(100.0f);
//	};

	if( m_curState==CHelicopter::eMovingByAttackTraj ){
		m_destEnemy->Center(m_destEnemyPos);
		updateMGunDir();

		if(m_allow_fire){
			MGunFireStart();
			
			float d = XFORM().c.distance_to_xz(m_destEnemyPos);
			
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

		if( m_destEnemy->getDestroy() )	{
			m_destEnemy = 0;
			setState(CHelicopter::eInitiatePatrolZone);
		};

	}else
		MGunFireEnd();

	if(getRocketCount()<4)
		for(u32 i=getRocketCount(); i<4; ++i) {
			CRocketLauncher::SpawnRocket(*m_sRocketSection, this);
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
if(who==this)
	return;

	bonesIt It = m_hitBones.find(element);
	if(It != m_hitBones.end() && hit_type==ALife::eHitTypeFireWound) {
		float curHealth = GetfHealth();
		curHealth -= P*It->second*10.0f;
		SetfHealth(curHealth);

		Log("----Helicopter::PilotHit(). type=",hit_type);
		Log("----Helicopter::PilotHit(). power=",P);
		Log("----Helicopter::PilotHit(). health=",curHealth);
		Log("----Helicopter::PilotHit(). k=",It->second);
		Log("----------------------------------------");

	}else {
		float hit_power		= P/100.f;
		hit_power			*= m_HitTypeK[hit_type];

		SetfHealth(GetfHealth()-hit_power);
		float h= GetfHealth();
		Log("----Helicopter::Hit(). type=",hit_type);
		Log("----Helicopter::Hit(). power=",hit_power);
		Log("----Helicopter::Hit(). health=",h);
		Log("----Helicopter::Hit(). k=",m_HitTypeK[hit_type]);
		Log("----------------------------------------");
	};
	
	if ( GetfHealth() <= 0.0f ) {
		Die();
		return;
	}

	CGameObject* GO = dynamic_cast<CGameObject*>(who);
	if (GO){
		switch (GO->SUB_CLS_ID){
			case CLSID_OBJECT_ACTOR: 
				doHunt(who);
//				doHunt2(who);
			break;
		default:
			break;
		}
	}

}

void CHelicopter::doHunt(CObject* dest)
{
	VERIFY(this != dest);

	if( m_curState==CHelicopter::eMovingByPatrolZonePath ){


		m_destEnemy		= dest;
		dest->Center	(m_destEnemyPos);

	/*	updateMGunDir();
		Log("CHelicopter::doHunt allow=",m_allow_fire);
		Log("CHelicopter::doHunt m_cur_x_rot=",m_cur_x_rot);
		Log("CHelicopter::doHunt m_tgt_x_rot=",m_tgt_x_rot);

		Log("CHelicopter::doHunt m_cur_y_rot=",m_cur_y_rot);
		Log("CHelicopter::doHunt m_tgt_y_rot=",m_tgt_y_rot);
	*/
		setState(CHelicopter::eInitiateHunt);

	}
}

void CHelicopter::doHunt2(CObject* dest)
{
	if( m_curState==CHelicopter::eMovingByPatrolZonePath ){

		m_destEnemy		= dest;
		dest->Center	(m_destEnemyPos);
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
//	PPhysicsShell()=P_build_Shell	(this,false);
//	setState(CHelicopter::eInitiateWaitBetweenPatrol);
//	setState(CHelicopter::eDead);
	SetfHealth(100.0f);
//	StartParticles();
}
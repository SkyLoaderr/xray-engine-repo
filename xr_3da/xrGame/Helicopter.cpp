#include "stdafx.h"
#include "helicopter.h"
#include "xrserver_objects_alife.h"
#include "PhysicsShell.h"
#include "CustomRocket.h"
#include "ExplosiveRocket.h"

CHelicopter::CHelicopter()
{
	init();
}

CHelicopter::~CHelicopter()
{
}

CHelicopter::EHeliState			
CHelicopter::state()
{
	return m_curState;
}

void							
CHelicopter::setState(CHelicopter::EHeliState s)
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
/*	case CHelicopter::eMovingToAttackTraj:
		str = "eMovingToAttackTraj";
		break;*/
	case CHelicopter::eMovingByAttackTraj:
		str = "eMovingByAttackTraj";
		break;
	case CHelicopter::eInitiatePatrolZone:
		str = "eInitiatePatrolZone";
		break;
/*	case CHelicopter::eInitiateAttackTraj:
		str = "eInitiateAttackTraj";
		break;*/

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

	default:
		str = "unknown";
		break;
	};

	Msg("---CHelicopter::state[%d]==(%s)", Device.dwTimeGlobal, str);
}


//CAI_ObjectLocation
void				
CHelicopter::init()
{
	m_destEnemy = 0;
//	m_velocity = 5.0f;
	m_velocity = 25.0f;
	m_altitude = 40.0f;
	m_cur_x_rot = 0.0f;
	m_cur_y_rot = 0.0f;
	m_tgt_x_rot = 0.0f;
	m_tgt_y_rot = 0.0f;
	m_bind_x_rot= 0.f;
	m_bind_y_rot= 0.f;
	m_maxFireDist = 100.0f;
	m_allow_fire= FALSE;
	m_movementMngr.init(this);
	setState(CHelicopter::eIdleState);
	SetfHealth(100.0f);

	m_stayPos.set(-200.0f, 30.0f, -200.0f);


	m_time_delay_before_start	= 1000;//1 sec
	m_time_patrol_period		= 60000;//1 min
	m_time_delay_between_patrol	= 20000;//20 sec

	m_time_last_patrol_start	= 0;
	m_time_last_patrol_end		= 0;

	m_HitTypeK.resize(ALife::eHitTypeMax);

	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeK[i] = 1.0f;

//	FireStart();
}

void		
CHelicopter::reinit()
{
	inherited::reinit	();
}


//CGameObject
void		
CHelicopter::Load(LPCSTR section)
{
	inherited::Load			(section);
	CShootingObject::Load	(section);
	CRocketLauncher::Load	(section);

	m_sAmmoType = pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load(*m_sAmmoType);

	m_sRocketSection		= pSettings->r_string	(section,"rocket_class");

	m_HitTypeK[ALife::eHitTypeBurn]			= pSettings->r_float(section,"burn_immunity");
	m_HitTypeK[ALife::eHitTypeStrike]		= pSettings->r_float(section,"strike_immunity");
	m_HitTypeK[ALife::eHitTypeShock]		= pSettings->r_float(section,"shock_immunity");
	m_HitTypeK[ALife::eHitTypeWound]		= pSettings->r_float(section,"wound_immunity");
	m_HitTypeK[ALife::eHitTypeRadiation]	= pSettings->r_float(section,"radiation_immunity");
	m_HitTypeK[ALife::eHitTypeTelepatic]	= pSettings->r_float(section,"telepatic_immunity");
	m_HitTypeK[ALife::eHitTypeChemicalBurn] = pSettings->r_float(section,"chemical_burn_immunity");
	m_HitTypeK[ALife::eHitTypeFireWound]	= pSettings->r_float(section,"fire_wound_immunity");
	m_HitTypeK[ALife::eHitTypeExplosion]	= pSettings->r_float(section,"explosion_immunity");

}

void		
CHelicopter::reload(LPCSTR section)
{
	inherited::reload	(section);
}

BOOL		
CHelicopter::net_Spawn(LPVOID	DC)
{
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	CRocketLauncher::SpawnRocket(*m_sRocketSection, dynamic_cast<CGameObject*>(this/*H_Parent()*/));

	// assigning m_animator here
	CSE_Abstract		*abstract=(CSE_Abstract*)(DC);
	CSE_ALifeHelicopter	*heli	= dynamic_cast<CSE_ALifeHelicopter*>(abstract);
	VERIFY				(heli);

	R_ASSERT			(Visual()&&PKinematics(Visual()));
	CKinematics* K		= PKinematics(Visual());
	CInifile* pUserData	= K->LL_UserData();

	m_rotate_x_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_x_bone"));
	m_rotate_y_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_y_bone"));
	m_fire_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_fire_bone"));

	LPCSTR s = pUserData->r_string("helicopter_definition","hit_section");

	if( pUserData->section_exist(s) )
	{
		int lc = pUserData->line_count(s);
		LPCSTR name;
		LPCSTR value;
		s16 boneID;
		for (int i=0 ;i<lc; ++i) 
		{
			pUserData->r_line( s, i, &name, &value);
			boneID=K->LL_BoneID(name);
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


	setVisible			(true);
	setEnabled			(true);

	setState			(eWaitForStart);
	m_stayPos			= XFORM().c;
	return				(TRUE);
}

void		
CHelicopter::net_Destroy()
{
	inherited::net_Destroy();
}

void		
CHelicopter::net_Export(NET_Packet &P)
{
}

void		
CHelicopter::net_Import (NET_Packet &P)
{
}

void		
CHelicopter::renderable_Render()
{
	inherited::renderable_Render();
}

void		
CHelicopter::UpdateCL()
{
	inherited::UpdateCL	();
	
	if(PPhysicsShell())
	{
		PPhysicsShell()->InterpolateGlobalTransform(&XFORM());
		return;
	};

	m_movementMngr.onFrame( XFORM(),Device.fTimeDelta );

	m_engineSound.set_position(XFORM().c);

	//weapon
	CKinematics* K	= PKinematics(Visual());
	K->Calculate	();

	m_fire_bone_xform	= K->LL_GetTransform(m_fire_bone);

	m_fire_bone_xform.mulA(XFORM());
	m_fire_pos.set(0,0,0); 
	m_fire_bone_xform.transform_tiny(m_fire_pos);
	m_fire_dir.set(0,0,1); 
	m_fire_bone_xform.transform_dir(m_fire_dir);

	UpdateFire();

	if( state()==CHelicopter::eMovingByAttackTraj	|| 
		state()==CHelicopter::eInitiateHunt			/*||
		state()==CHelicopter::eInitiateAttackTraj	||
		state()==CHelicopter::eMovingToAttackTraj		*/)
	{
		updateMGunDir();

		if(!m_destEnemy || m_destEnemy->getDestroy() )
		{
			m_destEnemy = 0;
			setState(CHelicopter::eInitiatePatrolZone);
		};
	}
	// lerp angle
	angle_lerp	(m_cur_x_rot, m_tgt_x_rot, PI, Device.fTimeDelta);
	angle_lerp	(m_cur_y_rot, m_tgt_y_rot, PI, Device.fTimeDelta);
}

void		
CHelicopter::shedule_Update(u32	time_delta)
{
	if (!getEnabled())	return;
	inherited::shedule_Update	(time_delta);
	
	if( GetfHealth() >= 0.0f )
	{
		m_movementMngr.shedule_Update(time_delta);
	};

	if ( GetfHealth() <= 0.0f && !PPhysicsShell() )
	{//die
		PPhysicsShell()=P_build_Shell	(this,false);
	}

	if( state()==CHelicopter::eMovingByAttackTraj )
	{
		m_destEnemy->Center(m_destEnemyPos);
		updateMGunDir();
		
		if(m_allow_fire)
		{
			FireStart();
			
			if(m_pRocket)
			{
				CExplosiveRocket* pGrenade = dynamic_cast<CExplosiveRocket*>(m_pRocket);
				VERIFY(pGrenade);
				pGrenade->SetCurrentParentID(this->ID());
				LaunchRocket(ParticlesXFORM(),  m_fire_dir, m_fire_dir);
			}
		}else
			FireEnd();

	}else
		FireEnd();

	if(!m_pRocket)
		CRocketLauncher::SpawnRocket(*m_sRocketSection, this);
}

void		
CHelicopter::Hit(	float P, 
					Fvector &dir, 
					CObject* who, 
					s16 element, 
					Fvector position_in_bone_space, 
					float impulse,  
					ALife::EHitType hit_type/* = ALife::eHitTypeWound*/)
{
/*	if(hit_type != ALife::eHitTypeFireWound)
		return;*/

	//нормализуем силу удара -------
	float hit_power		= P/100.f;
//	hit_power			= HitOutfitEffect(hit_power, hit_type);
	hit_power			*= m_HitTypeK[hit_type];
//-----------

	bonesIt It = m_hitBones.find(element);
	if(It != m_hitBones.end())
	{
		float curHealth = GetfHealth();
				curHealth -= P*5.0f*It->second;

		SetfHealth(curHealth);
	}else
//		CEntity::Hit(P,dir,who,element,position_in_bone_space,impulse,hit_type );
	{
		SetfHealth(GetfHealth()-hit_power);
//		SetfHealth(-0.5f);
		float h= GetfHealth();
		Log("----Helicopter::Hit(). health=%f",h);
	};

	CGameObject* GO = dynamic_cast<CGameObject*>(who);
	if (GO){
		switch (GO->SUB_CLS_ID){
			case CLSID_OBJECT_ACTOR: 
				doHunt(who);
			break;
		default:
			break;
		}
	}

}

void					
CHelicopter::doHunt(CObject* dest)
{
	if( state()==CHelicopter::eInitiateHunt || 
		/*state()==CHelicopter::eMovingToAttackTraj ||*/
		state()==CHelicopter::eMovingByAttackTraj)
		return;

	m_destEnemy = dest;
	dest->Center	(m_destEnemyPos);

	setState(CHelicopter::eInitiateHunt);
//
//		m_movementMngr.buildHuntPath(dest->XFORM().c);
}

void 
CHelicopter::OnEvent(NET_Packet& P, u16 type) 
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
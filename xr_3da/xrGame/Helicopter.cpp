#include "stdafx.h"
#include "helicopter.h"
#include "xrserver_objects_alife.h"
#include "PhysicsShell.h"
#include "level.h"
#include "ai_sounds.h"
#include "clsid_game.h"
#include "../skeletonanimated.h"
#include "script_callback_ex.h"
#include "game_object_space.h"
#include "script_game_object.h"
#include "../LightAnimLibrary.h"
#include "MainUI.h"
#include "HudManager.h"
#include "physicscommon.h"
//50fps fixed
float STEP=0.02f;



CHelicopter::CHelicopter()
{
	init();
	m_pParticle		= NULL;
	m_light_render	= NULL;
	m_lanim			= NULL;

	ISpatial*		self				=	smart_cast<ISpatial*> (this);
//	if (self)		self->spatial.type  &= ~STYPE_VISIBLEFORAI;
	if (self)		self->spatial.type  |=  STYPE_VISIBLEFORAI;

	m_movement.parent	= this;
	m_body.parent		= this;
}

CHelicopter::~CHelicopter()
{
	HUD_SOUND::DestroySound		(m_sndShot);
	HUD_SOUND::DestroySound		(m_sndShotRocket);
	

}

void CHelicopter::setState(CHelicopter::EHeliState s)
{
	m_curState = s;
	
#ifdef DEBUG
	LPCSTR str;
	switch			(m_curState) {
	case CHelicopter::eAlive:			str = "eAlive";				break;
	case CHelicopter::eDead:				str = "eDead";					break;
	default:								str = "unknown";				break;
	};
	if (bDebug)	Msg("---CHelicopter::state[%d]==(%s)", Device.dwTimeGlobal, str);
#endif
}



void CHelicopter::init()
{
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

	SetfHealth					(100.0f);
}

void CHelicopter::reinit(){
	inherited::reinit	();
	m_movement.reinit	();
	m_body.reinit		(); 
	m_enemy.reinit		();
};


void CHelicopter::Load(LPCSTR section)
{
	inherited::Load						(section);
	m_movement.Load						(section);
	m_body.Load							(section);
	m_enemy.Load						(section);


	m_death_ang_vel						= pSettings->r_fvector3(section,"death_angular_vel");
	m_death_lin_vel_k					= pSettings->r_float(section,"death_lin_vel_koeff");





	CHitImmunity::LoadImmunities		(section);

	//weapons
	CShootingObject::Load				(section);
	HUD_SOUND::LoadSound				(section,"snd_shoot", m_sndShot, TRUE, SOUND_TYPE_WEAPON_SHOOTING);
	HUD_SOUND::LoadSound				(section,"snd_shoot_rocket", m_sndShotRocket, TRUE, SOUND_TYPE_WEAPON_SHOOTING);
	CRocketLauncher::Load				(section);

	UseFireTrail						(m_enemy.bUseFireTrail);//temp force reloar disp params

	m_sAmmoType							= pSettings->r_string(section, "ammo_class");
	m_CurrentAmmo.Load					(*m_sAmmoType);

	m_sRocketSection					= pSettings->r_string	(section,"rocket_class");


	m_use_rocket_on_attack				= !!pSettings->r_bool(section,"use_rocket");
	m_use_mgun_on_attack				= !!pSettings->r_bool(section,"use_mgun");
	m_min_rocket_dist					= pSettings->r_float(section,"min_rocket_attack_dist");
	m_max_rocket_dist					= pSettings->r_float(section,"max_rocket_attack_dist");
	m_min_mgun_dist						= pSettings->r_float(section,"min_mgun_attack_dist");
	m_max_mgun_dist						= pSettings->r_float(section,"max_mgun_attack_dist");
	m_time_between_rocket_attack		= pSettings->r_u32(section,"time_between_rocket_attack");
	m_syncronize_rocket					= !!pSettings->r_bool(section,"syncronize_rocket");
	m_barrel_dir_tolerance				= pSettings->r_float(section,"barrel_dir_tolerance");

//lighting & sounds
	m_smoke_particle					= pSettings->r_string(section,"smoke_particle");
	m_explode_particle					= pSettings->r_string(section,"explode_particle");

	shared_str expl_snd					= pSettings->r_string	(section,"explode_sound");
	m_explodeSound.create				(TRUE,*expl_snd);

	m_light_range						= pSettings->r_float(section,"light_range");
	m_light_brightness					= pSettings->r_float(section,"light_brightness");

	m_light_color						= pSettings->r_fcolor(section,"light_color");
	m_light_color.a						= 1.f;
	m_light_color.mul_rgb				(m_light_brightness);
	LPCSTR lanim						= pSettings->r_string	(section,"light_color_animmator");
	m_lanim								= LALib.FindItem(lanim);


}

void CHelicopter::reload(LPCSTR section)
{
	inherited::reload	(section);
}

void CollisionCallbackAlife(bool& do_colide,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{	do_colide=false; }
void ContactCallbackAlife(CDB::TRI* T,dContactGeom* c)
{
}
BOOL CHelicopter::net_Spawn(CSE_Abstract*	DC)
{

	SetfHealth(100.0f);
	setState(CHelicopter::eAlive);
	m_flame_started					=false;
	m_light_started					=false;
	m_exploded						=false;
	m_dead							=false;
	
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	CPHSkeleton::Spawn((CSE_Abstract*)(DC));
	for(u32 i=0; i<4; ++i)
		CRocketLauncher::SpawnRocket(*m_sRocketSection, smart_cast<CGameObject*>(this));

	// assigning m_animator here
	CSE_Abstract		*abstract	=(CSE_Abstract*)(DC);
	CSE_ALifeHelicopter	*heli		= smart_cast<CSE_ALifeHelicopter*>(abstract);
	VERIFY				(heli);

	R_ASSERT						(Visual()&&smart_cast<CKinematics*>(Visual()));
	CKinematics* K					= smart_cast<CKinematics*>(Visual());
	CInifile* pUserData				= K->LL_UserData();

	m_rotate_x_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_x_bone"));
	m_rotate_y_bone			= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_rotate_y_bone"));
	m_fire_bone				= K->LL_BoneID	(pUserData->r_string("helicopter_definition","wpn_fire_bone"));
	m_death_bones_to_hide	= pUserData->r_string("on_death_mode","scale_bone");
	m_left_rocket_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","left_rocket_bone"));
	m_right_rocket_bone		= K->LL_BoneID	(pUserData->r_string("helicopter_definition","right_rocket_bone"));

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
	biX.set_callback		(bctCustom,BoneMGunCallbackX,this);
	CBoneInstance& biY		= smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(m_rotate_y_bone);	
	biY.set_callback		(bctCustom,BoneMGunCallbackY,this);
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

	m_engineSound.create			(TRUE,*heli->engine_sound);
	m_engineSound.play_at_pos		(0,XFORM().c,sm_Looped);
	
	CShootingObject::Light_Create	();


	setVisible						(true);
	setEnabled						(true);



	m_stepRemains						= 0.0f;

//lighting
	m_light_render						= ::Render->light_create();
	m_light_render->set_shadow			(false);
	m_light_render->set_type			(IRender_Light::POINT);
	m_light_render->set_range			(m_light_range);
	m_light_render->set_color			(m_light_color);

	if(g_Alive())processing_activate	();
	TurnEngineSound(false);

#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW-1);
#endif

	return TRUE;
}

void CHelicopter::net_Destroy()
{
	inherited::net_Destroy				();
	CShootingObject::Light_Destroy		();
	CShootingObject::StopFlameParticles	();
	CPHSkeleton::RespawnInit			();
	m_engineSound.stop					();
	CParticlesObject::Destroy			(m_pParticle);
	m_light_render.destroy				();
	m_movement.net_Destroy				();
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif

}

void	CHelicopter::SpawnInitPhysics	(CSE_Abstract	*D)	
{
	
	PPhysicsShell()=P_build_Shell	(this,false);
	if(g_Alive())
	{
		PPhysicsShell()->EnabledCallbacks				(FALSE);
		PPhysicsShell()->set_ObjectContactCallback		(CollisionCallbackAlife);
		PPhysicsShell()->set_ContactCallback			(ContactCallbackAlife);
		PPhysicsShell()->Disable						();
	}
}

void	CHelicopter::net_Save			(NET_Packet& P)	
{
	inherited::net_Save					(P);
	CPHSkeleton::SaveNetState			(P);
}

float GetCurrAcc(float V0, float V1, float dist, float a0, float a1);

void CHelicopter::MoveStep()
{
	Fvector dir, pathDir;
	float desired_H = m_movement.currPathH;
	float desired_P;
	if(m_movement.type != eMovNone){

		float dist = m_movement.currP.distance_to(m_movement.desiredPoint);

		dir.sub(m_movement.desiredPoint,m_movement.currP);
		dir.normalize_safe();
		pathDir = dir;
		dir.getHP(desired_H, desired_P);
		

		if(angle_difference(m_movement.currPathH,desired_H)>PI_DIV_3)
			m_movement.curLinearAcc = -m_movement.LinearAcc_bk;
		else
			m_movement.curLinearAcc = GetCurrAcc(	m_movement.curLinearSpeed,
													m_movement.GetSpeedInDestPoint(),
													dist*0.95f,
													m_movement.LinearAcc_fw,
													-m_movement.LinearAcc_bk);


		angle_lerp	(m_movement.currPathH, desired_H, m_movement.GetAngSpeedHeading(m_movement.curLinearSpeed), STEP);
		angle_lerp	(m_movement.currPathP, desired_P, m_movement.GetAngSpeedPitch(m_movement.curLinearSpeed), STEP);
		
		dir.setHP(m_movement.currPathH, m_movement.currPathP);

		float vp = m_movement.curLinearSpeed*STEP+(m_movement.curLinearAcc*STEP*STEP)/2.0f;
		m_movement.currP.mad	(dir, vp);
		m_movement.curLinearSpeed += m_movement.curLinearAcc*STEP;
		
		clamp(m_movement.curLinearSpeed,0.0f,m_movement.maxLinearSpeed);
	}else{ //go stopping
		if( !fis_zero(m_movement.curLinearSpeed) ){
			m_movement.curLinearAcc = -m_movement.LinearAcc_bk;

			float vp = m_movement.curLinearSpeed*STEP+(m_movement.curLinearAcc*STEP*STEP)/2.0f;
			dir.setHP(m_movement.currPathH, m_movement.currPathP);
			dir.normalize_safe();
			m_movement.currP.mad	(dir, vp);
			m_movement.curLinearSpeed += m_movement.curLinearAcc*STEP;
			clamp(m_movement.curLinearSpeed,0.0f,m_movement.maxLinearSpeed);

		}else{
			m_movement.curLinearAcc		= 0.0f;
			m_movement.curLinearSpeed	= 0.0f;
		}
		
	};

	if(	m_body.b_looking_at_point){
		Fvector desired_dir;
		desired_dir.sub(m_body.looking_point, m_movement.currP ).normalize_safe();

		float center_desired_H,tmp_P;
		desired_dir.getHP(center_desired_H, tmp_P);
		angle_lerp			(m_body.currBodyH, center_desired_H, m_movement.GetAngSpeedHeading(m_movement.curLinearSpeed), STEP);
	}else{
		angle_lerp			(m_body.currBodyH, m_movement.currPathH, m_movement.GetAngSpeedHeading(m_movement.curLinearSpeed), STEP);
	}


	float needBodyP = -m_body.model_pitch_k*m_movement.curLinearSpeed;
	if(m_movement.curLinearAcc < 0) needBodyP*=-1;
	angle_lerp	(m_body.currBodyP, needBodyP, m_body.model_angSpeedPitch, STEP);


	float sign;
	Fvector cp;
	cp.crossproduct (pathDir,dir);
	(cp.y>0.0)?sign=1.0f:sign=-1.0f;
	float ang_diff = angle_difference (m_movement.currPathH, desired_H);
	
	float needBodyB = -ang_diff*sign*m_body.model_bank_k*m_movement.curLinearSpeed;
	angle_lerp	(m_body.currBodyB, needBodyB, m_body.model_angSpeedBank, STEP);
	

	XFORM().setHPB(m_body.currBodyH,m_body.currBodyP,m_body.currBodyB);

	XFORM().translate_over(m_movement.currP);
}

void CHelicopter::UpdateCL()
{
	inherited::UpdateCL	();
	if(PPhysicsShell() && (state() == CHelicopter::eDead) ){

		PPhysicsShell()->InterpolateGlobalTransform(&XFORM());

		CKinematics* K		= smart_cast<CKinematics*>(Visual());
		K->CalculateBones	();
		//smoke
		UpdateHeliParticles();
		return;
	}
	else
		PPhysicsShell()->SetTransform(XFORM());

	m_movement.Update();

	m_stepRemains+=Device.fTimeDelta;
	while(m_stepRemains>STEP){
		MoveStep();
		m_stepRemains-=STEP;
	}
#ifdef DEBUG
	if(bDebug){
		CGameFont* F		= UI()->Font()->pFontDI;
		F->SetAligment		(CGameFont::alCenter);
		F->SetSizeI			(0.02f);
		F->OutSetI			(0.f,-0.8f);
		F->SetColor			(0xffffffff);
		F->OutNext			("Heli: speed=%4.4f acc=%4.4f dist=%4.4f",m_movement.curLinearSpeed, m_movement.curLinearAcc, m_movement.GetDistanceToDestPosition());
	}
#endif

///////////////////////////////////////////////////////////////////////////
	m_engineSound.set_position(XFORM().c);


	m_enemy.Update();
	//weapon
	UpdateWeapons();
	UpdateHeliParticles();

	CKinematics* K		= smart_cast<CKinematics*>(Visual());
	K->CalculateBones	();
}

void CHelicopter::shedule_Update(u32 time_delta)
{
	if (!getEnabled())	return;

	inherited::shedule_Update	(time_delta);
	CPHSkeleton::Update			(time_delta);
	
	if(state() != CHelicopter::eDead){
		for(u32 i=getRocketCount(); i<4; ++i)
			CRocketLauncher::SpawnRocket(*m_sRocketSection, this);
	}
}



void CHelicopter::goPatrolByPatrolPath (LPCSTR path_name, int start_idx)
{
	m_movement.goPatrolByPatrolPath (path_name, start_idx);
}


void CHelicopter::goByRoundPath(Fvector center, float radius, bool clockwise)
{
	m_movement.goByRoundPath(center, radius, clockwise);
}

void CHelicopter::LookAtPoint(Fvector point, bool do_it)
{
	m_body.LookAtPoint(point,do_it);
}

void CHelicopter::save(NET_Packet &output_packet)
{
	m_movement.save	(output_packet);
	m_body.save		(output_packet);
	m_enemy.save	(output_packet);
	output_packet.w_vec3(XFORM().c);
	output_packet.w_float(m_barrel_dir_tolerance);
}

void CHelicopter::load(IReader &input_packet)
{
	m_movement.load				(input_packet);
	m_body.load					(input_packet);
	m_enemy.load				(input_packet);
	input_packet.r_fvector3		(XFORM().c);
	m_barrel_dir_tolerance		= input_packet.r_float();
	UseFireTrail				(m_enemy.bUseFireTrail);//force reloar disp params
}

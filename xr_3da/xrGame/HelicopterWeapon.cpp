#include "stdafx.h"
#include "helicopter.h"
#include "ExplosiveRocket.h"
#include "xrMessages.h"
#include "net_utils.h"
#include "../skeletoncustom.h"
#include "Level.h"

void __stdcall 
CHelicopter::BoneMGunCallbackX(CBoneInstance *B)
{
	CHelicopter	* P = static_cast<CHelicopter*>(B->Callback_Param);
	Fmatrix rX;		rX.rotateX		(P->m_cur_x_rot);
	B->mTransform.mulB(rX);
}

void __stdcall 
CHelicopter::BoneMGunCallbackY(CBoneInstance *B)
{
	CHelicopter	* P = static_cast<CHelicopter*>(B->Callback_Param);
	Fmatrix rY;		rY.rotateY		(P->m_cur_y_rot);
	B->mTransform.mulB(rY);
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

void CHelicopter::MGunUpdateFire()
{

	fTime -= Device.fTimeDelta;


	CShootingObject::UpdateFlameParticles();
	CShootingObject::UpdateLight();

	if(!IsWorking()) {
		if(fTime<0) fTime = 0.f;
		return;
	}

	if(fTime<=0) {
		OnShot();
		fTime += fTimeToFire;
	}

}
void CHelicopter::OnShot		()
{
	Fvector fire_pos,fire_dir;
	fire_pos = get_CurrentFirePoint();
	fire_dir = m_fire_dir;

	float fire_trail_speed		= GetCurrVelocity()/2.0f;
	clamp(fire_trail_speed,3.0f,300.0f);
	if(m_enemy.bUseFireTrail){
		Fvector enemy_pos = m_enemy.destEnemyPos;

		float	dt		= Device.fTimeGlobal - m_enemy.fStartFireTime; VERIFY(dt>=0);
		float	dist	= m_enemy.fire_trail_length - dt*fire_trail_speed;
		if(dist<0)		return;

		Fvector fp		= fire_pos;
		fp.y			= enemy_pos.y;
		Fvector	fd;
		fd.sub(enemy_pos,fp).normalize_safe();
		if(dist > (m_enemy.fire_trail_length/2.0f) ){
			fd.mul(-1.0f);
			dist = dist - (m_enemy.fire_trail_length/2.0f);
		}else{
			dist = (m_enemy.fire_trail_length/2.0f) - dist;
		}
		

		static float fire_trace_width = pSettings->r_float(*cNameSect(),"fire_trace_width");
		enemy_pos.mad(fd,dist);
		Fvector disp_dir;
		disp_dir.random_point(fire_trace_width);

		enemy_pos.add(disp_dir);
		fire_dir.sub(enemy_pos,fire_pos).normalize_safe();
	};

	FireBullet(fire_pos, fire_dir, fireDispersionBase, m_CurrentAmmo, ID(), ID(), OnServer());

	StartShotParticles	();
	if(m_bShotLight) Light_Start();


	StartFlameParticles		();
	StartSmokeParticles		(fire_pos, zero_vel);
	OnShellDrop				(fire_pos, zero_vel);

	HUD_SOUND::PlaySound	(m_sndShot, fire_pos, this, false);

}

void CHelicopter::MGunFireStart()
{
	if(!m_use_mgun_on_attack)
		return;

	if(FALSE==IsWorking() && m_enemy.bUseFireTrail){
		//start calc fire trail
		m_enemy.fStartFireTime			= Device.fTimeGlobal;
	}

	CShootingObject::FireStart	();
}

void CHelicopter::MGunFireEnd()
{
	CShootingObject::FireEnd	();
	StopFlameParticles			();
	m_enemy.fStartFireTime		= -1.0f;
}

bool between(const float& src, const float& min, const float& max){return( (src>min)&&(src<max));}

void CHelicopter::UpdateWeapons		()
{
	if( isOnAttack() ){
		UpdateMGunDir();
	}else{
		m_tgt_x_rot = 0.0f;
		m_tgt_y_rot = 0.0f;
	};

	// lerp angle
	angle_lerp	(m_cur_x_rot, m_tgt_x_rot, PI, Device.fTimeDelta);
	angle_lerp	(m_cur_y_rot, m_tgt_y_rot, PI, Device.fTimeDelta);

	if( isOnAttack() ){

		if(m_allow_fire){
			
			float d = XFORM().c.distance_to_xz(m_enemy.destEnemyPos);
			
			if( between(d,m_min_mgun_dist,m_max_mgun_dist) )
					MGunFireStart();
			
			if( between(d,m_min_rocket_dist,m_max_rocket_dist) &&
				(Device.dwTimeGlobal-m_last_rocket_attack > m_time_between_rocket_attack) ) {
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

	}else
		MGunFireEnd();

	MGunUpdateFire();
}

void CHelicopter::UpdateMGunDir()
{
	CKinematics* K		= smart_cast<CKinematics*>(Visual());
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

	m_allow_fire		= TRUE;
	Fmatrix XFi;
	XFi.invert			(XFORM());
	Fvector dep;
	XFi.transform_tiny	(dep,m_enemy.destEnemyPos);
	{// x angle
		Fvector A_;		A_.sub(dep,m_bind_x);	m_i_bind_x_xform.transform_dir(A_); A_.normalize();
		m_tgt_x_rot		= angle_normalize_signed(m_bind_x_rot-A_.getP());
		float sv_x		= m_tgt_x_rot;
		clamp			(m_tgt_x_rot,-m_lim_x_rot.y,-m_lim_x_rot.x);
		if (!fsimilar(sv_x,m_tgt_x_rot,EPS_L)) m_allow_fire=FALSE;
	}
	{// y angle
		Fvector A_;		A_.sub(dep,m_bind_y);	m_i_bind_y_xform.transform_dir(A_); A_.normalize();
		m_tgt_y_rot		= angle_normalize_signed(m_bind_y_rot-A_.getH());
		float sv_y		= m_tgt_y_rot;
		clamp			(m_tgt_y_rot,-m_lim_y_rot.y,-m_lim_y_rot.x);
		if (!fsimilar(sv_y,m_tgt_y_rot,EPS_L)) m_allow_fire=FALSE;
	}
	
	Fvector d1,d2;
	d1.set(m_cur_x_rot,m_cur_y_rot,0).normalize_safe();
	d2.set(m_tgt_x_rot,m_tgt_y_rot,0).normalize_safe();
	
	if ( rad2deg(acos(d1.dotproduct(d2))) > m_barrel_dir_tolerance )
		m_allow_fire=FALSE;

}

void CHelicopter::startRocket(u16 idx)
{
	if((getRocketCount()>=1)&&m_use_rocket_on_attack) {
		CExplosiveRocket* pGrenade = smart_cast<CExplosiveRocket*>(getCurrentRocket());
		VERIFY(pGrenade);
		pGrenade->SetInitiator(this->ID());
		
		Fmatrix rocketXFORM;
		(idx==1)?rocketXFORM=m_left_rocket_bone_xform:rocketXFORM=m_right_rocket_bone_xform;

		Fvector vel;
		Fvector dir;
		dir.sub(m_enemy.destEnemyPos, rocketXFORM.c ).normalize_safe();
		vel.mul(dir,CRocketLauncher::m_fLaunchSpeed);

		Fmatrix xform;
		xform.identity();
		xform.k.set(dir);
		Fvector::generate_orthonormal_basis(xform.k,xform.j,xform.i);
		xform.c = rocketXFORM.c;
		VERIFY2(_valid(xform),"CHelicopter::startRocket. Invalid xform");
		LaunchRocket(xform,  vel, zero_vel);

		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16(u16( getCurrentRocket()->ID()));
		u_EventSend(P);

		dropCurrentRocket();

		m_last_launched_rocket = idx;
		HUD_SOUND::PlaySound(m_sndShotRocket, xform.c, this, false);

	}
}

const Fmatrix& CHelicopter::get_ParticlesXFORM()

{
	return m_fire_bone_xform;
}

const Fvector&	CHelicopter::get_CurrentFirePoint()
{
	return m_fire_pos;
}

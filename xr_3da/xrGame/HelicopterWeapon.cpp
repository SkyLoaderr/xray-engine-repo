#include "stdafx.h"
#include "helicopter.h"
#include "ExplosiveRocket.h"
#include "xrMessages.h"

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


const Fmatrix& CHelicopter::ParticlesXFORM() const
{
	return m_fire_bone_xform;
}

IRender_Sector* CHelicopter::Sector()
{
	return 0;
}

const Fvector&	CHelicopter::CurrentFirePoint()
{
	return m_fire_pos;
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
	FireBullet(CurrentFirePoint(),m_fire_dir, 
		fireDispersionBase,
		m_CurrentAmmo, 
		ID(),
		ID());

	StartFlameParticles();
	StartSmokeParticles(m_fire_pos, zero_vel);
	OnShellDrop(m_fire_pos, zero_vel);

}

void CHelicopter::MGunFireStart()
{
	if(!m_use_mgun_on_attack)
		return;

	CShootingObject::FireStart();
}

void CHelicopter::MGunFireEnd()
{
	if(!m_use_mgun_on_attack)
		return;

	CShootingObject::FireEnd();
	StopFlameParticles	();
}

void CHelicopter::updateMGunDir()
{
//	if(!m_destEnemy)	return;

	m_allow_fire		= TRUE;
	Fmatrix XFi;
	XFi.invert			(XFORM());
	Fvector dep;
	XFi.transform_tiny	(dep,m_data.m_destEnemyPos);
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
}

void CHelicopter::startRocket(u16 idx)
{
	if((getRocketCount()>=1)&&m_use_rocket_on_attack) {
		CExplosiveRocket* pGrenade = smart_cast<CExplosiveRocket*>(getCurrentRocket());
		VERIFY(pGrenade);
		pGrenade->SetCurrentParentID(this->ID());
		
		Fmatrix rocketXFORM;
		(idx==1)?rocketXFORM=m_left_rocket_bone_xform:rocketXFORM=m_right_rocket_bone_xform;

		Fvector vel;
		vel.mul(m_fire_dir,CRocketLauncher::m_fLaunchSpeed);
		LaunchRocket(rocketXFORM,  vel, zero_vel);

		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16(u16( getCurrentRocket()->ID()));
		u_EventSend(P);

		dropCurrentRocket();

		m_last_launched_rocket = idx;
	}
}

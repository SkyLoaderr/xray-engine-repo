#include "stdafx.h"
#include "helicopter.h"

void __stdcall 
CHelicopter::BoneMGunCallbackX(CBoneInstance *B)
{
	CHelicopter	* P = dynamic_cast<CHelicopter*> (static_cast<CObject*>(B->Callback_Param));
	Fmatrix rX;		rX.rotateX		(P->m_cur_x_rot);
	B->mTransform.mulB(rX);
}

void __stdcall 
CHelicopter::BoneMGunCallbackY(CBoneInstance *B)
{
	CHelicopter	* P = dynamic_cast<CHelicopter*> (static_cast<CObject*>(B->Callback_Param));
	Fmatrix rY;		rY.rotateY		(P->m_cur_y_rot);
	B->mTransform.mulB(rY);
}


const Fmatrix&	
CHelicopter::ParticlesXFORM() const
{
	return m_fire_bone_xform;
}

IRender_Sector*	
CHelicopter::Sector()
{
	return 0;
}

const Fvector&	
CHelicopter::CurrentFirePoint()
{
	return m_fire_pos;
}


void 
CHelicopter::UpdateFire()
{

	fTime -= Device.fTimeDelta;


	CShootingObject::UpdateFlameParticles();
	CShootingObject::UpdateLight();

	if(!IsWorking()) 
	{
		if(fTime<0) fTime = 0.f;
		return;
	}

	if(fTime<=0)
	{
		OnShot();
		fTime += fTimeToFire;
	}

}
void 
CHelicopter::OnShot		()
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

void 
CHelicopter::FireStart()
{
	CShootingObject::FireStart();
}

void 
CHelicopter::FireEnd()
{
	CShootingObject::FireEnd();
	StopFlameParticles	();
}

void					
CHelicopter::updateMGunDir()
{
	if(!m_destEnemy)	return;

	m_allow_fire		= TRUE;
	Fmatrix XFi;
	XFi.invert			(XFORM());
	Fvector dep;
	XFi.transform_tiny	(dep,m_destEnemyPos);
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
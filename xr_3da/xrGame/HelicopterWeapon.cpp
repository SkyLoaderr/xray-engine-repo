#include "stdafx.h"
#include "helicopter.h"

void __stdcall 
CHelicopter::BoneMGunCallbackX(CBoneInstance *B)
{
	CHelicopter	* P = dynamic_cast<CHelicopter*> (static_cast<CObject*>(B->Callback_Param));
	Fmatrix rX;		rX.rotateX		(P->m_bone_x_angle);
	B->mTransform.mulB(rX);
}

void __stdcall 
CHelicopter::BoneMGunCallbackY(CBoneInstance *B)
{
	CHelicopter	* P = dynamic_cast<CHelicopter*> (static_cast<CObject*>(B->Callback_Param));
	Fmatrix rY;		rY.rotateY		(P->m_bone_y_angle);
	B->mTransform.mul(P->m_bind_y_xform,rY);
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
	if(!m_destEnemy)
		return;
	//		повернуть пулемет
	//		m_destEnemyPos
	//		m_bone_x_angle = 0.03f;

	CKinematics* K	= PKinematics(Visual());

	Fmatrix M;//	= K->LL_GetTransform(m_rotate_x_bone);
	M.mul(XFORM(), m_bind_y_xform);
//	M.mulA(XFORM());

	Fmatrix Mi;
	Mi.invert(M);
	Fvector A_;
	A_.sub(m_destEnemyPos, M.c);
	Mi.transform_tiny(A_);
	A_.normalize();
	Fvector B_ = M.k;
	
//	A_.y = 0; A_.normalize();
//	B_.y = 0; B_.normalize();

	m_bone_y_angle = acos( A_.dotproduct(B_) );
//	Log("-----m_bone_y_angle", m_bone_y_angle);

}
#include "stdafx.h"
#include "phantom.h"
#include "../../NET_utils.h"
#include "../../level.h"

CPhantom::CPhantom()
{
	fDHeading			= 0;
	fSpeed				= 7.f;
	fASpeed				= 1.3f;
	vHPB.set			(0,0,0);
	fGoalChangeTime		= 0.f;
	fGoalChangeDelta	= 4.f;
	vGoalDir.set		(10.0f*Random.randF(),10.0f*Random.randF(),10.0f*Random.randF());
	vVarGoal.set		(0,0,0);
	vCurrentDir.set		(0,0,1);
}

CPhantom::~CPhantom()
{
}

//---------------------------------------------------------------------
void CPhantom::Load( LPCSTR section )
{
	inherited::Load				(section);
	//////////////////////////////////////////////////////////////////////////
	ISpatial*			self = smart_cast<ISpatial*> (this);
	if (self) {
		self->spatial.type &=~STYPE_VISIBLEFORAI;
		self->spatial.type &=~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

	m_particles	 = pSettings->r_string(section,"particles");
}
BOOL CPhantom::net_Spawn		(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC)) return FALSE;
	
	setVisible		(TRUE);
	setEnabled		(TRUE);

	m_enemy = Level().CurrentEntity();
	VERIFY	(m_enemy);

	PlayParticles	();

	return	TRUE;
}
void CPhantom::net_Destroy()
{
	inherited::net_Destroy	();

	PlayParticles			();
}

//---------------------------------------------------------------------
void CPhantom::shedule_Update(u32 DT)
{
	spatial.type &=~STYPE_VISIBLEFORAI;

	inherited::shedule_Update(DT);
}

void CPhantom::UpdateCL()
{
	inherited::UpdateCL();

	// назначить глобальную анимацию
	if (!m_motion) {
		// выбор анимации
		CSkeletonAnimated *skeleton_animated = smart_cast<CSkeletonAnimated*>(Visual());
		
		m_motion = skeleton_animated->ID_Cycle	("$editor");
		skeleton_animated->PlayCycle			(m_motion);
	}

	Fvector vP;
	m_enemy->Center		(vP);
	if (vP.distance_to_sqr(Position())<1.f){
		// hit enemy

		// destroy
		NET_Packet		P;
		u_EventGen		(P,GE_DESTROY,ID());
		u_EventSend		(P);
	}else{
		UpdatePosition	();
	}
}
//---------------------------------------------------------------------
// Core events
void CPhantom::net_Export	(NET_Packet& P)					// export to server
{
	// export 
	R_ASSERT			(Local());

	u8					flags = 0;
	P.w_float_q16		(g_Health(),-500,1000);

	P.w_float			(0);
	P.w_u32				(0);
	P.w_u32				(0);

	P.w_u32				(Device.dwTimeGlobal);
	P.w_u8				(flags);

	float				yaw, pitch, bank;
	XFORM().getHPB		(yaw,pitch,bank);
	P.w_angle8			(yaw);
	P.w_angle8			(yaw);
	P.w_angle8			(pitch);
	P.w_angle8			(0);
	P.w_u8				(u8(g_Team()));
	P.w_u8				(u8(g_Squad()));
	P.w_u8				(u8(g_Group()));
}

void CPhantom::net_Import	(NET_Packet& P)
{
	// import
	R_ASSERT			(Remote());

	u8					flags;

	float health;
	P.r_float_q16		(health,-500,1000);
	fEntityHealth = health;

	float fDummy;
	u32 dwDummy;
	P.r_float			(fDummy);
	P.r_u32				(dwDummy);
	P.r_u32				(dwDummy);

	P.r_u32				(dwDummy);
	P.r_u8				(flags);

	float				yaw, pitch, bank = 0, roll = 0;

	P.r_angle8			(yaw);
	P.r_angle8			(yaw);
	P.r_angle8			(pitch);
	P.r_angle8			(roll);

	id_Team				= P.r_u8();
	id_Squad			= P.r_u8();
	id_Group			= P.r_u8();

	XFORM().setHPB		(yaw,pitch,bank);
}

//---------------------------------------------------------------------
void CPhantom::HitSignal	(float /**HitAmount/**/, Fvector& /**local_dir/**/, CObject* who, s16 /**element/**/)
{
	NET_Packet		P;
	u_EventGen		(P,GE_DESTROY,ID());
	u_EventSend		(P);
}
//---------------------------------------------------------------------

void CPhantom::PlayParticles()
{
	Fvector				my_center, enemy_center, dir;

	Center				(my_center);
	m_enemy->Center		(enemy_center);
	
	dir.sub				(enemy_center, my_center);
	dir.normalize_safe	();

	CParticlesObject* ps = xr_new<CParticlesObject>(m_particles);

	// вычислить позицию и направленность партикла
	Fmatrix				pos;
	
	pos.identity		();
	pos.k.set			(dir);
	Fvector::generate_orthonormal_basis_normalized(pos.k,pos.j,pos.i);
	// установить позицию
	pos.translate_over	(my_center);

	ps->UpdateParent(pos, zero_vel);
	ps->Play();
}

//---------------------------------------------------------------------
void CPhantom::UpdatePosition() 
{
	if(fGoalChangeTime<=0)	{
		fGoalChangeTime += fGoalChangeDelta+fGoalChangeDelta*Random.randF(-0.5f,0.5f);
		Fvector vP;
//		vP.set(Device.vCameraPosition.x,Device.vCameraPosition.y,Device.vCameraPosition.z);
		m_enemy->Center	(vP);
		
		vGoalDir.x		= vP.x+vVarGoal.x*Random.randF(-0.5f,0.5f); 
		vGoalDir.y		= vP.y+vVarGoal.y*Random.randF(-0.5f,0.5f);
		vGoalDir.z		= vP.z+vVarGoal.z*Random.randF(-0.5f,0.5f);
	}
	fGoalChangeTime		-= Device.fTimeDelta;
	
	// Update position and orientation of the planes
	float fAT			= fASpeed * Device.fTimeDelta;

	Fvector& vDirection = XFORM().k;

	// Tweak orientation based on last position and goal
	Fvector vOffset;
	vOffset.sub(vGoalDir,Position());

	// First, tweak the pitch
	if( vOffset.y > 1.0){			// We're too low
		vHPB.y += fAT;
		if( vHPB.y > 0.8f )	vHPB.y = 0.8f;
	}else if( vOffset.y < -1.0){	// We're too high
		vHPB.y -= fAT;
		if( vHPB.y < -0.8f )vHPB.y = -0.8f;
	}else							// Add damping
		vHPB.y *= 0.95f;

	// Now figure out yaw changes
	vOffset.y           = 0.0f;
	vDirection.y		= 0.0f;

	vDirection.normalize();
	vOffset.normalize	();

	float fDot = vDirection.dotproduct(vOffset);
	fDot = (1.0f-fDot)/2.0f * fAT * 10.0f;

	vOffset.crossproduct(vOffset,vDirection);

	if( vOffset.y > 0.01f )		fDHeading = ( fDHeading * 9.0f + fDot ) * 0.1f;
	else if( vOffset.y < 0.01f )fDHeading = ( fDHeading * 9.0f - fDot ) * 0.1f;

	vHPB.x  +=  fDHeading;
	vHPB.z  = -fDHeading * 9.0f;

	// Update position

	Fvector vOldPosition;
	vOldPosition.set(Position());

	XFORM().setHPB	(vHPB.x,vHPB.y,vHPB.z);
	Position().mad	(vOldPosition,vDirection,fSpeed*Device.fTimeDelta);
}


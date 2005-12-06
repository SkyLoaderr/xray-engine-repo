// ActorEffector.h:	 менеджер эффекторов актера
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ActorEffector.h"

#include "../Environment.h"
#include "../CameraBase.h"
#include "../CameraManager.h"
#include "../igame_persistent.h"


#include "CameraEffector.h"
#include "../ObjectAnimator.h"
#include "object_broker.h"


CActorEffector::CActorEffector()
{
	vPosition.set	(0,0,0);
	vDirection.set	(0,0,1);
	vNormal.set		(0,1,0);

	fFov			= 90;
	fFar			= 100;
	fAspect			= 1.f;
}

CActorEffector::~CActorEffector()
{
	for (CameraEffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		xr_delete(*it);
}

CCameraEffector* CActorEffector::GetEffector(ECameraEffectorType type)	
{ 
	for (CameraEffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		if ((*it)->GetType()==type) return *it;
	return 0;
}

CCameraEffector* CActorEffector::AddEffector(CCameraEffector* ef)
{
	RemoveEffector(ef->GetType());
	m_Effectors.push_back(ef);
	return m_Effectors.back();
}

void CActorEffector::RemoveEffector(ECameraEffectorType type)
{
	for (CameraEffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		if ((*it)->GetType()==type)
		{ 
			xr_delete(*it);
			m_Effectors.erase(it);
			return;
		}
}

void CActorEffector::Update(const CCameraBase* C)
{	
	Update(C->vPosition,C->vDirection,C->vNormal, C->f_fov, C->f_aspect, g_pGamePersistent->Environment.CurrentEnv.far_plane, C->m_Flags.flags); 
}

void CActorEffector::Update(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags)
{
	// camera
	if (flags&CCameraBase::flPositionRigid)
		vPosition.set		(P);
	else
		vPosition.inertion	(P,	psCamInert);
	if (flags&CCameraBase::flDirectionRigid){
		vDirection.set		(D);
		vNormal.set			(N);
	}else{
		vDirection.inertion	(D,	psCamInert);
		vNormal.inertion	(N,	psCamInert);
	}

	// Normalize
	vDirection.normalize	();
	vNormal.normalize		();
	vRight.crossproduct		(vNormal,vDirection);
	vNormal.crossproduct	(vDirection,vRight);

	float aspect				= Device.fHeight_2/Device.fWidth_2;
	float src					= 10*Device.fTimeDelta;	clamp(src,0.f,1.f);
	float dst					= 1-src;
	fFov						= fFov*dst		+ fFOV_Dest*src;
	fFar						= fFar*dst		+ fFAR_Dest*src;
	fAspect						= fAspect*dst	+ (fASPECT_Dest*aspect)*src;

	// Effector
	if (m_Effectors.size())
	{
		for (int i=m_Effectors.size()-1; i>=0; i--)
		{
			CCameraEffector* eff = m_Effectors[i];
			if (!((eff->LifeTime()>0)&&eff->Process(vPosition,vDirection,vNormal,fFov,fFar,fAspect)))
			{
				m_Effectors.erase(m_Effectors.begin()+i);
				xr_delete(eff);
			}
		}

		// Normalize
		vDirection.normalize	();
		vNormal.normalize		();
		vRight.crossproduct		(vNormal,vDirection);
		vNormal.crossproduct	(vDirection,vRight);
	}
}

void CActorEffector::ApplyDevice ()
{
	// Device params
	Device.mView.build_camera_dir(vPosition,vDirection,vNormal);

	Device.vCameraPosition.set	( vPosition		);
	Device.vCameraDirection.set	( vDirection	);
	Device.vCameraTop.set		( vNormal		);
	Device.vCameraRight.set		( vRight		);

	// projection
	Device.fFOV					= fFov;
	Device.fASPECT				= fAspect;
	Device.mProject.build_projection(deg2rad(fFov*fAspect), fAspect, VIEWPORT_NEAR, fFar);
}
#include "../effectorPP.h"
#include "level.h"

class CShockPPEffector : public CEffectorPP {
	typedef CEffectorPP inherited;	
	SndShockEffector * m_shockEff;
	float m_end_time;
public:
	CShockPPEffector		(float life_time, SndShockEffector * eff):CEffectorPP(cefppHit,life_time),m_shockEff(eff){
		m_end_time = Device.fTimeGlobal + fLifeTime;
	};
	virtual	BOOL	Process					(SPPInfo& pp){
		inherited::Process(pp);
		pp = pp_identity;
//		float dk				= (float(m_shockEff->m_snd_length-m_shockEff->m_cur_length)/float(m_shockEff->m_snd_length));
		float tt				= Device.fTimeGlobal;
		float dk				= (m_end_time-tt)/fLifeTime;

//		pp.duality.h			= 0.1f*_sin(Device.fTimeGlobal)*dk;
//		pp.duality.v			= 0.1f*_cos(Device.fTimeGlobal)*dk;

		pp.duality.h			= (fLifeTime*0.1f/6.0f)*_sin(Device.fTimeGlobal)*dk;
		pp.duality.v			= (fLifeTime*0.1f/6.0f)*_cos(Device.fTimeGlobal)*dk;
		return TRUE;
	}
};

#define SND_MIN_VOLUME_FACTOR (0.1f)

SndShockEffector::SndShockEffector	()
{
	m_cur_length			= 0;
	m_stored_volume			= -1.0f;
}

SndShockEffector::~SndShockEffector	()
{
	psSoundVFactor		= m_stored_volume;
	Level().Cameras.RemoveEffector(cefppHit);
}

bool SndShockEffector::Active()
{
	return (m_cur_length<=m_snd_length);
}

void SndShockEffector::Start(int snd_length, float power)
{
	m_snd_length = snd_length;

	if( m_stored_volume<0.0f )
		m_stored_volume = psSoundVFactor;
	

	m_cur_length		= 0;
	psSoundVFactor		= m_stored_volume*SND_MIN_VOLUME_FACTOR;
	
	static float		xxx = 6.0f/150.0f; //6sec on max power(150)
	Level().Cameras.AddEffector(xr_new<CShockPPEffector>(power*xxx,this));
}

void SndShockEffector::Update()
{
	m_cur_length		+= Device.dwTimeDelta;
	float x				= float(m_cur_length)/m_snd_length;
	float y				= 2.f*x-1;
	if (y>0.f){
		psSoundVFactor	= y*(m_stored_volume-m_stored_volume*SND_MIN_VOLUME_FACTOR)+m_stored_volume*SND_MIN_VOLUME_FACTOR;
	}
}

CAnimatorCamEffector::CAnimatorCamEffector	(ECameraEffectorType type)
:inherited(type, 1000.0)
{
	m_objectAnimator = xr_new<CObjectAnimator>();
}

CAnimatorCamEffector::~CAnimatorCamEffector	()
{
	delete_data	(m_objectAnimator);
}

void CAnimatorCamEffector::Start	(LPCSTR fn)
{
	m_objectAnimator->Load		(fn);
	m_objectAnimator->Play		(Cyclic());
	fLifeTime					= m_objectAnimator->GetLength();
}

BOOL CAnimatorCamEffector::Process (Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	if(!Unlimited()){
		fLifeTime				-= Device.fTimeDelta;			
		if(fLifeTime<0) return FALSE;
	};

	const Fmatrix& m			= m_objectAnimator->XFORM();
	m_objectAnimator->Update	(Device.fTimeDelta);

	Fmatrix Mdef;
	Mdef.identity				();
	Mdef.j						= n;
	Mdef.k						= d;
	Mdef.i.crossproduct			(n,d);
	Mdef.c						= p;

	Fmatrix mr;
	mr.mul						(Mdef,m);
	d							= mr.k;
	n							= mr.j;
	p							= mr.c;

	return						TRUE;
}

CAnimatorCamLerpEffector::CAnimatorCamLerpEffector(ECameraEffectorType type, GET_KOEFF_FUNC f)
:inherited(type)
{
	m_func		= f;
}

BOOL CAnimatorCamLerpEffector::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	if(!Unlimited()){
		fLifeTime		-= Device.fTimeDelta;			
		if(fLifeTime<0) return FALSE;
	}

	const Fmatrix& m			= m_objectAnimator->XFORM();
	m_objectAnimator->Update	(Device.fTimeDelta);

	Fmatrix Mdef;
	Mdef.identity				();
	Mdef.j						= n;
	Mdef.k						= d;
	Mdef.i.crossproduct			(n,d);
	Mdef.c						= p;

	Fmatrix mr;
	mr.mul						(Mdef,m);


	Fquaternion					q_src, q_dst, q_res;
	q_src.set					(Mdef);
	q_dst.set					(mr);

	float	t					= m_func();
	clamp						(t,0.0f,1.0f);

	q_res.slerp					(q_src, q_dst, t);
	
	Fmatrix						res;
	res.rotation				(q_res);
	res.c.lerp					(p, mr.c, t);

	d							= res.k;
	n							= res.j;
	p							= res.c;

	return TRUE;
}

#include "ActorCondition.h"

CActorAlcoholCamEffector::CActorAlcoholCamEffector(CActorCondition* c)
:inherited(eCEAlcohol, GET_KOEFF_FUNC(c, &CActorCondition::GetAlcohol))
{
	Start			("camera_effects\\drunk.anm");
}

#pragma warning(push)
#pragma warning(disable:4355) // 'this' : used in base member initializer list
CFireHitCamEffector::CFireHitCamEffector	(ECameraEffectorType type,float power)
:inherited(type, GET_KOEFF_FUNC(this, &CFireHitCamEffector::GetPower))
{
	m_power			= power;
	clamp			(m_power, 0.0f, 1.0f);
}
#pragma warning(pop)

//////////////////////////////////////////////////////////////////////////

#define DELTA_ANGLE_X	0.5f * PI / 180
#define DELTA_ANGLE_Y	0.5f * PI / 180
#define DELTA_ANGLE_Z	0.5f * PI / 180
#define ANGLE_SPEED		1.5f	

CControllerPsyHitCamEffector::CControllerPsyHitCamEffector(ECameraEffectorType type, const Fvector &src_pos, const Fvector &target_pos, float time)
	:inherited(eCEControllerPsyHit, flt_max)
{
	m_time_total			= time;
	m_time_current			= 0;
	m_dangle_target.set		(angle_normalize(Random.randFs(DELTA_ANGLE_X)),angle_normalize(Random.randFs(DELTA_ANGLE_Y)),angle_normalize(Random.randFs(DELTA_ANGLE_Z)));
	m_dangle_current.set	(0.f, 0.f, 0.f);
	m_position_source		= src_pos;
	m_direction.sub			(target_pos,src_pos);
	m_distance				= m_direction.magnitude();
	m_direction.normalize	();
}

const float	_base_fov		= 170.f;
const float	_max_fov_add	= 160.f;


BOOL CControllerPsyHitCamEffector::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	Fmatrix	Mdef;
	Mdef.identity		();
	Mdef.j.set			(n);
	Mdef.k.set			(m_direction);
	Mdef.i.crossproduct	(n,m_direction);
	Mdef.c.set			(p);

	//////////////////////////////////////////////////////////////////////////

	if (angle_lerp(m_dangle_current.x, m_dangle_target.x, ANGLE_SPEED, Device.fTimeDelta)) {
		m_dangle_target.x = angle_normalize(Random.randFs(DELTA_ANGLE_X));
	}

	if (angle_lerp(m_dangle_current.y, m_dangle_target.y, ANGLE_SPEED, Device.fTimeDelta)) {
		m_dangle_target.y = angle_normalize(Random.randFs(DELTA_ANGLE_Y));
	}

	if (angle_lerp(m_dangle_current.z, m_dangle_target.z, ANGLE_SPEED, Device.fTimeDelta)) {
		m_dangle_target.z = angle_normalize(Random.randFs(DELTA_ANGLE_Z));
	}
	
	//////////////////////////////////////////////////////////////////////////

	if (m_time_current > m_time_total) m_time_current = m_time_total;

	float perc_past	= m_time_current / m_time_total;
	float cur_dist	= m_distance * perc_past;

	Mdef.c.mad	(m_position_source, m_direction, cur_dist);
	fFov = _base_fov - _max_fov_add*perc_past;

	m_time_current	+= Device.fTimeDelta;
	
	//////////////////////////////////////////////////////////////////////////

	// Установить углы смещения
	Fmatrix		R;
	if (m_time_current > m_time_total) 
		R.identity	();
	else 
		R.setHPB	(m_dangle_current.x,m_dangle_current.y,m_dangle_current.z);

	Fmatrix		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);
	p.set		(mR.c);

	return TRUE;
}



// ActorEffector.h:	 менеджер эффекторов актера
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ActorEffector.h"

#include "../Environment.h"
#include "../CameraBase.h"
#include "../CameraManager.h"
#include "../igame_persistent.h"


#include "CameraEffector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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

	// Save affected matrix and vectors
	affected_vPosition.set				(vPosition);
	affected_vDirection.set				(vDirection);
	affected_vNormal.set				(vNormal);
	affected_vRight.crossproduct		(vNormal,vDirection);

	// Save un-affected matrix and vectors
	unaffected_mView.build_camera_dir	(vPosition,vDirection,vNormal);
	unaffected_vPosition.set			(vPosition);
	unaffected_vDirection.set			(vDirection);
	unaffected_vNormal.set				(vNormal);
	unaffected_vRight.crossproduct		(vNormal,vDirection);

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
			Fvector sp=vPosition;
			Fvector sd=vDirection;
			Fvector sn=vNormal;
			if ((eff->LifeTime()>0)&&eff->Process(vPosition,vDirection,vNormal,fFov,fFar,fAspect))
			{
				if (eff->Affected())
				{
					sp.sub(vPosition,sp);	
					sd.sub(vDirection,sd);	
					sn.sub(vNormal,sn);

					affected_vPosition.add	(sp);
					affected_vDirection.add	(sd);
					affected_vNormal.add	(sn);
				}
			}
			else
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
		affected_vDirection.normalize	();
		affected_vNormal.normalize		();
		affected_vRight.crossproduct	(vNormal,vDirection);
		affected_vNormal.crossproduct	(vDirection,vRight);
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
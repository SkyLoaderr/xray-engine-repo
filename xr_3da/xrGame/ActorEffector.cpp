// ActorEffector.h:	 менеджер эффекторов актера
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ActorEffector.h"

#include "stdafx.h"

#include "..\\Environment.h"
#include "..\\CameraBase.h"
#include "..\\CameraManager.h"
#include "..\\Effector.h"

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
	for (EffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		xr_delete(*it);
}

CEffector* CActorEffector::GetEffector(EEffectorType type)	
{ 
	for (EffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		if ((*it)->eType==type) return *it;
	return 0;
}

CEffector* CActorEffector::AddEffector(CEffector* ef)
{
	RemoveEffector(ef->eType);
	m_Effectors.push_back(ef);
	return m_Effectors.back();
}

void CActorEffector::RemoveEffector(EEffectorType type)
{
	for (EffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		if ((*it)->eType==type)
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
			CEffector* eff = m_Effectors[i];
			Fvector sp=vPosition;
			Fvector sd=vDirection;
			Fvector sn=vNormal;
			if ((eff->fLifeTime>0)&&eff->Process(vPosition,vDirection,vNormal,fFov,fFar,fAspect))
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

void CActorEffector::Apply (CCameraBase* C)
{
	C->vPosition.set	( vPosition		);
	C->vDirection.set	( vDirection	);
	C->vNormal.set		( vNormal		);
}
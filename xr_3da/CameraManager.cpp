// CameraManager.cpp: implementation of the CCameraManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CameraManager.h"
#include "CameraLook.h"
#include "CameraDebug.h"
#include "CameraFirstEye.h"
#include "Effector.h"
#include "xr_ini.h"
#include "xr_creator.h"

#include "x_ray.h"
#include "xr_smallfont.h"

float psCamInert		= 0.7f;
float psCamSlideInert	= 0.25f;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraManager::CCameraManager()
{
	vPosition.set	(0,0,0);
	vDirection.set	(0,0,1);
	vNormal.set		(0,1,0);

	fFov			= 90;
	fFar			= 100;
	fAspect			= 1.f;
}

CCameraManager::~CCameraManager()
{
	for (EffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		_DELETE(*it);
}

CEffector* CCameraManager::GetEffector(EEffectorType type)	
{ 
	for (EffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		if ((*it)->eType==type) return *it;
	return 0;
}

CEffector* CCameraManager::AddEffector(CEffector* ef)
{
	RemoveEffector(ef->eType);
	m_Effectors.push_back(ef);
	return m_Effectors.back();
}

void CCameraManager::RemoveEffector(EEffectorType type){
	for (EffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		if ((*it)->eType==type){ 
			_DELETE(*it);
			m_Effectors.erase(it);
			return;
		}
}

void CCameraManager::Update(const CCameraBase* C)
{	Update(C->vPosition,C->vDirection,C->vNormal, C->f_fov, C->f_aspect, pCreator->Environment.Current.Far); }

void CCameraManager::Update(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest)
{
	// camera
	vPosition.inertion		(P,	psCamInert);
	vDirection.inertion		(D,	psCamInert);
	vNormal.inertion		(N,	psCamInert);
	
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

	// Effector
	if (m_Effectors.size()){
		for (int i=m_Effectors.size()-1; i>=0; i--){
			CEffector* eff = m_Effectors[i];
			Fvector sp=vPosition;
			Fvector sd=vDirection;
			Fvector sn=vNormal;
			eff->Process(vPosition,vDirection,vNormal);
			if (eff->Affected()){
				sp.sub(vPosition,sp);	sd.sub(vDirection,sd);	sn.sub(vNormal,sn);
				affected_vPosition.add	(sp);
				affected_vDirection.add	(sd);
				affected_vNormal.add	(sn);
			}
			if (eff->fLifeTime<=0){ 
				m_Effectors.erase(m_Effectors.begin()+i);
				_DELETE(eff);
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
	
	// Device params
	Device.mView.build_camera_dir(vPosition,vDirection,vNormal);
	
	Device.vCameraPosition.set	( vPosition		);
	Device.vCameraDirection.set	( vDirection	);
	Device.vCameraTop.set		( vNormal		);
	Device.vCameraRight.set		( vRight		);
	
	// projection
	float aspect				= Device.fHeight_2/Device.fWidth_2;
	float src					= 10*Device.fTimeDelta;	clamp(src,0.f,1.f);
	float dst					= 1-src;
	fFov						= fFov*dst		+ fFOV_Dest*src;
	fFar						= fFar*dst		+ fFAR_Dest*src;
	fAspect						= fAspect*dst	+ (fASPECT_Dest*aspect)*src;
	Device.fFOV					= fFov;
	Device.fASPECT				= fAspect;
	Device.mProject.build_projection(deg2rad(fFov*fAspect), fAspect, VIEWPORT_NEAR, fFar);
}

void CCameraManager::Dump()
{
	Fmatrix mInvCamera;
	Fvector _R,_U,_T,_P;
	
	mInvCamera.invert(Device.mView);
	_R.set( mInvCamera._11, mInvCamera._12, mInvCamera._13 );
	_U.set( mInvCamera._21, mInvCamera._22, mInvCamera._23 );
	_T.set( mInvCamera._31, mInvCamera._32, mInvCamera._33 );
	_P.set( mInvCamera._41, mInvCamera._42, mInvCamera._43 );
	Log("CCameraManager::Dump::vPosition  = ",_P);
	Log("CCameraManager::Dump::vDirection = ",_T);
	Log("CCameraManager::Dump::vNormal    = ",_U);
	Log("CCameraManager::Dump::vRight     = ",_R);
}

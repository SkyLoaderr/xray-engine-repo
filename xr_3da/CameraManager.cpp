// CameraManager.cpp: implementation of the CCameraManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igame_level.h"

#include "Environment.h"
#include "CameraBase.h"
#include "CameraManager.h"
#include "Effector.h"
#include "EffectorPP.h"

#include "x_ray.h"
#include "gamefont.h"
#include "render.h"

float	psCamInert		= 0.7f;
float	psCamSlideInert	= 0.25f;
SPPInfo g_gefPP			= { 0, 0, { 0, 0 }, { 0, 0, { 0, 0, 0, 0 } } };

void SPPInfo::normalize() 
{
	noise.intensity = _max(_min(noise.intensity, 1.f), 0.f);
	noise.color.r = _max(_min(noise.color.r, 1.f), 0.f);
	noise.color.g = _max(_min(noise.color.g, 1.f), 0.f);
	noise.color.b = _max(_min(noise.color.b, 1.f), 0.f);
	noise.color.a = _max(_min(noise.color.a, 1.f), 0.f);
}

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

	unaffected_PP = affected_PP = g_gefPP;
}

CCameraManager::~CCameraManager()
{
	for (EffectorIt it=m_Effectors.begin(); it!=m_Effectors.end(); it++ )
		xr_delete(*it);
	for (EffectorPPIt it=m_EffectorsPP.begin(); it!=m_EffectorsPP.end(); it++ )
		xr_delete(*it);
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
			xr_delete(*it);
			m_Effectors.erase(it);
			return;
		}
}

CEffectorPP* CCameraManager::GetEffector(EEffectorPPType type)	
{ 
	for (EffectorPPIt it=m_EffectorsPP.begin(); it!=m_EffectorsPP.end(); it++ )
		if ((*it)->eType==type) return *it;
	return 0;
}

CEffectorPP* CCameraManager::AddEffector(CEffectorPP* ef) {
	RemoveEffector(ef->eType);
	m_EffectorsPP.push_back(ef);
	return m_EffectorsPP.back();
}

void CCameraManager::RemoveEffector(EEffectorPPType type){
	for (EffectorPPIt it=m_EffectorsPP.begin(); it!=m_EffectorsPP.end(); it++ )
		if ((*it)->eType==type){ 
			xr_delete(*it);
			m_EffectorsPP.erase(it);
			return;
		}
}

void CCameraManager::Update(const CCameraBase* C)
{	
	Update(C->vPosition,C->vDirection,C->vNormal, C->f_fov, C->f_aspect, g_pGameLevel->Environment->Current.Far, C->bApplyInert); 
}

void CCameraManager::Update(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, BOOL bApplyInert)
{
	// camera
	if (bApplyInert){
		vPosition.inertion	(P,	psCamInert);
		vDirection.inertion	(D,	psCamInert);
		vNormal.inertion	(N,	psCamInert);
	}else{
		vPosition.set		(P);
		vDirection.set		(D);
		vNormal.set			(N);
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
	if (m_Effectors.size()){
		for (int i=m_Effectors.size()-1; i>=0; i--){
			CEffector* eff = m_Effectors[i];
			Fvector sp=vPosition;
			Fvector sd=vDirection;
			Fvector sn=vNormal;
			if ((eff->fLifeTime>0)&&eff->Process(vPosition,vDirection,vNormal,fFov,fFar,fAspect))
			{
				if (eff->Affected()){
					sp.sub(vPosition,sp);	sd.sub(vDirection,sd);	sn.sub(vNormal,sn);
					affected_vPosition.add	(sp);
					affected_vDirection.add	(sd);
					affected_vNormal.add	(sn);
				}
			}else{
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

	// EffectorPP
	affected_PP = unaffected_PP;
	if(m_EffectorsPP.size()) {
		for(int i = m_EffectorsPP.size()-1; i >= 0; i--) {
			CEffectorPP* eff = m_EffectorsPP[i];
			SPPInfo l_PPInf = unaffected_PP;
			if((eff->fLifeTime>0)&&eff->Process(l_PPInf)) {
				affected_PP += l_PPInf;
			} else RemoveEffector(eff->eType);
		}
		affected_PP.normalize();
	}
	
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

	// postprocess
	IRender_Target*		T	= ::Render->getTarget();
	T->set_duality_h		(affected_PP.duality.h);
	T->set_duality_v		(affected_PP.duality.v);
	T->set_blur				(affected_PP.blur);
	T->set_gray				(affected_PP.gray);
	T->set_noise			(affected_PP.noise.intensity);
	T->set_noise_scale		(affected_PP.noise.grain);
	T->set_noise_color		(affected_PP.noise.color);
	T->set_noise_fps		(10.f);
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

#include "stdafx.h"
#include <msacm.h>

#include "xrSound.h"
#include "xr_streamsnd.h"
#include "xr_cda.h"
#include "MusicStream.h"
#include "3dsound.h"
#include "3DSoundRender.h"
#include "cl_intersect.h"

CSoundManager							Sound_Implementation;
ENGINE_API	CSound_manager_interface*	Sound = &Sound_Implementation;

// sound props
u32		psSoundFreq				= 0;
u32		psSoundModel			= 0;
float	psSoundVMaster			= 1.0f;
float	psSoundVEffects			= 1.0f;
float	psSoundVMusic			= 0.7f;

//-----------------------------------------------------------------------------
void CSoundManager::SetVMusic	( )
{
	if (!bPresent)				return;
	pMusicStreams->Update		();
}

//-----------------------------------------------------------------------------
void CSoundManager::OnFrame		( )
{
	if (!bPresent)				return;

	Device.Statistic.Sound.Begin();
	if (Device.bActive) {
		if (fMusicVolume	!= psSoundVMusic)	SetVMusic	();

		pMusicStreams->OnMove	();
		pSoundRender->OnMove	();
		if (pCDA) pCDA->OnMove	();
	}
	Device.Statistic.Sound.End	();
}

//-----------------------------------------------------------------------------
void	CSoundManager::Create				( sound& S, BOOL _3D, const char* fName, BOOL bCtrlFreq, int type )
{
	if (!bPresent) return;
	FILE_NAME	fn;
	strcpy		(fn,fName);
	char*		E = strext(fn);
	if (E)		*E = 0;
	S.handle	= pSoundRender->CreateSound(fn,TRUE,bCtrlFreq);
	S.g_type	= type;
}
void	CSoundManager::Play					( sound& S, CObject* O, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || S.handle==soundUndefinedHandle) return;
	S.g_object		= O;
	if (S.feedback)	S.feedback->Rewind	();
	else			pSoundRender->Play	(S.handle,&S,bLoop,iLoopCnt);
}
void	CSoundManager::Play_Unlimited		( sound& S, CObject* O, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || S.handle==soundUndefinedHandle) return;
	pSoundRender->Play	(S.handle,0,bLoop,iLoopCnt);
}
void	CSoundManager::PlayAtPos			( sound& S, CObject* O, const Fvector &pos, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || S.handle==soundUndefinedHandle) return;
	S.g_object		= O;
	if (S.feedback)	S.feedback->Rewind	();
	else			pSoundRender->Play	(S.handle,&S,bLoop,iLoopCnt);
	S.feedback->SetPosition				(pos);
}
void	CSoundManager::PlayAtPos_Unlimited	( sound& S, CObject* O, const Fvector &pos, BOOL bLoop, int iLoopCnt)
{
	if (!bPresent || S.handle==soundUndefinedHandle) return;
	pSoundRender->Play		(S.handle,0,bLoop,iLoopCnt);
	S.feedback->SetPosition	(pos);
}
void	CSoundManager::Delete				( sound& S )
{
	if (!bPresent || S.handle==soundUndefinedHandle) {
		S.handle	= soundUndefinedHandle;
		S.feedback	= 0;
		return;
	}
	if (S.feedback)	S.feedback->Stop();
	pSoundRender->DeleteSound(S.handle);
}
//-----------------------------------------------------------------------------
CSound_stream_interface* CSoundManager::CreateStream	( LPCSTR fName )
{
	if (!bPresent) return NULL;
	return pMusicStreams->CreateSound(fName);
}
void	CSoundManager::DeleteStream			( CSound_stream_interface* pSnd )
{
	if (!bPresent)	return;
	pMusicStreams->DeleteSound	((CSoundStream*)pSnd);
}
//-----------------------------------------------------------------------------
BOOL CSoundManager::IsOccluded	(	Fvector& P, float R, Fvector* occ )
{
	if (0==pGeometry)	return FALSE;

	// Calculate RAY params
	Fvector base			= Device.vCameraPosition;
	Fvector	pos,dir;
	float	range;
	pos.random_dir			();
	pos.mul					(R);
	pos.add					(P);
	dir.sub					(pos,base);
	range = dir.magnitude	();
	dir.div					(range);

	// 1. Check cached polygon
	float _u,_v,_range;
	if (CDB::TestRayTri(base,dir,occ,_u,_v,_range,true))
		if (_range>0 && _range<range) return TRUE;

	// 2. Polygon doesn't picked up - real database query
	DB.ray_options			(CDB::OPT_ONLYNEAREST);
	DB.ray_query			(pGeometry,base,dir,range);
	if (0==DB.r_count()) {
		return FALSE;
	} else {
		// cache polygon
		const CDB::RESULT*	R = DB.r_begin();
		const CDB::TRI&		T = pGeometry->get_tris() [ R->id ];
		occ[0].set	(*T.verts[0]);
		occ[1].set	(*T.verts[1]);
		occ[2].set	(*T.verts[2]);
		return TRUE;
	}
}

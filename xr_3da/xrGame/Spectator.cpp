// Actor.cpp: implementation of the CSpectator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "spectator.h"
#include "..\effectorfall.h"
#include "..\CameraLook.h"
#include "..\CameraFirstEye.h"
#include "..\xr_level_controller.h"
#include "actor.h"
#include "hudmanager.h"

//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSpectator::CSpectator() : CGameObject()
{
	// Cameras
	cameras[eacFirstEye]	= new CCameraFirstEye	(this, pSettings, "actor_firsteye_cam", false);
	cameras[eacLookAt]		= new CCameraLook		(this, pSettings, "actor_look_cam",		false);
	cameras[eacFreeLook]	= new CCameraLook		(this, pSettings, "actor_free_cam",	false);
	cameras[eacFreeFly]		= new CCameraFirstEye	(this, pSettings, "actor_firsteye_cam", false);

	cam_active				= eacFreeLook;
	look_idx				= 0;
}

CSpectator::~CSpectator()
{
	for (int i=0; i<eacMaxCam; i++) _DELETE(cameras[i]);
}

void CSpectator::UpdateCL()
{
	inherited::UpdateCL();
	if (pCreator->CurrentViewEntity()==this){
		if (eacFreeFly!=cam_active){
			int idx			= 0;
			game_cl_GameState::Player* P = Game().local_player;
			if (P&&(P->team>=0)&&(P->team<(int)Level().Teams.size())){
				CTeam& T		= Level().Teams[P->team];
				for (u32 i=0; i<T.Squads.size(); i++){
					CSquad& S = T.Squads[i];
					for (u32 j=0; j<S.Groups.size(); j++){
						CGroup& G = S.Groups[j];
						for (u32 k=0; k<G.Members.size(); k++){
							CActor* A = dynamic_cast<CActor*>(G.Members[k]);
							if (A&&A->g_Alive()){
								if(idx==look_idx){
									cam_Update	(A);
									return;
								}
								idx++;
							}
						}
					}
				}
			}
			// не найден объект с таким индексом - сбросим на первый объект
			look_idx = 0;
			// никого нет за кем смотреть - переключимся на 
			if (0==idx) cam_Set(eacFreeFly);
		}
		// по умолчанию eacFreeFly
		cam_Update		(0);
	}
}

void CSpectator::Update	(u32 DT)
{
	if (!getEnabled())	return;
	if (!Ready())		return;
}

void CSpectator::OnKeyboardPress(int cmd)
{
	if (Remote())												return;

	if (GAME_PHASE_PENDING	== Game().phase){
		if (kWPN_FIRE == cmd){
			// Switch our "ready" status
			NET_Packet			P;
			u_EventGen			(P,GEG_PLAYER_READY,ID());
			u_EventSend			(P);
		}
		return;
	}
	switch(cmd) {
	case kCAM_1:	cam_Set			(eacFirstEye);				break;
	case kCAM_2:	cam_Set			(eacLookAt);				break;
	case kCAM_3:	cam_Set			(eacFreeLook);				break;
	case kCAM_4:	cam_Set			(eacFreeFly);				break;
	case kWPN_FIRE:	look_idx++;									break;
	}
}

void CSpectator::OnKeyboardRelease(int cmd)
{
}

void CSpectator::OnKeyboardHold(int cmd)
{
	if (Remote())		return;

	if ((cam_active==eacFreeFly)||(cam_active==eacFreeLook)){
		CCameraBase* C	= cameras	[cam_active];
		Fvector vmove={0,0,0};
		switch(cmd){
		case kUP:
		case kDOWN: 
		case kCAM_ZOOM_IN: 
		case kCAM_ZOOM_OUT: 
			cameras[cam_active]->Move(cmd); break;
		case kLEFT:
		case kRIGHT:
			if (cam_active!=eacFreeLook) cameras[cam_active]->Move(cmd); break;
		case kFWD:			
			vmove.mad( C->vDirection, 4.f*Device.fTimeDelta );
			break;
		case kBACK:
			vmove.mad( C->vDirection, -4.f*Device.fTimeDelta );
			break;
		case kR_STRAFE:{
			Fvector right;
			right.crossproduct(C->vNormal,C->vDirection);
			vmove.mad( right, 4.f*Device.fTimeDelta );
			}break;
		case kL_STRAFE:{
			Fvector right;
			right.crossproduct(C->vNormal,C->vDirection);
			vmove.mad( right, -4.f*Device.fTimeDelta );
			}break;
		}
		svTransform.c.add( vmove );
	}
}

void CSpectator::OnMouseMove(int dx, int dy)
{
	if (Remote())	return;

	CCameraBase* C	= cameras	[cam_active];
	float scale		= (C->f_fov/DEFAULT_FOV)*psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d = float(dx)*scale;
		cameras[cam_active]->Move((d<0)?kLEFT:kRIGHT, _abs(d));
	}
	if (dy){
		float d = ((psMouseInvert)?-1:1)*float(dy)*scale*3.f/4.f;
		cameras[cam_active]->Move((d>0)?kUP:kDOWN, _abs(d));
	}
}

void CSpectator::cam_Set	(EActorCameras style)
{
	CCameraBase* old_cam = cameras[cam_active];
	cam_active = style;
	old_cam->OnDeactivate();
	cameras[cam_active]->OnActivate(old_cam);
}

void CSpectator::cam_Update	(CActor* A)
{
	CHUDManager* HUD				= Level().HUD();
	HUD->pFontDI->SetColor			(0xF0808080);
	HUD->pFontDI->SetSize			(0.03f);
	HUD->pFontDI->SetAligment		(CGameFont::alCenter);
	if (A){
		const Fmatrix& M			= A->clXFORM();
		CCameraBase* cam			= cameras[cam_active];
		switch(cam_active) {
		case eacFirstEye:{
			Fvector P;
			P.add					(M.c,1.6f);
			cam->Set				(P,M.k,M.j);
			}break;
		case eacLookAt:{
			float y,p,r;
			M.getHPB				(y,p,r);
			cam->Set				(-y,-p,-r);
			}
		case eacFreeLook:{
			cam->SetParent			(A);
			Fvector point, dangle;
			point.set				(0.f,1.6f,0.f);
			M.transform_tiny		(point);
			cam->Update				(point,dangle);
			}break;
		}
		pCreator->Cameras.Update(cam);
		// hud output
		HUD->pFontDI->Out			(0.f,0.9f,"%s(%d%%)",A->cName(),iFloor(A->g_Health()+0.5f));
	}else{
		Fvector point, dangle;
		point.set					(0.f,1.6f,0.f);
		svTransform.transform_tiny	(point);

		// apply shift
		dangle.set					(0,0,0);
		CCameraBase* cam			= cameras[eacFreeFly];
		cam->Update					(point,dangle);
		pCreator->Cameras.Update	(cam);
		// hud output
		HUD->pFontDI->Out			(0.f,0.9f,"free-fly camera");
	}
}

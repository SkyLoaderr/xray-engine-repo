// Actor.cpp: implementation of the CSpectator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "spectator.h"
#include "../effectorfall.h"
#include "CameraLook.h"
#include "CameraFirstEye.h"
#include "actor.h"
#include "hudmanager.h"
#include "xrServer_Objects.h"
#include "game_cl_base.h"
#include "level.h"
#include "xr_level_controller.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"

//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSpectator::CSpectator() : CGameObject()
{
	// Cameras
	cameras[eacFirstEye]	= xr_new<CCameraFirstEye>	(this, pSettings, "actor_firsteye_cam", false);
	cameras[eacLookAt]		= xr_new<CCameraLook>		(this, pSettings, "actor_look_cam",		false);
	cameras[eacFreeLook]	= xr_new<CCameraLook>		(this, pSettings, "actor_free_cam",	false);
	cameras[eacFreeFly]		= xr_new<CCameraFirstEye>	(this, pSettings, "actor_firsteye_cam", false);

//	cam_active				= eacFreeFly;
	cam_active				= eacFreeLook;
	look_idx				= 0;
}

CSpectator::~CSpectator()
{
	for (int i=0; i<eacMaxCam; ++i) xr_delete(cameras[i]);
}

void CSpectator::UpdateCL()
{
	inherited::UpdateCL();
	if (g_pGameLevel->CurrentViewEntity()==this){
		if (eacFreeFly!=cam_active){
			int idx			= 0;
			game_PlayerState* P = Game().local_player;
			if (P&&(P->team>=0)&&(P->team<(int)Level().seniority_holder().teams().size())){
				const CTeamHierarchyHolder& T		= Level().seniority_holder().team(P->team);
				for (u32 i=0; i<T.squads().size(); ++i){
					const CSquadHierarchyHolder& S = T.squad(i);
					for (u32 j=0; j<S.groups().size(); ++j){
						const CGroupHierarchyHolder& G = S.group(j);
						for (u32 k=0; k<G.members().size(); ++k){
							CActor* A = dynamic_cast<CActor*>(G.members()[k]);
							if (A/*&&A->g_Alive()*/){
								if(idx==look_idx){
									cam_Update	(A);
									return;
								}
								++idx;
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

void CSpectator::shedule_Update		(u32 DT)
{
	inherited::shedule_Update	(DT);
	if (!getEnabled())	return;
	if (!Ready())		return;
}

static float Accel_mul = 1.0f;

void CSpectator::IR_OnKeyboardPress(int cmd)
{
	if (Remote())												return;

/*	if (kWPN_FIRE == cmd)
	{
		if ((GAME_PHASE_PENDING	== Game().phase) || 
			(GAME_PHASE_INPROGRESS	== Game().phase && HUD().GetUI()->UIGame()->CanBeReady()))
		{
			NET_Packet			P;
//			u_EventGen			(P,GEG_PLAYER_READY,ID());

			u_EventGen		(P,GE_GAME_EVENT,ID()	);
			P.w_u16(GAME_EVENT_PLAYER_READY);

			u_EventSend			(P);
			return;
		}
		else
			return;
	};
*/
	switch(cmd) 
	{
	case kACCEL:
		{
			Accel_mul = 4.0f;
		}break;
	case kCAM_1:	cam_Set			(eacFirstEye);				break;
	case kCAM_2:	cam_Set			(eacLookAt);				break;
	case kCAM_3:	cam_Set			(eacFreeLook);				break;
	case kCAM_4:	cam_Set			(eacFreeFly);				break;
	case kWPN_FIRE:	++look_idx;									break;
	}
}

void CSpectator::IR_OnKeyboardRelease(int cmd)
{
	switch (cmd)
	{
	case kACCEL:
		{
			Accel_mul = 1.0f;
		}break;
	}
}


void CSpectator::IR_OnKeyboardHold(int cmd)
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
			if (eacFreeLook!=cam_active) cameras[cam_active]->Move(cmd); break;
		case kFWD:			
			vmove.mad( C->vDirection, 4.f*Device.fTimeDelta*Accel_mul );
			break;
		case kBACK:
			vmove.mad( C->vDirection, -4.f*Device.fTimeDelta*Accel_mul );
			break;
		case kR_STRAFE:{
			Fvector right;
			right.crossproduct(C->vNormal,C->vDirection);
			vmove.mad( right, 4.f*Device.fTimeDelta*Accel_mul );
			}break;
		case kL_STRAFE:{
			Fvector right;
			right.crossproduct(C->vNormal,C->vDirection);
			vmove.mad( right, -4.f*Device.fTimeDelta*Accel_mul );
			}break;
		}
		XFORM().c.add( vmove );
	}
}

void CSpectator::IR_OnMouseMove(int dx, int dy)
{
	if (Remote())	return;

	CCameraBase* C	= cameras	[cam_active];
	float scale		= (C->f_fov/DEFAULT_FOV)*psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d = float(dx)*scale;
		cameras[cam_active]->Move((d<0)?kLEFT:kRIGHT, _abs(d));
	}
	if (dy){
		float d = ((psMouseInvert.test(1))?-1:1)*float(dy)*scale*3.f/4.f;
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
	HUD().pFontDI->SetColor			(0xF0808080);
	HUD().pFontDI->SetSize			(0.03f);
	HUD().pFontDI->SetAligment		(CGameFont::alCenter);
	if (A){
		const Fmatrix& M			= A->XFORM();
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
			point.set	(0.f,1.6f,0.f);
			M.transform_tiny		(point);
			if(!A->g_Alive()) point.set	(A->Position());
			cam->Update				(point,dangle);
			}break;
		}
		g_pGameLevel->Cameras.Update(cam);
		// hud output
		HUD().pFontDI->Out			(0.f,0.9f,"%s(%d%%)",*A->cName(),iFloor(A->g_Health()+0.5f));
	}else{
		Fvector point, dangle;
		point.set					(0.f,1.6f,0.f);
		XFORM().transform_tiny	(point);

		// apply shift
		dangle.set					(0,0,0);
		CCameraBase* cam			= cameras[eacFreeFly];
		cam->Update					(point,dangle);
		g_pGameLevel->Cameras.Update	(cam);
		// hud output
		/*
		if (true || Game().Phase() != GAME_PHASE_PENDING)
		{
			HUD().pFontDI->SetColor		(0xffffffff);
			HUD().pFontDI->Out			(0.f,0.0f,"SPECTATOR : Free-fly camera");
		};
		*/
	};
}

BOOL			CSpectator::net_Spawn				( LPVOID	DC )
{
	BOOL res = inherited::net_Spawn(DC);
	if (!res) return FALSE;

	CSE_Abstract			*E	= (CSE_Abstract*)(DC);
	if (!E) return FALSE;

	cam_active				= eacFreeFly;
	look_idx				= 0;

	cameras[cam_active]->Set		(-E->o_Angle.y,-E->o_Angle.x,0);		// set's camera orientation
//	cameras[cam_active]->Set		(0, E->o_Angle.x,0);		// set's camera orientation
	cameras[cam_active]->vPosition.set(E->o_Position);

	if (OnServer())
	{
		E->s_flags.set(M_SPAWN_OBJECT_LOCAL, TRUE);
	};
	return TRUE;
};
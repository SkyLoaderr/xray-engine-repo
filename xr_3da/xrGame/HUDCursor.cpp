// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:13:00 , by user : Oles , from computer : OLES
// HUDCursor.cpp: implementation of the CHUDCursor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hudcursor.h"
#include "hudmanager.h"

#include "../Environment.h"
#include "../CustomHUD.h"
#include "Entity.h"
#include "level.h"
#include "game_cl_base.h"
#include "../igame_persistent.h"


#include "InventoryOwner.h"
#include "relation_registry.h"
#include "character_info.h"

#include "string_table.h"

#define C_ON_ENEMY		D3DCOLOR_XRGB(0xff,0,0)
#define C_ON_NEUTRAL	D3DCOLOR_XRGB(0xff,0xff,0xff)
#define C_ON_FRIEND		D3DCOLOR_XRGB(0,0xff,0)


#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)
#define C_SIZE		0.025f
#define NEAR_LIM	0.5f

#define SHOW_INFO_SPEED	0.5f
#define HIDE_INFO_SPEED	10.f

float	g_fMinReconDist		= 2.0f;
float	g_fMaxReconDist		= 50.0f;
float	g_fMinReconSpeed	= 0.5f;
float	g_fMaxReconSpeed	= 10.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHUDCursor::CHUDCursor	()
{    
	fuzzyShowInfo		= 0.f;
	pick_dist			= 0.f;
	hGeom.create		(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
	hShader.create		("hud\\cursor","ui\\cursor");

	RQ.set				(NULL, 0.f, -1);

	Load				();
	m_bShowCrosshair	= false;
}

CHUDCursor::~CHUDCursor	()
{
}


void CHUDCursor::Load		()
{
	HUDCrosshair.Load();
}

IC u32 subst_alpha(u32 val, u8 a){ return u32(val&0x00FFFFFF)|u32(a<<24); }

void CHUDCursor::CursorOnFrame ()
{
	Fvector				p1,p2,dir;

	p1					= Device.vCameraPosition;
	dir					= Device.vCameraDirection;
	
	// Render cursor
	pick_dist			= g_pGamePersistent->Environment.CurrentEnv.far_plane*0.99f;
	if(Level().CurrentEntity()){
		Level().CurrentEntity()->setEnabled(false);
		if (Level().ObjectSpace.RayPick( p1, dir, pick_dist, collide::rqtBoth, RQ ))
			pick_dist	= RQ.range<NEAR_LIM?NEAR_LIM:RQ.range;
		Level().CurrentEntity()->setEnabled(true);
	}
}

void CHUDCursor::Render()
{
	Fvector		p1,p2,dir;

	CObject*	O		= Level().CurrentEntity();
	if (0==O)	return;
	CEntity*	E		= smart_cast<CEntity*>(O);
	if (0==E)	return;

	p1					= Device.vCameraPosition;
	dir					= Device.vCameraDirection;
	
	// Render cursor
	u32 C				= C_DEFAULT;
	
	FVF::TL				PT;
	p2.mad				(p1,dir,pick_dist);
	PT.transform		(p2,Device.mFullTransform);
	float				di_size = C_SIZE/powf(PT.p.w,.2f);

	CGameFont* F		= HUD().Font().pFontDI;
	F->SetAligment		(CGameFont::alCenter);
	F->SetSize			(0.02f);
	F->OutSet			(0.f,0.f+di_size*2);
#ifdef DEBUG
	if (psHUD_Flags.test(HUD_CROSSHAIR_DIST)){
		F->SetColor		(C);
		F->OutNext		("%3.1f",pick_dist);
	}
#endif

	if (psHUD_Flags.test(HUD_INFO)){ 
		if (RQ.O){
			CEntityAlive*	E = smart_cast<CEntityAlive*>(RQ.O);
			CEntityAlive*	pCurEnt = smart_cast<CEntityAlive*>(Level().CurrentEntity());
			PIItem	l_pI = smart_cast<PIItem>(RQ.O);

			string256 name_buf;
			LPCSTR object_name = *RQ.O->cName();



			if (E && (E->g_Health()>0)){
				if (GameID() == GAME_SINGLE){
					CInventoryOwner* our_inv_owner		= smart_cast<CInventoryOwner*>(pCurEnt);
					CInventoryOwner* others_inv_owner	= smart_cast<CInventoryOwner*>(E);

					if(our_inv_owner && others_inv_owner){
						CStringTable strtbl;
						sprintf(name_buf, "%s, %s", *strtbl(others_inv_owner->CharacterInfo().Community().id()),
													*strtbl(others_inv_owner->Name()));
						object_name = name_buf;

						switch(RELATION_REGISTRY().GetRelationType(others_inv_owner, our_inv_owner))
						{
						case ALife::eRelationTypeEnemy:
							C = C_ON_ENEMY; break;
						case ALife::eRelationTypeNeutral:
							C = C_ON_NEUTRAL; break;
						case ALife::eRelationTypeFriend:
							C = C_ON_FRIEND; break;
						}
					}

					if (fuzzyShowInfo>0.5f){
						F->SetColor	(subst_alpha(C,u8(iFloor(255.f*(fuzzyShowInfo-0.5f)*2.f))));
						F->OutNext	("%s", object_name);
					}
					fuzzyShowInfo += SHOW_INFO_SPEED*Device.fTimeDelta;
				}else{
					if (pCurEnt){	
						if (GameID() == GAME_DEATHMATCH)			C = C_ON_ENEMY;
						else{	
							if (E->g_Team() != pCurEnt->g_Team())	C = C_ON_ENEMY;
							else									C = C_ON_FRIEND;
						};
						if (pick_dist >= g_fMinReconDist && pick_dist <= g_fMaxReconDist){
							float ddist = (pick_dist - g_fMinReconDist)/(g_fMaxReconDist - g_fMinReconDist);
							float dspeed = g_fMinReconSpeed + (g_fMaxReconSpeed - g_fMinReconSpeed)*ddist;
							fuzzyShowInfo += Device.fTimeDelta/dspeed;
						}else{
							if (pick_dist < g_fMinReconDist) fuzzyShowInfo += g_fMinReconSpeed*Device.fTimeDelta;
							else fuzzyShowInfo = 0;
						};

						if (fuzzyShowInfo>0.5f){
							clamp(fuzzyShowInfo,0.f,1.f);
							int alpha_C = iFloor(255.f*(fuzzyShowInfo-0.5f)*2.f);
							u8 alpha_b = u8(alpha_C & 0x00ff);
							F->SetColor	(subst_alpha(C,alpha_b));
							F->OutNext	("%s",*RQ.O->cName());
						}
					};
				};
			}else if (l_pI){
				if (fuzzyShowInfo>0.5f){
					F->SetColor	(subst_alpha(C,u8(iFloor(255.f*(fuzzyShowInfo-0.5f)*2.f))));
					F->OutNext	("%s",l_pI->NameComplex());
				}
				fuzzyShowInfo += SHOW_INFO_SPEED*Device.fTimeDelta;
			}
		}else{
			fuzzyShowInfo -= HIDE_INFO_SPEED*Device.fTimeDelta;
		}
		clamp(fuzzyShowInfo,0.f,1.f);
	}

	//отрендерить кружочек или крестик
	if(!m_bShowCrosshair){
		// actual rendering
		u32			vOffset;
		FVF::TL*	pv		= (FVF::TL*)RCache.Vertex.Lock(4,hGeom.stride(),vOffset);
		float			size= float(::Render->getTarget()->get_width()) * di_size;
		float			w_2	= float(::Render->getTarget()->get_width())		/ 2;
		float			h_2	= float(::Render->getTarget()->get_height())	/ 2;

		// Convert to screen coords
		float cx		    = (PT.p.x+1)*w_2;
		float cy		    = (PT.p.y+1)*h_2;

		pv->set				(cx - size, cy + size, PT.p.z, PT.p.w, C, 0, 1); ++pv;
		pv->set				(cx - size, cy - size, PT.p.z, PT.p.w, C, 0, 0); ++pv;
		pv->set				(cx + size, cy + size, PT.p.z, PT.p.w, C, 1, 1); ++pv;
		pv->set				(cx + size, cy - size, PT.p.z, PT.p.w, C, 1, 0); ++pv;

		// unlock VB and Render it as triangle LIST
		RCache.Vertex.Unlock(4,hGeom.stride());
		RCache.set_Shader	(hShader);
		RCache.set_Geometry	(hGeom);
		RCache.Render		(D3DPT_TRIANGLELIST,vOffset,0,4,0,2);
	}else{
		//отрендерить прицел
		HUDCrosshair.OnRender		();
	}
}
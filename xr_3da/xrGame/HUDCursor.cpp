// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:13:00 , by user : Oles , from computer : OLES
// HUDCursor.cpp: implementation of the CHUDCursor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hudcursor.h"
#include "hudmanager.h"

#include "..\CustomHUD.h"
#include "Entity.h"

#define C_ON_ENEMY	D3DCOLOR_XRGB(0xff,0,0)
#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)
#define C_SIZE		0.025f
#define NEAR_LIM	0.5f

#define SHOW_INFO_SPEED	0.5f
#define HIDE_INFO_SPEED	10.f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHUDCursor::CHUDCursor	()
{    
	hShader				= 0;
	hVS					= 0;
	fuzzyShowInfo		= 0.f;
	OnDeviceCreate		();
	
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
}

CHUDCursor::~CHUDCursor	()
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	
	OnDeviceDestroy		();
}

void CHUDCursor::OnDeviceCreate()
{
	REQ_CREATE	();
	hVS			= Device.Shader._CreateVS	(FVF::F_TL);
	hShader		= Device.Shader.Create		("hud\\cursor","ui\\cursor",FALSE);
}
void CHUDCursor::OnDeviceDestroy()
{
	Device.Shader.Delete		(hShader);
	Device.Shader._DeleteVS		(hVS);
}

IC u32 subst_alpha(u32 val, u8 a){ return u32(val&0x00FFFFFF)|u32(a<<24); }

void CHUDCursor::Render()
{
	Fvector		p1,p2,dir;

	CObject*	O = pCreator->CurrentEntity();
	if (0==O)	return;
	CEntity*	E = dynamic_cast<CEntity*>(O);
	if (0==E)	return;

//	int			cur_team = E->g_Team();

	p1	= Device.vCameraPosition;
	dir = Device.vCameraDirection;
	
	// Render cursor
	float		dist=Level().Environment.c_Far*0.99f;
	
	pCreator->CurrentEntity()->setEnabled(false);
	u32 C			= C_DEFAULT;
	Collide::ray_query	RQ;

	if (pCreator->ObjectSpace.RayPick( p1, dir, dist, RQ )){
		// if (RQ.O && (RQ.O->CLS_ID == CLSID_ENTITY) && (((CEntity*)RQ.O)->id_Team!=cur_team)) C = C_ON_ENEMY;
		dist = RQ.range;
	}
	pCreator->CurrentEntity()->setEnabled(true);
	
	if ( dist<NEAR_LIM) dist=NEAR_LIM;
	
	FVF::TL			PT;
	p2.mad			(p1,dir,dist);
	PT.transform	(p2,Device.mFullTransform);
	float			di_size = C_SIZE/powf(PT.p.w,.2f);

	CGameFont* F = Level().HUD()->pFontDI;
	F->SetAligment	(CGameFont::alCenter);
	F->SetSize		(0.02f);
	F->OutSet		(0.f,0.f+di_size*2);
	if (psHUD_Flags&HUD_CROSSHAIR_DIST){
		F->SetColor	(C);
		F->OutNext	("%3.1f",dist);
	}
	if (psHUD_Flags&HUD_INFO){ 
		if (RQ.O){
			CEntityAlive*	E = dynamic_cast<CEntityAlive*>(RQ.O);
			if (E && (E->g_Health()>0)){
				if (fuzzyShowInfo>0.5f){
					F->SetColor	(subst_alpha(C,u8(iFloor(255.f*(fuzzyShowInfo-0.5f)*2.f))));
					F->OutNext	("%s",RQ.O->cName());
				}
				fuzzyShowInfo += SHOW_INFO_SPEED*Device.fTimeDelta;
			}
		}else{
			fuzzyShowInfo -= HIDE_INFO_SPEED*Device.fTimeDelta;
		}
		clamp(fuzzyShowInfo,0.f,1.f);
	}
	// actual rendering
	u32			vOffset;
	FVF::TL*	pv		= (FVF::TL*)Device.Streams.Vertex.Lock(4,hVS->dwStride,vOffset);
	float			size= float(::Render->getTarget()->get_width()) * di_size;
	float			w_2	= float(::Render->getTarget()->get_width())		/ 2;
	float			h_2	= float(::Render->getTarget()->get_height())	/ 2;
	
	// Convert to screen coords
	float cx        = (PT.p.x+1)*w_2;
	float cy        = (PT.p.y+1)*h_2;
	
	pv->set(cx - size, cy + size, PT.p.z, PT.p.w, C, 0, 1); pv++;
	pv->set(cx - size, cy - size, PT.p.z, PT.p.w, C, 0, 0); pv++;
	pv->set(cx + size, cy + size, PT.p.z, PT.p.w, C, 1, 1); pv++;
	pv->set(cx + size, cy - size, PT.p.z, PT.p.w, C, 1, 0); pv++;
	
	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock(4,hVS->dwStride);
	Device.Shader.set_Shader	(hShader);
	Device.Primitive.setVertices(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices	(vOffset,Device.Streams.QuadIB);;
	Device.Primitive.Render		(D3DPT_TRIANGLELIST,0,4,0,2);
}

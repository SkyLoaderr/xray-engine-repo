// XR_Cursor.cpp: implementation of the CCursor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_cursor.h"
#include "xr_creator.h"
#include "xr_animator.h"
#include "cameramanager.h"
#include "xr_area.h"
#include "environment.h"
#include "std_classes.h"
#include "xr_object.h"
#include "CustomHUD.h"

// font
#include "xr_smallfont.h"
#include "x_ray.h"

#define C_ON_ACTOR	D3DCOLOR_XRGB(0xff,0,0)
#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)
#define C_SIZE		0.025f
#define NEAR_LIM	0.5f


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCursor::CCursor()
{    
	Stream		= Device.Streams.Create	(FVF::F_TL,4);
	hShader		= Device.Shader.Create	("font","ui\\cursor",FALSE);
}

CCursor::~CCursor()
{
	Device.Shader.Delete(hShader);
}

void CCursor::Render()
{
	Fvector		p1,p2,dir;

	if (0 == (psHUD_Flags&HUD_CROSSHAIR)) return;

	p1	= Device.vCameraPosition;
	dir = Device.vCameraDirection;
	
	// Render cursor
	float		dist=500.f;
	
	pCreator->CurrentEntity()->bEnabled = false;
	DWORD C			= C_DEFAULT;
	Collide::ray_query	RQ;

//	if (pCreator->iGetKeyState(DIK_RSHIFT)) __asm int 3;
	if (pCreator->ObjectSpace.RayPick( p1, dir, dist, RQ ))
	{
		if (RQ.O && RQ.O->CLS_ID == CLSID_ENTITY) C = C_ON_ACTOR;
		dist = RQ.range;
	}
	pCreator->CurrentEntity()->bEnabled = true;
	
	if ( dist<NEAR_LIM) dist=NEAR_LIM;
	
	FVF::TL			PT;
	p2.direct		(p1,dir,dist);
	PT.transform	(p2,Device.mFullTransform);
	float			di_size = C_SIZE/powf(PT.p.w,.2f);

	if (psHUD_Flags&HUD_CROSSHAIR_DIST) {
		pApp->pFont->Color	(C);
		pApp->pFont->Size	(di_size*1.5f);
		pApp->pFont->Out(PT.p.x,PT.p.y+di_size*2,"~%3.1f",dist);
		if (RQ.O)
			pApp->pFont->Out(PT.p.x,PT.p.y+di_size*4,"~%s",RQ.O->cName());
	}
	
	// actual rendering
	DWORD			vOffset;
	FVF::TL*	pv	= (FVF::TL*)Stream->Lock(4,vOffset);
	float			size= Device.dwWidth * di_size;
	
	// Convert to screen coords
	float cx        = Device._x2real(PT.p.x);
	float cy        = Device._y2real(PT.p.y);
	
	pv->set(cx - size, cy + size, PT.p.z, PT.p.w, C, 0, 1); pv++;
	pv->set(cx - size, cy - size, PT.p.z, PT.p.w, C, 0, 0); pv++;
	pv->set(cx + size, cy + size, PT.p.z, PT.p.w, C, 1, 1); pv++;
	pv->set(cx + size, cy - size, PT.p.z, PT.p.w, C, 1, 0); pv++;
	
	// unlock VB and Render it as triangle list
	Stream->Unlock	(4);
	Device.Shader.Set		(hShader);
	Device.Primitive.Draw	(Stream,4,2,vOffset,Device.Streams_QuadIB);

	// Device.Primitive.dbg_DrawLINE(precalc_identity,p1,p2,C);
}

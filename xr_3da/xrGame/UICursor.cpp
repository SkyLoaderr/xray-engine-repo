// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:58 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uicursor.h"

#include "../CustomHUD.h"
#include "HUDManager.h"
#include "UI.h"

#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)

#define SENSITIVITY_DEFAULT 1.5f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICursor::CUICursor()
{    
	m_fSensitivity	= SENSITIVITY_DEFAULT;
	bHoldMode		= false;
	bVisible		= false;
	vPos.set		(0.f,0.f);
	vHoldPos.set	(0,0);
	vDelta.set		(0,0);

	hGeom.create	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
	hShader.create	("hud\\cursor","ui\\ui_cursor");

}
//--------------------------------------------------------------------
CUICursor::~CUICursor	()
{
}
//--------------------------------------------------------------------
void CUICursor::Render	()
{
	CGameFont* F		= HUD().Font().pFontDI;
	F->SetAligment		(CGameFont::alCenter);
	F->SetSize			(0.02f);
	F->OutSet			(0.f,-0.9f);
	F->SetColor			(0xffffffff);
	Ivector2			pt = GetPos();
	F->OutNext			("%d-%d",pt.x, pt.y);

	if(bHoldMode){
		F->OutNext		("Hold Mode");
		return;
	};


	_VertexStream*	Stream	= &RCache.Vertex; 
	// actual rendering
	u32			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(4,hGeom.stride(),vOffset);
	float			size= 2 * Device.dwWidth * 0.015f;
	
	// Convert to screen coords
	float cx					= (vPos.x+1)*(Device.dwWidth/2);
	float cy					= (vPos.y+1)*(Device.dwHeight/2);
	
	u32 C						= C_DEFAULT;

	pv->set(cx, cy+size,		.0001f,.9999f, C, 0, 1); ++pv;
	pv->set(cx, cy,				.0001f,.9999f, C, 0, 0); ++pv;
	pv->set(cx+size, cy+size,	.0001f,.9999f, C, 1, 1); ++pv;
	pv->set(cx+size, cy,		.0001f,.9999f, C, 1, 0); ++pv;
	
	// unlock VB and Render it as triangle LIST
	Stream->Unlock			(4,hGeom.stride());
	RCache.set_Shader		(hShader);
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4,0,2);



}
//--------------------------------------------------------------------


//move cursor to screen coordinates
void CUICursor::SetPos(int x, int y)
{
	vDelta.set		(0,0);
	vPos.x =(float)2*x/(float)UI_BASE_WIDTH - 1.0f;
	vPos.y =(float)2*y/(float)UI_BASE_HEIGHT - 1.0f;

	if(vPos.x<-1) vPos.x=-1;
	if(vPos.x>1) vPos.x=1;
	if(vPos.y<-1) vPos.y=-1;
	if(vPos.y>1) vPos.y=1;
}

void CUICursor::GetPos(int& x, int& y)
{
	x = (int)((vPos.x+1.0f)*(float)UI_BASE_WIDTH/2);
	y = (int)((vPos.y+1.0f)*(float)UI_BASE_HEIGHT/2);

}

Ivector2 CUICursor::GetPos()
{
	Ivector2 pt;

	int x, y;
	GetPos(x, y);
	pt.x = x;
	pt.y = y;

	return  pt;
}

Ivector2 CUICursor::GetPosDelta()
{
	Ivector2 res;
	res.x = iFloor( (vDelta.x/2.0f)*(float)UI_BASE_WIDTH );
	res.y = iFloor( (vDelta.y/2.0f)*(float)UI_BASE_HEIGHT );
	return res;
}

void CUICursor::HoldMode		(bool b)
{
	if(bHoldMode==b) return;
	if(b){
		vHoldPos = vPos;
	}else{
		vPos = vHoldPos;
	}
	bHoldMode = b;
}

void CUICursor::MoveBy(int dx, int dy)
{

	vDelta.x = (float)m_fSensitivity*dx/(float)UI_BASE_WIDTH;
	vDelta.y = (float)m_fSensitivity*dy/(float)UI_BASE_HEIGHT;

	if(!bHoldMode){
		vPos.x += vDelta.x;
		vPos.y += vDelta.y;
	}
	clamp(vPos.x, -1.f, 1.f);
	clamp(vPos.y, -1.f, 1.f);
}

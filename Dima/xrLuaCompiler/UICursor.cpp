// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:58 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uicursor.h"

#include "..\CustomHUD.h"

#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)

#define SENSITIVITY_DEFAULT 1.5f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICursor::CUICursor()
{    
	bVisible	= false;
	vPos.set	(0.f,0.f);

	hGeom.create	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
	hShader.create	("hud\\cursor","ui\\ui_cursor");

	m_fSensitivity = SENSITIVITY_DEFAULT;
}
//--------------------------------------------------------------------
CUICursor::~CUICursor	()
{
}
//--------------------------------------------------------------------
void CUICursor::Render	()
{
	_VertexStream*	Stream	= &RCache.Vertex; 
	// actual rendering
	u32			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(4,hGeom.stride(),vOffset);
	float			size= 2 * Device.dwWidth * 0.015f;
	
	// Convert to screen coords
	float cx					= (vPos.x+1)*(Device.dwWidth/2);
	float cy					= (vPos.y+1)*(Device.dwHeight/2);
	
	u32 C						= C_DEFAULT;

	pv->set(cx, cy+size,		.0001f,.9999f, C, 0, 1); pv++;
	pv->set(cx, cy,				.0001f,.9999f, C, 0, 0); pv++;
	pv->set(cx+size, cy+size,	.0001f,.9999f, C, 1, 1); pv++;
	pv->set(cx+size, cy,		.0001f,.9999f, C, 1, 0); pv++;
	
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
	vPos.x =(float)2*x/(float)Device.dwWidth - 1.0f;
	vPos.y =(float)2*y/(float)Device.dwHeight - 1.0f;

	if(vPos.x<-1) vPos.x=-1;
	if(vPos.x>1) vPos.x=1;
	if(vPos.y<-1) vPos.y=-1;
	if(vPos.y>1) vPos.y=1;
}

void CUICursor::GetPos(int& x, int& y)
{
	x = (int)((vPos.x+1.0f)*(float)Device.dwWidth/2);
	y = (int)((vPos.y+1.0f)*(float)Device.dwHeight/2);
}

POINT CUICursor::GetPos()
{
	POINT pt;

	int x, y;
	GetPos(x, y);
	pt.x = x;
	pt.y = y;

	return  pt;
}


void CUICursor::MoveBy(int dx, int dy)
{
	vPos.x +=(float)m_fSensitivity*dx/(float)Device.dwWidth;
	vPos.y +=(float)m_fSensitivity*dy/(float)Device.dwHeight;

	if(vPos.x<-1) vPos.x=-1;
	if(vPos.x>1) vPos.x=1;
	if(vPos.y<-1) vPos.y=-1;
	if(vPos.y>1) vPos.y=1;

}

#include "stdafx.h"
#pragma hdrstop

#include "render.h"
//---------------------------------------------------------------------------
float 	ssaLIMIT;
float	g_fSCREEN;
CRender 	Render_implementation;
CRender* 	Render = &Render_implementation;
//---------------------------------------------------------------------------

CRender::CRender	()
{
}

CRender::~CRender	()
{
}

BOOL CRender::occ_visible(Fbox&	B)
{
	DWORD mask		= 0xff;
	return ViewBase.testAABB(B.min,B.max,mask);
}

BOOL CRender::occ_visible(sPoly& P)
{
	return ViewBase.testPolyInside(P);
}	


// XR_Animator.cpp: implementation of the CAnimator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "XR_Animator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnimator::CAnimator(char *texturename, int fStart, int fCount, float _ptime, float px, float py, float size, float ImSize, BOOL bAntialias)
{
	fLocalTime	=	0;
	fPlayTime	=	_ptime;
	iStart		=	fStart;
	iCount		=	fCount;
	fTexSize	=	ImSize;
	iFrameDim	=	iFloor(1.f/fTexSize);

	bAntialiasFrames = bAntialias;

	hTexture	=	Device.Texture.Create(
		texturename,
		"std_aop",
		false);
	SetPosition(px,py,.0001f,.9999f,size);
}

CAnimator::~CAnimator()
{
	Device.Texture.Delete(hTexture);
}

void CAnimator::Render(float alpha)
{
float		tu,tv,a;
int			frame,frame2;
FTLvertex	Vertex[4];
Fcolor		cur;

	float fSPF = fPlayTime/float(iCount);
	fLocalTime+=Device.fTimeDelta;
	if (fLocalTime>=fPlayTime) fLocalTime-=fPlayTime;
	frame	= iFloor( fLocalTime/fSPF );
	frame2	= frame+1;
	if (frame2>=iCount) frame2=0;
	a		= fmodf(fLocalTime,fSPF)/fSPF;

	frame	+=iStart;
	frame2	+=iStart;

	float cx	= X_TO_REAL(fPX);
	float cy	= Y_TO_REAL(fPY);
	float Size	= fSize*Device.dwWidth;


	tu=(frame%iFrameDim)*fTexSize;
	tv=(frame/iFrameDim)*fTexSize;
	if (bAntialiasFrames) {
		cur.set(1,1,1,(1-a)*alpha);
	} else {
		cur.set(1,1,1,alpha);
	}
	DWORD cc=cur.get();
	Vertex[0].set(cx+Size,cy-Size,fSZ,fRHW,cc,cc,tu,tv);
	Vertex[1].set(cx+Size,cy+Size,fSZ,fRHW,cc,cc,tu,tv+fTexSize);
	Vertex[2].set(cx-Size,cy-Size,fSZ,fRHW,cc,cc,tu+fTexSize,tv);
	Vertex[3].set(cx-Size,cy+Size,fSZ,fRHW,cc,cc,tu+fTexSize,tv+fTexSize);

	Device.rsSet			(vAlpha);
	Device.Texture.Set		(hTexture);
//	Device.Draw.PrimitiveSP	(D3DPT_TRIANGLESTRIP,FTLVERTEX,Vertex,4);
}

void CAnimator::SetPosition(float x, float y, float sz, float rhw, float size)
{
	fPX		= x;
	fPY		= y;
	fSZ		= sz;
	fRHW	= rhw;
	fSize	= size;
}

// XR_Animator.h: interface for the CAnimator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XR_ANIMATOR_H__E7BD1FEB_D406_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_XR_ANIMATOR_H__E7BD1FEB_D406_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

class CAnimator
{
	float		fLocalTime, fPlayTime;
	int			iStart,iCount,iFrameDim;
	float		fPX,fPY,fSize;
	float		fTexSize;
	float		fSZ,fRHW;
	BOOL		bAntialiasFrames;
	int			hTexture;
public:
	void SetPosition(float x, float y, float sz=0.f, float rhw=1.f, float size=.03f);
	void Render(float alpha=1.0f);

	CAnimator (char *texturename, int fStart, int fCount, float fPTime, float px, float py, float size, float ImSize=32.f/256.f, BOOL bAntialias=false);
	~CAnimator();
};

#endif // !defined(AFX_XR_ANIMATOR_H__E7BD1FEB_D406_11D3_B4E3_4854E82A090D__INCLUDED_)

#include "stdafx.h"
#pragma hdrstop

#include <d3dx9.h>
#include "frustum.h"

void CBackend::OnFrameEnd	()
{
	for (u32 stage=0; stage<HW.Caps.raster.dwStages; stage++)
		CHK_DX(HW.pDevice->SetTexture(0,0));
	CHK_DX				(HW.pDevice->SetStreamSource	(0,0,0,0));
	CHK_DX				(HW.pDevice->SetIndices			(0));
	CHK_DX				(HW.pDevice->SetVertexShader	(0));
	CHK_DX				(HW.pDevice->SetPixelShader		(0));
	Invalidate			();
}

void CBackend::OnFrameBegin	()
{
	PGO					(Msg("PGO:*****frame[%d]*****",Device.dwFrame));
	Memory.mem_fill		(&stat,0,sizeof(stat));
	Vertex.Flush		();
	Index.Flush			();
	set_Stencil			(FALSE);
}

void CBackend::Invalidate	()
{
	pRT[0]						= NULL;
	pRT[1]						= NULL;
	pRT[2]						= NULL;
	pRT[3]						= NULL;
	pZB							= NULL;

	decl						= NULL;
	vb							= NULL;
	ib							= NULL;
	vb_stride					= 0;

	state						= NULL;
	ps							= NULL;
	vs							= NULL;
	ctable						= NULL;

	T							= NULL;
	M							= NULL;
	C							= NULL;

	textures[0]=textures[1]=textures[2]=textures[3]=textures[4]=textures[5]=textures[6]=textures[7]=NULL;
	matrices[0]=matrices[1]=matrices[2]=matrices[3]=matrices[4]=matrices[5]=matrices[6]=matrices[7]=NULL;
	cull_mode					= CULL_NONE;
	colorwrite_mask				= u32(-1);
}

void	CBackend::set_ClipPlanes(u32 _enable, Fplane*	_planes /*=NULL */, u32 count/* =0*/)
{
	if (0==HW.Caps.geometry.dwClipPlanes)	return;
	if (!_enable)	{
		CHK_DX	(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE));
		return;
	}

	// Enable and setup planes
	VERIFY	(_planes && count);
	if		(count>HW.Caps.geometry.dwClipPlanes)	count=HW.Caps.geometry.dwClipPlanes;

	D3DXMATRIX			worldToClipMatrixIT;
	D3DXMatrixInverse	(&worldToClipMatrixIT,NULL,(D3DXMATRIX*)&Device.mFullTransform);
	D3DXMatrixTranspose	(&worldToClipMatrixIT,&worldToClipMatrixIT);
	for		(u32 it=0; it<count; it++)		{
		Fplane&		P			= _planes	[it];
		D3DXPLANE	planeWorld	(-P.n.x,-P.n.y,-P.n.z,-P.d), planeClip;
		D3DXPlaneNormalize		(&planeWorld,	&planeWorld);
		D3DXPlaneTransform		(&planeClip,	&planeWorld, &worldToClipMatrixIT);
		CHK_DX					(HW.pDevice->SetClipPlane(it,planeClip));
	}

	// Enable them
	u32		e_mask	= (1<<count)-1;
	CHK_DX	(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,e_mask));
}

void	CBackend::set_ClipPlanes(u32 _enable, Fmatrix*	_xform  /*=NULL */, u32 fmask/* =0xff */)
{
	if (0==HW.Caps.geometry.dwClipPlanes)	return;
	if (!_enable)	{
		CHK_DX	(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,FALSE));
		return;
	}
	VERIFY		(_xform && fmask);
	CFrustum	F;
	F.CreateFromMatrix	(*_xform,fmask);
	set_ClipPlanes		(_enable,F.planes,F.p_count);
}

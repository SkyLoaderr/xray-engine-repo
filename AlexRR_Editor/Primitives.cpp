//----------------------------------------------------
// file: Primitives.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "Primitives.h"
#include "Texture.h"

//----------------------------------------------------


const IColor boxcolor = {255,255,255,0};
const int boxindexcount = 24;
const WORD boxindices[boxindexcount] = {
	0, 1, 1, 2, 2, 3, 3, 0,
	0, 4, 1, 5, 2, 6, 3, 7,
	4, 5, 5, 6, 6, 7, 7, 4 };


//----------------------------------------------------

void DrawSelectionBox( IAABox& box ){
	
	D3DLVERTEX verts[8];

	for( int i=0; i<8;i++){
		verts[i].color = boxcolor.value;
		verts[i].specular = boxcolor.value;
		verts[i].tu = 0;
		verts[i].tv = 0;
	}

	IVector& vmin = box.vmin;
	IVector& vmax = box.vmax;

	verts[0].x = vmin.x;
	verts[0].y = vmax.y;
	verts[0].z = vmin.z;

	verts[1].x = vmin.x;
	verts[1].y = vmax.y;
	verts[1].z = vmax.z;

	verts[2].x = vmax.x;
	verts[2].y = vmax.y;
	verts[2].z = vmax.z;

	verts[3].x = vmax.x;
	verts[3].y = vmax.y;
	verts[3].z = vmin.z;

	verts[4].x = vmin.x;
	verts[4].y = vmin.y;
	verts[4].z = vmin.z;

	verts[5].x = vmin.x;
	verts[5].y = vmin.y;
	verts[5].z = vmax.z;

	verts[6].x = vmax.x;
	verts[6].y = vmin.y;
	verts[6].z = vmax.z;

	verts[7].x = vmax.x;
	verts[7].y = vmin.y;
	verts[7].z = vmin.z;


	TM.BindNone();

	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE,D3DMCS_COLOR2) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );

	CDX( UI.d3d()->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		verts,8, (LPWORD)boxindices, boxindexcount, 0 ));
}


//----------------------------------------------------

const int planeindexcount1 = 8;
const WORD planeindices1[planeindexcount1] = {
	0, 1, 1, 2, 2, 3, 3, 0 };

const int planeindexcount2 = 6;
const WORD planeindices2[planeindexcount1] = {
	0, 1, 2, 2, 3, 0 };

void DrawQuadPlane( IVector *points, bool selected ){

	_ASSERTE( points );

	D3DLVERTEX verts1[4];
	D3DLVERTEX verts2[4];

	IColor color1,color2;
	if( selected ){
		color1.set( 255, 127, 255, 127 );
		color2.set( 255, 127, 127, 255 );
	} else {
		color1.set( 140, 140, 140, 127 );
		color2.set( 127, 255, 127, 255 );
	}

	for(int i=0; i<4; i++){
		
		verts1[i].color = color1.value;
		verts1[i].specular = color1.value;
		
		verts1[i].tu = 0.f;
		verts1[i].tv = 0.f;

		verts1[i].x = points[i].x;
		verts1[i].y = points[i].y;
		verts1[i].z = points[i].z;

		verts2[i].color = color2.value;
		verts2[i].specular = color2.value;
		
		verts2[i].tu = 0.f;
		verts2[i].tv = 0.f;

		verts2[i].x = points[i].x;
		verts2[i].y = points[i].y;
		verts2[i].z = points[i].z;
	}


	TM.BindNone();

	IMatrix matrix;
	matrix.identity();

	DWORD d3dsaves[5];

	// -- save render state --
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,d3dsaves+0) );
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE,d3dsaves+1) );
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,d3dsaves+2) );
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE,d3dsaves+3) );
	CDX( UI.d3d()->GetRenderState(D3DRENDERSTATE_COLORVERTEX,d3dsaves+4) );

	CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)&matrix) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE,D3DMCS_COLOR2) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA) );

	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE) );
	CDX( UI.d3d()->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,D3DFVF_LVERTEX,
		verts1, 4, (LPWORD)planeindices2, planeindexcount2, 0 ));

	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );
	CDX( UI.d3d()->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		verts2, 4, (LPWORD)planeindices1, planeindexcount1, 0 ));

	// -- restore render state --
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,d3dsaves[0]) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE,d3dsaves[1]) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,d3dsaves[2]) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE,d3dsaves[3]) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_COLORVERTEX,d3dsaves[4]) );

	// restore
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE) );
	CDX( UI.d3d()->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW) );
}

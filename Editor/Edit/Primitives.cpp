//----------------------------------------------------
// file: Primitives.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Primitives.h"
#include "Texture.h"
#include "ui_main.h"

//----------------------------------------------------

const FPcolor boxcolor = {255,255,255,0};
const int boxindexcount = 24;
const WORD boxindices[boxindexcount] = {
	0, 1, 1, 2, 2, 3, 3, 0,
	0, 4, 1, 5, 2, 6, 3, 7,
	4, 5, 5, 6, 6, 7, 7, 4 };


//----------------------------------------------------

void DrawSelectionBox( IAABox& box, FPcolor* c ){

	FLvertex verts[8];

    FPcolor cc = (c)?(*c):boxcolor;

	for( int i=0; i<8;i++){
		verts[i].color.c = cc.c;
		verts[i].specular.c = cc.c;
		verts[i].tu = 0;
		verts[i].tv = 0;
	}

	Fvector& vmin = box.vmin;
	Fvector& vmax = box.vmax;

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
    UI.D3D_ApplyRenderState(UI.SB_DrawWire);
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

void DrawQuadPlane( Fvector *points, bool selected, float alpha ){
	_ASSERTE( points );

	FLvertex verts1[4];
	FLvertex verts2[4];

	FPcolor color1,color2;
	if( selected ){
		color1.set( 255, 127, 255, int(255*alpha) );
		color2.set( 255, 0, 0, 255 );
	} else {
		color1.set( 140, 140, 140, int(255*alpha) );
		color2.set( 127, 255, 127, 255 );
	}

	for(int i=0; i<4; i++){

		verts1[i].color.set(color1);
		verts1[i].specular.set(color1);

		verts1[i].tu = 0.f;
		verts1[i].tv = 0.f;

		verts1[i].x = points[i].x;
		verts1[i].y = points[i].y;
		verts1[i].z = points[i].z;

		verts2[i].color.set(color2);           
		verts2[i].specular.set(color2);

		verts2[i].tu = 0.f;
		verts2[i].tv = 0.f;

		verts2[i].x = points[i].x;
		verts2[i].y = points[i].y;
		verts2[i].z = points[i].z;
	}

	CDX( UI.d3d()->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,D3DFVF_LVERTEX,
		verts1, 4, (LPWORD)planeindices2, planeindexcount2, 0 ));

	CDX( UI.d3d()->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		verts2, 4, (LPWORD)planeindices1, planeindexcount1, 0 ));
}


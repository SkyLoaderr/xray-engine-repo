// file: D3DUtils.cpp

#include "Pch.h"
#pragma hdrstop

#include "D3DUtils.h"

#define LINE_DIVISION  64

// for drawing sphere
static D3DLVERTEX linebuffer1[LINE_DIVISION];
static D3DLVERTEX linebuffer2[LINE_DIVISION];
static D3DLVERTEX linebuffer3[LINE_DIVISION];
static IVector circledef1[LINE_DIVISION];
static IVector circledef2[LINE_DIVISION];
static IVector circledef3[LINE_DIVISION];
static WORD lineindices[LINE_DIVISION*2];


void DU_InitUtilLibrary(){

	for(int i=0; i<(LINE_DIVISION-1); i++){
		lineindices[i*2] = i;
		lineindices[i*2+1] = i+1;
	}
	lineindices[LINE_DIVISION*2-2] = LINE_DIVISION-1;
	lineindices[LINE_DIVISION*2-1] = 0;

	for(i=0;i<LINE_DIVISION;i++){
		float angle = M_PI * 2.f * (i / (float)LINE_DIVISION);
		float _cos = cosf( angle );
		float _sin = sinf( angle );
		circledef1[i].x = _cos;
		circledef1[i].y = _sin;
		circledef1[i].z = 0;
		circledef2[i].x = 0;
		circledef2[i].y = _cos;
		circledef2[i].z = _sin;
		circledef3[i].x = _sin;
		circledef3[i].y = 0;
		circledef3[i].z = _cos;
	}
}

void DU_UninitUtilLibrary(){
}



//----------------



void DU_DrawLineSphere(
	LPDIRECT3DDEVICE7 device,
	DU_SphereDefinition *params )
{
	_ASSERTE( params );
	_ASSERTE( device );

	for( int i=0; i<LINE_DIVISION; i++){
		linebuffer1[i].x = circledef1[i].x * params->radius  + params->center.x;
		linebuffer1[i].y = circledef1[i].y * params->radius  + params->center.y;
		linebuffer1[i].z = circledef1[i].z * params->radius  + params->center.z;
		linebuffer1[i].color = params->color.value;
		linebuffer1[i].specular = params->color.value;
		linebuffer2[i].x = circledef2[i].x * params->radius  + params->center.x;
		linebuffer2[i].y = circledef2[i].y * params->radius  + params->center.y;
		linebuffer2[i].z = circledef2[i].z * params->radius  + params->center.z;
		linebuffer2[i].color = params->color.value;
		linebuffer2[i].specular = params->color.value;
		linebuffer3[i].x = circledef3[i].x * params->radius  + params->center.x;
		linebuffer3[i].y = circledef3[i].y * params->radius  + params->center.y;
		linebuffer3[i].z = circledef3[i].z * params->radius  + params->center.z;
		linebuffer3[i].color = params->color.value;
		linebuffer3[i].specular = params->color.value;
	}

	CDX( device->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		linebuffer1, LINE_DIVISION,
		lineindices, LINE_DIVISION*2,
		0 ));

	CDX( device->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		linebuffer2, LINE_DIVISION,
		lineindices, LINE_DIVISION*2,
		0 ));

	CDX( device->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		linebuffer3, LINE_DIVISION,
		lineindices, LINE_DIVISION*2,
		0 ));
}


//----------------


void DU_DrawDirectionalLight(
	LPDIRECT3DDEVICE7 device,
	DU_DirectionDefinition *params )
{

}



//----------------

// file: D3DUtils.cpp

#include "Pch.h"
#pragma hdrstop

#include "D3DUtils.h"
#include "Primitives.h"

#define LINE_DIVISION  32

// for drawing sphere
static FLvertex linebuffer1[LINE_DIVISION];
static FLvertex linebuffer2[LINE_DIVISION];
static FLvertex linebuffer3[LINE_DIVISION];
static Fvector circledef1[LINE_DIVISION];
static Fvector circledef2[LINE_DIVISION];
static Fvector circledef3[LINE_DIVISION];
static WORD lineindices[LINE_DIVISION*2];

static FLvertex crossbuffer[7];
static WORD crossindices[12];

void DU_InitUtilLibrary(){

	for(int i=0;i<6;i++){
		crossindices[2*i] = 0;
		crossindices[2*i+1] = i+1;
	}

	for(i=0; i<(LINE_DIVISION-1); i++){
		lineindices[i*2] = i;
		lineindices[i*2+1] = i+1;
	}
	lineindices[LINE_DIVISION*2-2] = LINE_DIVISION-1;
	lineindices[LINE_DIVISION*2-1] = 0;

	for(i=0;i<LINE_DIVISION;i++){
		float angle = M_PI * 2.f * (i / (float)LINE_DIVISION);
		float _cos = cos( angle );
		float _sin = sin( angle );
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

void DU_DrawLineSphere( LPDIRECT3DDEVICE7 device, DU_SphereDefinition *params )
{
	_ASSERTE( params );
	_ASSERTE( device );

    Fvector t;
	for( int i=0; i<LINE_DIVISION; i++){
        t.mul(circledef1[i],params->radius); t.add(params->center);
        linebuffer1[i].set(t,params->color,params->color,0,0);
        t.mul(circledef2[i],params->radius); t.add(params->center);
        linebuffer2[i].set(t,params->color,params->color,0,0);
        t.mul(circledef3[i],params->radius); t.add(params->center);
        linebuffer3[i].set(t,params->color,params->color,0,0);
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

	for(i=0;i<7;i++)
        crossbuffer[i].set(params->center,params->color,params->color,0,0);
	crossbuffer[1].x += params->radius;
	crossbuffer[2].x -= params->radius;
	crossbuffer[3].y += params->radius;
	crossbuffer[4].y -= params->radius;
	crossbuffer[5].z += params->radius;
	crossbuffer[6].z -= params->radius;

    Fmatrix m_Identity;
    m_Identity.identity();
	CDX( device->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		crossbuffer, 7, crossindices, 12, 0 ));
}


//----------------


void DU_DrawDirectionalLight(
	LPDIRECT3DDEVICE7 device,
	DU_DirectionDefinition *params )
{
	_ASSERTE( device );
    params->direction.normalize_safe();

    IAABox b;

    b.vmin.set(params->center.x-0.5f,params->center.y-0.5f,params->center.z-0.5f);
    b.vmax.set(params->center.x+0.5f,params->center.y+0.5f,params->center.z+0.5f);

    DrawSelectionBox(b,&params->color);

    linebuffer1[0].set(params->center,params->color,params->color,0,0);
    linebuffer1[1].set(params->direction,params->color,params->color,0,0);
    linebuffer1[1].p.mul(100);

	CDX( device->DrawPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		linebuffer1, 2,
		0 ));
}

void DU_DrawLine( LPDIRECT3DDEVICE7 device,
	Fvector& center, Fvector& direction, float radius, FPcolor& color)
{
	_ASSERTE( device );
    direction.normalize_safe();

	linebuffer1[0].x = center.x;
	linebuffer1[0].y = center.y;
	linebuffer1[0].z = center.z;
	linebuffer1[0].color.c = linebuffer1[0].specular.c = color.c;

	linebuffer1[1].x = direction.x * radius  + center.x;
	linebuffer1[1].y = direction.y * radius  + center.y;
	linebuffer1[1].z = direction.z * radius  + center.z;
	linebuffer1[1].color.c = linebuffer1[1].specular.c = color.c;
	CDX( device->DrawPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		linebuffer1, 2,
		0 ));
}
//----------------

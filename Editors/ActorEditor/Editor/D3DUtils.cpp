// file: D3DUtils.cpp

#include "stdafx.h"
#pragma hdrstop

#include "gamefont.h"
#include "d3dutils.h"
#include "du_box.h"
#include "du_sphere.h"

#define LINE_DIVISION  32  // не меньше 6!!!!!
namespace DU{
// for drawing sphere
static Fvector circledef1[LINE_DIVISION];
static Fvector circledef2[LINE_DIVISION];
static Fvector circledef3[LINE_DIVISION];

const u32 boxcolor = D3DCOLOR_RGBA(255,255,255,0);
static const int boxvertcount = 48;
static Fvector boxvert[boxvertcount];

// identity box
const u32 identboxcolor = D3DCOLOR_RGBA(255,255,255,0);
static const int identboxwirecount = 24;
static Fvector identboxwire[identboxwirecount] = {
	-0.5f, -0.5f, -0.5f,	-0.5f, +0.5f, -0.5f,    	-0.5f, +0.5f, -0.5f,	+0.5f, +0.5f, -0.5f,
    +0.5f, +0.5f, -0.5f,	+0.5f, -0.5f, -0.5f,		+0.5f, -0.5f, -0.5f,	-0.5f, -0.5f, -0.5f,
    -0.5f, +0.5f, +0.5f,	+0.5f, +0.5f, +0.5f,		+0.5f, +0.5f, +0.5f,	+0.5f, -0.5f, +0.5f,
    +0.5f, -0.5f, +0.5f,	-0.5f, -0.5f, +0.5f,    	-0.5f, -0.5f, +0.5f,	-0.5f, +0.5f, +0.5f,
    -0.5f, +0.5f, -0.5f,	-0.5f, +0.5f, +0.5f,		+0.5f, +0.5f, -0.5f,	+0.5f, +0.5f, +0.5f,
    +0.5f, -0.5f, -0.5f,	+0.5f, -0.5f, +0.5f,		-0.5f, -0.5f, -0.5f,	-0.5f, -0.5f, +0.5f
};

/*
static const int identboxindexcount = 36;
static const WORD identboxindices[identboxindexcount] = {
	0, 1, 2,   2, 3, 0,
	3, 2, 6,   6, 7, 3,
	6, 4, 5,   6, 5, 7,
    4, 1, 5,   1, 0, 5,
    3, 5, 0,   3, 7, 5,
    1, 4, 6,   1, 6, 2};
static const int identboxindexwirecount = 24;
static const WORD identboxindiceswire[identboxindexwirecount] = {
	0, 1, 	1, 2,
	2, 3, 	3, 0,
	4, 6, 	6, 7,
    7, 5, 	5, 4,
    1, 4, 	2, 6,
    3, 7, 	0, 5};
*/

#define SIGN(x) ((x<0)?-1:1)

static SGeometry* 	vs_L=0;
static SGeometry* 	vs_TL=0;
static SGeometry* 	vs_LIT=0;

static FLvertexVec 	m_GridPoints;

u32 m_ColorAxis	= 0xff000000;
u32 m_ColorGrid	= 0xff909090;
u32 m_ColorGridTh = 0xffb4b4b4;
u32 m_SelectionRect=D3DCOLOR_RGBA(127,255,127,64);

u32 m_ColorSafeRect = 0xffB040B0;

void UpdateGrid(int number_of_cell, float square_size, int subdiv){
	m_GridPoints.clear();
// grid
	int m_GridSubDiv[2];
	int m_GridCounts[2];
    Fvector2 m_GridStep;

    m_GridStep.set(square_size,square_size);
	m_GridSubDiv[0] = subdiv;
	m_GridSubDiv[1] = subdiv;
	m_GridCounts[0] = number_of_cell;//iFloor(size/step)*subdiv;
	m_GridCounts[1] = number_of_cell;//iFloor(size/step)*subdiv;

	FVF::L left,right;
	left.p.y = right.p.y = 0;

	for(int thin=0; thin<2; thin++){
		for(int i=-m_GridCounts[0]; i<=m_GridCounts[0]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[0]) ){
				left.p.z = -m_GridCounts[1]*m_GridStep.y;
				right.p.z = m_GridCounts[1]*m_GridStep.y;
				left.p.x = i*m_GridStep.x;
				right.p.x = left.p.x;
				left.color = (i%m_GridSubDiv[0]) ? m_ColorGrid : m_ColorGridTh;
				right.color = left.color;
				m_GridPoints.push_back( left );
				m_GridPoints.push_back( right );
			}
		}
		for(i=-m_GridCounts[1]; i<=m_GridCounts[1]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[1]) ){
				left.p.x = -m_GridCounts[0]*m_GridStep.x;
				right.p.x = m_GridCounts[0]*m_GridStep.x;
				left.p.z = i*m_GridStep.y;
				right.p.z = left.p.z;
				left.color = (i%m_GridSubDiv[1]) ? m_ColorGrid : m_ColorGridTh;
				right.color = left.color;
				m_GridPoints.push_back( left );
				m_GridPoints.push_back( right );
			}
		}
	}
}

void OnDeviceCreate(){
	for(int i=0;i<LINE_DIVISION;i++){
		float angle = M_PI * 2.f * (i / (float)LINE_DIVISION);
        float _sin, _cos;
        _sincos(angle,_sin,_cos);
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
    // initialize identity box
	Fbox bb;
	bb.set(-0.505f,-0.505f,-0.505f, 0.505f,0.505f,0.505f);
	for (i=0; i<8; i++){
    	Fvector S;
    	Fvector p;
        bb.getpoint(i,p);
        S.set(SIGN(p.x),SIGN(p.y),SIGN(p.z));
    	boxvert[i*6+0].set(p);
    	boxvert[i*6+1].set(p.x-S.x*0.25f,p.y,p.z);
    	boxvert[i*6+2].set(p);
    	boxvert[i*6+3].set(p.x,p.y-S.y*0.25f,p.z);
    	boxvert[i*6+4].set(p);
    	boxvert[i*6+5].set(p.x,p.y,p.z-S.z*0.25f);
    }
    // create render stream
	vs_L 		= Device.Shader.CreateGeom(FVF::F_L,RCache.Vertex.Buffer(),RCache.Index.Buffer());
    vs_TL		= Device.Shader.CreateGeom(FVF::F_TL,RCache.Vertex.Buffer(),RCache.Index.Buffer());
    vs_LIT		= Device.Shader.CreateGeom(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.Index.Buffer());
}

void OnDeviceDestroy(){
	Device.Shader.DeleteGeom(vs_L);
	Device.Shader.DeleteGeom(vs_TL);
	Device.Shader.DeleteGeom(vs_LIT);
}
//----------------

void DrawSpotLight(const Fvector& p, const Fvector& d, float range, float phi, u32 clr)
{
    Fmatrix T;
	Fvector p1;
    float H,P;
    float da	= PI_MUL_2/LINE_DIVISION;
	float r		= range*tanf(phi/2);
    d.getHP		(H,P);
    T.setHPB	(H,P,0);
    T.translate_over(p);
	for (float angle=0; angle<PI_MUL_2; angle+=da){
		float 	sa,ca; _sincos(angle,sa,ca);
		p1.x	= r * ca;
		p1.y	= r * sa;
        p1.z	= range;
        T.transform_tiny(p1);
    	DrawLine(p,p1,clr);
    }
    p1.mad(p,d,range);
   	DrawLine(p,p1,clr);
}

void DrawDirectionalLight(const Fvector& p, const Fvector& d, float radius, float range, u32 c)
{
    float r=radius*0.71f;
	Fvector R,N,D; D.normalize(d);
	Fmatrix rot;

    N.set		(0,1,0);
	if (fabsf(D.y)>0.99f) N.set(1,0,0);
	R.crossproduct(N,D); R.normalize();
	N.crossproduct(D,R); N.normalize();
    rot.set(R,N,D,p);
	float sz=radius+range;

	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32				vBase;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(6,vs_L->vb_stride,vBase);
    pv->set			(0,0,r,		c); rot.transform_tiny(pv->p); pv++;
    pv->set			(0,0,sz,	c); rot.transform_tiny(pv->p); pv++;
    pv->set			(-r,0,r,	c); rot.transform_tiny(pv->p); pv++;
    pv->set			(-r,0,sz,	c); rot.transform_tiny(pv->p); pv++;
    pv->set			(r,0,r,		c); rot.transform_tiny(pv->p); pv++;
    pv->set			(r,0,sz,	c); rot.transform_tiny(pv->p); pv++;
	Stream->Unlock	(6,vs_L->vb_stride);

	// and Render it as triangle list
    Device.DP		(D3DPT_LINELIST,vs_L,vBase,3);

    Fbox b;
    b.min.set(-r,-r,-r);
    b.max.set(r,r,r);

	DrawLineSphere	( p, radius, c, true );
}

void DrawPointLight(const Fvector& p, float radius, u32 c)
{
	DrawCross(p, radius,radius,radius, radius,radius,radius, c, true);
}

void DrawEntity(u32 clr, Shader* s)
{
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(5,vs_L->vb_stride,vBase);
    pv->set			(0.f,0.f,0.f,clr); pv++;
    pv->set			(0.f,1.f,0.f,clr); pv++;
    pv->set			(0.f,1.f,.5f,clr); pv++;
    pv->set			(0.f,.5f,.5f,clr); pv++;
    pv->set			(0.f,.5f,0.f,clr); pv++;
	Stream->Unlock	(5,vs_L->vb_stride);
	// render flagshtok
    Device.SetShader(Device.m_WireShader);
    Device.DP		(D3DPT_LINESTRIP,vs_L,vBase,4);

    if (s) Device.SetShader(s);
    {
        // fill VB
        FVF::LIT*	pv	 = (FVF::LIT*)Stream->Lock(6,vs_LIT->vb_stride,vBase);
        pv->set		(0.f,1.f,0.f,clr,0.f,0.f);	pv++;
        pv->set		(0.f,1.f,.5f,clr,1.f,0.f);	pv++;
        pv->set		(0.f,.5f,.5f,clr,1.f,1.f);	pv++;
        pv->set		(0.f,.5f,0.f,clr,0.f,1.f);	pv++;
        pv->set		(0.f,.5f,.5f,clr,1.f,1.f);	pv++;
        pv->set		(0.f,1.f,.5f,clr,1.f,0.f);	pv++;
        Stream->Unlock	(6,vs_LIT->vb_stride);
        // and Render it as line list
        Device.DP		(D3DPT_TRIANGLEFAN,vs_LIT,vBase,4);
    }
}

void DrawFlag(const Fvector& p, float heading, float height, float sz, float sz_fl, u32 clr, bool bDrawEntity){
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(2,vs_L->vb_stride,vBase);
    pv->set			(p,clr); pv++;
    pv->set			(p.x,p.y+height,p.z,clr); pv++;
	Stream->Unlock	(2,vs_L->vb_stride);
	// and Render it as triangle list
    Device.DP		(D3DPT_LINELIST,vs_L,vBase,1);

    if (bDrawEntity){
		// fill VB
        float rx		= sin(heading);
        float rz		= cos(heading);
		FVF::L*	pv	 	= (FVF::L*)Stream->Lock(6,vs_L->vb_stride,vBase);
        sz				*= 0.8f;
        pv->set			(p.x,p.y+height,p.z,clr);											pv++;
        pv->set			(p.x+rx*sz,p.y+height,p.z+rz*sz,clr);                               pv++;
        sz				*= 0.5f;
        pv->set			(p.x,p.y+height*(1.f-sz_fl*.5f),p.z,clr);                           pv++;
        pv->set			(p.x+rx*sz*0.6f,p.y+height*(1.f-sz_fl*.5f),p.z+rz*sz*0.75f,clr);   	pv++;
        pv->set			(p.x,p.y+height*(1.f-sz_fl),p.z,clr);                               pv++;
        pv->set			(p.x+rx*sz,p.y+height*(1.f-sz_fl),p.z+rz*sz,clr);                   pv++;
		Stream->Unlock	(6,vs_L->vb_stride);
		// and Render it as line list
    	Device.DP		(D3DPT_LINELIST,vs_L,vBase,3);
    }else{
		// fill VB
		FVF::L*	pv	 	= (FVF::L*)Stream->Lock(6,vs_L->vb_stride,vBase);
	    pv->set			(p.x,p.y+height*(1.f-sz_fl),p.z,clr); 								pv++;
    	pv->set			(p.x,p.y+height,p.z,clr); 											pv++;
	    pv->set			(p.x+sin(heading)*sz,((pv-2)->p.y+(pv-1)->p.y)/2,p.z+cos(heading)*sz,clr); pv++;
    	pv->set			(*(pv-3)); 															pv++;
	    pv->set			(*(pv-2)); 															pv++;
    	pv->set			(*(pv-4)); 															pv++;
		Stream->Unlock	(6,vs_L->vb_stride);
		// and Render it as triangle list
    	Device.DP		(D3DPT_TRIANGLELIST,vs_L,vBase,2);
    }
}

//------------------------------------------------------------------------------

void DrawRomboid(const Fvector& p, float r, u32 c){
static const WORD IL[24]={0,2, 2,5, 0,5, 3,5, 3,0, 4,3, 4,0, 4,2, 1,2, 1,5, 1,3, 1,4};
static const WORD IT[24]={2,4,0, 4,3,0, 3,5,0, 5,2,0, 4,2,1, 2,5,1, 5,3,1, 3,4,1};
	u32			vBase,iBase;

    Fcolor C;
    C.set			(c);
    C.mul_rgb		(0.75);
    u32 c1 =		C.get();

	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(6,vs_L->vb_stride,vBase);
    pv->set			(p.x,	p.y+r,	p.z,	c1); pv++;
    pv->set			(p.x,	p.y-r,	p.z,	c1); pv++;
    pv->set			(p.x,	p.y,	p.z-r,	c1); pv++;
    pv->set			(p.x,	p.y,	p.z+r,	c1); pv++;
    pv->set			(p.x-r,	p.y,	p.z,	c1); pv++;
    pv->set			(p.x+r,	p.y,	p.z,	c1); pv++;
	Stream->Unlock	(6,vs_L->vb_stride);

	_IndexStream*	StreamI	= &RCache.Index;
    WORD* i 		= StreamI->Lock(24,iBase);
    for (int k=0; k<24; k++,i++) *i=IT[k];
    StreamI->Unlock(24);

	// and Render it as triangle list
	Device.DIP		(D3DPT_TRIANGLELIST,vs_L,0,vBase,6, iBase,12);

    // draw lines
	pv	 			= (FVF::L*)Stream->Lock(6,vs_L->vb_stride,vBase);
    pv->set			(p.x,	p.y+r,	p.z,	c); pv++;
    pv->set			(p.x,	p.y-r,	p.z,	c); pv++;
    pv->set			(p.x,	p.y,	p.z-r,	c); pv++;
    pv->set			(p.x,	p.y,	p.z+r,	c); pv++;
    pv->set			(p.x-r,	p.y,	p.z,	c); pv++;
    pv->set			(p.x+r,	p.y,	p.z,	c); pv++;
	Stream->Unlock	(6,vs_L->vb_stride);

    i 				= StreamI->Lock(24,iBase);
    for (k=0; k<24; k++,i++) *i=IL[k];
    StreamI->Unlock	(24);

	Device.DIP		(D3DPT_LINELIST,vs_L,0,vBase,6, iBase,12);
}
//------------------------------------------------------------------------------

void DrawSound(const Fvector& p, float r, u32 c){
	DrawCross(p, r,r,r, r,r,r, c, true);
}
//------------------------------------------------------------------------------
void DrawSphere(const Fvector& p, float radius, u32 clr)
{
	u32			vBase,iBase;

	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(DU_SPHERE_NUMVERTEX,vs_L->vb_stride,vBase);
    for (int k=0; k<DU_SPHERE_NUMVERTEX; k++,pv++){
    	Fvector& v	= du_sphere_vertices[k];
	    pv->set		(v.x*radius+p.x,	v.y*radius+p.y,	v.z*radius+p.z,	clr);
    }
	Stream->Unlock	(DU_SPHERE_NUMVERTEX,vs_L->vb_stride);

	_IndexStream*	StreamI	= &RCache.Index;
    WORD* i 		= StreamI->Lock(DU_SPHERE_NUMFACES*3,iBase);
    for (k=0; k<DU_SPHERE_NUMFACES*3; k++,i++) *i=du_sphere_faces[k];
    StreamI->Unlock(DU_SPHERE_NUMFACES*3);

	// and Render it as triangle list
	Device.DIP		(D3DPT_TRIANGLELIST,vs_L,vBase,0,DU_SPHERE_NUMVERTEX, iBase,DU_SPHERE_NUMFACES);
}

void DrawLineSphere(const Fvector& p, float radius, u32 c, bool bCross)
{
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase, iBase;
    int i;
	FVF::L*	pv;
    // seg 0
	pv	 			= (FVF::L*)Stream->Lock(LINE_DIVISION+1,vs_L->vb_stride,vBase);
	for( i=0; i<LINE_DIVISION; i++,pv++){ pv->p.mad(p,circledef1[i],radius); pv->color=c;}
    pv->set(*(pv-LINE_DIVISION));
	Stream->Unlock	(LINE_DIVISION+1,vs_L->vb_stride);
	Device.DP		(D3DPT_LINESTRIP,vs_L,vBase,LINE_DIVISION);
    // seg 1
	pv	 			= (FVF::L*)Stream->Lock(LINE_DIVISION+1,vs_L->vb_stride,vBase);
	for( i=0; i<LINE_DIVISION; i++){ pv->p.mad(p,circledef2[i],radius); pv->color=c; pv++; }
    pv->set(*(pv-LINE_DIVISION)); pv++;
	Stream->Unlock	(LINE_DIVISION+1,vs_L->vb_stride);
	Device.DP		(D3DPT_LINESTRIP,vs_L,vBase,LINE_DIVISION);
    // seg 2
	pv	 			= (FVF::L*)Stream->Lock(LINE_DIVISION+1,vs_L->vb_stride,vBase);
	for( i=0; i<LINE_DIVISION; i++){ pv->p.mad(p,circledef3[i],radius); pv->color=c; pv++; }
    pv->set(*(pv-LINE_DIVISION)); pv++;
	Stream->Unlock	(LINE_DIVISION+1,vs_L->vb_stride);
	Device.DP		(D3DPT_LINESTRIP,vs_L,vBase,LINE_DIVISION);

    if (bCross) DrawCross(p, radius,radius,radius, radius,radius,radius, c);
}

//----------------------------------------------------
void dbgDrawPlacement(const Fvector& p, int sz, u32 clr, LPCSTR caption, u32 clr_font)
{
	VERIFY( Device.bReady );
    Fvector c,r,n,d;
	float w	= p.x*Device.mFullTransform._14 + p.y*Device.mFullTransform._24 + p.z*Device.mFullTransform._34 + Device.mFullTransform._44;
    if (w<0) return; // culling

	float s = sz;
	Device.mFullTransform.transform(c,p);
	c.x = iFloor(Device._x2real(c.x)); c.y = iFloor(Device._y2real(-c.y));

	_VertexStream*	Stream	= &RCache.Vertex;
    u32 vBase;
	FVF::TL* pv	= (FVF::TL*)Stream->Lock(5,vs_TL->vb_stride,vBase);
	pv->p.set(c.x-s,c.y-s,0,1); pv->color=clr; pv++;
	pv->p.set(c.x+s,c.y-s,0,1); pv->color=clr; pv++;
	pv->p.set(c.x+s,c.y+s,0,1); pv->color=clr; pv++;
	pv->p.set(c.x-s,c.y+s,0,1); pv->color=clr; pv++;
	pv->p.set(c.x-s,c.y-s,0,1); pv->color=clr; pv++;
	Stream->Unlock(5,vs_TL->vb_stride);

	// Render it as line strip
    Device.DP		(D3DPT_LINESTRIP,vs_TL,vBase,4);
    if (caption){
	    Device.pSystemFont->SetColor(clr_font);
    	Device.pSystemFont->Out(c.x,c.y+s,"%s",caption);
    }
}

void dbgDrawVert(const Fvector& p0, u32 clr, LPCSTR caption)
{
	dbgDrawPlacement(p0,3,clr,caption);
	DrawCross		(p0,0.01f,0.01f,0.01f, 0.01f,0.01f,0.01f, clr,false);
}

void dbgDrawEdge(const Fvector& p0,	const Fvector& p1, u32 clr, LPCSTR caption)
{
	dbgDrawPlacement(p0,3,clr,caption);
	DrawCross		(p0,0.01f,0.01f,0.01f, 0.01f,0.01f,0.01f, clr,false);
	DrawCross		(p1,0.01f,0.01f,0.01f, 0.01f,0.01f,0.01f, clr,false);
    DrawLine		(p0,p1,clr);
}

void dbgDrawFace(const Fvector& p0,	const Fvector& p1, const Fvector& p2, u32 clr, LPCSTR caption)
{
	dbgDrawPlacement(p0,3,clr,caption);
	DrawCross		(p0,0.01f,0.01f,0.01f, 0.01f,0.01f,0.01f, clr,false);
	DrawCross		(p1,0.01f,0.01f,0.01f, 0.01f,0.01f,0.01f, clr,false);
	DrawCross		(p2,0.01f,0.01f,0.01f, 0.01f,0.01f,0.01f, clr,false);
    DrawLine		(p0,p1,clr);
    DrawLine		(p1,p2,clr);
    DrawLine		(p2,p0,clr);
}
//----------------------------------------------------

void DrawLine(const Fvector& p0, const Fvector& p1, u32 c){
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(2,vs_L->vb_stride,vBase);
    pv->set			(p0,c); pv++;
    pv->set			(p1,c); pv++;
	Stream->Unlock	(2,vs_L->vb_stride);
	// and Render it as triangle list
    Device.DP		(D3DPT_LINELIST,vs_L,vBase,1);
}

//----------------------------------------------------
void DrawSelectionBox(const Fvector& C, const Fvector& S, u32* c){
    u32 cc=(c)?*c:boxcolor;

	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase,iBase;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(boxvertcount,vs_L->vb_stride,vBase);
    for (int i=0; i<boxvertcount; i++,pv++){
	    pv->p.mul(boxvert[i],S);
        pv->p.add(C);
        pv->color	= cc;
    }
	Stream->Unlock	(boxvertcount,vs_L->vb_stride);

	// and Render it as triangle list
	Device.SetRS	(D3DRS_FILLMODE,D3DFILL_SOLID);
    Device.DP		(D3DPT_LINELIST,vs_L,vBase,boxvertcount/2);
    Device.SetRS	(D3DRS_FILLMODE,Device.dwFillMode);
}

void DrawIdentBox(bool bSolid, bool bWire, u32 cc)
{
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
    if (bWire){
        u32			vBase;
		FVF::L*	pv	 	= (FVF::L*)Stream->Lock(identboxwirecount,vs_L->vb_stride,vBase);
        for (int i=0; i<identboxwirecount; i++,pv++)
            pv->set		(identboxwire[i],cc);
		Stream->Unlock(identboxwirecount,vs_L->vb_stride);
	    Device.DP		(D3DPT_LINELIST,vs_L,vBase,identboxwirecount/2);
    }
    if (bSolid){
        u32			vBase;
		FVF::L*	pv	 	= (FVF::L*)Stream->Lock(DU_BOX_NUMVERTEX2,vs_L->vb_stride,vBase);
        for (int i=0; i<DU_BOX_NUMVERTEX2; i++,pv++)
            pv->set		(du_box_vertices2[i],cc);
		Stream->Unlock(DU_BOX_NUMVERTEX2,vs_L->vb_stride);
	    Device.DP		(D3DPT_TRIANGLELIST,vs_L,vBase,DU_BOX_NUMFACES);
    }
}

void DrawBox(const Fvector& offs, const Fvector& Size, bool bWire, u32 c)
{
	_VertexStream*	Stream	= &RCache.Vertex;
    if (bWire){
        u32			vBase;
		FVF::L*	pv	 	= (FVF::L*)Stream->Lock(identboxwirecount,vs_L->vb_stride,vBase);
        for (int i=0; i<identboxwirecount; i++, pv++){
            pv->p.mul	(identboxwire[i],Size);
            pv->p.mul	(2);
            pv->p.add	(offs);
            pv->color	= c;
        }
		Stream->Unlock(identboxwirecount,vs_L->vb_stride);

	    Device.DP		(D3DPT_LINELIST,vs_L,vBase,identboxwirecount/2);
    }else{
        u32			vBase;
		FVF::L*	pv	 	= (FVF::L*)Stream->Lock(DU_BOX_NUMVERTEX2,vs_L->vb_stride,vBase);
        for (int i=0; i<DU_BOX_NUMVERTEX2; i++, pv++){
            pv->p.mul	(du_box_vertices2[i],Size);
            pv->p.mul	(2);
            pv->p.add	(offs);
            pv->color	= c;
        }
		Stream->Unlock(DU_BOX_NUMVERTEX2,vs_L->vb_stride);

	    Device.DP		(D3DPT_TRIANGLELIST,vs_L,vBase,DU_BOX_NUMFACES);
    }
}
//----------------------------------------------------

void DrawPlane  (const Fvector& center, const Fvector2& scale, const Fvector& rotate, u32 c, bool bCull, bool bBorder, u32 cb)
{
    Fmatrix M;
    M.setHPB(rotate.y,rotate.x,rotate.z);
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(4,vs_L->vb_stride,vBase);
    pv->set			(-scale.x, 0, -scale.y, c); M.transform_tiny(pv->p); pv->p.add(center); pv++;
    pv->set			(+scale.x, 0, -scale.y, c); M.transform_tiny(pv->p); pv->p.add(center); pv++;
    pv->set			(+scale.x, 0, +scale.y, c); M.transform_tiny(pv->p); pv->p.add(center); pv++;
    pv->set			(-scale.x, 0, +scale.y, c); M.transform_tiny(pv->p); pv->p.add(center); pv++;
	Stream->Unlock(4,vs_L->vb_stride);

    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP		(D3DPT_TRIANGLEFAN,vs_L,vBase,2);
    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);

    if (bBorder){
		FVF::L*	pv	 = (FVF::L*)Stream->Lock(5,vs_L->vb_stride,vBase);
        pv->set		(-scale.x, 0, -scale.y, cb); M.transform_tiny(pv->p); pv->p.add(center); pv++;
        pv->set		(+scale.x, 0, -scale.y, cb); M.transform_tiny(pv->p); pv->p.add(center); pv++;
        pv->set		(+scale.x, 0, +scale.y, cb); M.transform_tiny(pv->p); pv->p.add(center); pv++;
        pv->set		(-scale.x, 0, +scale.y, cb); M.transform_tiny(pv->p); pv->p.add(center); pv++;
        pv->set		(*(pv-4));
		Stream->Unlock(5,vs_L->vb_stride);
	    Device.DP	(D3DPT_LINESTRIP,vs_L,vBase,4);
    }
}
//----------------------------------------------------

void DrawCross(const Fvector& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, u32 clr, bool bRot45)
{
	_VertexStream*	Stream	= &RCache.Vertex;
	// actual rendering
	u32			vBase;
	FVF::L*	pv	 	= (FVF::L*)Stream->Lock(bRot45?12:6,vs_L->vb_stride,vBase);
    pv->set(p.x+szx2,p.y,p.z,clr); pv++;
    pv->set(p.x-szx1,p.y,p.z,clr); pv++;
    pv->set(p.x,p.y+szy2,p.z,clr); pv++;
    pv->set(p.x,p.y-szy1,p.z,clr); pv++;
    pv->set(p.x,p.y,p.z+szz2,clr); pv++;
    pv->set(p.x,p.y,p.z-szz1,clr); pv++;
    if (bRot45){
    	Fmatrix M;
        M.setHPB(PI_DIV_4,PI_DIV_4,PI_DIV_4);
	    for(int i=0;i<6;i++,pv++){
        	pv->p.sub((pv-6)->p,p);
        	M.transform_dir(pv->p);
        	pv->p.add(p);
            pv->color = clr;
        }
    }
	// unlock VB and Render it as triangle list
	Stream->Unlock(bRot45?12:6,vs_L->vb_stride);
    Device.DP(D3DPT_LINELIST,vs_L,vBase,bRot45?6:3);
}

void DrawPivot(const Fvector& pos, float sz){
	Device.SetShader(Device.m_WireShader);
    DrawCross(pos, sz, sz, sz, sz, sz, sz, 0xFF7FFF7F);
}

void DrawAxis(const Fmatrix& T)
{
	_VertexStream*	Stream	= &RCache.Vertex;
	VERIFY( Device.bReady );
    Fvector p[6];
    u32 	c[6];

    // colors
    c[0]=c[2]=c[4]=0x00222222; c[1]=0x00FF0000; c[3]=0x0000FF00; c[5]=0x000000FF;

    // position
  	p[0].mad(T.c,T.k,0.25f);
    p[1].set(p[0]); p[1].x+=.015f;
    p[2].set(p[0]);
    p[3].set(p[0]); p[3].y+=.015f;
    p[4].set(p[0]);
    p[5].set(p[0]); p[5].z+=.015f;

    u32 vBase;
	FVF::TL* pv	= (FVF::TL*)Stream->Lock(6,vs_TL->vb_stride,vBase);
    // transform to screen
    float dx=-float(Device.dwWidth)/2.2f;
    float dy=float(Device.dwHeight)/2.25f;

    for (int i=0; i<6; i++,pv++){
	    pv->color = c[i]; pv->transform(p[i],Device.mFullTransform);
	    pv->p.set(iFloor(Device._x2real(pv->p.x)+dx),iFloor(Device._y2real(pv->p.y)+dy),0,1);
        p[i].set(pv->p.x,pv->p.y,0);
    }

	// unlock VB and Render it as triangle list
	Stream->Unlock(6,vs_TL->vb_stride);
	Device.SetRS(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
	Device.SetShader(Device.m_WireShader);
    Device.DP(D3DPT_LINELIST,vs_TL,vBase,3);
	Device.SetRS(D3DRS_SHADEMODE,Device.dwShadeMode);

    Device.pSystemFont->SetColor(0xFF909090);
    Device.pSystemFont->Out(p[1].x,p[1].y,"x");
    Device.pSystemFont->Out(p[3].x,p[3].y,"y");
    Device.pSystemFont->Out(p[5].x,p[5].y,"z");
    Device.pSystemFont->SetColor(0xFF000000);
    Device.pSystemFont->Out(p[1].x-1,p[1].y-1,"x");
    Device.pSystemFont->Out(p[3].x-1,p[3].y-1,"y");
    Device.pSystemFont->Out(p[5].x-1,p[5].y-1,"z");
}

void DrawObjectAxis(const Fmatrix& T)
{
	VERIFY( Device.bReady );
	_VertexStream*	Stream	= &RCache.Vertex;
    Fvector c,r,n,d;
	float w	= T.c.x*Device.mFullTransform._14 + T.c.y*Device.mFullTransform._24 + T.c.z*Device.mFullTransform._34 + Device.mFullTransform._44;
    if (w<0) return; // culling

	float s = w*0.1;
								Device.mFullTransform.transform(c,T.c);
    r.mul(T.i,s); r.add(T.c); 	Device.mFullTransform.transform(r);
    n.mul(T.j,s); n.add(T.c); 	Device.mFullTransform.transform(n);
    d.mul(T.k,s); d.add(T.c); 	Device.mFullTransform.transform(d);
	c.x = iFloor(Device._x2real(c.x)); c.y = iFloor(Device._y2real(-c.y));
    r.x = iFloor(Device._x2real(r.x)); r.y = iFloor(Device._y2real(-r.y));
    n.x = iFloor(Device._x2real(n.x)); n.y = iFloor(Device._y2real(-n.y));
    d.x = iFloor(Device._x2real(d.x)); d.y = iFloor(Device._y2real(-d.y));

    u32 vBase;
	FVF::TL* pv	= (FVF::TL*)Stream->Lock(6,vs_TL->vb_stride,vBase);
	pv->p.set(c.x,c.y,0,1); pv->color=0xFF222222; pv++;
	pv->p.set(d.x,d.y,0,1); pv->color=0xFF0000FF; pv++;
	pv->p.set(c.x,c.y,0,1); pv->color=0xFF222222; pv++;
	pv->p.set(r.x,r.y,0,1); pv->color=0xFFFF0000; pv++;
	pv->p.set(c.x,c.y,0,1); pv->color=0xFF222222; pv++;
	pv->p.set(n.x,n.y,0,1); pv->color=0xFF00FF00;
	Stream->Unlock(6,vs_TL->vb_stride);

	// Render it as line list
	Device.SetRS	(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
	Device.SetShader(Device.m_WireShader);
    Device.DP		(D3DPT_LINELIST,vs_TL,vBase,3);
	Device.SetRS	(D3DRS_SHADEMODE,Device.dwShadeMode);

    Device.pSystemFont->SetColor(0xFF909090);
    Device.pSystemFont->Out(r.x,r.y,"x");
    Device.pSystemFont->Out(n.x,n.y,"y");
    Device.pSystemFont->Out(d.x,d.y,"z");
    Device.pSystemFont->SetColor(0xFF000000);
    Device.pSystemFont->Out(r.x-1,r.y-1,"x");
    Device.pSystemFont->Out(n.x-1,n.y-1,"y");
    Device.pSystemFont->Out(d.x-1,d.y-1,"z");
}

void DrawSafeRect()
{
	VERIFY( Device.bReady );
	_VertexStream*	Stream	= &RCache.Vertex;
	// fill VB
    Irect rect;
    if ((0.75f*float(Device.dwWidth))>float(Device.dwHeight))
    	rect.set(Device.m_RenderWidth_2-1.33f*float(Device.m_RenderHeight_2),0,Device.m_RenderWidth_2+1.33f*float(Device.m_RenderHeight_2),Device.dwHeight-1);
    else
    	rect.set(0,Device.m_RenderHeight_2-0.75f*float(Device.m_RenderWidth_2),Device.dwWidth-1,Device.m_RenderHeight_2+0.75f*float(Device.m_RenderWidth_2));

    u32 vBase;
	FVF::TL* pv	= (FVF::TL*)Stream->Lock(6,vs_TL->vb_stride,vBase);
    pv->set(0, 0, m_ColorSafeRect,0.f,0.f); 									pv++;
    pv->set(Device.dwWidth, 0, m_ColorSafeRect,0.f,0.f); 						pv++;
    pv->set((int)Device.dwWidth,(int)Device.dwHeight,m_ColorSafeRect,0.f,0.f); 	pv++;
    pv->set(0, Device.dwHeight,m_ColorSafeRect,0.f,0.f); 						pv++;
    pv->set(0, 0, m_ColorSafeRect,0.f,0.f); 									pv++;
    pv->set((int)Device.dwWidth,(int)Device.dwHeight,m_ColorSafeRect,0.f,0.f); 	pv++;
    pv->set(0, Device.dwHeight,m_ColorSafeRect,0.f,0.f); 						pv++;
	Stream->Unlock(6,vs_TL->vb_stride);
	// Render it as triangle list
	Device.SetShader(Device.m_SelectionShader);
}

void DrawGrid()
{
	VERIFY( Device.bReady );
	_VertexStream*	Stream	= &RCache.Vertex;
    u32 vBase,iBase;
	// fill VB
	FVF::L*	pv	= (FVF::L*)Stream->Lock(m_GridPoints.size(),vs_L->vb_stride,vBase);
    for (FLvertexIt v_it=m_GridPoints.begin(); v_it!=m_GridPoints.end(); v_it++,pv++) pv->set(*v_it);
	Stream->Unlock(m_GridPoints.size(),vs_L->vb_stride);
	// Render it as triangle list
    Fmatrix ddd;
    ddd.identity();
    RCache.set_xform_world(ddd);
	Device.SetShader(Device.m_WireShader);
    Device.DP(D3DPT_LINELIST,vs_L,vBase,m_GridPoints.size()/2);
}

void DrawSelectionRect(const Ivector2& m_SelStart, const Ivector2& m_SelEnd){
	VERIFY( Device.bReady );
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
    u32 vBase;
	FVF::TL* pv	= (FVF::TL*)Stream->Lock(4,vs_TL->vb_stride,vBase);
    pv->set(m_SelStart.x*Device.m_ScreenQuality, m_SelStart.y*Device.m_ScreenQuality, m_SelectionRect,0.f,0.f); pv++;
    pv->set(m_SelStart.x*Device.m_ScreenQuality, m_SelEnd.y*Device.m_ScreenQuality,   m_SelectionRect,0.f,0.f); pv++;
    pv->set(m_SelEnd.x*Device.m_ScreenQuality,   m_SelEnd.y*Device.m_ScreenQuality,   m_SelectionRect,0.f,0.f); pv++;
    pv->set(m_SelEnd.x*Device.m_ScreenQuality,   m_SelStart.y*Device.m_ScreenQuality, m_SelectionRect,0.f,0.f); pv++;
	Stream->Unlock(4,vs_TL->vb_stride);
	// Render it as triangle list
    Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
	Device.SetShader(Device.m_SelectionShader);
    Device.DP(D3DPT_TRIANGLEFAN,vs_TL,vBase,2);
    Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

void DrawPrimitiveL	(D3DPRIMITIVETYPE pt, u32 pc, Fvector* vertices, int vc, u32 color, bool bCull, bool bCycle){
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase, dwNeed=(bCycle)?vc+1:vc;
	FVF::L*	pv	= (FVF::L*)Stream->Lock(dwNeed,vs_L->vb_stride,vBase);
    for(int k=0; k<vc; k++,pv++)
    	pv->set		(vertices[k],color);
    if (bCycle)		pv->set(*(pv-vc));
	Stream->Unlock(dwNeed,vs_L->vb_stride);

    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP		(pt,vs_L,vBase,pc);
    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

void DrawPrimitiveTL(D3DPRIMITIVETYPE pt, u32 pc, FVF::TL* vertices, int vc, bool bCull, bool bCycle){
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase, dwNeed=(bCycle)?vc+1:vc;
	FVF::TL* pv		= (FVF::TL*)Stream->Lock(dwNeed,vs_TL->vb_stride,vBase);
    for(int k=0; k<vc; k++,pv++)
    	pv->set		(vertices[k]);
    if (bCycle)		pv->set(*(pv-vc));
	Stream->Unlock(dwNeed,vs_TL->vb_stride);

    if (!bCull) 	Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP		(pt,vs_TL,vBase,pc);
    if (!bCull) 	Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

void DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, u32 pc, FVF::LIT* vertices, int vc, bool bCull, bool bCycle){
	// fill VB
	_VertexStream*	Stream	= &RCache.Vertex;
	u32			vBase, dwNeed=(bCycle)?vc+1:vc;
	FVF::LIT* 	pv	= (FVF::LIT*)Stream->Lock(dwNeed,vs_LIT->vb_stride,vBase);
    for(int k=0; k<vc; k++,pv++)
    	pv->set		(vertices[k]);
    if (bCycle)		pv->set(*(pv-vc));
	Stream->Unlock(dwNeed,vs_LIT->vb_stride);

    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP		(pt,vs_LIT,vBase,pc);
    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

void DrawLink(const Fvector& p0, const Fvector& p1, float sz, u32 clr){
	DrawLine(p1,p0,clr);
    Fvector pp[2],D,R,N={0,1,0};
    D.sub(p1,p0); D.normalize();
    R.crossproduct(N,D); R.mul(0.5f); D.mul(2.0f); N.mul(0.5f);
    // LR
	pp[0].add(R,D); pp[0].mul(sz*-0.5f);	pp[0].add(p1);
	R.invert();
	pp[1].add(R,D); pp[1].mul(sz*-0.5f);	pp[1].add(p1);
	DrawLine(p1,pp[0],clr);
 	DrawLine(p1,pp[1],clr);
    // UB
	pp[0].add(N,D); pp[0].mul(sz*-0.5f);	pp[0].add(p1);
    N.invert();
	pp[1].add(N,D); pp[1].mul(sz*-0.5f);	pp[1].add(p1);
    DrawLine(p1,pp[0],clr);
    DrawLine(p1,pp[1],clr);
}

void OnRender(){
}

}// namespace

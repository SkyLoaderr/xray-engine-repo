// file: D3DUtils.cpp

#include "stdafx.h"
#pragma hdrstop

#include "D3DUtils.h"
#include "UI_Main.h"

#define LINE_DIVISION  32  // не меньше 6!!!!!
namespace DU{
// for drawing sphere
static Fvector circledef1[LINE_DIVISION];
static Fvector circledef2[LINE_DIVISION];
static Fvector circledef3[LINE_DIVISION];
static const WORD circleindices[LINE_DIVISION*2] = { 0, 1,   1, 2,  2, 3,  3, 4,  4, 5,  5, 6,  6, 7,  7, 8,
													 8, 9,   9,10, 10,11, 11,12, 12,13, 13,14, 14,15, 15,16,
                                                     16,17, 17,18, 18,19, 19,20, 20,21, 21,22, 22,23, 23,24,
                                                     24,25, 25,26, 26,27, 27,28, 28,29, 29,30, 30,31, 31,0};

const DWORD boxcolor = D3DCOLOR_RGBA(255,255,255,0);
static const int boxvertcount = 32;
static Fvector boxvert[boxvertcount];
static const int boxindexcount = 48;
static const WORD boxindices[boxindexcount] = {
	0,  1,  0,  2,  0,  3,
	4,  5,  4,  6,  4,  7,
	8,  9,  8,  10, 8,  11,
	12, 13, 12, 14, 12, 15,
	16, 17, 16, 18, 16, 19,
	20, 21, 20, 22, 20, 23,
	24, 25, 24, 26, 24, 27,
	28, 29, 28, 30, 28, 31
};
// identity box
const DWORD identboxcolor = D3DCOLOR_RGBA(255,255,255,0);
static const int identboxvertcount = 8;
static Fvector identboxvert[identboxvertcount] = {
	-0.5f, -0.5f, -0.5f,
	-0.5f, +0.5f, -0.5f,
	+0.5f, +0.5f, -0.5f,
	+0.5f, -0.5f, -0.5f,
	-0.5f, +0.5f, +0.5f,
	-0.5f, -0.5f, +0.5f,
	+0.5f, +0.5f, +0.5f,
	+0.5f, -0.5f, +0.5f,
};
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

#define SIGN(x) ((x<0)?-1:1)

static CVertexStream* LStream=0;
static CVertexStream* TLStream=0;
static CVertexStream* LITStream=0;

static FLvertexVec 	m_GridPoints;

DWORD m_ColorAxis	= 0xff000000;
DWORD m_ColorGrid	= 0xff909090;
DWORD m_ColorGridTh = 0xffb4b4b4;
DWORD m_SelectionRect=D3DCOLOR_RGBA(127,255,127,64);


void UpdateGrid(float size, float step, int subdiv){
	m_GridPoints.clear();
// grid
	int m_GridSubDiv[2];
	int m_GridCounts[2];
    Fvector2 m_GridStep;

    m_GridStep.set(step/subdiv,step/subdiv);
	m_GridSubDiv[0] = subdiv;
	m_GridSubDiv[1] = subdiv;
	m_GridCounts[0] = iFloor(size/step)*subdiv;
	m_GridCounts[1] = iFloor(size/step)*subdiv;

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

void InitUtilLibrary(){
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
    	boxvert[i*4+0].set(p);
    	boxvert[i*4+1].set(p.x-S.x*0.25f,p.y,p.z);
    	boxvert[i*4+2].set(p.x,p.y-S.y*0.25f,p.z);
    	boxvert[i*4+3].set(p.x,p.y,p.z-S.z*0.25f);
    }
    // create render stream
	LStream 	= Device.Streams.Create(FVF::F_L,65535);
    TLStream   	= Device.Streams.Create(FVF::F_TL,65535);
    LITStream	= Device.Streams.Create(FVF::F_LIT,65535);
	// grid
    UpdateGrid	(1000,5,5);
}

void UninitUtilLibrary(){
}
//----------------

void DrawDirectionalLight(const Fvector& p, const Fvector& d, float radius, float range, DWORD c)
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
	DWORD			vBase;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(6,vBase);
    pv->set			(0,0,r,		c); pv++;
    pv->set			(0,0,sz,	c); pv++;
    pv->set			(-r,0,r,	c); pv++;
    pv->set			(-r,0,sz,	c); pv++;
    pv->set			(r,0,r,		c); pv++;
    pv->set			(r,0,sz,	c); pv++;
	LStream->Unlock	(6);
	// and Render it as triangle list
	Device.SetTransform(D3DTS_WORLD,rot);
    Device.DP		(D3DPT_LINELIST,LStream,vBase,3);

    Fbox b;
    b.min.set(-r,-r,-r);
    b.max.set(r,r,r);

    DrawSelectionBox(b,&c);
	Device.SetTransform(D3DTS_WORLD,precalc_identity);
}

void DrawPointLight(const Fvector& p, float radius, DWORD c)
{
	DrawCross(p, radius,radius,radius, radius,radius,radius, c, true);
}

void DrawFlag(const Fvector& p, float heading, float height, float sz, float sz_fl, DWORD clr){
	// fill VB
	DWORD			vBase;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(2,vBase);
    pv->set			(p,clr); pv++;
    pv->set			(p.x,p.y+height,p.z,clr); pv++;
	LStream->Unlock	(2);
	// and Render it as triangle list
    Device.DP		(D3DPT_LINELIST,LStream,vBase,1);

	// fill VB
	pv	 			= (FVF::L*)LStream->Lock(6,vBase);
    pv->set			(p.x,p.y+height*(1.f-sz_fl),p.z,clr); pv++;
    pv->set			(p.x,p.y+height,p.z,clr); pv++;
    pv->set			(p.x+sin(heading)*sz,((pv-2)->p.y+(pv-1)->p.y)/2,p.z+cos(heading)*sz,clr); pv++;
    pv->set			(*(pv-3)); pv++;
    pv->set			(*(pv-2)); pv++;
    pv->set			(*(pv-4)); pv++;
	LStream->Unlock	(6);
	// and Render it as triangle list
    Device.DP		(D3DPT_TRIANGLELIST,LStream,vBase,2);
}

//------------------------------------------------------------------------------

void DrawRomboid(const Fvector& p, float r, DWORD c){
static const WORD IL[24]={0,2, 2,5, 0,5, 3,5, 3,0, 4,3, 4,0, 4,2, 1,2, 1,5, 1,3, 1,4};
static const WORD IT[24]={2,4,0, 4,3,0, 3,5,0, 5,2,0, 4,2,1, 2,5,1, 5,3,1, 3,4,1};
	DWORD			vBase,iBase;

    Fcolor C;
    C.set			(c);
    C.mul_rgb		(0.75);
    DWORD c1 =		C.get();

	// fill VB
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(6,vBase);
    pv->set			(p.x,	p.y+r,	p.z,	c1); pv++;
    pv->set			(p.x,	p.y-r,	p.z,	c1); pv++;
    pv->set			(p.x,	p.y,	p.z-r,	c1); pv++;
    pv->set			(p.x,	p.y,	p.z+r,	c1); pv++;
    pv->set			(p.x-r,	p.y,	p.z,	c1); pv++;
    pv->set			(p.x+r,	p.y,	p.z,	c1); pv++;
	LStream->Unlock	(6);

    CIndexStream* is= Device.Streams.Get_IB();
    WORD* i 		= is->Lock(24,iBase);
    for (int k=0; k<24; k++,i++) *i=IT[k];
    is->Unlock		(24);

	// and Render it as triangle list
	Device.DIP		(D3DPT_TRIANGLELIST,LStream,vBase,6, is, iBase,12);

    // draw lines
	pv	 			= (FVF::L*)LStream->Lock(6,vBase);
    pv->set			(p.x,	p.y+r,	p.z,	c); pv++;
    pv->set			(p.x,	p.y-r,	p.z,	c); pv++;
    pv->set			(p.x,	p.y,	p.z-r,	c); pv++;
    pv->set			(p.x,	p.y,	p.z+r,	c); pv++;
    pv->set			(p.x-r,	p.y,	p.z,	c); pv++;
    pv->set			(p.x+r,	p.y,	p.z,	c); pv++;
	LStream->Unlock	(6);

    is				= Device.Streams.Get_IB();
    i 				= is->Lock(24,iBase);
    for (k=0; k<24; k++,i++) *i=IL[k];
    is->Unlock		(24);

	Device.DIP		(D3DPT_LINELIST,LStream,vBase,6, is, iBase,12);
}
//------------------------------------------------------------------------------

void DrawSound(const Fvector& p, float r, DWORD c){
	DrawCross(p, r,r,r, r,r,r, c, true);
}
//------------------------------------------------------------------------------
void DrawLineSphere(const Fvector& p, float radius, DWORD c, bool bCross){
	// fill VB
	DWORD			vBase, iBase;
    int i;
	FVF::L*	pv;
    // seg 0
    pv			 	= (FVF::L*)LStream->Lock(LINE_DIVISION+1,vBase);
	for( i=0; i<LINE_DIVISION; i++,pv++){ pv->p.mad(p,circledef1[i],radius); pv->color=c;}
    pv->set(*(pv-LINE_DIVISION));
	LStream->Unlock	(LINE_DIVISION+1);
	Device.DP		(D3DPT_LINESTRIP,LStream,vBase,LINE_DIVISION);
    // seg 1
    pv			 	= (FVF::L*)LStream->Lock(LINE_DIVISION+1,vBase);
	for( i=0; i<LINE_DIVISION; i++){ pv->p.mad(p,circledef2[i],radius); pv->color=c; pv++; }
    pv->set(*(pv-LINE_DIVISION)); pv++;
	LStream->Unlock	(LINE_DIVISION+1);
	Device.DP		(D3DPT_LINESTRIP,LStream,vBase,LINE_DIVISION);
    // seg 2
    pv			 	= (FVF::L*)LStream->Lock(LINE_DIVISION+1,vBase);
	for( i=0; i<LINE_DIVISION; i++){ pv->p.mad(p,circledef3[i],radius); pv->color=c; pv++; }
    pv->set(*(pv-LINE_DIVISION)); pv++;
	LStream->Unlock	(LINE_DIVISION+1);
	Device.DP		(D3DPT_LINESTRIP,LStream,vBase,LINE_DIVISION);

    if (bCross) DrawCross(p, radius,radius,radius, radius,radius,radius, c);
}

void DrawLine(const Fvector& p0, const Fvector& p1, DWORD c){
	// fill VB
	DWORD			vBase;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(2,vBase);
    pv->set			(p0,c); pv++;
    pv->set			(p1,c); pv++;
	LStream->Unlock	(2);
	// and Render it as triangle list
    Device.DP		(D3DPT_LINELIST,LStream,vBase,1);
}

//----------------------------------------------------
void DrawSelectionBox(const Fvector& C, const Fvector& S, DWORD* c){
    DWORD cc=(c)?*c:boxcolor;

	// fill VB
	DWORD			vBase,iBase;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(boxvertcount,vBase);
    for (int i=0; i<boxvertcount; i++,pv++){
	    pv->p.mul(boxvert[i],S);
        pv->p.add(C);
        pv->color	= cc;
    }
	LStream->Unlock	(boxvertcount);

	// fill IB
    CIndexStream* is= Device.Streams.Get_IB();
	WORD* I 		= is->Lock(boxindexcount,iBase);
    CopyMemory		(I,boxindices,sizeof(WORD)*boxindexcount);
    is->Unlock		(boxindexcount);

	// and Render it as triangle list
	Device.SetRS	(D3DRS_FILLMODE,D3DFILL_SOLID);
    Device.DIP		(D3DPT_LINELIST,LStream,vBase,boxvertcount,is,iBase,boxindexcount/2);
    Device.SetRS	(D3DRS_FILLMODE,Device.dwFillMode);
}

void DrawIdentBox(bool bSolid, bool bWire, DWORD* c){
    DWORD cc=(c)?*c:identboxcolor;

	// fill VB
	DWORD			vBase,iBase;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(identboxvertcount,vBase);
    for (int i=0; i<identboxvertcount; i++,pv++)
    	pv->set		(identboxvert[i],cc);
	LStream->Unlock	(identboxvertcount);

    if (bWire){
        // fill IB
        CIndexStream* is= Device.Streams.Get_IB();
        WORD* I 		= is->Lock(boxindexcount,iBase);
        CopyMemory		(I,identboxindiceswire,sizeof(WORD)*identboxindexwirecount);
        is->Unlock		(identboxindexwirecount);

	    Device.DIP		(D3DPT_LINELIST,LStream,vBase,identboxvertcount,is,iBase,identboxindexwirecount/2);
    }
    if (bSolid){
        // fill IB
        CIndexStream* is= Device.Streams.Get_IB();
        WORD* I 		= is->Lock(identboxindexcount,iBase);
        CopyMemory		(I,identboxindices,sizeof(WORD)*identboxindexcount);
        is->Unlock		(identboxindexcount);

	    Device.DIP		(D3DPT_TRIANGLELIST,LStream,vBase,identboxvertcount,is,iBase,identboxindexcount/3);
    }
}

void DrawBox(const Fvector& offs, const Fvector& Size, bool bWire, DWORD c){
	// fill VB
	DWORD			vBase,iBase;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(identboxvertcount,vBase);
    for (int i=0; i<identboxvertcount; i++, pv++){
    	pv->p.mul	(identboxvert[i],Size);
    	pv->p.mul	(2);
    	pv->p.add	(offs);
        pv->color	= c;
    }
	LStream->Unlock	(identboxvertcount);

    if (bWire){
        // fill IB
        CIndexStream* is= Device.Streams.Get_IB();
        WORD* I 		= is->Lock(boxindexcount,iBase);
        CopyMemory		(I,identboxindiceswire,sizeof(WORD)*identboxindexwirecount);
        is->Unlock		(identboxindexwirecount);

	    Device.DIP		(D3DPT_LINELIST,LStream,vBase,identboxvertcount,is,iBase,identboxindexwirecount/2);
    }else{
        // fill IB
        CIndexStream* is= Device.Streams.Get_IB();
        WORD* I 		= is->Lock(identboxindexcount,iBase);
        CopyMemory		(I,identboxindices,sizeof(WORD)*identboxindexcount);
        is->Unlock		(identboxindexcount);

	    Device.DIP		(D3DPT_TRIANGLELIST,LStream,vBase,identboxvertcount,is,iBase,identboxindexcount/3);
    }
}
//----------------------------------------------------

void DrawPlane  (const Fvector& center, const Fvector2& scale, const Fvector& rotate, DWORD c, bool bCull, bool bBorder, DWORD cb){
    Fmatrix M;
    M.setHPB(rotate.y,rotate.x,rotate.z);
	// fill VB
	DWORD			vBase;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(4,vBase);
    pv->set			(-scale.x, 0, -scale.y, c); M.transform_tiny(pv->p); pv->p.add(center); pv++;
    pv->set			(+scale.x, 0, -scale.y, c); M.transform_tiny(pv->p); pv->p.add(center); pv++;
    pv->set			(+scale.x, 0, +scale.y, c); M.transform_tiny(pv->p); pv->p.add(center); pv++;
    pv->set			(-scale.x, 0, +scale.y, c); M.transform_tiny(pv->p); pv->p.add(center); pv++;
	LStream->Unlock	(4);

    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP		(D3DPT_TRIANGLEFAN,LStream,vBase,2);
    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);

    if (bBorder){
        FVF::L*	pv	= (FVF::L*)LStream->Lock(5,vBase);
        pv->set		(-scale.x, 0, -scale.y, cb); M.transform_tiny(pv->p); pv->p.add(center); pv++;
        pv->set		(+scale.x, 0, -scale.y, cb); M.transform_tiny(pv->p); pv->p.add(center); pv++;
        pv->set		(+scale.x, 0, +scale.y, cb); M.transform_tiny(pv->p); pv->p.add(center); pv++;
        pv->set		(-scale.x, 0, +scale.y, cb); M.transform_tiny(pv->p); pv->p.add(center); pv++;
        pv->set		(*(pv-4));
        LStream->Unlock	(5);
	    Device.DP	(D3DPT_LINESTRIP,LStream,vBase,4);
    }
}
//----------------------------------------------------

void DrawCross(const Fvector& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, DWORD clr, bool bRot45){
	// actual rendering
	DWORD			vOffset;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(bRot45?12:6,vOffset);
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
	LStream->Unlock(bRot45?12:6);
    Device.DP(D3DPT_LINELIST,LStream,vOffset,bRot45?6:3);
}

void DrawPivot(const Fvector& pos, float sz){
	Device.SetShader(Device.m_WireShader);
    DrawCross(pos, sz, sz, sz, sz, sz, sz, 0xFF7FFF7F);
}

void DrawAxis(){
	VERIFY( Device.bReady );
    Fvector p[6];
    DWORD 	c[6];

    // colors
    c[0]=c[2]=c[4]=0x00222222; c[1]=0x00FF0000; c[3]=0x0000FF00; c[5]=0x000000FF;

    // position
    p[0].direct(Device.m_Camera.GetPosition(),Device.m_Camera.GetDirection(),0.25f);
    p[1].set(p[0]); p[1].x+=.015f;
    p[2].set(p[0]);
    p[3].set(p[0]); p[3].y+=.015f;
    p[4].set(p[0]);
    p[5].set(p[0]); p[5].z+=.015f;

    DWORD vBase;
	FVF::TL* pv = (FVF::TL*)TLStream->Lock(6,vBase);
    // transform to screen
    float dx=-Device.dwWidth/2.2f;
    float dy=Device.dwHeight/2.25f;

    for (int i=0; i<6; i++,pv++){
	    pv->color = c[i]; pv->transform(p[i],Device.mFullTransform);
	    pv->p.x	= Device._x2real(pv->p.x)+dx;   pv->p.y	= Device._y2real(pv->p.y)+dy;
    }

	// unlock VB and Render it as triangle list
	TLStream->Unlock(6);
	Device.SetRS(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
	Device.SetShader(Device.m_WireShader);
    Device.DP(D3DPT_LINELIST,TLStream,vBase,3);
	Device.SetRS(D3DRS_SHADEMODE,Device.dwShadeMode);
}

void DrawGrid(){
	VERIFY( Device.bReady );
    DWORD vBase,iBase;
	// fill VB
	FVF::L* pv = (FVF::L*)LStream->Lock(m_GridPoints.size(),vBase);
    for (FLvertexIt v_it=m_GridPoints.begin(); v_it!=m_GridPoints.end(); v_it++,pv++) pv->set(*v_it);
	LStream->Unlock(m_GridPoints.size());
	// Render it as triangle list
    Device.SetTransform(D3DTS_WORLD,precalc_identity);
	Device.SetShader(Device.m_WireShader);
    Device.DP(D3DPT_LINELIST,LStream,vBase,m_GridPoints.size()/2);
}

void DrawSelectionRect(const Ipoint& m_SelStart, const Ipoint& m_SelEnd){
	VERIFY( Device.bReady );
	// fill VB
    DWORD vBase;
	FVF::TL* pv = (FVF::TL*)TLStream->Lock(4,vBase);
    pv->set(m_SelStart.x*Device.m_ScreenQuality, m_SelStart.y*Device.m_ScreenQuality, 0,0,m_SelectionRect,0,0); pv++;
    pv->set(m_SelStart.x*Device.m_ScreenQuality, m_SelEnd.y*Device.m_ScreenQuality,   0,0,m_SelectionRect,0,0); pv++;
    pv->set(m_SelEnd.x*Device.m_ScreenQuality,   m_SelEnd.y*Device.m_ScreenQuality,   0,0,m_SelectionRect,0,0); pv++;
    pv->set(m_SelEnd.x*Device.m_ScreenQuality,   m_SelStart.y*Device.m_ScreenQuality, 0,0,m_SelectionRect,0,0); pv++;
	TLStream->Unlock(4);
	// Render it as triangle list
    Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
	Device.SetShader(Device.m_SelectionShader);
    Device.DP(D3DPT_TRIANGLEFAN,TLStream,vBase,2);
    Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

void DrawPrimitiveL	(D3DPRIMITIVETYPE pt, DWORD pc, Fvector* vertices, int vc, DWORD color, bool bCull, bool bCycle){
	// fill VB
	DWORD			vBase, dwNeed=(bCycle)?vc+1:vc;
	FVF::L*	pv	 	= (FVF::L*)LStream->Lock(dwNeed,vBase);
    for(int k=0; k<vc; k++,pv++)
    	pv->set		(vertices[k],color);
    if (bCycle)		pv->set(*(pv-vc));
	LStream->Unlock	(dwNeed);

    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP		(pt,LStream,vBase,pc);
    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

void DrawPrimitiveTL(D3DPRIMITIVETYPE pt, DWORD pc, FVF::TL* vertices, int vc, bool bCull, bool bCycle){
	// fill VB
	DWORD			vBase, dwNeed=(bCycle)?vc+1:vc;
	FVF::TL*	pv 	= (FVF::TL*)TLStream->Lock(dwNeed,vBase);
    for(int k=0; k<vc; k++,pv++)
    	pv->set		(vertices[k]);
    if (bCycle)		pv->set(*(pv-vc));
	TLStream->Unlock	(dwNeed);

    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP		(pt,TLStream,vBase,pc);
    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

void DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, DWORD pc, FVF::LIT* vertices, int vc, bool bCull, bool bCycle){
	// fill VB
	DWORD			vBase, dwNeed=(bCycle)?vc+1:vc;
	FVF::LIT*	pv 	= (FVF::LIT*)LITStream->Lock(dwNeed,vBase);
    for(int k=0; k<vc; k++,pv++)
    	pv->set		(vertices[k]);
    if (bCycle)		pv->set(*(pv-vc));
	LITStream->Unlock	(dwNeed);

    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP		(pt,LITStream,vBase,pc);
    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}
}// namespace

// file: D3DUtils.cpp

#include "stdafx.h"
#pragma hdrstop

#include "D3DUtils.h"
#include "UI_Main.h"

#define LINE_DIVISION  32  // не меньше 6!!!!!
namespace DU{
// for drawing sphere
static FVF::L linebuffer1[LINE_DIVISION];
static FVF::L linebuffer2[LINE_DIVISION];
static FVF::L linebuffer3[LINE_DIVISION];
static Fvector circledef1[LINE_DIVISION];
static Fvector circledef2[LINE_DIVISION];
static Fvector circledef3[LINE_DIVISION];
static WORD lineindices[LINE_DIVISION*2];

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
static FVF::L identboxvert[identboxvertcount] = {
	-0.5f, -0.5f, -0.5f, 0x0,
	-0.5f, +0.5f, -0.5f, 0x0,
	+0.5f, +0.5f, -0.5f, 0x0,
	+0.5f, -0.5f, -0.5f, 0x0,
	-0.5f, +0.5f, +0.5f, 0x0,
	-0.5f, -0.5f, +0.5f, 0x0,
	+0.5f, +0.5f, +0.5f, 0x0,
	+0.5f, -0.5f, +0.5f, 0x0
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

static FLvertexVec 	m_GridPoints;
static WORDVec 		m_GridIndices;

DWORD m_ColorGrid	= 0xff969696;
DWORD m_ColorGridTh = 0xffb4b4b4;
DWORD m_SelectionRect=D3DCOLOR_RGBA(127,255,127,64);


void UpdateGrid(){
// grid
	int m_GridSubDiv[2];
	int m_GridCounts[2];
    Fvector2 m_GridStep;

    m_GridStep.set(4.f,4.f);
	m_GridSubDiv[0] = 5;
	m_GridSubDiv[1] = 5;
	m_GridCounts[0] = 250;
	m_GridCounts[1] = 250;

	FVF::L left,right;
	left.p.y = right.p.y = 0;

	for(int thin=0; thin<2; thin++){
		for(int i=-m_GridCounts[0]; i<m_GridCounts[0]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[0]) ){
				left.p.z = -m_GridCounts[1]*m_GridStep.y;
				right.p.z = m_GridCounts[1]*m_GridStep.y;
				left.p.x = i*m_GridStep.x;
				right.p.x = left.p.x;
				left.color = (i%m_GridSubDiv[0]) ? m_ColorGrid : m_ColorGridTh;
				right.color = left.color;
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( left );
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( right );
			}
		}
		for(i=-m_GridCounts[1]; i<m_GridCounts[1]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[1]) ){
				left.p.x = -m_GridCounts[0]*m_GridStep.x;
				right.p.x = m_GridCounts[0]*m_GridStep.x;
				left.p.z = i*m_GridStep.y;
				right.p.z = left.p.z;
				left.color = (i%m_GridSubDiv[1]) ? m_ColorGrid : m_ColorGridTh;
				right.color = left.color;
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( left );
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( right );
			}
		}
	}
}

void InitUtilLibrary(){
	for(int i=0; i<(LINE_DIVISION-1); i++){
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
	LStream 	= Device.Streams.Create(FVF::F_L,2048);
    TLStream   	= Device.Streams.Create(FVF::F_TL,2048);
	// grid
    UpdateGrid	();
}

void UninitUtilLibrary(){
}
//----------------

void DrawDirectionalLight(const Fvector& p, const Fvector& d, float radius, float range, DWORD c)
{
/*    float r=radius*0.71f;

	Fvector R,N,D; D.normalize(d);
	Fmatrix rot;

    N.set		(0,1,0);
	if (fabsf(D.y)>0.99f) N.set(1,0,0);
	R.crossproduct(N,D); R.normalize();
	N.crossproduct(D,R); N.normalize();
    rot.set(R,N,D,p);

//    Fvector dir; dir.direct(p,d,radius+10);
//    linebuffer1[0].set(p.x,p.y,p.z,c,c,0,0);
	float sz=radius+range;
    linebuffer1[0].set(0,0,r,	c);
    linebuffer1[1].set(0,0,sz,	c);
    linebuffer1[2].set(-r,0,r,	c);
    linebuffer1[3].set(-r,0,sz,	c);//
    linebuffer1[4].set(r,0,r,	c);
    linebuffer1[5].set(r,0,sz,	c);//
	Device.SetTransform(D3DTS_WORLD,rot);
	Device.DP(D3DPT_LINELIST,FVF::F_L, linebuffer1, 6);

    Fbox b;
    b.min.set(-r,-r,-r);
    b.max.set(r,r,r);

    DrawSelectionBox(b,&c);
	Device.SetTransform(D3DTS_WORLD,precalc_identity);
*/
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
/*    Fvector t;
	for( int i=0; i<LINE_DIVISION; i++){
        t.mul(circledef1[i],radius); t.add(p);
        linebuffer1[i].set(t,c);
        t.mul(circledef2[i],radius); t.add(p);
        linebuffer2[i].set(t,c);
        t.mul(circledef3[i],radius); t.add(p);
        linebuffer3[i].set(t,c);
	}

	Device.DIP( D3DPT_LINELIST,FVF::F_L,
		linebuffer1, LINE_DIVISION,		lineindices, LINE_DIVISION*2);

	Device.DIP( D3DPT_LINELIST,FVF::F_L,
		linebuffer2, LINE_DIVISION,		lineindices, LINE_DIVISION*2);

	Device.DIP( D3DPT_LINELIST,FVF::F_L,
		linebuffer3, LINE_DIVISION,		lineindices, LINE_DIVISION*2);

    if (bCross) DrawCross(p, radius,radius,radius, radius,radius,radius, c);
*/
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
    for (int k=0; k<boxindexcount; k++,I++) *I=boxindices[k];
    is->Unlock		(boxindexcount);

	// and Render it as triangle list
	Device.SetRS	(D3DRS_FILLMODE,D3DFILL_SOLID);
    Device.DIP		(D3DPT_LINELIST,LStream,vBase,boxvertcount,is,iBase,boxindexcount/2);
    Device.SetRS	(D3DRS_FILLMODE,UI->dwRenderFillMode);
}

void DrawIdentBox(bool bWire, DWORD* c){
/*    DWORD cc=(c)?*c:identboxcolor;

    for (int i=0; i<identboxvertcount; i++)
    	identboxvert[i].color = cc;

    if (bWire){
        Device.DIP(D3DPT_LINELIST, FVF::F_L, identboxvert, identboxvertcount,
                        (WORD*)identboxindiceswire,identboxindexwirecount);
    }else{
        Device.DIP(D3DPT_TRIANGLELIST, FVF::F_L, identboxvert, identboxvertcount,
                        (WORD*)identboxindices,identboxindexcount);
    }
*/
}

void DrawBox(const Fvector& offs, const Fvector& Size, bool bWire, DWORD c){
/*	FVF::L box[8]; CopyMemory(box,identboxvert,sizeof(FVF::L)*8);

    for (int i=0; i<8; i++){FVF::L& v=box[i]; v.p.mul(Size); v.p.mul(2); v.p.add(offs); v.color=c;}

    if (bWire){
        Device.DIP(D3DPT_LINELIST, FVF::F_L, box, identboxvertcount,
                        (WORD*)identboxindiceswire,identboxindexwirecount);
    }else{
        Device.DIP(D3DPT_TRIANGLELIST, FVF::F_L, box, identboxvertcount,
                        (WORD*)identboxindices,identboxindexcount);
    }
*/}
//----------------------------------------------------
DWORD subst_a(DWORD val, BYTE a){
    BYTE r, g, b;
    b = (BYTE) (val >>  0);
    g = (BYTE) (val >>  8);
    r = (BYTE) (val >> 16);
    return ((DWORD)(a<<24) | (r<<16) | (g<<8) | (b));
}
DWORD bgr2rgb(DWORD val){
    BYTE r, g, b;
    r = (BYTE) (val >>  0);
    g = (BYTE) (val >>  8);
    b = (BYTE) (val >> 16);
    return ((DWORD)(r<<16) | (g<<8) | (b));
}
DWORD rgb2bgr(DWORD val){
    BYTE r, g, b;
    r = (BYTE) (val >> 16);
    g = (BYTE) (val >>  8);
    b = (BYTE) (val >> 0);
    return ((DWORD)(b<<16) | (g<<8) | (r));
}

void DrawPlane  (const Fvector& center, const Fvector2& scale, const Fvector& rotate, DWORD c, bool bCull, bool bBorder, DWORD cb){
/*    Fmatrix M;
    M.setHPB(rotate.y,rotate.x,rotate.z);
    crossbuffer[0].set(-scale.x, 0, -scale.y, c);
    crossbuffer[1].set(+scale.x, 0, -scale.y, c);
    crossbuffer[2].set(+scale.x, 0, +scale.y, c);
    crossbuffer[3].set(-scale.x, 0, +scale.y, c);

    for (int i=0; i<4; i++){
        M.transform_tiny(crossbuffer[i].p);
        crossbuffer[i].p.add(center);
    }

    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
    Device.DP(D3DPT_TRIANGLEFAN,FVF::F_L, crossbuffer, 4);
    if (!bCull) Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);

    if (bBorder){
        for (int i=0; i<4; i++) crossbuffer[i].color = cb;
        crossbuffer[4].set(crossbuffer[0].p,crossbuffer[0].color);
        Device.DP(D3DPT_LINESTRIP,FVF::F_L, crossbuffer, 5);
    }
*/
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

void DrawPivot(const Fvector& pos){
	Device.Shader.Set(Device.m_WireShader);
    DrawCross(pos, 5, 5, 5, 5, 5, 5, 0xFF7FFF7F);
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
    float dx=-Device.m_RenderWidth/2.2f;
    float dy=Device.m_RenderHeight/2.25f;

    for (int i=0; i<6; i++,pv++){
	    pv->color = c[i]; pv->transform(p[i],Device.mFullTransform);
	    pv->p.x	= Device._x2real(pv->p.x)+dx;   pv->p.y	= Device._y2real(pv->p.y)+dy;
    }

	// unlock VB and Render it as triangle list
	TLStream->Unlock(6);
	Device.SetRS(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
	Device.Shader.Set(Device.m_WireShader);
    Device.DP(D3DPT_LINELIST,TLStream,vBase,3);
	Device.SetRS(D3DRS_SHADEMODE,UI->dwRenderShadeMode);
}

void DrawGrid(){
	VERIFY( Device.bReady );
    DWORD vBase,iBase;
	// fill VB
	FVF::L* pv = (FVF::L*)LStream->Lock(m_GridPoints.size(),vBase);
    for (FLvertexIt v_it=m_GridPoints.begin(); v_it!=m_GridPoints.end(); v_it++,pv++) pv->set(*v_it);
	LStream->Unlock(m_GridPoints.size());
	// fill IB
    CIndexStream* IS = Device.Streams.Get_IB();
	WORD* i = IS->Lock(m_GridIndices.size(),iBase);
    for (WORDIt i_it=m_GridIndices.begin(); i_it!=m_GridIndices.end(); i_it++,i++) *i = *i_it;
	IS->Unlock(m_GridIndices.size());
	// Render it as triangle list
    Device.SetTransform(D3DTS_WORLD,precalc_identity);
	Device.Shader.Set(Device.m_WireShader);
    Device.DIP(	D3DPT_LINELIST,LStream,vBase,m_GridPoints.size(),IS,iBase,m_GridIndices.size()/2);
}

void DrawSelectionRect(const Fvector2& m_SelStart, const Fvector2& m_SelEnd){
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
	Device.Shader.Set(Device.m_SelectionShader);
    Device.DP(D3DPT_TRIANGLEFAN,TLStream,vBase,2);
    Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

}// namespace

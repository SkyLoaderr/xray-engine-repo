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

static FVF::L crossbuffer[6];

const DWORD boxcolor = RGBA_MAKE(255,255,255,0);
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
const DWORD identboxcolor = RGBA_MAKE(255,255,255,0);
static const int identboxvertcount = 8;
static const Fvector identboxvert[identboxvertcount] = {
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

//    Fvector dir; dir.direct(p,d,radius+10);
//    linebuffer1[0].set(p.x,p.y,p.z,c,c,0,0);
	float sz=radius+range;
    linebuffer1[0].set(0,0,r,	c);
    linebuffer1[1].set(0,0,sz,	c);
    linebuffer1[2].set(-r,0,r,	c);
    linebuffer1[3].set(-r,0,sz,	c);//
    linebuffer1[4].set(r,0,r,	c);
    linebuffer1[5].set(r,0,sz,	c);//
	UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,rot);
	UI->Device.DP(D3DPT_LINELIST,FVF::F_L, linebuffer1, 6);

    Fbox b;
    b.min.set(-r,-r,-r);
    b.max.set(r,r,r);

    DrawSelectionBox(b,&c);
	UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
}

void DrawPointLight(const Fvector& p, float radius, DWORD c)
{
	DrawCross(p, radius,radius,radius, radius,radius,radius, c, true);
}

void DrawFlag(const Fvector& p, float heading, float height, float sz, float sz_fl, DWORD clr){
    for (int i=0; i<6; i++)
    	linebuffer1[i].color = clr;

	linebuffer1[0].p.set(p);

	linebuffer1[1].p.set(p);
	linebuffer1[1].p.y+=height;

	UI->Device.DP(D3DPT_LINELIST,FVF::F_L, linebuffer1, 2);

	linebuffer1[0].p.y = p.y+height*(1.f-sz_fl);

	linebuffer1[2].p.x = p.x+sin(heading)*sz;
	linebuffer1[2].p.y = (linebuffer1[0].p.y+linebuffer1[1].p.y)/2;
	linebuffer1[2].p.z = p.z+cos(heading)*sz;

    linebuffer1[3].p.set(linebuffer1[0].p);
    linebuffer1[4].p.set(linebuffer1[2].p);
    linebuffer1[5].p.set(linebuffer1[1].p);

	UI->Device.DP(D3DPT_TRIANGLELIST,FVF::F_L, linebuffer1, 6);
}

//------------------------------------------------------------------------------

void DrawRomboid(const Fvector& p, float r, DWORD c){
	Fcolor C;
    C.set(c);
    C.mul_rgb(0.75);
    FVF::L v[6];
    WORD IL[24]={0,2, 2,5, 0,5, 3,5, 3,0, 4,3, 4,0, 4,2, 1,2, 1,5, 1,3, 1,4};
    WORD IT[24]={2,4,0, 4,3,0, 3,5,0, 5,2,0, 4,2,1, 2,5,1, 5,3,1, 3,4,1};

    v[0].set(p.x,	p.y+r,	p.z,	c);
    v[1].set(p.x,	p.y-r,	p.z,	c);
    v[2].set(p.x,	p.y,	p.z-r,	c);
    v[3].set(p.x,	p.y,	p.z+r,	c);
    v[4].set(p.x-r,	p.y,	p.z,	c);
    v[5].set(p.x+r,	p.y,	p.z,	c);

	UI->Device.DIP(D3DPT_TRIANGLELIST,FVF::F_L, v, 6, IT, 24);
    for (int i=0; i<6; i++) v[i].color=c;
	UI->Device.DIP(D3DPT_LINELIST,FVF::F_L, v, 6, IL, 24);
}
//------------------------------------------------------------------------------

void DrawSound(const Fvector& p, float r, DWORD c){
    FVF::L v[6];
    WORD I[24]={0,2, 2,5, 0,5, 3,5, 3,0, 4,3, 4,0, 4,2, 1,2, 1,5, 1,3, 1,4};

    v[0].set(p.x,	p.y+r,	p.z,	c);
    v[1].set(p.x,	p.y-r,	p.z,	c);
    v[2].set(p.x,	p.y,	p.z-r,	c);
    v[3].set(p.x,	p.y,	p.z+r,	c);
    v[4].set(p.x-r,	p.y,	p.z,	c);
    v[5].set(p.x+r,	p.y,	p.z,	c);

	UI->Device.DIP(D3DPT_LINELIST,FVF::F_L, v, 6, I, 24);
}
//------------------------------------------------------------------------------
void DrawLineSphere(const Fvector& p, float radius, DWORD c, bool bCross){
    Fvector t;
	for( int i=0; i<LINE_DIVISION; i++){
        t.mul(circledef1[i],radius); t.add(p);
        linebuffer1[i].set(t,c);
        t.mul(circledef2[i],radius); t.add(p);
        linebuffer2[i].set(t,c);
        t.mul(circledef3[i],radius); t.add(p);
        linebuffer3[i].set(t,c);
	}

	UI->Device.DIP( D3DPT_LINELIST,FVF::F_L,
		linebuffer1, LINE_DIVISION,		lineindices, LINE_DIVISION*2);

	UI->Device.DIP( D3DPT_LINELIST,FVF::F_L,
		linebuffer2, LINE_DIVISION,		lineindices, LINE_DIVISION*2);

	UI->Device.DIP( D3DPT_LINELIST,FVF::F_L,
		linebuffer3, LINE_DIVISION,		lineindices, LINE_DIVISION*2);

    if (bCross) DrawCross(p, radius,radius,radius, radius,radius,radius, c);
}

void DrawLine(const Fvector& p0, const Fvector& p1, DWORD c){
    FVF::L v[2];
    v[0].set(p0,c);
    v[1].set(p1,c);
    UI->Device.DP(D3DPT_LINELIST, FVF::F_L, v, 2);
}

//----------------------------------------------------
void DrawSelectionBox(const Fvector& C, const Fvector& S, DWORD* c){
    DWORD cc=(c)?*c:boxcolor;
	FVF::L B[boxvertcount];
    Fvector p;
    for (int i=0; i<boxvertcount; i++){
    	p.mul(boxvert[i],S);
        p.add(C);
    	B[i].set(p,cc);
    }
    
	UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,D3DFILL_SOLID);
	UI->Device.DIP(D3DPT_LINELIST,FVF::F_L,
		B, boxvertcount, (LPWORD)boxindices, boxindexcount);
    UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,UI->dwRenderFillMode);
}

void DrawIdentBox(bool bWire, DWORD* c){
    DWORD cc=(c)?*c:identboxcolor;

    DWORDVec colors;
    colors.resize(identboxvertcount,cc);
    D3DDRAWPRIMITIVESTRIDEDDATA dt;
    dt.position.lpvData = (void*)identboxvert;
    dt.position.dwStride = sizeof(Fvector);
    dt.diffuse.lpvData = colors.begin();
    dt.diffuse.dwStride = sizeof(DWORD);

    if (bWire){
        UI->Device.DIPS(D3DPT_LINELIST, FVF::F_L,
                        &dt, identboxvertcount, 
                        (WORD*)identboxindiceswire,identboxindexwirecount);
    }else{
        UI->Device.DIPS(D3DPT_TRIANGLELIST, FVF::F_L,
                        &dt, identboxvertcount, 
                        (WORD*)identboxindices,identboxindexcount);
    }
}

void DrawBox(const Fvector& offs, const Fvector& Size, bool bWire, DWORD c){
    Fvector box[8]; CopyMemory(box,identboxvert,sizeof(Fvector)*8);
    DWORDVec colors;
    colors.resize(identboxvertcount,c);
    D3DDRAWPRIMITIVESTRIDEDDATA dt;
    dt.position.lpvData = (void*)box;
    dt.position.dwStride = sizeof(Fvector);
    dt.diffuse.lpvData = colors.begin();
    dt.diffuse.dwStride = sizeof(DWORD);

    for (int i=0; i<8; i++){ box[i].mul(Size); box[i].mul(2); box[i].add(offs);}
    
    if (bWire){
        UI->Device.DIPS(D3DPT_LINELIST, FVF::F_L,
                        &dt, identboxvertcount, 
                        (WORD*)identboxindiceswire,identboxindexwirecount);
    }else{
        UI->Device.DIPS(D3DPT_TRIANGLELIST, FVF::F_L,
                        &dt, identboxvertcount, 
                        (WORD*)identboxindices,identboxindexcount);
    }
}
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
    Fmatrix M;
    M.setHPB(rotate.y,rotate.x,rotate.z);
    crossbuffer[0].set(-scale.x, 0, -scale.y, c);
    crossbuffer[1].set(+scale.x, 0, -scale.y, c);
    crossbuffer[2].set(+scale.x, 0, +scale.y, c);
    crossbuffer[3].set(-scale.x, 0, +scale.y, c);

    for (int i=0; i<4; i++){
        M.transform_tiny(crossbuffer[i].p);
        crossbuffer[i].p.add(center);
    }

    if (!bCull) UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
    UI->Device.DP(D3DPT_TRIANGLEFAN,FVF::F_L, crossbuffer, 4);
    if (!bCull) UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);

    if (bBorder){
        for (int i=0; i<4; i++) crossbuffer[i].color = cb;
        crossbuffer[4].set(crossbuffer[0].p,crossbuffer[0].color);
        UI->Device.DP(D3DPT_LINESTRIP,FVF::F_L, crossbuffer, 5);
    }
}
//----------------------------------------------------

void DrawCross(const Fvector& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, DWORD clr, bool bRot45){
    for(int i=0;i<6;i++) crossbuffer[i].set(p,clr);
    crossbuffer[0].p.x += szx2;
    crossbuffer[1].p.x -= szx1;
    crossbuffer[2].p.y += szy2;
    crossbuffer[3].p.y -= szy1;
    crossbuffer[4].p.z += szz2;
    crossbuffer[5].p.z -= szz1;
    UI->Device.DP(D3DPT_LINELIST,FVF::F_L, crossbuffer, 6);
    if (bRot45){
    	Fmatrix M;
        M.setHPB(PI_DIV_4,PI_DIV_4,PI_DIV_4);
	    for(int i=0;i<6;i++){ 
        	crossbuffer[i].p.sub(p);
        	M.transform_dir(crossbuffer[i].p);
        	crossbuffer[i].p.add(p);
        }
	    UI->Device.DP(D3DPT_LINELIST,FVF::F_L, crossbuffer, 6);
    }
}


/*
const int planeindexcount1 = 8;
const WORD planeindices1[planeindexcount1] = {
	0, 1, 1, 2, 2, 3, 3, 0 };

const int planeindexcount2 = 6;
const WORD planeindices2[planeindexcount1] = {
	0, 1, 2, 2, 3, 0 };

void DrawQuadPlane( Fvector *points, bool selected, float alpha ){
	VERIFY( points );

	FLvertex verts1[4];
	FLvertex verts2[4];

	DWORD color1,color2;
	if( selected ){
		color1=RGBA_MAKE( 255, 127, 255, int(255*alpha) );
		color2=RGBA_MAKE( 255, 0, 0, 255 );
	} else {
		color1=RGBA_MAKE( 140, 140, 140, int(255*alpha) );
		color2=RGBA_MAKE( 127, 255, 127, 255 );
	}

	for(int i=0; i<4; i++){
		verts1[i].color=color1;
		verts1[i].specular=color1;

		verts1[i].tu = 0.f;
		verts1[i].tv = 0.f;

		verts1[i].x = points[i].x;
		verts1[i].y = points[i].y;
		verts1[i].z = points[i].z;

		verts2[i].color=color2;
		verts2[i].specular=color2;

		verts2[i].tu = 0.f;
		verts2[i].tv = 0.f;

		verts2[i].x = points[i].x;
		verts2[i].y = points[i].y;
		verts2[i].z = points[i].z;
	}

	UI->Device.DIP(
		D3DPT_TRIANGLELIST,FVF::F_L,
		verts1, 4, (LPWORD)planeindices2, planeindexcount2);

	UI->Device.DIP(
		D3DPT_LINELIST,FVF::F_L,
		verts2, 4, (LPWORD)planeindices1, planeindexcount1);
}
*/
}

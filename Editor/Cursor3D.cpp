//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Cursor3D.h"
#include "ui_main.h"
#include "scene.h"
#include "texture.h"

//---------------------------------------------------------------------------
static WORD CrossIndices[4]={0,2,1,3};

#pragma package(smart_init)

C3DCursor::C3DCursor(){
    m_Visible = false;
    brush_radius = 1.f;
    brush_up_depth = 1.f;
    brush_dn_depth = 20.f;
    SetBrushSegment();
    eStyle = csLasso;
}
//---------------------------------------------------------------------------

C3DCursor::~C3DCursor(){
    m_RenderBuffer.clear();
}
//---------------------------------------------------------------------------
                                               
void C3DCursor::SetBrushSegment(float segment){
    m_RenderBuffer.resize(int(segment+1));
    d_angle = float(PI_MUL_2)/float(segment);
}
//---------------------------------------------------------------------------

void C3DCursor::SetColor(Fcolor& c){
	dwColor = c.get();
    for (FLvertexIt v=m_RenderBuffer.begin(); v!=m_RenderBuffer.end(); v++)
        (*v).color=dwColor;
}
//---------------------------------------------------------------------------
//#define UP_OFFS 1
void C3DCursor::GetPickPoint (Fvector& src, Fvector& dst, Fvector* N){
    Fvector start;
    SRayPickInfo pinf;
    start.set(src); start.y+=brush_up_depth;
    pinf.rp_inf.range = brush_up_depth+brush_dn_depth;
    pinf.pt.set(src);
    Fvector pick_dir;
    pick_dir.set(0,-1,0);
    if(Scene->RayPick(start, pick_dir, OBJCLASS_EDITOBJECT, &pinf, false, true)){
        dst.set(pinf.pt);
        if (N) N->mknormal(pinf.rp_inf.p[0], pinf.rp_inf.p[1], pinf.rp_inf.p[2]);
    }else{
        dst.set(src);
        if (N) N->set(0,1,0);
    }
}
//---------------------------------------------------------------------------

void C3DCursor::Render(){
    if (m_Visible&&!UI->Device.m_Camera.IsMoving()){
        SRayPickInfo pinf;
        Fvector start, dir, N, D;
        POINT start_pt;
        Fvector2 pt;
        GetCursorPos(&start_pt); start_pt=UI->GetD3DWindow()->ScreenToClient(start_pt);
        pt.set(float(start_pt.x),float(start_pt.y));
        UI->Device.m_Camera.MouseRayFromPoint(start,dir,pt);
        if (UI->PickGround(pinf.pt,start,dir, -1)){
            N.set(0,1,0);
            D.set(0,0,1);

            switch (eStyle){
            case csLasso:{
                Fmatrix m_ViewMat;
                Fvector at;
                at.sub(pinf.pt, N);
                m_ViewMat.build_camera(pinf.pt, at, D); m_ViewMat.invert();
                Fvector p;
                float s_a = 0;
                for (DWORD idx=0; idx<m_RenderBuffer.size()-1; s_a+=d_angle, idx++){
                    p.set(cosf(s_a)*brush_radius, sinf(s_a)*brush_radius, 0);
                    m_ViewMat.transform(p);
                    GetPickPoint(p, m_RenderBuffer[idx].p, NULL);
                }
                m_RenderBuffer.back().p.set(m_RenderBuffer[0].p);

    //            UI->D3D_RenderNearer(0.0001);
                UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
				UI->Device.Shader.Set(UI->Device.m_WireShader);
                UI->Device.DP(D3DPT_LINESTRIP,FVF::F_L,
                    &(m_RenderBuffer.front()), m_RenderBuffer.size());
    //            UI->D3D_ResetNearer();
            }break;
            case csCross:{
                Fmatrix m_ViewMat;
                Fvector at;
                at.sub(pinf.pt, N);
                m_ViewMat.build_camera(pinf.pt, at, D); m_ViewMat.invert();
                Fvector p;
                float s_a = 0;
            	FVF::L pt[4];
                for (DWORD idx=0; idx<4; s_a+=PI_DIV_2, idx++){
                    p.set(cosf(s_a)*0.15f, sinf(s_a)*0.15f, 0.f);
                    m_ViewMat.transform(p);
	                pt[idx].set(p,dwColor);
                }
                UI->Device.RenderNearer(0.001);
                UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
				UI->Device.Shader.Set(UI->Device.m_WireShader);
                UI->Device.DIP(D3DPT_LINELIST,FVF::F_L, pt, 4, CrossIndices, 4);
                UI->Device.ResetNearer();
            }break;
            case csPoint:{
            	FVF::TL pt[5];
                pt[0].transform(pinf.pt,UI->Device.m_FullTransform);
                pt[0].color = dwColor;
                pt[0].p.x = X_TO_REAL(pt[0].p.x);
                pt[0].p.y = Y_TO_REAL(pt[0].p.y);
				pt[1].set(pt[0].p.x-1,pt[0].p.y  ,pt[0].p.z,pt[0].p.w,dwColor,0,0);
				pt[2].set(pt[0].p.x+1,pt[0].p.y  ,pt[0].p.z,pt[0].p.w,dwColor,0,0);
				pt[3].set(pt[0].p.x  ,pt[0].p.y-1,pt[0].p.z,pt[0].p.w,dwColor,0,0);
				pt[4].set(pt[0].p.x  ,pt[0].p.y+1,pt[0].p.z,pt[0].p.w,dwColor,0,0);
                UI->Device.RenderNearer(0.001);
                UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
				UI->Device.Shader.Set(UI->Device.m_WireShader);
                UI->Device.DP(D3DPT_POINTLIST,FVF::F_TL, pt, 5);
                UI->Device.ResetNearer();
            }break;
            }
        }
    }
}
//---------------------------------------------------------------------------

bool C3DCursor::PrepareBrush(){
    SRayPickInfo pinf;
    bool bPickObject, bPickGround;
    Fvector N, D;
    POINT start_pt;
    Fvector2 pt;
    GetCursorPos(&start_pt); start_pt=UI->GetD3DWindow()->ScreenToClient(start_pt);
    pt.set(float(start_pt.x),float(start_pt.y));
    UI->Device.m_Camera.MouseRayFromPoint(brush_start,brush_dir,pt);
    bPickObject = !!Scene->RayPick( brush_start, brush_dir, OBJCLASS_EDITOBJECT, &pinf, false, true);
    if (!bPickObject) bPickGround = UI->PickGround(pinf.pt, brush_start, brush_dir);
    if (bPickObject||bPickGround){
        N.set(0,1,0); D.set(0,0,1);
        Fvector at;   at.sub(pinf.pt, N);
        brush_mat.build_camera(pinf.pt, at, D); brush_mat.invert();
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

void C3DCursor::GetRandomBrushPos(Fvector& pos, Fvector& norm){
    Fvector p, start, dir;
    SRayPickInfo pinf;
    float s_a = Random.randF(PI_MUL_2);
    float dist = sqrtf(Random.randF())*brush_radius;
    p.set(cosf(s_a)*dist, sinf(s_a)*dist, 0);
    brush_mat.transform(p);
    GetPickPoint(p, pos, &norm);
}
//---------------------------------------------------------------------------

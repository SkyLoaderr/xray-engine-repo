//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Cursor3D.h"
#include "ui_main.h"
#include "scene.h"
#include "texture.h"
#include "D3DUtils.h"

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
    m_RenderBuffer.resize(segment);
    d_angle = float(PI_MUL_2)/float(segment);
}
//---------------------------------------------------------------------------

void C3DCursor::SetColor(Fcolor& c){
	dwColor = c.get();
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
    if (m_Visible&&!Device.m_Camera.IsMoving()){
        SRayPickInfo pinf;
        Fvector start, dir, N, D;
        POINT start_pt;
        Ipoint pt;
        GetCursorPos(&start_pt); start_pt=UI->GetD3DWindow()->ScreenToClient(start_pt);
        pt.set(float(start_pt.x),float(start_pt.y));
        Device.m_Camera.MouseRayFromPoint(start,dir,pt);
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
                for (DWORD idx=0; idx<m_RenderBuffer.size(); s_a+=d_angle, idx++){
                    p.set(cosf(s_a)*brush_radius, sinf(s_a)*brush_radius, 0);
                    m_ViewMat.transform(p);
                    GetPickPoint(p, m_RenderBuffer[idx], NULL);
                }

//                UI->D3D_RenderNearer(0.0001);
                Device.SetTransform(D3DTS_WORLD,precalc_identity);
				Device.Shader.Set(Device.m_WireShader);
                DU::DrawPrimitiveL(D3DPT_LINESTRIP,m_RenderBuffer.size(),m_RenderBuffer.begin(),m_RenderBuffer.size(),dwColor,true,true);
//                UI->D3D_ResetNearer();
            }break;
            case csPoint:{
            	FVF::TL pt[5];
                pt[0].transform(pinf.pt,Device.mFullTransform);
                pt[0].color = dwColor;
                pt[0].p.x = Device._x2real(pt[0].p.x);
                pt[0].p.y = Device._y2real(pt[0].p.y);
				pt[1].set(pt[0].p.x-1,pt[0].p.y  ,pt[0].p.z,pt[0].p.w,dwColor,0,0);
				pt[2].set(pt[0].p.x+1,pt[0].p.y  ,pt[0].p.z,pt[0].p.w,dwColor,0,0);
				pt[3].set(pt[0].p.x  ,pt[0].p.y-1,pt[0].p.z,pt[0].p.w,dwColor,0,0);
				pt[4].set(pt[0].p.x  ,pt[0].p.y+1,pt[0].p.z,pt[0].p.w,dwColor,0,0);
                Device.RenderNearer(0.001);
                Device.SetTransform(D3DTS_WORLD,precalc_identity);
				Device.Shader.Set(Device.m_WireShader);
                DU::DrawPrimitiveTL(D3DPT_POINTLIST,5,pt,5,true,true);
                Device.ResetNearer();
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
    Ipoint pt;
    GetCursorPos(&start_pt); start_pt=UI->GetD3DWindow()->ScreenToClient(start_pt);
    pt.set(float(start_pt.x),float(start_pt.y));
    Device.m_Camera.MouseRayFromPoint(brush_start,brush_dir,pt);
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

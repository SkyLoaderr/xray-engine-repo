//----------------------------------------------------
// file: EParticlesObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESceneAIMapTools.h"
#include "D3dUtils.h"
#include "render.h"
#include "SceneObject.h"
#include "bottombar.h"
#include "ui_leveltools.h"

typedef Fvector2 t_node_tc[4];
static const float dtc = 0.25f;
static t_node_tc node_tc[16]=
{
	{0.f+0*dtc,0.25f+0*dtc,	0.25f+0*dtc,0.25f+0*dtc,	0.25f+0*dtc,0.f+0*dtc,	0.f+0*dtc,0.f+0*dtc},
	{0.f+1*dtc,0.25f+0*dtc,	0.25f+1*dtc,0.25f+0*dtc,	0.25f+1*dtc,0.f+0*dtc,	0.f+1*dtc,0.f+0*dtc},
	{0.f+2*dtc,0.25f+0*dtc,	0.25f+2*dtc,0.25f+0*dtc,	0.25f+2*dtc,0.f+0*dtc,	0.f+2*dtc,0.f+0*dtc},
	{0.f+3*dtc,0.25f+0*dtc,	0.25f+3*dtc,0.25f+0*dtc,	0.25f+3*dtc,0.f+0*dtc,	0.f+3*dtc,0.f+0*dtc},

	{0.f+0*dtc,0.25f+1*dtc,	0.25f+0*dtc,0.25f+1*dtc,	0.25f+0*dtc,0.f+1*dtc,	0.f+0*dtc,0.f+1*dtc},
	{0.f+1*dtc,0.25f+1*dtc,	0.25f+1*dtc,0.25f+1*dtc,	0.25f+1*dtc,0.f+1*dtc,	0.f+1*dtc,0.f+1*dtc},
	{0.f+2*dtc,0.25f+1*dtc,	0.25f+2*dtc,0.25f+1*dtc,	0.25f+2*dtc,0.f+1*dtc,	0.f+2*dtc,0.f+1*dtc},
	{0.f+3*dtc,0.25f+1*dtc,	0.25f+3*dtc,0.25f+1*dtc,	0.25f+3*dtc,0.f+1*dtc,	0.f+3*dtc,0.f+1*dtc},

	{0.f+0*dtc,0.25f+2*dtc,	0.25f+0*dtc,0.25f+2*dtc,	0.25f+0*dtc,0.f+2*dtc,	0.f+0*dtc,0.f+2*dtc},
	{0.f+1*dtc,0.25f+2*dtc,	0.25f+1*dtc,0.25f+2*dtc,	0.25f+1*dtc,0.f+2*dtc,	0.f+1*dtc,0.f+2*dtc},
	{0.f+2*dtc,0.25f+2*dtc,	0.25f+2*dtc,0.25f+2*dtc,	0.25f+2*dtc,0.f+2*dtc,	0.f+2*dtc,0.f+2*dtc},
	{0.f+3*dtc,0.25f+2*dtc,	0.25f+3*dtc,0.25f+2*dtc,	0.25f+3*dtc,0.f+2*dtc,	0.f+3*dtc,0.f+2*dtc},

	{0.f+0*dtc,0.25f+3*dtc,	0.25f+0*dtc,0.25f+3*dtc,	0.25f+0*dtc,0.f+3*dtc,	0.f+0*dtc,0.f+3*dtc},
	{0.f+1*dtc,0.25f+3*dtc,	0.25f+1*dtc,0.25f+3*dtc,	0.25f+1*dtc,0.f+3*dtc,	0.f+1*dtc,0.f+3*dtc},
	{0.f+2*dtc,0.25f+3*dtc,	0.25f+2*dtc,0.25f+3*dtc,	0.25f+2*dtc,0.f+3*dtc,	0.f+2*dtc,0.f+3*dtc},
	{0.f+3*dtc,0.25f+3*dtc,	0.25f+3*dtc,0.25f+3*dtc,	0.25f+3*dtc,0.f+3*dtc,	0.f+3*dtc,0.f+3*dtc},
};

void ESceneAIMapTools::OnDeviceCreate()
{
	m_Shader.create("editor\\ai_node","ed\\ed_ai_arrows_01");
    m_RGeom.create(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.Index.Buffer());        
}

void ESceneAIMapTools::OnDeviceDestroy()
{
	m_Shader.destroy();
    m_RGeom.destroy();
}

static const u32 block_size = 0x2000;
void ESceneAIMapTools::OnRender(int priority, bool strictB2F)
{
	if (m_Flags.is(flHideNodes)) return;
    if (1==priority){
        if (false==strictB2F){
            RCache.set_xform_world(Fidentity);
			if (OBJCLASS_AIMAP==LTools->CurrentClassID()){
	            u32 clr = 0xffffc000;
	            Device.SetShader	(Device.m_WireShader);
    	        DU.DrawSelectionBox	(m_AIBBox,&clr);
            }
            if (Valid()){
                // render nodes
                Device.SetShader	(m_Shader);
                Device.SetRS		(D3DRS_CULLMODE,		D3DCULL_NONE);
                Irect rect;
                HashRect			(Device.m_Camera.GetPosition(),m_VisRadius,rect);

                u32 vBase;
                _VertexStream* Stream= &RCache.Vertex;
                FVF::LIT* pv		= (FVF::LIT*)Stream->Lock(block_size,m_RGeom->vb_stride,vBase);
                u32	cnt				= 0;
//				Device.Statistic.TEST0.Begin();
//				Device.Statistic.TEST2.Begin();
                for (int x=rect.x1; x<=rect.x2; x++){
                    for (int z=rect.y1; z<=rect.y2; z++){
                        AINodeVec* nodes	= HashMap(x,z);
                        if (nodes){
                            for (AINodeIt it=nodes->begin(); it!=nodes->end(); it++){
                                SAINode& N = **it;
//								if (N.flags.is(SAINode::flHide)) continue;
                                if (Render->ViewBase.testSphere_dirty(N.Pos,m_Params.fPatchSize)){
                                    u32 clr;
                                    if (N.flags.is(SAINode::flSelected))clr = 0xffffffff;
                                    else 								clr = N.flags.is(SAINode::flHLSelected)?0xff909090:0xff606060;
                                    int k = 0;
                                    if (N.n1) k |= 1<<0;
                                    if (N.n2) k |= 1<<1;
                                    if (N.n3) k |= 1<<2;
                                    if (N.n4) k |= 1<<3;
                                    Fvector		v;
                                    FVF::LIT	v1,v2,v3,v4;
                                    float st 	= 0.25f;
                                    float tt	= 0.01f;
                                    Fvector	DUP; DUP.set(0,1,0);   
                                    v.set(N.Pos.x-st,N.Pos.y,N.Pos.z-st);	N.Plane.intersectRayPoint(v,DUP,v1.p);	v1.p.mad(v1.p,N.Plane.n,tt); v1.t.set(node_tc[k][0]); v1.color=clr;	// minX,minZ
                                    v.set(N.Pos.x+st,N.Pos.y,N.Pos.z-st);	N.Plane.intersectRayPoint(v,DUP,v2.p);	v2.p.mad(v2.p,N.Plane.n,tt); v2.t.set(node_tc[k][1]); v2.color=clr;	// maxX,minZ
                                    v.set(N.Pos.x+st,N.Pos.y,N.Pos.z+st);	N.Plane.intersectRayPoint(v,DUP,v3.p);	v3.p.mad(v3.p,N.Plane.n,tt); v3.t.set(node_tc[k][2]); v3.color=clr;	// maxX,maxZ
                                    v.set(N.Pos.x-st,N.Pos.y,N.Pos.z+st);	N.Plane.intersectRayPoint(v,DUP,v4.p);	v4.p.mad(v4.p,N.Plane.n,tt); v4.t.set(node_tc[k][3]); v4.color=clr;	// minX,maxZ
                                    pv->set(v3); pv++;
                                    pv->set(v2); pv++;
                                    pv->set(v1); pv++;
                                    pv->set(v1); pv++;
                                    pv->set(v4); pv++;
                                    pv->set(v3); pv++;
                                    cnt+=6;
                                    if (cnt>=block_size-6){
                                        Stream->Unlock	(cnt,m_RGeom->vb_stride);
                                        Device.DP		(D3DPT_TRIANGLELIST,m_RGeom,vBase,cnt/3);
                                        pv 				= (FVF::LIT*)Stream->Lock(block_size,m_RGeom->vb_stride,vBase);
                                        cnt				= 0;
                                    }	
                                }
                            }
                        }
                    }
                }
//                Device.Statistic.TEST2.End();
//                Device.Statistic.TEST0.End();
				Stream->Unlock		(cnt,m_RGeom->vb_stride);
                if (cnt) Device.DP	(D3DPT_TRIANGLELIST,m_RGeom,vBase,cnt/3);
                Device.SetRS		(D3DRS_CULLMODE,		D3DCULL_CCW);
            }
        }else{
/*            // render snap
            if (m_Flags.is(flDrawSnapObjects))
                for(ObjectIt _F=m_SnapObjects.begin();_F!=m_SnapObjects.end();_F++) 
                    if((*_F)->Visible()) ((CSceneObject*)(*_F))->RenderSelection(0x4046B646);
*/        }
    }
}
//----------------------------------------------------


 
#include "stdafx.h"
#pragma hdrstop

#include "ESceneAIMapTools.h"                            
#include "scene.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "ui_aimaptools.h"   

SAINode* ESceneAIMapTools::PickNode(const Fvector& start, const Fvector& dir, float dist)
{
	CTimer T;
	T.Start();

	SAINode* R 	= 0;
    float psz	= (m_Params.fPatchSize/2)*(m_Params.fPatchSize/2);
	SPickQuery	PQ;
	if (Scene.RayQuery(PQ,start,dir,dist,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,GetSnapList())){
    	Fvector pt;
        pt.mad(start,dir,PQ.r_begin()->range);

        Irect rect;
        HashRect(pt,10.f*m_Params.fPatchSize,rect);
        for (int x=rect.x1; x<=rect.x2; x++){
            for (int z=rect.y1; z<=rect.y2; z++){
                AINodeVec* nodes	= HashMap(x,z);
                if (nodes){
                    float dist 		= flt_max;
                    for (AINodeIt it=nodes->begin(); it!=nodes->end(); it++){
                        Fvector dest;
                        SAINode* N 	= *it;
                        if (N->Plane.intersectRayPoint(start,dir,dest)){
                            if (N->Pos.distance_to_sqr(dest)<psz){
                                float d = start.distance_to_sqr(dest);
                                if (d<dist){
                                    R 	= N;
                                    dist= d;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return R;
}
bool ESceneAIMapTools::PickGround(Fvector& dest, const Fvector& start, const Fvector& dir, float dist)
{         	         
	SPickQuery	PQ;
	if (!GetSnapList()->empty()){
        if (Scene.RayQuery(PQ,start,dir,dist,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,GetSnapList())){
            dest.mad(start,dir,PQ.r_begin()->range);
            return true;
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Fill object list and try again.");
    }
    return false;
}
                             
int ESceneAIMapTools::RaySelect(bool flag, float& distance, const Fvector& start, const Fvector& direction)
{
	int count=0;
    switch (Tools.GetSubTarget()){
    case estAIMapNode:{
        SAINode * N = PickNode(start, direction, distance);
        if (N){ N->flags.set(SAINode::flSelected,flag); count++;}
    }break;
    }
    UpdateHLSelected	();
    UI.RedrawScene		();
    return count;
}

int ESceneAIMapTools::FrustumSelect(bool flag, const CFrustum& frustum)
{
    int count = 0;

    switch (Tools.GetSubTarget()){
    case estAIMapNode:{
        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++){
            SAINode& N 	= **it;
            u32 mask 	= 0xffff;
            Fbox bb; bb.set(N.Pos,N.Pos); bb.min.sub(m_Params.fPatchSize*0.35f); bb.max.add(m_Params.fPatchSize*0.35f);
            if (frustum.testSAABB(N.Pos,m_Params.fPatchSize,bb.min,bb.max,mask)){
                (*it)->flags.set(SAINode::flSelected,flag);
                count++;
            }
        }
    }break;
    }
    
    UpdateHLSelected	();
    UI.RedrawScene		();

    return count;
}



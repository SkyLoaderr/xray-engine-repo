#include "stdafx.h"
#pragma hdrstop

#include "ESceneAIMapTools.h"
#include "scene.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "ui_aimaptools.h"   

SAINode* ESceneAIMapTools::PickNode(const Fvector& start, const Fvector& dir, float dist)
{
//*
	SPickQuery	PQ;
	SAINode* R 	= 0;
	if (Scene.RayQuery(PQ,start,dir,dist,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,&m_SnapObjects)){
    	Fvector pt;
        pt.mad(start,dir,PQ.r_begin()->range);

        Irect rect;
        HashRect(pt,m_Params.fPatchSize,rect);
		float psz		= (m_Params.fPatchSize/2)*(m_Params.fPatchSize/2);
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
/*/
	SPickQuery	PQ;
	if (Scene.RayQuery(PQ,start,dir,dist,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,&m_SnapObjects)){
    	Fvector pt;
        pt.mad(start,dir,PQ.r_begin()->range);
		return ESceneAIMapTools::FindNode(pt,m_Params.fPatchSize*0.3f+0.05f);
    }
    return 0;
//*/
}
SAIEmitter* ESceneAIMapTools::PickEmitter(const Fvector& start, const Fvector& dir, float dist)
{
	SAIEmitter* E=0;
    for (AIEmitterIt it=m_Emitters.begin(); it!=m_Emitters.end(); it++){
        Fvector pos,ray2;
        pos.set(it->pos.x,it->pos.y+0.5f,it->pos.z);
        ray2.sub( pos, start );
        float d = ray2.dotproduct(dir);
        if (d>0){
            float d2 = ray2.magnitude();
            if( ((d2*d2-d*d) < (0.5f*0.5f)) && (d>0.5f) ){
                if (d<dist){
                    dist = d;
                    E = it;
                }
            }
        }
    }
    return E;
}

bool ESceneAIMapTools::PickGround(Fvector& dest, const Fvector& start, const Fvector& dir, float dist)
{         	         
	SPickQuery	PQ;
	if (!m_SnapObjects.empty()){     
        if (Scene.RayQuery(PQ,start,dir,dist,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,&m_SnapObjects)){
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
    case estAIMapEmitter:{
        SAINode * N = PickNode(start, direction, distance);
        if (N){ N->flags.set(SAINode::flSelected,flag); count++;}
    }break;
    case estAIMapNode:{
        SAIEmitter * E = PickEmitter(start, direction, distance);
        if (E){ E->flags.set(SAIEmitter::flSelected,flag); count++;}
    }break;
    }
    UpdateHLSelected	();
    UI.RedrawScene		();
    return count;
}

int ESceneAIMapTools::FrustumSelect(bool flag)
{
    int count = 0;

	CFrustum frustum;
    if (!UI.SelectionFrustum(frustum)) return 0;

    switch (Tools.GetSubTarget()){
    case estAIMapEmitter:{
	    for (AIEmitterIt it=m_Emitters.begin(); it!=m_Emitters.end(); it++)
        	if (frustum.testSphere_dirty(it->pos,0.5f)){ 
            	it->flags.set(SAIEmitter::flSelected,flag);
                count++;
            }
    }break;
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



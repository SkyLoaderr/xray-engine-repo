#include "stdafx.h"
#pragma hdrstop

#include "ESceneAIMapTools.h"
#include "scene.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "ui_aimaptools.h"

static SPickQuery PQ;
SAINode* ESceneAIMapTools::PickNode(const Fvector& start, const Fvector& dir, float dist)
{
	if (Scene.RayQuery(PQ,start,dir,dist,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,&m_SnapObjects)){
    	Fvector pt;
        pt.mad(start,dir,PQ.r_begin()->range);
		return ESceneAIMapTools::FindNode(pt,m_Params.fPatchSize*0.3f+0.05f);
    }
    return 0;
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

int ESceneAIMapTools::SelectObjects(bool flag)
{
	int count = 0;

    switch (Tools.GetSubTarget()){
    case estAIMapEmitter:{
	    for (AIEmitterIt it=m_Emitters.begin(); it!=m_Emitters.end(); it++)
        	it->flags.set(SAIEmitter::flSelected,flag);
		count = m_Emitters.size();
    }break;
    case estAIMapNode:{
        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
            (*it)->flags.set(SAINode::flSelected,flag);
		count = m_Nodes.size();
    }break;
    }
    UpdateHLSelected	();
    UI.RedrawScene		();
    return count;
}

int ESceneAIMapTools::RemoveSelection()
{
	int count=0;
    switch (Tools.GetSubTarget()){
    case estAIMapEmitter:{
	    for (int k=0; k<(int)m_Emitters.size(); k++){
        	if (m_Emitters[k].flags.is(SAIEmitter::flSelected)){
            	m_Emitters.erase(m_Emitters.begin()+k);
				count++;
                k--;
            }
        }
    }break;
    case estAIMapNode:{
    	if (m_Nodes.size()==SelectionCount(true)){
        	Clear(true);
        }else{
            for (int k=0; k<(int)m_Nodes.size(); k++){
                AINodeIt it = m_Nodes.begin()+k;
                if ((*it)->flags.is(SAINode::flSelected)){
                    // unregister node from hash
                    UnregisterNode 	(*it);
                    // remove all link to this node
                    Irect rect;
                    HashRect		((*it)->Pos,m_Params.fPatchSize,rect);
                    for (int x=rect.x1; x<=rect.x2; x++){
                        for (int z=rect.y1; z<=rect.y2; z++){
                            AINodeVec& nodes	= HashMap(x,z);
                            for (AINodeIt h_it=nodes.begin(); h_it!=nodes.end(); h_it++){
                                if ((*h_it)->n1==*it) 		(*h_it)->n1	= 0;
                                else if ((*h_it)->n2==*it) 	(*h_it)->n2	= 0;
                                else if ((*h_it)->n3==*it) 	(*h_it)->n3	= 0;
                                else if ((*h_it)->n4==*it) 	(*h_it)->n4	= 0;
                            }
                        }
                    }
                    // delete node & erase from list
                    xr_delete		(*it);
                    m_Nodes.erase	(it);
                
                    k--;
                    count++;
                }
            }
        }
    }break;
    }
    if (count){ 
	    UpdateHLSelected	();
    	UI.RedrawScene		();
    }
    return count;
}

int ESceneAIMapTools::InvertSelection()
{
	int count=0;
    switch (Tools.GetSubTarget()){
    case estAIMapEmitter:{
	    for (AIEmitterIt it=m_Emitters.begin(); it!=m_Emitters.end(); it++)
        	it->flags.invert(SAIEmitter::flSelected);
		count = m_Emitters.size();
    }break;
    case estAIMapNode:{
        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
            (*it)->flags.invert(SAINode::flSelected);
	    count = m_Nodes.size();
    }break;
    }
    UpdateHLSelected	();
    UI.RedrawScene		();
    return count;
}

int ESceneAIMapTools::SelectionCount(bool testflag)
{
	int count = 0;
    switch (Tools.GetSubTarget()){
    case estAIMapEmitter:{
	    for (AIEmitterIt it=m_Emitters.begin(); it!=m_Emitters.end(); it++)
            if (it->flags.is(SAIEmitter::flSelected)==testflag)
				count++;
    }break;
    case estAIMapNode:{
        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
            if ((*it)->flags.is(SAINode::flSelected)==testflag)
                count++;
    }break;
    }
    return count;
}


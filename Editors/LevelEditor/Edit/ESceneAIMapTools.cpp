//----------------------------------------------------
// file: EParticlesObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESceneAIMapTools.h"
#include "xrLevel.h"
#include "Scene.h"
#include "ui_main.h"
#include "ui_tools.h"
#include "ui_aimaptools.h"

// chunks
#define AIMAP_VERSION  				0x0002
//----------------------------------------------------
#define AIMAP_CHUNK_VERSION			0x0001       
#define AIMAP_CHUNK_FLAGS			0x0002
#define AIMAP_CHUNK_BOX				0x0003
#define AIMAP_CHUNK_PARAMS			0x0004
#define AIMAP_CHUNK_NODES			0x0006
#define AIMAP_CHUNK_SNAP_OBJECTS	0x0007
//----------------------------------------------------

void SAINode::PointLF(Fvector& D, float patch_size)
{
	Fvector	d;	d.set(0,-1,0);
	Fvector	v	= Pos;	
	float	s	= patch_size/2;
	v.x			-= s;
	v.z			+= s;
	Plane.intersectRayPoint(v,d,D);
}

void SAINode::PointFR(Fvector& D, float patch_size)
{
	Fvector	d;	d.set(0,-1,0);
	Fvector	v	= Pos;	
	float	s	= patch_size/2;
	v.x			+= s;
	v.z			+= s;
	Plane.intersectRayPoint(v,d,D);
}

void SAINode::PointRB(Fvector& D, float patch_size)
{
	Fvector	d;	d.set(0,-1,0);
	Fvector	v	= Pos;	
	float	s	= patch_size/2;
	v.x			+= s;
	v.z			-= s;
	Plane.intersectRayPoint(v,d,D);
}

void SAINode::PointBL(Fvector& D, float patch_size)
{
	Fvector	d;	d.set(0,-1,0);
	Fvector	v	= Pos;	
	float	s	= patch_size/2;
	v.x			-= s;
	v.z			-= s;
	Plane.intersectRayPoint(v,d,D);
}

void SAINode::Load(IReader& F, ESceneAIMapTools* tools)
{
	u32 			id;
    u16 			pl;
	NodePosition 	np;
    F.r				(&id,3); 			n1 = (SAINode*)tools->UnpackLink(id);
    F.r				(&id,3); 			n2 = (SAINode*)tools->UnpackLink(id);
    F.r				(&id,3); 			n3 = (SAINode*)tools->UnpackLink(id);
    F.r				(&id,3); 			n4 = (SAINode*)tools->UnpackLink(id);
	pl				= F.r_u16(); 		pvDecompress(Plane.n,pl);
    F.r				(&np,sizeof(np)); 	tools->UnpackPosition(Pos,np,tools->m_AIBBox,tools->m_Params);
	Plane.build		(Pos,Plane.n);
    flags.set		(F.r_u8());
}

void SAINode::Save(IWriter& F, ESceneAIMapTools* tools)
{
	u32 			id;
    u16 			pl;
	NodePosition 	np;
    
    id = n1?(u32)n1->idx:InvalidNode; F.w(&id,3);
    id = n2?(u32)n2->idx:InvalidNode; F.w(&id,3);
    id = n3?(u32)n3->idx:InvalidNode; F.w(&id,3);
    id = n4?(u32)n4->idx:InvalidNode; F.w(&id,3);
    pl = pvCompress (Plane.n);	 F.w_u16(pl);
	tools->PackPosition(np,Pos,tools->m_AIBBox,tools->m_Params); F.w(&np,sizeof(np));
    F.w_u8			(flags.get());
}

ESceneAIMapTools::ESceneAIMapTools()
{
	ClassID   	= OBJCLASS_AIMAP;
    m_Shader	= 0;
    m_Flags.zero();
    
    m_AIBBox.invalidate	();
//    m_Header.size_y				= m_Header.aabb.max.y-m_Header.aabb.min.y+EPS_L;
	hash_Initialize();
    m_VisRadius	= 30;
    m_CFModel	= 0;
}
//----------------------------------------------------

ESceneAIMapTools::~ESceneAIMapTools()
{
}
//----------------------------------------------------

void ESceneAIMapTools::Clear(bool bOnlyNodes)
{
	hash_Clear			();
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
    	xr_delete		(*it);
	m_Nodes.clear		();
	if (!bOnlyNodes){
	    m_SnapObjects.clear	();
    }
}
//----------------------------------------------------

void ESceneAIMapTools::CalculateNodesBBox(Fbox& bb)
{
    bb.invalidate();
	for (AINodeIt b_it=m_Nodes.begin(); b_it!=m_Nodes.end(); b_it++) 
    	bb.modify((*b_it)->Pos);
}
//----------------------------------------------------

void ESceneAIMapTools::OnFrame()
{
	if (m_Flags.is(flUpdateHL)){
    	m_Flags.set(flUpdateHL,FALSE);
        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
			(*it)->flags.set(SAINode::flHLSelected,FALSE);
        for (it=m_Nodes.begin(); it!=m_Nodes.end(); it++){
            SAINode& N = **it;
            if (N.flags.is(SAINode::flSelected))
                for (int k=0; k<4; k++) 
                    if (N.n[k]) N.n[k]->flags.set(SAINode::flHLSelected,TRUE);
        }
    }
}
//----------------------------------------------------

void ESceneAIMapTools::EnumerateNodes()
{
    u32 idx			= 0;
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++,idx++)
    	(*it)->idx	= idx;
}

void ESceneAIMapTools::DenumerateNodes()
{
	u32 cnt=m_Nodes.size();
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++){
    	if	(!((((u32)(*it)->n1<cnt)||((u32)(*it)->n1==InvalidNode))&&
        			 (((u32)(*it)->n2<cnt)||((u32)(*it)->n2==InvalidNode))&&
                     (((u32)(*it)->n3<cnt)||((u32)(*it)->n3==InvalidNode))&&
                     (((u32)(*it)->n4<cnt)||((u32)(*it)->n4==InvalidNode)))){
                     ELog.Msg(mtError,"Node: has wrong link [%3.2f, %3.2f, %3.2f], {%d,%d,%d,%d}",VPUSH((*it)->Pos),(*it)->n1,(*it)->n2,(*it)->n3,(*it)->n4);
                     (*it)->n1 = 0;
                     (*it)->n2 = 0;
                     (*it)->n3 = 0;
                     (*it)->n4 = 0;
                     continue;
                     }
//                     ,"AINode: Wrong link found.");
    	(*it)->n1	= ((u32)(*it)->n1==InvalidNode)?0:m_Nodes[(u32)(*it)->n1];
    	(*it)->n2	= ((u32)(*it)->n2==InvalidNode)?0:m_Nodes[(u32)(*it)->n2];
    	(*it)->n3	= ((u32)(*it)->n3==InvalidNode)?0:m_Nodes[(u32)(*it)->n3];
    	(*it)->n4	= ((u32)(*it)->n4==InvalidNode)?0:m_Nodes[(u32)(*it)->n4];
/*    
    	if (((u32)(*it)->n1<cnt)||((u32)(*it)->n1==InvalidNode)) (*it)->n1	= ((u32)(*it)->n1==InvalidNode)?0:m_Nodes[(u32)(*it)->n1]; 
        else (*it)->n1=0;
    	if (((u32)(*it)->n2<cnt)||((u32)(*it)->n2==InvalidNode)) (*it)->n2	= ((u32)(*it)->n2==InvalidNode)?0:m_Nodes[(u32)(*it)->n2];
        else (*it)->n2=0;
    	if (((u32)(*it)->n3<cnt)||((u32)(*it)->n3==InvalidNode)) (*it)->n3	= ((u32)(*it)->n3==InvalidNode)?0:m_Nodes[(u32)(*it)->n3];
        else (*it)->n3=0;
    	if (((u32)(*it)->n4<cnt)||((u32)(*it)->n4==InvalidNode)) (*it)->n4	= ((u32)(*it)->n4==InvalidNode)?0:m_Nodes[(u32)(*it)->n4];
        else (*it)->n4=0;
*/
    }
}

bool ESceneAIMapTools::Load(IReader& F)
{
	u32 version = 0;

    R_ASSERT(F.r_chunk(AIMAP_CHUNK_VERSION,&version));
    if( version!=AIMAP_VERSION ){
        ELog.DlgMsg( mtError, "AIMap: Unsupported version.");
        return false;
    }

    R_ASSERT(F.find_chunk(AIMAP_CHUNK_FLAGS));
    F.r				(&m_Flags,sizeof(m_Flags));
    
    R_ASSERT(F.find_chunk(AIMAP_CHUNK_BOX));
    F.r				(&m_AIBBox,sizeof(m_AIBBox));

    R_ASSERT(F.find_chunk(AIMAP_CHUNK_PARAMS));
    F.r				(&m_Params,sizeof(m_Params));

    R_ASSERT(F.find_chunk(AIMAP_CHUNK_NODES));
    m_Nodes.resize	(F.r_u32());
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++){
    	*it			= xr_new<SAINode>();
    	(*it)->Load	(F,this);
    }
	DenumerateNodes	();

	// snap objects
    if (F.find_chunk(AIMAP_CHUNK_SNAP_OBJECTS)){
    	string128 	buf;
		int cnt 	= F.r_u32();
        if (cnt){
	        for (int i=0; i<cnt; i++){
    	    	F.r_stringZ	(buf);
        	    CCustomObject* O = Scene.FindObjectByName(buf,OBJCLASS_SCENEOBJECT);
            	if (!O)		ELog.Msg(mtError,"AIMap: Can't find object '%s' in scene.",buf);
	            else		m_SnapObjects.push_back(O);
    	    }
        }
    }

    hash_FillFromNodes		();

    return true;
}
//----------------------------------------------------

void ESceneAIMapTools::OnSynchronize()
{
	RealUpdateSnapList	();
}
//----------------------------------------------------

void ESceneAIMapTools::Save(IWriter& F)
{
	F.open_chunk	(AIMAP_CHUNK_VERSION);
	F.w_u16			(AIMAP_VERSION);
	F.close_chunk	();

	F.open_chunk	(AIMAP_CHUNK_FLAGS);
    F.w				(&m_Flags,sizeof(m_Flags));
	F.close_chunk	();

	F.open_chunk	(AIMAP_CHUNK_BOX);
    F.w				(&m_AIBBox,sizeof(m_AIBBox));
	F.close_chunk	();

	F.open_chunk	(AIMAP_CHUNK_PARAMS);
    F.w				(&m_Params,sizeof(m_Params));
	F.close_chunk	();

    EnumerateNodes	();
	F.open_chunk	(AIMAP_CHUNK_NODES);
    F.w_u32			(m_Nodes.size());
	for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
    	(*it)->Save	(F,this);
	F.close_chunk	();

	F.open_chunk	(AIMAP_CHUNK_SNAP_OBJECTS);
    F.w_u32			(m_SnapObjects.size());
    for (ObjectIt o_it=m_SnapObjects.begin(); o_it!=m_SnapObjects.end(); o_it++)
    	F.w_stringZ	((*o_it)->Name);
    F.close_chunk	();
}
//----------------------------------------------------

bool ESceneAIMapTools::Valid()
{
	return !m_Nodes.empty();
}

bool ESceneAIMapTools::IsNeedSave()
{
	return (!m_Nodes.empty()||!m_SnapObjects.empty());
}

void ESceneAIMapTools::OnObjectRemove(CCustomObject* O)
{
	if (OBJCLASS_SCENEOBJECT==O->ClassID){
    	if (find(m_SnapObjects.begin(),m_SnapObjects.end(),O)!=m_SnapObjects.end()){
			m_SnapObjects.remove(O);
	    	RealUpdateSnapList();
        }
    }
}

int ESceneAIMapTools::AddNode(const Fvector& pos, bool bIgnoreConstraints, bool bAutoLink, int sz)
{
   	Fvector Pos				= pos;
    if (1==sz){
        SAINode* N 			= BuildNode(Pos,Pos,bIgnoreConstraints,true);
        if (N){
            N->flags.set	(SAINode::flSelected,TRUE); 
            if (bAutoLink) 	UpdateLinks(N,bIgnoreConstraints);
            return			1;
        }else{
            ELog.Msg		(mtError,"Can't create node.");
            return 			0;
        }
    }else{
		return BuildNodes	(Pos,sz,bIgnoreConstraints);
    }
}

void ESceneAIMapTools::RemoveNode(AINodeIt it)
{
	R_ASSERT(it!=m_Nodes.end());
	SAINode* node = *it;
    // unregister node from hash
	AINodeVec* V = HashMap(node->Pos); R_ASSERT2(V,"AINode position out of bounds.");
	for (AINodeIt I=V->begin(); I!=V->end(); I++)
    	if (node==*I) {V->erase(I); break;}
    // delete node & erase from list
    m_Nodes.erase	(it);
    xr_delete		(node);
}

void ESceneAIMapTools::RemoveInvalidNodes(int link)
{
    UI.ProgressStart	(m_Nodes.size(), "Removing invalid nodes...");
    // remove link to sel nodes
    for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++){
        for (int k=0; k<4; k++){ 
        	if ((*it)->n[k]&&(link==(*it)->n[k]->Links())) (*it)->n[k]=0;
        }
    }
    // realy remove node
	int count=0;
    for (int k=0; k<(int)m_Nodes.size(); k++){
        AINodeIt it = m_Nodes.begin()+k;
        if (link==(*it)->Links()){
        	RemoveNode	(it);
            k--;
            count++;
        }
        if (k%128==0) {
            UI.ProgressInc	();
            if (UI.NeedAbort()) break;
        }
    }
    UI.ProgressEnd		();
    ELog.DlgMsg			(mtInformation,"Removed '%d' invalid nodes.",count);
	Scene.UndoSave		();
}

int ESceneAIMapTools::SelectObjects(bool flag)
{
	int count = 0;

    switch (Tools.GetSubTarget()){
    case estAIMapNode:{
        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
			if (!(*it)->flags.is(SAINode::flHide))
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
    case estAIMapNode:{
    	if (m_Nodes.size()==(u32)SelectionCount(true)){
        	count 	= m_Nodes.size();
        	Clear	(true);
        }else{
        	// remove link to sel nodes
	        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++){
            	for (int k=0; k<4; k++) 
                	if ((*it)->n[k]&&(*it)->n[k]->flags.is(SAINode::flSelected)) (*it)->n[k]=0;
            }
            // remove sel nodes
            for (int k=0; k<(int)m_Nodes.size(); k++){
                AINodeIt it = m_Nodes.begin()+k;
                if ((*it)->flags.is(SAINode::flSelected)){
                	RemoveNode		(it);
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
    case estAIMapNode:{
        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
			if (!(*it)->flags.is(SAINode::flHide))
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
    case estAIMapNode:{
        for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
            if ((*it)->flags.is(SAINode::flSelected)==testflag)
                count++;
    }break;
    }
    return count;
}

int ESceneAIMapTools::ShowObjects(bool flag, bool bAllowSelectionFlag, bool bSelFlag)
{
	int count=0;
    for (AINodeIt it=m_Nodes.begin(); it!=m_Nodes.end(); it++)
        if (bAllowSelectionFlag){
	        if ((*it)->flags.is(SAINode::flSelected)==bSelFlag){
                (*it)->flags.set(SAINode::flHide,!flag);
                if (false==flag) (*it)->flags.set(SAINode::flSelected,FALSE);
                count++;
            }
        }else{
            (*it)->flags.set(SAINode::flHide,!flag);
            if (false==flag) (*it)->flags.set(SAINode::flSelected,FALSE);
            count++;
        }
    UpdateHLSelected();
    return count;
}

void ESceneAIMapTools::FillProp(LPCSTR pref, PropItemVec& items)
{
}


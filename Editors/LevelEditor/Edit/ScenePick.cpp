#include "stdafx.h"
#pragma hdrstop

#include "Scene.h"
#include "ui_main.h"
#include "SceneObject.h"
//------------------------------------------------------------------------------

int EScene::FrustumPick( const CFrustum& frustum, EObjClass classfilter, ObjectList& ol )
{
	int count = 0;

    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++){
            ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
            if (mt) 		count+=mt->FrustumPick(ol,frustum);
        }
    }else{
        ESceneCustomOTools* mt = GetOTools(classfilter);
        if (mt) 			count+=mt->FrustumPick(ol,frustum);
    }

	return count;
}
//------------------------------------------------------------------------------

int EScene::SpherePick( const Fvector& center, float radius, EObjClass classfilter, ObjectList& ol )
{
	int count = 0;

    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++){
            ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
            if (mt) 		count+=mt->SpherePick(ol,center, radius);
        }
    }else{
        ESceneCustomOTools* mt = GetOTools(classfilter);
        if (mt) 			count+=mt->SpherePick(ol,center, radius);
    }

	return count;
}
//------------------------------------------------------------------------------

int EScene::RayQuery(SPickQuery& PQ, const Fvector& start, const Fvector& dir, float dist, u32 flags, ObjectList* snap_list)
{
    VERIFY			(snap_list);
    PQ.prepare_rq	(start,dir,dist,flags);
	XRC.ray_options	(flags);
    for(ObjectIt _F=snap_list->begin();_F!=snap_list->end();_F++)
        ((CSceneObject*)(*_F))->RayQuery(PQ);
	return PQ.r_count();
}
//------------------------------------------------------------------------------

int EScene::BoxQuery(SPickQuery& PQ, const Fbox& bb, u32 flags, ObjectList* snap_list)
{
    VERIFY			(snap_list);
    PQ.prepare_bq	(bb,flags);
	XRC.box_options	(flags);
    for(ObjectIt _F=snap_list->begin();_F!=snap_list->end();_F++)
        ((CSceneObject*)(*_F))->BoxQuery(PQ);
	return PQ.r_count();
}
//------------------------------------------------------------------------------

int EScene::RayQuery(SPickQuery& PQ, const Fvector& start, const Fvector& dir, float dist, u32 flags, CDB::MODEL* model)
{
    PQ.prepare_rq	(start,dir,dist,flags);
	XRC.ray_options	(flags);
    XRC.ray_query	(model,start,dir,dist);
    for (int r=0; r<XRC.r_count(); r++)
        PQ.append	(model,XRC.r_begin()+r);
	return PQ.r_count();
}
//------------------------------------------------------------------------------

int EScene::BoxQuery(SPickQuery& PQ, const Fbox& bb, u32 flags, CDB::MODEL* model)
{
    PQ.prepare_bq	(bb,flags);
	XRC.box_options	(flags);
    Fvector c,d;
    bb.getcenter	(c);
    bb.getradius	(d);
    XRC.box_query	(model,c,d);
    for (int r=0; r<XRC.r_count(); r++)
        PQ.append	(model,XRC.r_begin()+r);
	return PQ.r_count();
}
//------------------------------------------------------------------------------

CCustomObject *EScene::RayPickObject(float nearest_dist, const Fvector& start, const Fvector& direction, EObjClass classfilter, SRayPickInfo* pinf, ObjectList* from_list)
{
	if(!valid()) return 0;

	CCustomObject *nearest_object = 0;
    if (from_list){
        for(ObjectIt _F=from_list->begin();_F!=from_list->end();_F++)
            if((*_F)->Visible()&&(*_F)->RayPick(nearest_dist,start,direction,pinf))
                nearest_object = (*_F);
    }else{
        if (classfilter==OBJCLASS_DUMMY){
			CCustomObject *obj = 0;
            SceneToolsMapPairIt _I = m_SceneTools.begin();
            SceneToolsMapPairIt _E = m_SceneTools.end();
            for (; _I!=_E; _I++){
                ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
                if (mt&&mt->RayPick(obj,nearest_dist,start,direction,pinf)) nearest_object = obj;
            }
        }else{
            ESceneCustomOTools* mt = GetOTools(classfilter); VERIFY(mt);
            if (mt) mt->RayPick(nearest_object,nearest_dist,start,direction,pinf);
        }
    }
	return nearest_object;
}
//------------------------------------------------------------------------------

int EScene::GetQueryObjects(ObjectList& lst, EObjClass classfilter, int iSel, int iVis, int iLock)
{
    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++){
            ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
            if (mt) mt->GetQueryObjects(lst, iSel, iVis, iLock);
        }
    }else{
        ESceneCustomOTools* mt = GetOTools(classfilter); VERIFY(mt);
        if (mt) mt->GetQueryObjects(lst, iSel, iVis, iLock);
    }
    return lst.size();
}
//------------------------------------------------------------------------------

int EScene::RaySelect(int flag, EObjClass classfilter)
{
	if( !valid() ) return 0;
	CCustomObject *nearest_object = RayPickObject(flt_max,UI.m_CurrentRStart,UI.m_CurrentRNorm,classfilter,0,0);
    if (nearest_object) nearest_object->Select(flag);
    UI.RedrawScene();
    return nearest_object?1:0;
}
//------------------------------------------------------------------------------

int EScene::BoxPickObjects(const Fbox& box, SBoxPickInfoVec& pinf, ObjectList* lst)
{
	if (lst){
        for(ObjectIt _F=lst->begin();_F!=lst->end();_F++){
            CSceneObject* _S = dynamic_cast<CSceneObject*>(*_F); 
            if (_S) _S->BoxPick(box,pinf);
        }
    }
    return pinf.size();
}
//------------------------------------------------------------------------------
 

#include "stdafx.h"
#pragma hdrstop

#include "Scene.h"
#include "ui_main.h"
//------------------------------------------------------------------------------

int EScene::SelectObjects( bool flag, EObjClass classfilter )
{
	int count = 0;

    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second)	count+=_I->second->SelectObjects(flag);
    }else{
        ESceneCustomMTools* mt = GetMTools(classfilter);
        if (mt) 			count+=mt->SelectObjects(flag);
    }

    UI.RedrawScene();
	return count;
}
//------------------------------------------------------------------------------

int EScene::FrustumSelect( int flag, EObjClass classfilter )
{
	CFrustum frustum;
	int count = 0;
    if (!UI.SelectionFrustum(frustum)) return 0;

    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second)	count+=_I->second->FrustumSelect(flag,frustum);
    }else{
        ESceneCustomMTools* mt = GetMTools(classfilter);
        if (mt) 			count+=mt->FrustumSelect(flag,frustum);
    }
    
    UI.RedrawScene();
	return count;
}
//------------------------------------------------------------------------------

int EScene::InvertSelection( EObjClass classfilter )
{
	int count = 0;

    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second)	count+=_I->second->InvertSelection();
    }else{
        ESceneCustomMTools* mt = GetMTools(classfilter);
        if (mt) 			count+=mt->InvertSelection();
    }
    
    UI.RedrawScene();
	return count;
}
//------------------------------------------------------------------------------

int EScene::RemoveSelection( EObjClass classfilter )
{
	int count = 0;

    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second)	count+=_I->second->RemoveSelection();
    }else{
        ESceneCustomMTools* mt = GetMTools(classfilter);
        if (mt) 			count+=mt->RemoveSelection();
    }

    UI.UpdateScene	(true);
	return count;
}
//------------------------------------------------------------------------------

int EScene::SelectionCount(bool testflag, EObjClass classfilter)
{
	int count = 0;

    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second)	count+=_I->second->SelectionCount(testflag);
    }else{
        ESceneCustomMTools* mt = GetMTools(classfilter);
        if (mt) 			count+=mt->SelectionCount(testflag);
    }

    return count;
}
//------------------------------------------------------------------------------

bool EScene::ContainsObject( CCustomObject* object, EObjClass classfilter )
{
	VERIFY( object );
	VERIFY( m_Valid );
    ObjectList& lst 	= ListObj(classfilter);
    ObjectIt it 		= std::find(lst.begin(), lst.end(), object);
    if (it!=lst.end())	return true;
    return false;
}
//------------------------------------------------------------------------------

int EScene::ObjCount()
{
	int cnt = 0;
    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    for (; _I!=_E; _I++){
    	ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
		if (mt) cnt+=mt->ObjCount();
    }
	return cnt;
}
//------------------------------------------------------------------------------

int EScene::LockObjects( bool flag, EObjClass classfilter, bool bAllowSelectionFlag, bool bSelFlag )
{
	int count = 0;
    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second){
		        ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
                if (mt)		count+=mt->LockObjects(flag, bAllowSelectionFlag, bSelFlag);
            }
    }else{
        ESceneCustomOTools* mt = GetOTools(classfilter);
        if (mt) 			count+=mt->LockObjects(flag, bAllowSelectionFlag, bSelFlag);
    }
    UI.RedrawScene();
	return count;
}
//------------------------------------------------------------------------------

int EScene::ShowObjects( bool flag, EObjClass classfilter, bool bAllowSelectionFlag, bool bSelFlag )
{
	int count = 0;
    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++)
            if (_I->second)	count+=_I->second->ShowObjects(flag, bAllowSelectionFlag, bSelFlag);
    }else{
        ESceneCustomMTools* mt = GetMTools(classfilter);
        if (mt) 			count+=mt->ShowObjects(flag, bAllowSelectionFlag, bSelFlag);
    }
    UI.RedrawScene();
	return count;
}
//------------------------------------------------------------------------------

void EScene::SynchronizeObjects()
{
    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    for (; _I!=_E; _I++)
        if (_I->second)	_I->second->OnSynchronize();
}
//------------------------------------------------------------------------------

void EScene::ZoomExtents( EObjClass cls, BOOL bSel )
{
	Fbox BB;	BB.invalidate();
	Fbox bb;
    ObjectList lst;
    if (GetQueryObjects(lst, cls, bSel, true, -1))
        for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            if ((*_F)->GetBox(bb)) BB.merge(bb);
    if (BB.is_valid()) Device.m_Camera.ZoomExtents(BB);
    else ELog.Msg(mtError,"Can't calculate bounding box. Nothing selected or some object unsupported this function.");
}
//------------------------------------------------------------------------------


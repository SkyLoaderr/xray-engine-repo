#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomOTools.h"
#include "ui_main.h"
#include "PropertiesListHelper.h"
#include "scene.h"

ESceneCustomOTools::ESceneCustomOTools(EObjClass cls):ESceneCustomMTools(cls)
{
	m_Flags.zero		();
}
//----------------------------------------------------

ESceneCustomOTools::~ESceneCustomOTools()
{
}
//----------------------------------------------------

void ESceneCustomOTools::UpdateSnapList()
{
}
//----------------------------------------------------

ObjectList*	ESceneCustomOTools::GetSnapList()
{
	return 0;
}
//----------------------------------------------------

BOOL ESceneCustomOTools::_AppendObject(CCustomObject* object)
{
	m_Objects.push_back(object);
    object->ParentTools = this;
    return TRUE;
}
//----------------------------------------------------

BOOL ESceneCustomOTools::_RemoveObject(CCustomObject* object)
{
	m_Objects.remove(object);
    return FALSE;
}
//----------------------------------------------------

void ESceneCustomOTools::Clear(bool bInternal)
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	xr_delete(*it);
    m_Objects.clear();
}
//----------------------------------------------------

void ESceneCustomOTools::OnFrame()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnFrame();
}
//----------------------------------------------------

void ESceneCustomOTools::OnRender(int priority, bool strictB2F)
{
//	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
//    	(*it)->Render(priority,strictB2F);
}
//----------------------------------------------------

void ESceneCustomOTools::OnSynchronize()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnSynchronize();
}
//----------------------------------------------------

void ESceneCustomOTools::OnSceneUpdate()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnSceneUpdate();
}
//----------------------------------------------------

void ESceneCustomOTools::OnDeviceCreate()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceCreate();
}
//----------------------------------------------------

void ESceneCustomOTools::OnDeviceDestroy()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceDestroy();
}
//----------------------------------------------------

bool ESceneCustomOTools::Validate()
{
	for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if (!(*it)->Validate()) return false;
    return true;
}

bool ESceneCustomOTools::Valid()
{
	return true;
}

bool ESceneCustomOTools::IsNeedSave()
{
	return !m_Objects.empty();
}

void ESceneCustomOTools::OnObjectRemove(CCustomObject* O)
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        (*_F)->OnObjectRemove(*_F);
}

int ESceneCustomOTools::SelectObjects(bool flag)
{
	int count = 0;
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()){
            (*_F)->Select( flag );
            count++;
        }
    UI.RedrawScene		();
    return count;
}

int ESceneCustomOTools::RemoveSelection()
{
	int count=0;

    ObjectIt _F = m_Objects.begin();
    while(_F!=m_Objects.end()){
        if((*_F)->Selected()){
            count ++;
            if ((*_F)->OnSelectionRemove()){
                ObjectIt _D = _F; _F++;
                Scene.RemoveObject(*_D,false);
                xr_delete	(*_D);
            }else{
                _F++;
            }
        }else{
            _F++;
        }
    }

	UI.RedrawScene		();
    return count;
}

int ESceneCustomOTools::InvertSelection()
{
	int count=0;

    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()){
            (*_F)->Select(-1);
            count++;
        }
        
    UI.RedrawScene		();
    return count;
}

int ESceneCustomOTools::SelectionCount(bool testflag)
{
	int count = 0;

    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()	&& ((*_F)->Selected() == testflag)) count++;
        
    return count;
}

int ESceneCustomOTools::ShowObjects(bool flag, bool bAllowSelectionFlag, bool bSelFlag)
{
	int count=0;

    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++){
        if (bAllowSelectionFlag){
            if ((*_F)->Selected()==bSelFlag){
                (*_F)->Show( flag );
                count++;
            }
        }else{
            (*_F)->Show( flag );
            count++;
        }
    }
    
    UI.RedrawScene();
    return count;
}

bool ESceneCustomOTools::PickGround(Fvector& dest, const Fvector& start, const Fvector& dir, float dist)
{
	return false;
}

BOOL ESceneCustomOTools::RayPick(CCustomObject*& object, float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	object = 0;
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()&&(*_F)->RayPick(distance,start,direction,pinf))
            object=*_F;
	return !!object;
}

BOOL ESceneCustomOTools::FrustumPick(ObjectList& lst, const CFrustum& frustum)
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()&&(*_F)->FrustumPick(frustum))
        	lst.push_back(*_F);
	return !lst.empty();
}

BOOL ESceneCustomOTools::SpherePick(ObjectList& lst, const Fvector& center, float radius)
{
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if((*_F)->Visible()&&(*_F)->SpherePick(center, radius))
        	lst.push_back(*_F);
	return !lst.empty();
}

int ESceneCustomOTools::RaySelect(bool flag, float& distance, const Fvector& start, const Fvector& direction)
{
    CCustomObject* nearest_object=0;
    if (RayPick(nearest_object,distance,start,direction,0)) nearest_object->Select(flag);

    UI.RedrawScene();
    return !!nearest_object;
}

int ESceneCustomOTools::FrustumSelect(bool flag, const CFrustum& frustum)
{
	ObjectList lst;

    FrustumPick(lst,frustum);
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
	    (*_F)->Select(flag);
    
	return 0;
}

int ESceneCustomOTools::GetQueryObjects(ObjectList& lst, int iSel, int iVis, int iLock)
{
	int count=0;
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++){
        if(	((iSel==-1)||((*_F)->Selected()==iSel))&&
            ((iVis==-1)||((*_F)->Visible()==iVis))&&
            ((iLock==-1)||((*_F)->Locked()==iLock))){
                lst.push_back(*_F);
                count++;
        	}
    }
    return count;
}

int ESceneCustomOTools::LockObjects(bool flag, bool bAllowSelectionFlag, bool bSelFlag)
{
	int count=0;
    for(ObjectIt _F = m_Objects.begin();_F!=m_Objects.end();_F++)
        if(bAllowSelectionFlag){
            if((*_F)->Selected()==bSelFlag){
                (*_F)->Lock( flag );
                count++;
            }
        }else{
            (*_F)->Lock( flag );
            count++;
        }
    return count;
}

CCustomObject* ESceneCustomOTools::FindObjectByName(LPCSTR name, CCustomObject* pass)
{
	ObjectIt _I = m_Objects.begin();
    ObjectIt _E = m_Objects.end();
	for(;_I!=_E;_I++) if((pass!=*_I)&&(0==strcmp((*_I)->Name,name))) return (*_I);
    return 0;
}

void ESceneCustomOTools::FillProp(LPCSTR pref, PropItemVec& items)
{
    for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if ((*it)->Selected()) (*it)->FillProp	(FHelper.PrepareKey(pref,"Items").c_str(),items);
}

bool ESceneCustomOTools::GetSummaryInfo(SSceneSummary* inf)
{
    for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->GetSummaryInfo(inf);
    return true;
}



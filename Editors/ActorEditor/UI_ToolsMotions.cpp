//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "UI_Main.h"
#include "leftbar.h"
#include "EditObject.h"
#include "PropertiesList.h"
#include "motion.h"
#include "bone.h"
#include "BodyInstance.h"
#include "fmesh.h"
#include "folderlib.h"
#include "ItemList.h"
//---------------------------------------------------------------------------

bool CActorTools::EngineModel::UpdateGeometryStream(CEditableObject* source)
{
	m_GeometryStream.clear();
    if (!source) return false;
    if (source->IsSkeleton())	return (source->PrepareSVGeometry(m_GeometryStream));
    else						return (source->PrepareOGF(m_GeometryStream));
}

bool CActorTools::EngineModel::UpdateMotionDefsStream(CEditableObject* source)
{
	m_MotionDefsStream.clear();
	return (source&&source->PrepareSVDefs(m_MotionDefsStream));
}

bool CActorTools::EngineModel::UpdateMotionKeysStream(CEditableObject* source)
{
	m_MotionKeysStream.clear();
	return (source&&source->PrepareSVKeys(m_MotionKeysStream));
}

bool CActorTools::EngineModel::UpdateVisual(CEditableObject* source, bool bUpdGeom, bool bUpdKeys, bool bUpdDefs)
{
	bool bRes = true;
	CMemoryWriter F;
	if (source->IsSkeleton()){
        if (bUpdGeom)	bRes = UpdateGeometryStream(source);
        if (!bRes){
        	ELog.Msg(mtError,"Can't create preview geometry.");
        	return false;
        }
        if (bUpdKeys)bRes = UpdateMotionKeysStream(source);
        if (bUpdDefs)bRes = UpdateMotionDefsStream(source);
        if (!bRes) ELog.Msg(mtError,"Can't create preview motions.");
        if (!bRes||!m_GeometryStream.size()||!m_MotionKeysStream.size()||!m_MotionDefsStream.size())
         	return false;
        F.w(m_GeometryStream.pointer(),m_GeometryStream.size());
        F.w(m_MotionKeysStream.pointer(),m_MotionKeysStream.size());
        F.w(m_MotionDefsStream.pointer(),m_MotionDefsStream.size());
    }else{
        bool bRes = true;
        if (bUpdGeom) 	bRes = UpdateGeometryStream(source);
        if (!bRes){
        	ELog.Msg(mtError,"Can't create preview geometry.");
        	return false;
        }
        if (!m_GeometryStream.size()) return false;
        F.w(m_GeometryStream.pointer(),m_GeometryStream.size());
    }
    IReader R(F.pointer(), F.size());
    Device.Models.Delete(m_pVisual);
    m_pVisual = Device.Models.Create(&R);
    m_pBlend = 0;
    return bRes;
}

//---------------------------------------------------------------------------

void CActorTools::EngineModel::PlayMotion(CSMotion* M)
{
    if (M&&IsRenderable()){
        if (M->m_Flags.is(esmFX)){
			for (int k=0; k<MAX_PARTS; k++){
            	if (!m_BPPlayCache[k].IsEmpty()){
                	CMotionDef* D = PKinematics(m_pVisual)->ID_Cycle_Safe(m_BPPlayCache[k].c_str());
                    if (D) D->PlayCycle(PKinematics(m_pVisual),k,false,0,0);
    	    	}
            }        
        	m_pBlend = PKinematics(m_pVisual)->PlayFX(M->Name(),1.f);
        }else{	
        	R_ASSERT(M->iBoneOrPart<MAX_PARTS);
            int idx 		= M->iBoneOrPart;
        	if (-1==idx)	for (int k=0; k<MAX_PARTS; k++) m_BPPlayCache[k] = M->Name();
            else			m_BPPlayCache[idx] = M->Name();
            m_pBlend		= 0;
			for (int k=0; k<MAX_PARTS; k++){
            	if (!m_BPPlayCache[k].IsEmpty()){
                	CMotionDef* D = PKinematics(m_pVisual)->ID_Cycle_Safe(m_BPPlayCache[k].c_str());
                    CBlend* B=0;
                    if (D){
                    	B = D->PlayCycle(PKinematics(m_pVisual),k,(idx==k)?true:false,0,0);
                        if (idx==k) m_pBlend = B;
                    }
    	    	}
            }        
        }
    }
}
void CActorTools::EngineModel::RestoreParams(TFormStorage* s)
{          
    for (int k=0; k<MAX_PARTS; k++)
    	m_BPPlayCache[k] = s->ReadString("bp_cache_"+AnsiString(k),"");
}

void CActorTools::EngineModel::SaveParams(TFormStorage* s)
{
    for (int k=0; k<MAX_PARTS; k++)
	    s->WriteString	("bp_cache_"+AnsiString(k),	m_BPPlayCache[k]);
}

//---------------------------------------------------------------------------

void CActorTools::OnMotionKeysModified()
{
	Modified			();
	m_Flags.set			(flUpdateMotionKeys,TRUE);
    if (fraLeftBar->ebRenderEngineStyle->Down){
		m_Flags.set		(flUpdateMotionKeys,FALSE);
        if (m_RenderObject.UpdateVisual(m_pEditObject,false,true,false)){
            PlayMotion();
        }else{
            m_RenderObject.DeleteVisual();
            fraLeftBar->SetRenderStyle(false);
        }
    }
    OnMotionDefsModified();
}

void CActorTools::OnMotionDefsModified()
{
	Modified			();
	m_Flags.set			(flUpdateMotionDefs,TRUE);
    if (fraLeftBar->ebRenderEngineStyle->Down){
		m_Flags.set		(flUpdateMotionDefs,FALSE);
        if (m_RenderObject.UpdateVisual(m_pEditObject,false,false,true)){
            PlayMotion();
        }else{
            m_RenderObject.DeleteVisual();
            fraLeftBar->SetRenderStyle(false);
        }
    }
    UndoSave			();
}

void CActorTools::OnGeometryModified()
{
	Modified			();
    if (fraLeftBar->ebRenderEngineStyle->Down){
		m_Flags.set		(flUpdateGeometry,FALSE);
        if (m_RenderObject.UpdateVisual(m_pEditObject,true,false,false)){
            PlayMotion();
        }else{
            m_RenderObject.DeleteVisual();
            fraLeftBar->SetRenderStyle(false);
        }
    }
    UndoSave			();
}
//---------------------------------------------------------------------------

bool CActorTools::AppendMotion(LPCSTR fn)
{
	VERIFY(m_pEditObject);
    return m_pEditObject->AppendSMotion(fn);
}

bool CActorTools::RemoveMotion(LPCSTR name)
{
	VERIFY(m_pEditObject);
    return m_pEditObject->RemoveSMotion(name);
}

bool CActorTools::SaveMotions(LPCSTR name, bool bSelOnly)
{
	VERIFY(m_pEditObject);
    ListItemsVec items;
    if (bSelOnly){
        if (m_ObjectItems->GetSelected(MOTIONS_PREFIX,items,true)){
            CMemoryWriter 	F;
            F.w_u32			(items.size());
            for (ListItemsIt it=items.begin(); it!=items.end(); it++)
                ((CSMotion*)(*it)->m_Object)->Save(F);
            F.save_to		(name);
            return true;
        }
    }else{
    	return m_pEditObject->SaveSMotions(name);
    }
    return false;
}

void CActorTools::MakePreview()
{
	if (m_pEditObject){
        CMemoryWriter F;
    	if (m_RenderObject.UpdateVisual(m_pEditObject,true,true,true)){
            PlayMotion();
        }else{
        	m_RenderObject.DeleteVisual();
	        fraLeftBar->SetRenderStyle(false);
        }
		m_Flags.set		(flUpdateGeometry|flUpdateMotionDefs|flUpdateMotionKeys,FALSE);
    }else{
    	ELog.DlgMsg(mtError,"Scene empty. Load object first.");
    }
}

void CActorTools::PlayMotion()
{
	if (m_pEditObject)
    	if (fraLeftBar->ebRenderEditorStyle->Down) m_pEditObject->SkeletonPlay();
        else if (fraLeftBar->ebRenderEngineStyle->Down) {
        	if (m_Flags.is(flUpdateMotionKeys))	{ OnMotionKeysModified();	}
        	if (m_Flags.is(flUpdateMotionDefs))	{ OnMotionDefsModified(); 	}
        	if (m_Flags.is(flUpdateGeometry))	{ OnGeometryModified(); 	}
            m_RenderObject.PlayMotion(m_pEditObject->GetActiveSMotion());
        }
}

void CActorTools::StopMotion()
{
	if (m_pEditObject)
    	if (fraLeftBar->ebRenderEditorStyle->Down) m_pEditObject->SkeletonStop();
        else if (fraLeftBar->ebRenderEngineStyle->Down&&m_RenderObject.m_pBlend) {
        	m_RenderObject.m_pBlend->playing	 = false;
        	m_RenderObject.m_pBlend->timeCurrent = 0;
        }
}

void CActorTools::PauseMotion()
{
	if (m_pEditObject)
    	if (fraLeftBar->ebRenderEditorStyle->Down) m_pEditObject->SkeletonPause();
        else if (fraLeftBar->ebRenderEngineStyle->Down&&m_RenderObject.m_pBlend) {
        	m_RenderObject.m_pBlend->playing=!m_RenderObject.m_pBlend->playing;
        }
}

bool CActorTools::RenameMotion(LPCSTR old_name, LPCSTR new_name)
{
	R_ASSERT(m_pEditObject);
	CSMotion* M = m_pEditObject->FindSMotionByName(old_name);	R_ASSERT(M);
	CSMotion* MN = m_pEditObject->FindSMotionByName(new_name);	R_ASSERT(!MN);
    M->SetName(new_name);
    return true;
}



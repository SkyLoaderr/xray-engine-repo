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
                    	B = D->PlayCycle(PKinematics(m_pVisual),k,(idx==k)?fraLeftBar->ebMixMotion->Down:false,0,0);
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
	m_bMotionModified = true;
	UI.Command(COMMAND_UPDATE_CAPTION);
    m_bNeedUpdateMotionKeys = true;
    if (fraLeftBar->ebRenderEngineStyle->Down){
        m_bNeedUpdateMotionKeys = false;
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
	m_bMotionModified = true;
	UI.Command(COMMAND_UPDATE_CAPTION);
    m_bNeedUpdateMotionDefs = true;
    if (fraLeftBar->ebRenderEngineStyle->Down){
        m_bNeedUpdateMotionDefs = false;
        if (m_RenderObject.UpdateVisual(m_pEditObject,false,false,true)){
            PlayMotion();
        }else{
            m_RenderObject.DeleteVisual();
            fraLeftBar->SetRenderStyle(false);
        }
    }
}

void CActorTools::OnGeometryModified()
{
    if (fraLeftBar->ebRenderEngineStyle->Down){
        m_bNeedUpdateGeometry = false;
        if (m_RenderObject.UpdateVisual(m_pEditObject,true,false,false)){
            PlayMotion();
        }else{
            m_RenderObject.DeleteVisual();
            fraLeftBar->SetRenderStyle(false);
        }
    }
}
//---------------------------------------------------------------------------

bool CActorTools::AppendMotion(LPCSTR name, LPCSTR fn)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->AppendSMotion(name,fn)){
	    OnMotionKeysModified();
		return true;
    }
	return false;
}

bool CActorTools::RemoveMotion(LPCSTR name)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->RemoveSMotion(name)){
	    OnMotionKeysModified();
		return true;
    }
	return false;
}

bool CActorTools::LoadMotions(LPCSTR name)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->LoadSMotions(name)){
	    OnMotionKeysModified();
		return true;
    }
	return false;
}

bool CActorTools::SaveMotions(LPCSTR name)
{
	VERIFY(m_pEditObject);
    return (m_pEditObject->SaveSMotions(name));
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
		m_bNeedUpdateMotionKeys = false;
        m_bNeedUpdateMotionDefs = false;
    }else{
    	ELog.DlgMsg(mtError,"Scene empty. Load object first.");
    }
}

void CActorTools::MotionOnChange(PropValue* sender)
{
	Flag8Value* V = (Flag8Value*)sender;
    if (V->GetValueEx()){
	    m_pCycleNode->Hidden	= true;
    	m_pFXNode->Hidden		= false;
    }else{
    	m_pFXNode->Hidden		= true;
	    m_pCycleNode->Hidden	= false;
    }
}
//------------------------------------------------------------------------------
                        
//------------------------------------------------------------------------------
void __fastcall CActorTools::BPOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	TokenValue2* V = dynamic_cast<TokenValue2*>(sender); R_ASSERT(V);
    (*((DWORD*)edit_val))--;
}
//------------------------------------------------------------------------------
void __fastcall CActorTools::BPOnBeforeEdit(PropValue* sender, LPVOID edit_val)
{
	TokenValue2* V = dynamic_cast<TokenValue2*>(sender); R_ASSERT(V);
    (*((DWORD*)edit_val))++;
}
//------------------------------------------------------------------------------
void __fastcall CActorTools::BPOnDraw(PropValue* sender, LPVOID draw_val)
{
	TokenValue2* V = dynamic_cast<TokenValue2*>(sender); R_ASSERT(V);
    (*((DWORD*)draw_val))++;
}
//------------------------------------------------------------------------------
void __fastcall CActorTools::OnMotionNameChange(PropValue* V)
{
    OnMotionKeysModified();
}
//------------------------------------------------------------------------------
void CActorTools::FillMotionProperties()
{
	R_ASSERT(m_pEditObject);
	CSMotion* SM = m_pEditObject->GetActiveSMotion();
    if (SM){
		PropItemVec items;
        PropValue* P=0;
        P=PHelper.CreateText	(items,"Name",		&SM->Name(), sizeof(SM->Name()));
        P->SetEvents			(FHelper.NameAfterEdit,FHelper.NameBeforeEdit,OnMotionNameChange);
        P->Owner()->SetEvents	(FHelper.NameDraw);
        P->Owner()->tag			= (int)FHelper.FindObject(fraLeftBar->tvMotions,SM->Name()); //VERIFY(P->Owner()->tag);
        P=PHelper.CreateFloat	(items,"Speed",		&SM->fSpeed,  0.f,20.f,0.01f,2);
        P=PHelper.CreateFloat	(items,"Accrue",	&SM->fAccrue, 0.f,20.f,0.01f,2);
        P=PHelper.CreateFloat	(items,"Falloff", 	&SM->fFalloff,0.f,20.f,0.01f,2);

        PropValue *C=0,*F=0,*TV=0;
        TV = PHelper.CreateFlag8(items,"Type FX", &SM->m_Flags, esmFX);
        TV->SetEvents			(0,0,MotionOnChange);
        {
            AStringVec lst;
            lst.push_back("--none--");
            for (BPIt it=m_pEditObject->FirstBonePart(); it!=m_pEditObject->LastBonePart(); it++) lst.push_back(it->alias);
			C=PHelper.CreateToken2	(items,"Cycle\\Bone part",	(u32*)&SM->iBoneOrPart,	&lst);
            C->SetEvents			(BPOnAfterEdit,BPOnBeforeEdit);
            C->Owner()->SetEvents	(BPOnDraw);
            P=PHelper.CreateFlag8	(items,"Cycle\\Stop at end",	&SM->m_Flags,	esmStopAtEnd);
            P=PHelper.CreateFlag8	(items,"Cycle\\No mix",			&SM->m_Flags,	esmNoMix);
            P=PHelper.CreateFlag8	(items,"Cycle\\Sync part",		&SM->m_Flags,	esmSyncPart);
        }
        {
            AStringVec lst;
            for (BoneIt it=m_pEditObject->FirstBone(); it!=m_pEditObject->LastBone(); it++) lst.push_back((*it)->Name());
            F=PHelper.CreateToken2	(items,"FX\\Start bone",		(u32*)&SM->iBoneOrPart,&lst);
            P=PHelper.CreateFloat		(items,"FX\\Power",				&SM->fPower,   	0.f,20.f,0.01f,2);
        }
        // fill values
        m_MotionProps->AssignItems(items,true);
        // find cycle/fx root node
	    R_ASSERT(C&&C->Owner()->Item()&&F&&F->Owner()->Item());
        m_pCycleNode 	= C->Owner()->Item()->Parent; R_ASSERT(m_pCycleNode); 
        m_pFXNode 		= F->Owner()->Item()->Parent; R_ASSERT(m_pFXNode); 
		// first init node
        MotionOnChange(TV);
    }else{
    	m_MotionProps->ClearProperties();	
		m_pCycleNode=0;
        m_pFXNode	=0;
    }
}

void CActorTools::PlayMotion()
{
	if (m_pEditObject)
    	if (fraLeftBar->ebRenderEditorStyle->Down) m_pEditObject->SkeletonPlay();
        else if (fraLeftBar->ebRenderEngineStyle->Down) {
        	if (m_bNeedUpdateMotionKeys){ OnMotionKeysModified();	}
        	if (m_bNeedUpdateMotionDefs){ OnMotionDefsModified(); 	}
        	if (m_bNeedUpdateGeometry)	{ OnGeometryModified(); 	}
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
	CSMotion* M = m_pEditObject->FindSMotionByName(old_name);
    R_ASSERT(M);
	CSMotion* MN = m_pEditObject->FindSMotionByName(new_name);
    R_ASSERT(!MN);
    M->SetName(new_name);
    OnMotionKeysModified();
    return true;
}


//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "UI_Main.h"
#include "leftbar.h"
#include "EditObject.h"
#include "xr_tokens.h"
#include "PropertiesList.h"
#include "motion.h"
#include "bone.h"
#include "BodyInstance.h"
#include "fmesh.h"
#include "xr_trims.h"
#include "folderlib.h"
//---------------------------------------------------------------------------

bool CActorTools::EngineModel::UpdateGeometryStream(CEditableObject* source)
{
	m_GeometryStream.clear();
    if (!source) return false;
    if (source->IsSkeleton())	return (source->PrepareSVGeometry(m_GeometryStream));
    else						return (source->PrepareOGF(m_GeometryStream));
}

bool CActorTools::EngineModel::UpdateMotionsStream(CEditableObject* source)
{
	m_MotionsStream.clear();
	return (source&&source->PrepareSVMotions(m_MotionsStream));
}

bool CActorTools::EngineModel::UpdateVisual(CEditableObject* source, bool bUpdGeom, bool bUpdMotions)
{
	bool bRes = true;
	CFS_Memory F;
	if (source->IsSkeleton()){
        if (bUpdGeom)	bRes = UpdateGeometryStream(source);
        if (!bRes){
        	ELog.Msg(mtError,"Can't create preview geometry.");
        	return false;
        }
        if (bUpdMotions)bRes = UpdateMotionsStream(source);
        if (!bRes) ELog.Msg(mtError,"Can't create preview motions.");
        if (!bRes||!m_GeometryStream.size()||!m_MotionsStream.size())
         	return false;
        F.write(m_GeometryStream.pointer(),m_GeometryStream.size());
        F.write(m_MotionsStream.pointer(),m_MotionsStream.size());
    }else{
        bool bRes = true;
        if (bUpdGeom) 	bRes = UpdateGeometryStream(source);
        if (!bRes){
        	ELog.Msg(mtError,"Can't create preview geometry.");
        	return false;
        }
        if (!m_GeometryStream.size()) return false;
        F.write(m_GeometryStream.pointer(),m_GeometryStream.size());
    }
    CStream R(F.pointer(), F.size());
    Device.Models.Delete(m_pVisual);
    m_pVisual = Device.Models.Create(&R);
    m_pBlend = 0;
    return bRes;
}
//---------------------------------------------------------------------------

void CActorTools::EngineModel::Render(const Fmatrix& mTransform)
{
    // render visual
    Device.SetTransform	(D3DTS_WORLD,mTransform);
    switch (m_pVisual->Type)
    {
    case MT_SKELETON:{
        CKinematics* pV					= (CKinematics*)m_pVisual;
        vector<CVisual*>::iterator I,E;
        I = pV->children.begin			();
        E = pV->children.end			();
        for (; I!=E; I++)
        {
            CVisual* V					= *I;
            Device.Shader.set_Shader	(V->hShader);
            V->Render					(m_fLOD);
        }
    }break;
    case MT_HIERRARHY:{
        FHierrarhyVisual* pV			= (FHierrarhyVisual*)m_pVisual;
        vector<CVisual*>::iterator 		I,E;
        I = pV->children.begin			();
        E = pV->children.end			();
        for (; I!=E; I++)
        {
            CVisual* V					= *I;
            Device.Shader.set_Shader	(V->hShader);
            V->Render					(m_fLOD);
        }
    }break;
    default:
        Device.Shader.set_Shader		(m_pVisual->hShader);
        m_pVisual->Render				(m_fLOD);
        break;
    }
}
//---------------------------------------------------------------------------

void CActorTools::MotionModified(){
	m_bMotionModified = true;
	UI.Command(COMMAND_UPDATE_CAPTION);
    if (fraLeftBar->ebRenderEngineStyle->Down)
    	if (m_RenderObject.UpdateVisual(m_pEditObject,false,true)){
        	PlayMotion();
        }else{
        	m_RenderObject.DeleteVisual();
	        fraLeftBar->SetRenderStyle(false);
        }
}
//---------------------------------------------------------------------------

bool CActorTools::AppendMotion(LPCSTR name, LPCSTR fn)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->AppendSMotion(name,fn)){
		MotionModified();
		return true;
    }
	return false;
}

bool CActorTools::RemoveMotion(LPCSTR name)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->RemoveSMotion(name)){
		MotionModified();
		return true;
    }
	return false;
}

bool CActorTools::LoadMotions(LPCSTR name)
{
	VERIFY(m_pEditObject);
    if (m_pEditObject->LoadSMotions(name)){
		MotionModified();
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
        CFS_Memory F;
    	if (m_RenderObject.UpdateVisual(m_pEditObject,true,true)){
            PlayMotion();
        }else{
        	m_RenderObject.DeleteVisual();
	        fraLeftBar->SetRenderStyle(false);
        }
    }else{
    	ELog.DlgMsg(mtError,"Scene empty. Load object first.");
    }
}

void CActorTools::MotionOnChange(PropValue* sender)
{
	Flag32Value* V = (Flag32Value*)sender;
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
void CActorTools::FillMotionProperties()
{
	R_ASSERT(m_pEditObject);
	CSMotion* SM = m_pEditObject->GetActiveSMotion();
    if (SM){
		PropItemVec items;
        PropValue* P=0;
        P=PHelper.CreateText	(items,"Name",		&SM->Name(), sizeof(SM->Name()));
        P->OnAfterEditEvent		= FHelper.NameAfterEdit;
        P->OnBeforeEditEvent	= FHelper.NameBeforeEdit;
        P->Owner()->OnDrawEvent	= FHelper.NameDraw;
        P->Owner()->tag			= (int)FHelper.FindObject(fraLeftBar->tvMotions,SM->Name()); VERIFY(P->Owner()->tag);
        PHelper.CreateFloat		(items,"Speed",		&SM->fSpeed,  0.f,20.f,0.01f,2);
        PHelper.CreateFloat		(items,"Accrue",	&SM->fAccrue, 0.f,20.f,0.01f,2);
        PHelper.CreateFloat		(items,"Falloff", 	&SM->fFalloff,0.f,20.f,0.01f,2);

        PropValue *C=0,*F=0,*TV=0;
        TV = PHelper.CreateFlag32(items,"Type FX", &SM->m_Flags, esmFX);
        TV->OnChangeEvent		= MotionOnChange;
        {
            AStringVec lst;
            lst.push_back("--none--");
            for (BPIt it=m_pEditObject->FirstBonePart(); it!=m_pEditObject->LastBonePart(); it++) lst.push_back(it->alias);
			C=PHelper.CreateToken2	(items,"Cycle\\Bone part",	(u32*)&SM->iBoneOrPart,	&lst);
            C->OnAfterEditEvent		= BPOnAfterEdit;
            C->OnBeforeEditEvent	= BPOnBeforeEdit;
            C->Owner()->OnDrawEvent=BPOnDraw;
            PHelper.CreateFlag32	(items,"Cycle\\Stop at end",	&SM->m_Flags,	esmStopAtEnd);
            PHelper.CreateFlag32	(items,"Cycle\\No mix",			&SM->m_Flags,	esmNoMix);
        }
        {
            AStringVec lst;
            for (BoneIt it=m_pEditObject->FirstBone(); it!=m_pEditObject->LastBone(); it++) lst.push_back((*it)->Name());
            F=PHelper.CreateToken2	(items,"FX\\Start bone",		(u32*)&SM->iBoneOrPart,&lst);
            PHelper.CreateFloat		(items,"FX\\Power",				&SM->fPower,   	0.f,20.f,0.01f,2);
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
            CSMotion* M=m_pEditObject->GetActiveSMotion();
            if (M&&m_RenderObject.IsRenderable())
            	if (M->m_Flags.is(esmFX))	m_RenderObject.m_pBlend = PKinematics(m_RenderObject.m_pVisual)->PlayFX(M->Name());
                else						m_RenderObject.m_pBlend = PKinematics(m_RenderObject.m_pVisual)->PlayCycle(M->Name(),fraLeftBar->ebMixMotion->Down);
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
    return true;
}


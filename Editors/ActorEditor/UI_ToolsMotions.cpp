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
        if (!bRes){
        	ELog.Msg(mtError,"Can't create preview motions.");
	        return false;
        }
        if (!m_GeometryStream.size()||!m_MotionsStream.size()) 		return false;
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
    	if (m_RenderObject.UpdateVisual(m_pEditObject,false,true))
        	PlayMotion();
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
	FlagValue* V = (FlagValue*)sender;
    if (V->GetValue()){
	    m_pCycleNode->Hidden	= true;
    	m_pFXNode->Hidden		= false;
    }else{
    	m_pFXNode->Hidden		= true;
	    m_pCycleNode->Hidden	= false;
    }
}

//------------------------------------------------------------------------------
void __fastcall CActorTools::NameOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	FOLDER::AfterTextEdit(fraLeftBar->tvMotions->Selected,((TextValue*)sender)->GetValue(),*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CActorTools::NameOnBeforeEdit(PropValue* sender, LPVOID edit_val)
{
	FOLDER::BeforeTextEdit(((TextValue*)sender)->GetValue(),*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CActorTools::NameOnDraw(PropValue* sender, LPVOID draw_val)
{
	FOLDER::TextDraw(((TextValue*)sender)->GetValue(),*(AnsiString*)draw_val);
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
		PropValueVec values;
    	FILL_PROP(values, "Name",		&SM->Name(),  	PROP::CreateText	(sizeof(SM->Name()),NameOnAfterEdit,NameOnBeforeEdit,NameOnDraw));
    	FILL_PROP(values, "Speed",		&SM->fSpeed,   	PROP::CreateFloat	(0.f,20.f,0.01f,2));
    	FILL_PROP(values, "Accrue",		&SM->fAccrue,  	PROP::CreateFloat	(0.f,20.f,0.01f,2));
    	FILL_PROP(values, "Falloff", 	&SM->fFalloff, 	PROP::CreateFloat	(0.f,20.f,0.01f,2));

        FlagValue* TV = PROP::CreateFlag(esmFX,0,0,0,MotionOnChange);
        FILL_PROP(values, "Type FX", 	&SM->m_dwFlags, TV);
        {
            AStringVec lst;
            lst.push_back("--none--");
            for (BPIt it=m_pEditObject->FirstBonePart(); it!=m_pEditObject->LastBonePart(); it++) lst.push_back(it->alias);
            FILL_PROP(values, "Cycle\\Bone part",	&SM->iBoneOrPart,	PROP::CreateToken2(&lst,BPOnAfterEdit,BPOnBeforeEdit,BPOnDraw));
            FILL_PROP(values, "Cycle\\Stop at end",	&SM->m_dwFlags,		PROP::CreateFlag(esmStopAtEnd));
            FILL_PROP(values, "Cycle\\No mix",		&SM->m_dwFlags,		PROP::CreateFlag(esmNoMix));
        }
        {
            AStringVec lst;
            for (BoneIt it=m_pEditObject->FirstBone(); it!=m_pEditObject->LastBone(); it++) lst.push_back((*it)->Name());
            FILL_PROP(values, "FX\\Start bone",		&SM->iBoneOrPart,	PROP::CreateToken2(&lst));
	    	FILL_PROP(values, "FX\\Power",			&SM->fPower,   		PROP::CreateFloat	(0.f,20.f,0.01f,2));
        }
        // fill values
        m_MotionProps->AssignValues(values,true);
        // find cycle/fx root node
	    PropValue* C 	= PROP::FindProp(values,"Cycle\\Bone part");	R_ASSERT(C&&C->item);
        m_pCycleNode 	= C->item->Parent; R_ASSERT(m_pCycleNode); 
	    PropValue* F 	= PROP::FindProp(values,"FX\\Start bone");		R_ASSERT(F&&F->item);
        m_pFXNode 		= F->item->Parent; R_ASSERT(m_pFXNode); 
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
            	if (M->IsFlag(esmFX))	m_RenderObject.m_pBlend = PKinematics(m_RenderObject.m_pVisual)->PlayFX(M->Name());
                else					m_RenderObject.m_pBlend = PKinematics(m_RenderObject.m_pVisual)->PlayCycle(M->Name(),fraLeftBar->ebMixMotion->Down);
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


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

void CActorTools::MotionOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	FlagValue* V = (FlagValue*)sender;
	R_ASSERT(edit_val);
    if (!(*(LPDWORD)edit_val&V->mask)){
	    m_pCycleNode->Hidden	= false;
    	m_pFXNode->Hidden		= true;
    }else{
	    m_pCycleNode->Hidden	= true;
    	m_pFXNode->Hidden		= false;
    }
}

//------------------------------------------------------------------------------
void __fastcall CActorTools::NameOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	FOLDER::AfterTextEdit(fraLeftBar->tvMotions->Selected,((TextValue*)sender)->val,*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CActorTools::NameOnBeforeEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	FOLDER::BeforeTextEdit(((TextValue*)sender)->val,*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CActorTools::NameOnDraw(PropValue* sender, LPVOID draw_val)
{
	FOLDER::TextDraw(((TextValue*)sender)->val,*(AnsiString*)draw_val);
}
//------------------------------------------------------------------------------
                        
//------------------------------------------------------------------------------
void __fastcall CActorTools::BPOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	TokenValue2* V = dynamic_cast<TokenValue2*>(sender); R_ASSERT(V);
    (*((DWORD*)edit_val))--;
}
//------------------------------------------------------------------------------
void __fastcall CActorTools::BPOnBeforeEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
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
    m_MotionProps->BeginFillMode();
    if (SM){
        m_MotionProps->AddItem(0,PROP_TEXT,  		"Name",		m_MotionProps->MakeTextValue (&SM->Name(),sizeof(SM->Name()),NameOnAfterEdit,NameOnBeforeEdit,NameOnDraw));
        m_MotionProps->AddItem(0,PROP_FLOAT, 		"Speed", 	m_MotionProps->MakeFloatValue(&SM->fSpeed,	0.f,20.f,0.01f,2));
        m_MotionProps->AddItem(0,PROP_FLOAT, 		"Accrue", 	m_MotionProps->MakeFloatValue(&SM->fAccrue,	0.f,20.f,0.01f,2));
        m_MotionProps->AddItem(0,PROP_FLOAT, 		"Falloff", 	m_MotionProps->MakeFloatValue(&SM->fFalloff,	0.f,20.f,0.01f,2));
        FlagValue* TV = m_MotionProps->MakeFlagValue(&SM->m_dwFlags,esmFX,MotionOnAfterEdit);
        m_MotionProps->AddItem(0,PROP_FLAG, 		"Type FX", 	TV);
        m_pCycleNode = m_MotionProps->AddItem(0,PROP_MARKER, "Cycle");
        {
            AStringVec lst;
            lst.push_back("--none--");
            for (BPIt it=m_pEditObject->FirstBonePart(); it!=m_pEditObject->LastBonePart(); it++) lst.push_back(it->alias);
            m_MotionProps->AddItem(m_pCycleNode,PROP_TOKEN2, 	"Bone part",	m_MotionProps->MakeTokenValue2(&SM->iBoneOrPart,&lst,BPOnAfterEdit,BPOnBeforeEdit,BPOnDraw));
            m_MotionProps->AddItem(m_pCycleNode,PROP_FLAG,		"Stop at end",	m_MotionProps->MakeFlagValue(&SM->m_dwFlags,esmStopAtEnd));
            m_MotionProps->AddItem(m_pCycleNode,PROP_FLAG,		"No mix",		m_MotionProps->MakeFlagValue(&SM->m_dwFlags,esmNoMix));
        }
        m_pFXNode = m_MotionProps->AddItem(0,PROP_MARKER, "FX");
        {
            AStringVec lst;
            for (BoneIt it=m_pEditObject->FirstBone(); it!=m_pEditObject->LastBone(); it++) lst.push_back((*it)->Name());
            m_MotionProps->AddItem(m_pFXNode,PROP_TOKEN2,	"Start bone",m_MotionProps->MakeTokenValue2(&SM->iBoneOrPart,&lst));
            m_MotionProps->AddItem(m_pFXNode,PROP_FLOAT, 	"Power", 	m_MotionProps->MakeFloatValue(&SM->fPower,	0.f,20.f,0.01f,2));
        }
        MotionOnAfterEdit(0,TV,TV->val);
    }else{
		m_pCycleNode=0;
        m_pFXNode	=0;
    }
	m_MotionProps->EndFillMode();
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


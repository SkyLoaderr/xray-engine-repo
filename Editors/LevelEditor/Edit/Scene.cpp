//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Scene.h"
#include "SceneObject.h"
#include "ui_main.h"
#include "PropertiesList.h"
#include "Sector.h"
#include "SoundManager.h"
#include "EParticlesObject.h"
#include "ui_tools.h"

#include "ESceneAIMapTools.h"
#include "DetailObjects.h"
#include "ESceneLightTools.h"
//----------------------------------------------------
EScene Scene;
//----------------------------------------------------
st_LevelOptions::st_LevelOptions()
{
	Reset();
}

void st_LevelOptions::InitDefaultText()
{
	char deffilename[MAX_PATH];
	int handle;
	m_BOPText="";
}

void st_LevelOptions::Reset()
{
	m_FNLevelPath	= "level";

    InitDefaultText	();

    m_BuildParams.Init();
}
//------------------------------------------------------------------------------

#define MAX_VISUALS 16384

EScene::EScene()
{
	m_Valid = false;
	m_Locked = 0;

    for (int i=0; i<OBJCLASS_COUNT; i++)
        m_SceneTools.insert(mk_pair((EObjClass)i,(ESceneCustomMTools*)NULL));

    // first init scene graph for objects
    mapRenderObjects.init(MAX_VISUALS);
// Build options
    m_SummaryInfo	= 0;
    ClearSnapList	(false);
}

EScene::~EScene()
{
	VERIFY( m_Valid == false );
    m_ESO_SnapObjects.clear	();
}

void EScene::OnCreate()
{
	Device.seqDevCreate.Add	(this,REG_PRIORITY_NORMAL);
	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);

    CreateSceneTools		();
    
	m_LastAvailObject 		= 0;
    m_LevelOp.Reset			();
	ELog.Msg				( mtInformation, "Scene: initialized" );
	m_Valid 				= true;
    m_RTFlags.zero			();
	UI.Command				(COMMAND_UPDATE_CAPTION);
	m_SummaryInfo 			= TProperties::CreateForm();
}

void EScene::OnDestroy()
{
	TProperties::DestroyForm(m_SummaryInfo);
    Unload					();
    UndoClear				();
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	ELog.Msg				( mtInformation, "Scene: cleared" );
	m_LastAvailObject 		= 0;
	m_Valid 				= false;

    for (int i=0; i<OBJCLASS_COUNT; i++)
    	xr_delete			(m_SceneTools[(EObjClass)i]);
    m_SceneTools.clear		();
}

void EScene::AppendObject( CCustomObject* object, bool bUndo )
{
	VERIFY			  	(object);
	VERIFY				(m_Valid);
    
    ESceneCustomOTools* mt	= GetOTools(object->ClassID); VERIFY3(mt,"Can't find Object Tools:",GetMTools(object->ClassID)->ClassDesc());
    mt->_AppendObject	(object);
    UI.UpdateScene		();
    if (bUndo){	
	    object->Select	(true);
    	UndoSave();
    }
}

bool EScene::RemoveObject( CCustomObject* object, bool bUndo )
{
	VERIFY				(object);
	VERIFY				(m_Valid);

    ESceneCustomOTools* mt 	= GetOTools(object->ClassID);
    if (mt){
    	mt->_RemoveObject(object);
        if (object->ClassID==OBJCLASS_SCENEOBJECT){
            // signal everyone "I'm deleting"
            m_ESO_SnapObjects.remove			(object);

            SceneToolsMapPairIt _I = m_SceneTools.begin();
            SceneToolsMapPairIt _E = m_SceneTools.end();
            for (; _I!=_E; _I++){
                ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
                if (mt) mt->OnObjectRemove(object);
            }
            UpdateSnapList						();
        }
        UI.UpdateScene	();
    }
    if (bUndo)		   	UndoSave();
    return true;
}

void EScene::OnFrame( float dT )
{
	if( !valid() ) return;
	if( locked() ) return;

    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second)		t_it->second->OnFrame();
}

void EScene::Clear()
{
	// clear snap
    ClearSnapList			(false);
	// clear scene tools
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second)		t_it->second->Clear();
        
    m_CompilerErrors.Clear	();

    m_RTFlags.set			(flRT_Unsaved|flRT_Modified,FALSE);
}
//----------------------------------------------------

bool EScene::GetBox(Fbox& box, EObjClass classfilter)
{
	return GetBox(box,ListObj(classfilter));
}
//----------------------------------------------------

bool EScene::GetBox(Fbox& box, ObjectList& lst)
{
    box.invalidate();
    bool bRes=false;
    for(ObjectIt it=lst.begin();it!=lst.end();it++){
        Fbox bb;
        if((*it)->GetBox(bb)){
            box.modify(bb.min);
            box.modify(bb.max);
            bRes=true;
        }
    }
    return bRes;
}
//----------------------------------------------------

void EScene::Modified()
{
	m_RTFlags.set(flRT_Modified|flRT_Unsaved,TRUE);
    UI.Command(COMMAND_UPDATE_CAPTION);
}

bool EScene::IsUnsaved()
{
    return (m_RTFlags.is(flRT_Unsaved) && (ObjCount()||!UI.GetEditFileName().IsEmpty()));
}
bool EScene::IsModified()
{
    return (m_RTFlags.is(flRT_Modified));
}

bool EScene::IfModified()
{
	if (locked()){ 
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        return false;
    }
    if (m_RTFlags.is(flRT_Unsaved) && (ObjCount()||!UI.GetEditFileName().IsEmpty())){
        int mr = ELog.DlgMsg(mtConfirmation, "The scene has been modified. Do you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; break;
        case mrNo: m_RTFlags.set(flRT_Unsaved,FALSE); break;
        case mrCancel: return false;
        }
    }
    return true;
}

void EScene::Unload()
{
	m_LastAvailObject = 0;
	Clear();
	if (m_SummaryInfo) m_SummaryInfo->HideProperties();
}

void EScene::OnObjectsUpdate()
{
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second)		t_it->second->OnSceneUpdate();
}

void EScene::OnDeviceCreate()
{
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second)		t_it->second->OnDeviceCreate();
}

void EScene::OnDeviceDestroy()
{
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second)		t_it->second->OnDeviceDestroy();
}
//------------------------------------------------------------------------------

void EScene::OnShowHint(AStringVec& dest)
{
    CCustomObject* obj = RayPickObject(UI.m_CurrentRStart,UI.m_CurrentRNorm,Tools.CurrentClassID(),0,0);
    if (obj) obj->OnShowHint(dest);
}
//------------------------------------------------------------------------------

void EScene::ExportGame(SExportStreams& F)
{
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second)		t_it->second->ExportGame(F);
}
//------------------------------------------------------------------------------

bool EScene::Validate(bool bNeedOkMsg, bool bTestPortal, bool bTestHOM, bool bTestGlow, bool bTestShaderCompatible)
{
	bool bRes = true;
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++){
        if (t_it->second){
        	if (!t_it->second->Validate()){
				ELog.Msg(mtError,"ERROR: Validate '%s' failed!",t_it);
                bRes = false;
            }
        }
    }

	if (bTestPortal){
		if (!PortalUtils.Validate(false)){
			ELog.Msg(mtError,"*ERROR: Scene has non associated face (face without sector)!");
            bRes = false;
    	}
    }
    if (bTestHOM){
        bool bHasHOM=false;
        ObjectList& lst = ListObj(OBJCLASS_SCENEOBJECT);
        for(ObjectIt it=lst.begin();it!=lst.end();it++){
            CEditableObject* O = ((CSceneObject*)(*it))->GetReference(); R_ASSERT(O);
            if (O->m_Flags.is(CEditableObject::eoHOM)){ bHasHOM = true; break; }
        }
        if (!bHasHOM)
            if (mrNo==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Level doesn't contain HOM.\nContinue anyway?"))
                return false;
    }
    if (ObjCount(OBJCLASS_SPAWNPOINT)==0){
    	ELog.Msg(mtError,"*ERROR: Can't find any Spawn Object.");
        bRes = false;
    }
    if (ObjCount(OBJCLASS_LIGHT)==0){
    	ELog.Msg(mtError,"*ERROR: Can't find any Light Object.");
        bRes = false;
    }
    if (ObjCount(OBJCLASS_SCENEOBJECT)==0){
    	ELog.Msg(mtError,"*ERROR: Can't find any Scene Object.");
        bRes = false;
    }
    if (bTestGlow){
        if (ObjCount(OBJCLASS_GLOW)==0){
            ELog.Msg(mtError,"*ERROR: Can't find any Glow Object.");
            bRes = false;
        }
    }
    if (FindDuplicateName()){
    	ELog.Msg(mtError,"*ERROR: Found duplicate object name.");
        bRes = false;
    }
    
    if (bTestShaderCompatible){
    	bool res = true;
        ObjectList& lst = ListObj(OBJCLASS_SCENEOBJECT);
		DEFINE_SET(CEditableObject*,EOSet,EOSetIt);
        EOSet objects;
        int static_obj = 0; 
        for(ObjectIt it=lst.begin();it!=lst.end();it++){
        	CSceneObject* S = (CSceneObject*)(*it);
        	if (S->IsStatic()||S->IsMUStatic()){
            	static_obj++;
	            CEditableObject* O = ((CSceneObject*)(*it))->GetReference(); R_ASSERT(O);
                if (objects.find(O)==objects.end()){
	    	        if (!O->CheckShaderCompatible()) res = false;
                    objects.insert(O);
                }
            }
        }
		if (!res){ 
        	ELog.Msg	(mtError,"*ERROR: Scene has non compatible shaders. See log.");
            bRes = false;
        }
		if (0==static_obj){ 
        	ELog.Msg	(mtError,"*ERROR: Can't find static geometry.");
            bRes = false;
        }
    }
    
    if (!SndLib.Validate()) 
        bRes = false;

    {
        ObjectList& lst = ListObj(OBJCLASS_PS);
        for(ObjectIt it=lst.begin();it!=lst.end();it++){
        	EParticlesObject* S = (EParticlesObject*)(*it);
            if (!S->GetParticles()){
		    	ELog.Msg(mtError,"*ERROR: Particle System hasn't reference.");
                bRes = false;
            }
        }
    }
    
    if (bRes){
    	if (bNeedOkMsg) ELog.DlgMsg(mtInformation,"Validation OK!");
    }else{
    	ELog.DlgMsg(mtInformation,"Validation FAILED!");
    }
    return bRes;
}



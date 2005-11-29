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
#include "ui_leveltools.h"
#include "guid_generator.h"

#include "ESceneAIMapTools.h"
#include "ESceneDOTools.h"
#include "ESceneLightTools.h"
//----------------------------------------------------
EScene* Scene;
//----------------------------------------------------
st_LevelOptions::st_LevelOptions()
{
	Reset();
}

void st_LevelOptions::Reset()
{
	m_FNLevelPath		= "level";
    m_LevelPrefix		= "level_prefix";
    m_LightHemiQuality	= 3;
    m_LightSunQuality	= 3;
	m_BOPText			= "";
    m_BuildParams.Init	();
    m_BuildParams.setHighQuality();
}

void st_LevelOptions::SetCustomQuality()
{
	m_BuildParams.m_quality	= ebqCustom;
}

void st_LevelOptions::SetDraftQuality()
{
    m_BuildParams.setDraftQuality();
    m_LightHemiQuality	= 0;
    m_LightSunQuality	= 0;
}

void st_LevelOptions::SetHighQuality()
{
    m_BuildParams.setHighQuality();
    m_LightHemiQuality	= 3;
    m_LightSunQuality	= 3;
}
//------------------------------------------------------------------------------


#define MAX_VISUALS 16384

EScene::EScene()
{
	m_Valid = false;
	m_Locked = 0;

    for (int i=0; i<OBJCLASS_COUNT; i++)
        m_SceneTools.insert(mk_pair((ObjClassID)i,(ESceneCustomMTools*)NULL));

    // first init scene graph for objects
    mapRenderObjects.init(MAX_VISUALS);
// 	Build options
    m_SummaryInfo	= 0;
    //ClearSnapList	(false);
}

EScene::~EScene()
{
	VERIFY( m_Valid == false );
    m_ESO_SnapObjects.clear	();
}

void EScene::OnCreate()
{
//.	Device.seqDevCreate.Add	(this,REG_PRIORITY_NORMAL);
//.	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);

    CreateSceneTools		();
    
	m_LastAvailObject 		= 0;
    m_LevelOp.Reset			();
	ELog.Msg				( mtInformation, "Scene: initialized" );
	m_Valid 				= true;
    m_RTFlags.zero			();
    ExecCommand				(COMMAND_UPDATE_CAPTION);
	m_SummaryInfo 			= TProperties::CreateForm("Level Summary Info", 0, alNone, 0,0,0, TProperties::plFolderStore|TProperties::plItemFolders);
}

void EScene::OnDestroy()
{
	TProperties::DestroyForm(m_SummaryInfo);
    Unload					(FALSE);
    UndoClear				();
//.	Device.seqDevCreate.Remove(this);
//.	Device.seqDevDestroy.Remove(this);
	ELog.Msg				( mtInformation, "Scene: cleared" );
	m_LastAvailObject 		= 0;
	m_Valid 				= false;
    DestroySceneTools		();
}

void EScene::AppendObject( CCustomObject* object, bool bUndo )
{
	VERIFY			  	(object);
	VERIFY				(m_Valid);
    
    ESceneCustomOTools* mt	= GetOTools(object->ClassID); VERIFY3(mt,"Can't find Object Tools:",GetMTools(object->ClassID)->ClassDesc());
    mt->_AppendObject	(object);
    UI->UpdateScene		();
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
    if (mt&&mt->IsEditable()){
    	mt->_RemoveObject(object);
        // signal everyone "I'm deleting"
//        if (object->ClassID==OBJCLASS_SCENEOBJECT)
        {
            m_ESO_SnapObjects.remove			(object);

            SceneToolsMapPairIt _I = m_SceneTools.begin();
            SceneToolsMapPairIt _E = m_SceneTools.end();
            for (; _I!=_E; _I++){
                ESceneCustomMTools* mt = dynamic_cast<ESceneCustomMTools*>(_I->second);
                if (mt) mt->OnObjectRemove(object);
            }
            UpdateSnapList						();
        }
        UI->UpdateScene	();
    }
    if (bUndo)		   	UndoSave();
    return true;
}

void EScene::BeforeObjectChange( CCustomObject* object )
{
	VERIFY				(object);
	VERIFY				(m_Valid);

    ESceneCustomOTools* mt 	= GetOTools(object->ClassID);
    if (mt&&mt->IsEditable()){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++){
            ESceneCustomMTools* mt 		= dynamic_cast<ESceneCustomMTools*>(_I->second);
            if (mt) mt->OnBeforeObjectChange(object);
        }
        UI->UpdateScene	();
    }
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

void EScene::Reset()
{
	// unload scene
    Unload				(FALSE);
    // reset tools
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second&&t_it->first!=OBJCLASS_DUMMY)
            t_it->second->Reset	();
}

void EScene::Unload		(BOOL bEditableOnly)
{
	m_LastAvailObject 	= 0;
	Clear				(bEditableOnly);
	if (m_SummaryInfo) 	m_SummaryInfo->HideProperties();
}

void EScene::Clear(BOOL bEditableToolsOnly)
{
	// clear snap
    ClearSnapList			(false);
	// clear scene tools
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second&&t_it->first!=OBJCLASS_DUMMY){ 
        	if (!bEditableToolsOnly||(bEditableToolsOnly&&t_it->second->IsEditable())){
	        	t_it->second->Clear();
            }
        }
        
    Tools->ClearDebugDraw	();

    m_RTFlags.set			(flRT_Unsaved|flRT_Modified,FALSE);

    m_GUID					= generate_guid();
    m_OwnerName				= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    m_CreateTime			= time(NULL);

    m_SaveCache.free		();
}
//----------------------------------------------------

bool EScene::GetBox(Fbox& box, ObjClassID classfilter)
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
    ExecCommand(COMMAND_UPDATE_CAPTION);
}

bool EScene::IsUnsaved()
{
    return (m_RTFlags.is(flRT_Unsaved) && (ObjCount()||!Tools->GetEditFileName().IsEmpty()));
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
    if (m_RTFlags.is(flRT_Unsaved) && (ObjCount()||!Tools->GetEditFileName().IsEmpty())){
        int mr = ELog.DlgMsg(mtConfirmation, "The scene has been modified. Do you want to save your changes?");
        switch(mr){
        case mrYes: if (!ExecCommand(COMMAND_SAVE)) return false; break;
		case mrNo:{ 
        	m_RTFlags.set(flRT_Unsaved,FALSE); 
            ExecCommand	(COMMAND_UPDATE_CAPTION);
        }break;
        case mrCancel: return false;
        }
    }
    return true;
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
    CCustomObject* obj = RayPickObject(flt_max,UI->m_CurrentRStart,UI->m_CurrentRNorm,LTools->CurrentClassID(),0,0);
    if (obj) obj->OnShowHint(dest);
}
//------------------------------------------------------------------------------

bool EScene::ExportGame(SExportStreams* F)
{
	bool bres = true;
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second)		if (!t_it->second->ExportGame(F)) bres=false;
    return bres;
}
//------------------------------------------------------------------------------

bool EScene::Validate(bool bNeedOkMsg, bool bTestPortal, bool bTestHOM, bool bTestGlow, bool bTestShaderCompatible, bool bFullTest)
{
	bool bRes = true;
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++){
        if (t_it->second){
        	if (!t_it->second->Validate(bFullTest)){
				ELog.Msg(mtError,"ERROR: Validate '%s' failed!",t_it->second->ClassDesc());
                bRes = false;
            }
        }
    }

	if (bTestPortal){
        if (Scene->ObjCount(OBJCLASS_SECTOR)||Scene->ObjCount(OBJCLASS_PORTAL))
            if (!PortalUtils.Validate(true))
				bRes = false;
    }
    if (bTestHOM){
        bool bHasHOM=false;
        ObjectList& lst = ListObj(OBJCLASS_SCENEOBJECT);
        for(ObjectIt it=lst.begin();it!=lst.end();it++){
            CEditableObject* O = ((CSceneObject*)(*it))->GetReference(); R_ASSERT(O);
            if (O->m_Flags.is(CEditableObject::eoHOM)){ bHasHOM = true; break; }
        }
        if (!bHasHOM)
			Msg("!Level doesn't contain HOM objects!");
//.			if (mrNo==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Level doesn't contain HOM.\nContinue anyway?"))
//.				return false;
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
    
    if (!SndLib->Validate()) 
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

xr_string EScene::LevelPath()
{
    xr_string path;
	if (m_LevelOp.m_FNLevelPath.size()){
        FS.update_path	(path,"$level$",m_LevelOp.m_FNLevelPath.c_str());
        path			+= "\\";
    }
    return path;
}

#include "ESceneLightTools.h"
void EScene::SelectLightsForObject(CCustomObject* obj)
{
	ESceneLightTools* lt = dynamic_cast<ESceneLightTools*>(Scene->GetOTools(OBJCLASS_LIGHT)); VERIFY(lt);
    lt->SelectLightsForObject(obj);
}

void EScene::HighlightTexture(LPCSTR t_name, bool allow_ratio, u32 t_width, u32 t_height, bool leave_previous)
{
    if (!leave_previous) 		Tools->ClearDebugDraw();
    SceneToolsMapPairIt t_it 	= m_SceneTools.begin();
    SceneToolsMapPairIt t_end 	= m_SceneTools.end();
    for (; t_it!=t_end; t_it++)
        if (t_it->second)		t_it->second->HighlightTexture(t_name,allow_ratio,t_width,t_height,!leave_previous);
    UI->RedrawScene				();
}

xr_token		js_token	[ ]={
	{ "1 - Low",			1	},
	{ "4 - Medium",			4	},
	{ "9 - High",			9	},
	{ 0,					0 	}
};

void EScene::OnBuildControlClick	(PropValue* sender, bool& bModif, bool& bSafe)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: m_LevelOp.SetDraftQuality();	break;
    case 1: m_LevelOp.SetHighQuality();		break;
    case 2: m_LevelOp.SetCustomQuality();	break;
	}
    ExecCommand(COMMAND_UPDATE_PROPERTIES);
}

void EScene::OnRTFlagsChange	(PropValue* sender)
{
    ExecCommand(COMMAND_UPDATE_PROPERTIES);
}

void EScene::FillProp(LPCSTR pref, PropItemVec& items, ObjClassID cls_id)
{
	PHelper().CreateCaption		(items,PrepareKey(pref,"Scene\\Name"),			LTools->m_LastFileName.c_str());
	PHelper().CreateCaption		(items,PrepareKey(pref,"Scene\\Created by"),	m_OwnerName.size()?m_OwnerName.c_str():"unknown");
	PHelper().CreateCaption		(items,PrepareKey(pref,"Scene\\Created at"),	(m_CreateTime!=0)?Trim(AnsiString(ctime(&m_CreateTime))).c_str():"unknown");
    PHelper().CreateRText		(items,PrepareKey(pref,"Scene\\Name prefix"),	&m_LevelOp.m_LevelPrefix);

    PropValue* V;
    PHelper().CreateRText		(items,PrepareKey(pref,"Scene\\Build options\\Level path"),		&m_LevelOp.m_FNLevelPath);
    PHelper().CreateRText		(items,PrepareKey(pref,"Scene\\Build options\\Custom data"),	&m_LevelOp.m_BOPText);
    // common
    ButtonValue* B;
    B=PHelper().CreateButton	(items,PrepareKey(pref,"Scene\\Build options\\Quality"), "Draft,High,Custom",0);
    B->OnBtnClickEvent.bind		(this,&EScene::OnBuildControlClick);

    BOOL enabled				= (m_LevelOp.m_BuildParams.m_quality==ebqCustom);
    V=PHelper().CreateU8		(items,PrepareKey(pref,"Scene\\Build options\\Lighting\\Hemisphere quality [0-3]"),	&m_LevelOp.m_LightHemiQuality,	0,3);		V->Owner()->Enable(enabled);
    V=PHelper().CreateU8		(items,PrepareKey(pref,"Scene\\Build options\\Lighting\\Sun shadow quality [0-3]"),	&m_LevelOp.m_LightSunQuality,	0,3);       V->Owner()->Enable(enabled);

    // Build Options
    // Normals & optimization
    V=PHelper().CreateFloat		(items,PrepareKey(pref,"Scene\\Build options\\Optimizing\\Normal smooth angle"), 	&m_LevelOp.m_BuildParams.m_sm_angle,					0.f,180.f);	V->Owner()->Enable(enabled);
    V=PHelper().CreateFloat		(items,PrepareKey(pref,"Scene\\Build options\\Optimizing\\Weld distance (m)"),		&m_LevelOp.m_BuildParams.m_weld_distance,				0.f,1.f,0.001f,4);	V->Owner()->Enable(enabled);

    // Light maps
    V=PHelper().CreateFloat		(items,PrepareKey(pref,"Scene\\Build options\\Lighting\\Pixel per meter"),			&m_LevelOp.m_BuildParams.m_lm_pixels_per_meter,			0.f,20.f);	V->Owner()->Enable(enabled);
    V=PHelper().CreateU32		(items,PrepareKey(pref,"Scene\\Build options\\Lighting\\Error (LM collapsing)"), 	&m_LevelOp.m_BuildParams.m_lm_rms,						0,255);		V->Owner()->Enable(enabled);
    V=PHelper().CreateU32		(items,PrepareKey(pref,"Scene\\Build options\\Lighting\\Error (LM zero)"),			&m_LevelOp.m_BuildParams.m_lm_rms_zero,					0,255);		V->Owner()->Enable(enabled);
    V=PHelper().CreateToken32	(items,PrepareKey(pref,"Scene\\Build options\\Lighting\\Jitter samples"),			&m_LevelOp.m_BuildParams.m_lm_jitter_samples, 			js_token);	V->Owner()->Enable(enabled);
    
    // tools options
    if (OBJCLASS_DUMMY==cls_id){
        SceneToolsMapPairIt _I 			= FirstTools();
        SceneToolsMapPairIt _E			= LastTools();
        for (; _I!=_E; _I++){
	        ESceneCustomMTools* mt		= _I->second;
            if ((_I->first!=OBJCLASS_DUMMY)&&mt&&mt->IsEditable()){
                PHelper().CreateCaption	(items,PrepareKey(mt->ClassDesc(),"Last modified by"),	mt->m_ModifName.size()?mt->m_ModifName.c_str():"unknown");
                PHelper().CreateCaption	(items,PrepareKey(mt->ClassDesc(),"Last modified at"),	(mt->m_ModifTime!=0)?Trim(AnsiString(ctime(&mt->m_ModifTime))).c_str():"unknown");
                _I->second->FillProp	(mt->ClassDesc(),items);
            }
        }
    }else{
        ESceneCustomMTools* mt			= GetMTools	(cls_id);
        if (mt&&mt->IsEditable()){
            PHelper().CreateCaption		(items,PrepareKey(mt->ClassDesc(),"Last modified by"),	mt->m_ModifName.size()?mt->m_ModifName.c_str():"unknown");
            PHelper().CreateCaption		(items,PrepareKey(mt->ClassDesc(),"Last modified at"),	(mt->m_ModifTime!=0)?Trim(AnsiString(ctime(&mt->m_ModifTime))).c_str():"unknown");
         	mt->FillProp				(mt->ClassDesc(),items);
        }
    }
}


//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "Texture.h"
#include "FS.h"
#include "ui_main.h"
#include "Frustum.h"
//#include "Texturizer.h"
#include "EditObject.h"
#include "DPatch.h"
#include "DetailObjects.h"
#include "Sector.h"
#include "Library.h"
#include "xr_ini.h"
#include "bottombar.h"
#include "leftbar.h"
//----------------------------------------------------
EScene* Scene;
//----------------------------------------------------
void st_Environment::Reset(){
    m_ViewDist		= 500;
    m_Fogness		= 0.9f;
    m_FogColor.set	(0.5f,0.5f,0.5f,0.f);
    m_AmbColor.set	(0,0,0,0);
    m_SkyColor.set	(1,1,1,1);
}

st_Environment::st_Environment(){
	Reset();
}

void st_LevelOptions::WriteToLTX(CInifile* pIni){
	AnsiString temp, ln;
	for (EnvIt e_it=m_Envs.begin(); e_it!=m_Envs.end(); e_it++){
    	ln.sprintf("env%d",e_it-m_Envs.begin());
    	temp.sprintf("%.3f,%.3f,%.3f,  %.3f,%.3f,%.3f,  %.3f,  %.3f,  %.3f,%.3f,%.3f,%.3f",	
        			e_it->m_AmbColor.r,e_it->m_AmbColor.g,e_it->m_AmbColor.b,
        			e_it->m_FogColor.r,e_it->m_FogColor.g,e_it->m_FogColor.b,
                    e_it->m_Fogness, e_it->m_ViewDist,
        			e_it->m_SkyColor.r,e_it->m_SkyColor.g,e_it->m_SkyColor.b, e_it->m_SkyColor.a);
		pIni->WriteString("environment", ln.c_str(), temp.c_str());
    }
}

void EScene::WriteToLTX(CInifile* pIni){
	m_LevelOp.WriteToLTX(pIni);
    if (m_SkyDome){
        AnsiString name = m_SkyDome->GetName();
        name = ChangeFileExt(name,".ogf");
        pIni->WriteString("environment", "sky", name.c_str() );
    }
}

st_LevelOptions::st_LevelOptions(){
	Reset();
}

void st_LevelOptions::InitDefaultText(){
	char deffilename[MAX_PATH];
	int handle;
	m_BOPText="";
	strcpy(deffilename,"default.scenetext");
	FS.m_Config.Update( deffilename );
    char buf[4096];
    if (FS.Exist(deffilename,true)&&FS.FileLength(deffilename)){
	    CFileStream F(deffilename);
		F.RstringZ(buf); m_BOPText = buf;
	}
}

void st_LevelOptions::Reset(){
	m_FNLevelPath	= "level";
	m_LevelName		= "Unnamed place";

    m_Envs.resize	(1);

    m_SkydomeObjName= "";
    InitDefaultText	();

    m_CurEnv		= 0;

	m_DOClusterSize = 8.f;
}
//------------------------------------------------------------------------------

#define MAX_VISUALS 4096

EScene::EScene(){
	m_Valid = false;
	m_Locked = 0;
    for (int i=0; i<OBJCLASS_COUNT; i++){
        ObjectList lst;
        EObjClass cls = (EObjClass)i;
        m_Objects.insert(make_pair(cls,lst));
    }
    // first init scene graph for objects
    mapRenderObjects.init(MAX_VISUALS);
// Build options
    m_BuildParams.Init();
    m_DetailPatches = new CDPatchSystem();
    m_DetailObjects	= new CDetailManager();
    m_SkyDome = 0;
    ClearSnapList();
}

EScene::~EScene(){
	VERIFY( m_Valid == false );
    m_SnapObjects.clear();
}

void EScene::Init(){
	m_LastAvailObject = 0;
    m_LevelOp.Reset();
	Log->Msg( mtInformation, "Scene: initialized" );
	m_Valid = true;
    m_Modified = false;
	UI->Command(COMMAND_UPDATE_CAPTION);
}

void EScene::Clear(){
	Log->Msg( mtInformation, "Scene: cleared" );
	m_LastAvailObject = 0;
	m_Valid = false;
    _DELETE(m_DetailPatches);
    _DELETE(m_DetailObjects);
    _DELETE(m_SkyDome);
}

void EScene::AddObject( SceneObject* object, bool bManual ){
	VERIFY( object );
	VERIFY( m_Valid );
    ObjectList& lst = ListObj(object->ClassID());
    lst.push_back( object );
    UI->UpdateScene();
	if (bManual){
	    object->Select(true);
    	UndoSave();
    }
}

void EScene::RemoveObject( SceneObject* object, bool bUndo ){
	VERIFY( object );
	VERIFY( m_Valid );
    ObjectList& lst = ListObj(object->ClassID());
    // remove object from Snap List if exists
    if ((object->ClassID()==OBJCLASS_EDITOBJECT)&&!m_SnapObjects.empty()) 
    	m_SnapObjects.remove(object);
    // remove from scene list
    lst.remove( object );
    if (object->IsInGroup()) m_Groups[object->GetGroupIndex()].objects.remove(object);
    UI->UpdateScene();
	if (bUndo) UndoSave();
}

bool EScene::ContainsObject( SceneObject* object, EObjClass classfilter ){
	VERIFY( object );
	VERIFY( m_Valid );
    ObjectList& lst = ListObj(classfilter);
    ObjectIt it = find( lst.begin(), lst.end(), object );
    if (it!=lst.end()) return true;
    return false;
}

int EScene::ObjCount(){
	int cnt = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	cnt+=(*it).second.size();
	cnt+=m_DetailPatches->ObjCount(); // добавить патчи
	return cnt;
}

int EScene::FrustumSelect( bool flag, EObjClass classfilter ){
	CFrustum frustum;
    if (!UI->SelectionFrustum(frustum)) return 0;

	if ((classfilter==OBJCLASS_DUMMY)||(classfilter==OBJCLASS_DO)) return m_DetailObjects->FrustumSelect(flag);
    
	int count = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first)){
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if((*_F)->Visible()){
                    if( (*_F)->FrustumPick(frustum, precalc_identity) ){
                        (*_F)->Select( flag );
				        if((*_F)->IsInGroup()&&!fraLeftBar->ebIgnoreGroup->Down)
                        	Scene->GroupSelect((*_F)->GetGroupIndex(),flag,false);
                        count++;
                    }
                }
        }
    }
    UI->RedrawScene();
	return count;
}
int EScene::FrustumPick( const CFrustum& frustum, EObjClass classfilter, ObjectList& ol ){
	int count = 0;
    ObjectList& lst = ListObj(classfilter);
	ObjectIt _F = lst.begin();
	for(;_F!=lst.end();_F++)
		if((*_F)->Visible()){
			if( (*_F)->FrustumPick(frustum, precalc_identity) ){
            	ol.push_back(*_F);
				count++;
			}
		}
	return count;
}

int EScene::SpherePick( const Fvector& center, float radius, EObjClass classfilter, ObjectList& ol ){
	int count = 0;
    ObjectList& lst = ListObj(classfilter);
	ObjectIt _F = lst.begin();
	for(;_F!=lst.end();_F++)
		if((*_F)->Visible()){
			if( (*_F)->SpherePick(center, radius, precalc_identity) ){
            	ol.push_back(*_F);
				count++;
			}
		}
	return count;
}

int EScene::SelectObjects( bool flag, EObjClass classfilter ){
	if ((classfilter==OBJCLASS_DUMMY)||(classfilter==OBJCLASS_DO)) return m_DetailObjects->SelectObjects(flag);

	int count = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
	    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
    	        if((*_F)->Visible()){
        	        (*_F)->Select( flag );
            	    count++;
	            }
    }
    UI->RedrawScene();
	return count;
}

int EScene::LockObjects( bool flag, EObjClass classfilter, bool bAllowSelectionFlag, bool bSelFlag ){
	int count = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if(bAllowSelectionFlag){
                    if((*_F)->Selected()==bSelFlag){
                        (*_F)->Lock( flag );
                        count++;
                    }
                }else{
                    (*_F)->Lock( flag );
                    count++;
                }
    }
    UI->RedrawScene();
	return count;
}

int EScene::ShowObjects( bool flag, EObjClass classfilter, bool bAllowSelectionFlag, bool bSelFlag ){
	int count = 0;
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
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
    }
    UI->RedrawScene();
	return count;
}

int EScene::InvertSelection( EObjClass classfilter ){
    if (classfilter==OBJCLASS_DPATCH) 	return m_DetailPatches->InvertSelection();
    if (classfilter==OBJCLASS_DO)     	return m_DetailObjects->InvertSelection();

	int count = 0;
    if (classfilter==OBJCLASS_DUMMY){
    	count += m_DetailPatches->InvertSelection();
		count += m_DetailObjects->InvertSelection();
    }
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
			for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if((*_F)->Visible()){
                    (*_F)->Select( !(*_F)->Selected() );
                    count++;
                }
    }

    UI->RedrawScene();
	return count;
}

int EScene::RemoveSelection( EObjClass classfilter ){
    if (classfilter==OBJCLASS_DPATCH) 	return m_DetailPatches->RemoveSelection();

	int count = 0;
    if (classfilter==OBJCLASS_DUMMY) count += m_DetailPatches->RemoveSelection();
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first)){
            ObjectIt _F = lst.begin();
            while(_F!=lst.end()){
            	if((*_F)->Selected()){
                    count ++;
                    ObjectIt _D = _F; _F++;
                 	RemoveObject(*_D,false);
                    _DELETE((*_D));
            	}else{
                	_F++;
                }
/*            
                if( (*_F)->Selected() && !(*_F)->Locked() ){
                    count ++;
                    _DELETE((*_F));
                    ObjectIt _D = _F; _F++;
                    lst.remove((*_D));
                }else{
                    _F++;
                }
*/
            }
        }
    }

    UI->UpdateScene(true);
	return count;
}

SceneObject *EScene::RayPick(const Fvector& start, const Fvector& direction, EObjClass classfilter, SRayPickInfo* pinf, bool bDynamicTest, bool bUseSnapList){
	if( !valid() )
		return 0;

	float nearest_dist = flt_max;
	SceneObject *nearest_object = 0;

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		ObjectList* lst=0;
    	if (it->first==OBJCLASS_EDITOBJECT)
        	 lst=(bUseSnapList&&fraLeftBar->ebEnableSnapList->Down&&!m_SnapObjects.empty())?&m_SnapObjects:&(it->second);
        else lst=&(it->second);
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first)){
		    if (classfilter==OBJCLASS_DPATCH){
            	m_DetailPatches->RayPickSelect(nearest_dist,start,direction);
            }else{
            	if (classfilter==OBJCLASS_DO){
	            	m_DetailObjects->RayPickSelect(nearest_dist,start,direction);
    	        }else{
        	        for(ObjectIt _F=lst->begin();_F!=lst->end();_F++){
            	        if((*_F)->Visible()){
                	        if((classfilter==OBJCLASS_EDITOBJECT)&&!bDynamicTest&&((CEditObject*)(*_F))->IsDynamic()) continue;
                    	    if((*_F)->RayPick(nearest_dist,start,direction,precalc_identity,pinf))
                        	    nearest_object = (*_F);
	                    }
                    }
                }
            }
        }
    }
	return nearest_object;
}

int EScene::BoxPick(const Fbox& box, SBoxPickInfoVec& pinf, ObjectList* snap_list){
	ObjectList& lst = (snap_list)?*snap_list:ListObj(OBJCLASS_EDITOBJECT);
	for(ObjectIt _F=lst.begin();_F!=lst.end();_F++)
        ((CEditObject*)*_F)->BoxPick(box,precalc_identity,pinf);
    return pinf.size();
}

int EScene::SelectionCount(bool testflag, EObjClass classfilter){
    if (classfilter==OBJCLASS_DPATCH) 	return m_DetailPatches->SelectionCount(testflag);
    if (classfilter==OBJCLASS_DO)     	return m_DetailObjects->SelectionCount(testflag);

	int count = 0;
    if (classfilter==OBJCLASS_DUMMY){
    	count += m_DetailPatches->SelectionCount(testflag);
		count += m_DetailObjects->SelectionCount(testflag);
    }
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
			for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
    		    if((*_F)->Visible()	&& ((*_F)->Selected() == testflag)) count++;
    }
	return count;
}

//----------------------------------------------------
void __fastcall object_Normal(EScene::mapObject_Node *N)
{
    ((CEditObject*)N->val)->Render( precalc_identity, rpNormal );
}
//----------------------------------------------------
void __fastcall object_AlphaNormal(EScene::mapObject_Node *N)
{
    ((CEditObject*)N->val)->Render( precalc_identity, rpAlphaNormal );
}
//----------------------------------------------------
void __fastcall object_AlphaLast(EScene::mapObject_Node *N)
{
    ((CEditObject*)N->val)->Render( precalc_identity, rpAlphaLast );
}
//----------------------------------------------------

#define RENDER_CLASS_NORMAL(C)\
 	UI->Device.Shader.Set(UI->Device.m_WireShader);\
    UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);\
    _E=LastObj(C); _F=FirstObj(C);\
    for(;_F!=_E;_F++) if((*_F)->Visible()) (*_F)->Render(precalc_identity, rpNormal);

#define RENDER_CLASS_ALPHA(C)\
 	UI->Device.Shader.Set(UI->Device.m_SelectionShader);\
    UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);\
    _E=LastObj(C); _F=FirstObj(C);\
    for(;_F!=_E;_F++) if((*_F)->Visible()) (*_F)->Render(precalc_identity, rpAlphaNormal);

void EScene::Render( Fmatrix *_Camera ){
	if( !valid() )	return;
//	if( locked() )	return;

	SetLights();

    ObjectIt _F,_E;

//	draw sky
	if (m_SkyDome&&fraBottomBar->miDrawSky->Checked){
    	Fmatrix mat;
        st_Environment& E = m_LevelOp.m_Envs[m_LevelOp.m_CurEnv];
        mat.translate(UI->Device.m_Camera.GetPosition());
		UI->Device.SetRS(D3DRS_TEXTUREFACTOR, E.m_SkyColor.get());
    	m_SkyDome->RenderSingle(mat);
    }

// draw lights, sounds, respawn points, pclipper, sector, event
    RENDER_CLASS_NORMAL(OBJCLASS_LIGHT);
    RENDER_CLASS_NORMAL(OBJCLASS_SOUND);
    RENDER_CLASS_NORMAL(OBJCLASS_RPOINT);
    RENDER_CLASS_NORMAL(OBJCLASS_AITPOINT);
    RENDER_CLASS_NORMAL(OBJCLASS_OCCLUDER);
    RENDER_CLASS_NORMAL(OBJCLASS_EVENT);
    RENDER_CLASS_NORMAL(OBJCLASS_SECTOR);
    RENDER_CLASS_NORMAL(OBJCLASS_PS);

// draw objects
	{
    	const Fvector* cam_pos=&UI->Device.m_Camera.GetPosition();
        ObjectList& lst = ListObj(OBJCLASS_EDITOBJECT);
        ObjectIt _E=lst.end(); _F=lst.begin();
        for(;_F!=_E;_F++){
            if( (*_F)->Visible()&& (*_F)->IsRender( precalc_identity ) ){
                CEditObject* _pT = (CEditObject*)(*_F);
                Fmatrix m; _pT->GetFullTransformToWorld(m);
                float distSQ = cam_pos->distance_to_sqr(m.c);
                mapRenderObjects.insertInAnyWay(distSQ,_pT);
            }
        }
    	// render normal objects
	    mapRenderObjects.traverseLR		(object_Normal);
    }
    // draw detail objects (normal)
    m_DetailObjects->Render(rpNormal);

	// draw detail patches
    m_DetailPatches->Render();

    // render normal alpha objects
    mapRenderObjects.traverseRL		(object_AlphaNormal);

    // render snap
    if (fraLeftBar->ebEnableSnapList->Down)
		for(_F=m_SnapObjects.begin();_F!=m_SnapObjects.end();_F++) if((*_F)->Visible()) ((CEditObject*)(*_F))->RenderSelection(precalc_identity);

    // draw detail objects (alpha)
    m_DetailObjects->Render(rpAlphaNormal);

    // draw detail objects (alpha)
    m_DetailObjects->Render(rpAlphaLast);

	// draw PS
    UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
    _F = FirstObj(OBJCLASS_PS);
    _E = LastObj(OBJCLASS_PS);
   	for(;_F!=_E;_F++) if((*_F)->Visible()) (*_F)->Render(precalc_identity, rpAlphaNormal);

	// draw clip planes, glows, event, sectors, portals
	RENDER_CLASS_ALPHA(OBJCLASS_OCCLUDER);
	RENDER_CLASS_ALPHA(OBJCLASS_GLOW);
	RENDER_CLASS_ALPHA(OBJCLASS_EVENT);
	RENDER_CLASS_ALPHA(OBJCLASS_SECTOR);
	RENDER_CLASS_ALPHA(OBJCLASS_PORTAL);

    // render last alpha objects
    mapRenderObjects.traverseRL		(object_AlphaLast);
    mapRenderObjects.clear			();

	ClearLights();
}

void EScene::UpdateSkydome(){
	_DELETE(m_SkyDome);
    if (!m_LevelOp.m_SkydomeObjName.IsEmpty()){
        CLibObject* LO = Lib->SearchObject(m_LevelOp.m_SkydomeObjName.c_str());
        if (!LO){
        	Log->DlgMsg(mtError,"Skydome %s can't find in library.",m_LevelOp.m_SkydomeObjName.c_str());
        	return ;
        }
        CEditObject* O = LO->GetReference();
        if (!O->IsDynamic()){
        	Log->DlgMsg(mtError,"Non-dynamic models can't be used as Skydome.");
        	return ;
        }
        if (O){
            m_SkyDome = new CEditObject("$Sky");
            m_SkyDome->SetRef(O);
        }
    }
}

void EScene::Update( float dT ){
	if( !valid() ) return;
	if( locked() ) return;

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
    		(*_F)->RTL_Update( dT );
    }
}

void EScene::ClearObjects(bool bDestroy){
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if (bDestroy)
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++) delete (*_F);
        lst.clear();
    }
    m_DetailPatches->Clear();
    m_DetailObjects->Clear();
    _DELETE(m_SkyDome);
    ClearSnapList();
}
//----------------------------------------------------

bool EScene::GetBox(Fbox& box, EObjClass classfilter){
	return GetBox(box,ListObj(classfilter));
}
//----------------------------------------------------

bool EScene::GetBox(Fbox& box, ObjectList& lst){
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

void EScene::Modified(){
	m_Modified = true;
    UI->Command(COMMAND_UPDATE_CAPTION);
}

bool EScene::IsModified(){
    return (m_Modified && (ObjCount()||UI->GetEditFileName()[0]));
}

bool EScene::IfModified(){
    if (m_Modified && (ObjCount()||UI->GetEditFileName()[0])){
        int mr = Log->DlgMsg(mtConfirmation, "The scene has been modified. Do you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI->Command(COMMAND_SAVE)) return false; else m_Modified = false; break;
        case mrNo: m_Modified = false; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void EScene::Unload(){
	m_LastAvailObject = 0;
	ClearObjects(true);
}


bool EScene::Validate(bool bNeedMsg, bool bTestPortal){
	if (bTestPortal){
        if (ObjCount(OBJCLASS_SECTOR)<2){
            Log->DlgMsg(mtError,"*ERROR: Can't find 'Sector'.");
            return false;
        }
        if (ObjCount(OBJCLASS_PORTAL)==0){
            Log->DlgMsg(mtError,"*ERROR: Can't find 'Portal'.");
            return false;
        }
		if (!PortalUtils.Validate(false)){
			Log->DlgMsg(mtError,"*ERROR: Scene has non associated face!");
	    	return false;
    	}
    }
    if (ObjCount(OBJCLASS_RPOINT)==0){
    	Log->DlgMsg(mtError,"*ERROR: Can't find 'Respawn Point'.\nPlease add at least one.");
        return false;
    }
    if (ObjCount(OBJCLASS_LIGHT)==0){
    	Log->DlgMsg(mtError,"*ERROR: Can't find 'Light'.\nPlease add at least one.");
        return false;
    }

//	if(!Scene->FindObjectByName(DEFAULT_SECTOR_NAME,OBJCLASS_SECTOR)){
//    	Log->DlgMsg(mtError,"*ERROR: Compute portals before compiling.");
//        return false;
//	}
    if (ObjCount(OBJCLASS_OCCLUDER)){
    	ObjectList& lst = ListObj(OBJCLASS_OCCLUDER);
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
        	if (!(*it)->Valid()){
	    		Log->DlgMsg(mtError,"*ERROR: Can't found 'PClipper'.\nPlease add at least one.");
    	    	return false;
            }
    }
//	if (ObjCount(OBJCLASS_GLOW)==0){
//    	Log->DlgMsg(mtError,"*ERROR: Can't found 'Glow'.\nPlease add at least one.");
//		return false;
//	}
    if (FindDuplicateName()){
    	Log->DlgMsg(mtError,"*ERROR: Found duplicate object name.\nResolve this problem and try again.");
        return false;
    }
    if (bNeedMsg)
    	Log->DlgMsg(mtInformation,"Validation OK!");
    return true;
}

SceneObject* EScene::FindObjectByName( char *name, EObjClass classfilter ){
	ObjectIt _F = FirstObj(classfilter);
    ObjectIt _E = LastObj(classfilter);
	for(;_F!=_E;_F++) if(0==stricmp((*_F)->GetName(),name)) return (*_F);
    return 0;
}

SceneObject* EScene::FindObjectByName( char *name, SceneObject* pass_object ){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	ObjectIt _F = lst.begin();
        ObjectIt _E = lst.end();
    	for(;_F!=_E;_F++){
            if( (0==stricmp((*_F)->GetName(),name)) && (pass_object!=(*_F)) )
                return (*_F);
        }
	}
    return 0;
}

bool EScene::FindDuplicateName(){
// find duplicate name
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            if (FindObjectByName((*_F)->GetName(), *_F)){
//                char buf[1024];
//                GenObjectName((*_F)->ClassID(),buf);
//                strcpy((*_F)->GetName(),buf);
                AnsiString s;
                s.sprintf("Duplicated Object name '%s'!",(*_F)->GetName());
                MessageDlg(s, mtError, TMsgDlgButtons() << mbOK, 0);
                return true;
            }
	}
    return false;
}

void EScene::OnObjectsUpdate(){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	(*_F)->OnSceneUpdate();
	}
}

void EScene::OnDeviceCreate(){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	(*_F)->OnDeviceCreate();
	}
}

void EScene::OnDeviceDestroy(){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	(*_F)->OnDeviceDestroy();
	}
}

int EScene::GetQueryObjects(ObjectList& objset, EObjClass classfilter, int iSel, int iVis, int iLock){
	ObjectIt _F = FirstObj(classfilter);
	ObjectIt _E = LastObj(classfilter);
	for(;_F!=_E;_F++)
    	if(	((iSel==-1)||((*_F)->Selected()==iSel))&&
        	((iVis==-1)||((*_F)->Visible()==iVis))&&
            ((iLock==-1)||((*_F)->Locked()==iLock)))
            	objset.push_back(*_F);
    return objset.size();
}

SceneObject* EScene::GetQueryObject(EObjClass classfilter, int iSel, int iVis, int iLock){
    SceneObject* O=0;
    int cnt=0;
	ObjectIt _F = FirstObj(classfilter);
	ObjectIt _E = LastObj(classfilter);
	for(;_F!=_E;_F++)
    	if(	((iSel==-1)||((*_F)->Selected()==iSel))&&
        	((iVis==-1)||((*_F)->Visible()==iVis))&&
            ((iLock==-1)||((*_F)->Locked()==iLock))){
                cnt++;
            	O=*_F;
            }
    return (cnt==1)?O:0;
}


void EScene::ZoomExtents( BOOL bSel ){
	BOOL bAllCat = fraLeftBar->ebIgnoreTarget->Down;
	Fbox BB;
	Fbox bb;
    BB.set(0,0,0,0,0,0);
    bool bFirstInit = true;
	if (bAllCat){
        for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
            ObjectList& lst = (*it).second;
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if ((*_F)->Visible()&&((bSel&&(*_F)->Selected())||(!bSel)))
                    if ((*_F)->GetBox(bb)) if (bFirstInit){ BB.set(bb); bFirstInit=false; }else{ BB.merge(bb);}
        }
    }else{
    	EObjClass cls = UI->CurrentClassID();
    	ObjectIt _F = FirstObj(cls);
	    ObjectIt _E = LastObj(cls);
    	for(;_F!=_E;_F++)
            if ((*_F)->Visible()&&((bSel&&(*_F)->Selected())||(!bSel)))
				if ((*_F)->GetBox(bb)) if (bFirstInit){ BB.set(bb); bFirstInit=false; }else{ BB.merge(bb);}
    }
    if (!bFirstInit) UI->Device.m_Camera.ZoomExtents(BB);
    else Log->Msg(mtError,"Can't calculate bounding box. Nothing selected or some object unsupported this function.");
}
//--------------------------------------------------------------------------------------------------
void EScene::UpdateGroups(){
	if (m_Groups.size()) m_Groups.clear();
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	if ((*_F)->IsInGroup()) GroupAddItem((*_F)->GetGroupIndex(),*_F,true);
	}
}
//--------------------------------------------------------------------------------------------------
int EScene::GroupGetEmptyIndex(){
	int idx = 0;
	if (m_Groups.size())
    	while (m_Groups.find(idx)!=m_Groups.end()) idx++;
    return idx;
}
//--------------------------------------------------------------------------------------------------
bool EScene::GroupAddItem(int idx, SceneObject* O, bool bLoadMode){
	if (bLoadMode||O->Group(idx)){	
    	m_Groups[idx].objects.push_back(O);
        return true;
    }else{
        Log->DlgMsg(mtError,"Object '%s' already in %d group.",O->GetName(),O->GetGroupIndex());
        return false;
    }
}
//--------------------------------------------------------------------------------------------------
bool EScene::GroupAddItems(int idx, ObjectList& lst){
	bool res;
	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
    	res = GroupAddItem(idx,*it);
        if (!res) return false;
    }
    return true;
}
//--------------------------------------------------------------------------------------------------
int EScene::GroupSelect(int idx, bool flag, bool bClearPrevSel){
    int count=0;
    if (idx<0) return count;
    if (bClearPrevSel) SelectObjects(false);
    if (m_Groups.find(idx)!=m_Groups.end()){
        ObjectList& lst = m_Groups[idx].objects;
		for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
			(*_F)->Select(flag);
            count++;
        }
    }
    return count;
}
//--------------------------------------------------------------------------------------------------
void EScene::GroupCreate(bool bMsg){
	if (fraLeftBar->ebIgnoreGroup->Down){ 
		if (bMsg) Log->DlgMsg(mtError,"Ignore groups checked. Group can't created.");
    	return;
	}
    EObjClass cls = UI->CurrentClassID();
	// validate objects
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
        if ((cls==OBJCLASS_DUMMY)||(cls==it->first)){
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
                if ((*_F)->Visible()&&(*_F)->Selected()&&(*_F)->IsInGroup()){ 
                    if (bMsg) Log->DlgMsg(mtError,"Object '%s' already in group#%d",(*_F)->GetName(),(*_F)->GetGroupIndex());
                    return;
                }
            }
        }
	}
    // calc group index
    int idx = GroupGetEmptyIndex();
    int count=0;
    // set group
    for(it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
        if ((cls==OBJCLASS_DUMMY)||(cls==it->first)){
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
                if((*_F)->Visible()	&& (*_F)->Selected()){
                    GroupAddItem(idx,*_F);
                    count++;
                }
            }
        }
	}
	if (bMsg) Log->DlgMsg(mtInformation,"Group#%d successfully created.\nContain %d object(s)",idx,count);
}
//--------------------------------------------------------------------------------------------------
void EScene::GroupDestroy(){
	if (fraLeftBar->ebIgnoreGroup->Down){ 
		Log->DlgMsg(mtError,"'Ignore groups' checked. Group can't destroy.");
    	return;
    }
    set<int> relevant;
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            if ((*_F)->Selected()&&(*_F)->IsInGroup())
	            relevant.insert((*_F)->GetGroupIndex());
	}
    if (!relevant.empty()){
        for (set<int>::iterator i_it=relevant.begin(); i_it!=relevant.end(); i_it++){
	    	for(ObjectIt _F = m_Groups[*i_it].objects.begin();_F!=m_Groups[*i_it].objects.end();_F++) (*_F)->Ungroup();
            m_Groups.erase(*i_it);
			Log->DlgMsg(mtInformation,"Group#%d destroyed.",*i_it);
        }
    }
}
//--------------------------------------------------------------------------------------------------
void EScene::GroupSave(){
	if (fraLeftBar->ebIgnoreGroup->Down){ 
		Log->DlgMsg(mtError,"'Ignore groups' checked. Group can't be save.");
    	return;
    }
    set<int> relevant;
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
        for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            if ((*_F)->Selected()&&(*_F)->IsInGroup())
                relevant.insert((*_F)->GetGroupIndex());
    }
    if (relevant.empty()){
		Log->DlgMsg(mtError,"Select group before save.");
    	return;
    }
    if (relevant.size()>1){
		Log->DlgMsg(mtError,"Must selected 1 group only.");
    	return;
    }
	AnsiString fn;
	if (FS.GetSaveName(&FS.m_Groups,fn))
    	SaveSelection(OBJCLASS_DUMMY,fn.c_str());
}
//--------------------------------------------------------------------------------------------------

void EScene::GroupUpdateBox(int idx){
	VERIFY(m_Groups.find(idx)!=m_Groups.end());
	st_GroupItem& gi = m_Groups[idx];
    if (gi.dwUpdateFrame==UI->Device.m_Statistic.dwTotalFrame) return;
    
    ObjectList& lst = gi.objects;

    ObjectIt i = lst.begin();
    for(;i!=lst.end();i++) if( (*i)->GetBox(gi.box) ) break;

    for(;i!=lst.end();i++){
        Fbox bb;
        if( (*i)->GetBox(bb) ){
            gi.box.modify( bb.min );
            gi.box.modify( bb.max );
        }
    }
    gi.dwUpdateFrame=UI->Device.m_Statistic.dwTotalFrame;
}

st_GroupItem& EScene::GetGroupItem(int idx){
	VERIFY(m_Groups.find(idx)!=m_Groups.end());
	return m_Groups[idx];
}

void EScene::UngroupAll(){
	int grp_cnt=m_Groups.size();
    if (grp_cnt){
        if (m_Groups.size()) m_Groups.clear();
        for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
            ObjectList& lst = it->second;
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if ((*_F)->IsInGroup()) (*_F)->Ungroup();
        }
		Log->DlgMsg(mtInformation,"'%d' group(s) destroyed.",grp_cnt);
    }else{
		Log->DlgMsg(mtInformation,"Can't find any group in scene.");
    }
}

void EScene::ResetAnimation(){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = it->second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	(*_F)->ResetAnimation();
	}
}

int EScene::AddToSnapList(){
    ObjectList lst;
    int count = GetQueryObjects(lst, OBJCLASS_EDITOBJECT, 1, 1, 0);
    if (count){
    	count = 0;
	    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
    		if (find(m_SnapObjects.begin(),m_SnapObjects.end(),*_F)==m_SnapObjects.end()){
				m_SnapObjects.push_back(*_F);
	            count++;
    	    }
        if (count){
		    UI->RedrawScene();
    		UpdateSnapList();
        }
    }
	return count;
}

int EScene::SetSnapList(){
	ClearSnapList();
	int count = 0;
    ObjectList& lst = ListObj(OBJCLASS_EDITOBJECT);
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        if((*_F)->Visible()&&(*_F)->Selected()){
	        m_SnapObjects.push_back(*_F);
            count++;
        }
    UI->RedrawScene();
    UpdateSnapList();
	return count;
}

void EScene::ClearSnapList(){
    m_SnapObjects.clear();
    UpdateSnapList();
}

void EScene::UpdateSnapList(){
	if (fraLeftBar) fraLeftBar->UpdateSnapList();
}

void EScene::SynchronizeObjects(){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            (*_F)->OnSynchronize();
	}
}

void EScene::OnShowHint(AStringVec& dest){
    SceneObject* obj = RayPick(UI->m_CurrentRStart,UI->m_CurrentRNorm,UI->CurrentClassID(),0,true,false);
    if (obj) obj->OnShowHint(dest);
}


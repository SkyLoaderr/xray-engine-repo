//----------------------------------------------------
// file: Scene.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "Texture.h"
#include "ui_tools.h"
#include "SceneObject.h"
#include "DetailObjects.h"
#include "Sector.h"
#include "Library.h"
#include "xr_ini.h"
#include "bottombar.h"
#include "leftbar.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "PropertiesList.h"
#include "SoundManager.h"
#include "EParticlesObject.h"
#include "ESceneAIMapTools.h"
//----------------------------------------------------
EScene Scene;
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
		pIni->w_string("environment", ln.c_str(), temp.c_str());
    }
}

void EScene::WriteToLTX(CInifile* pIni){
	m_LevelOp.WriteToLTX(pIni);
    if (m_SkyDome){
        AnsiString name = m_SkyDome->Name;
        name = ChangeFileExt(name,".ogf");
        pIni->w_string("environment", "sky", name.c_str() );
    }
}

st_LevelOptions::st_LevelOptions(){
	Reset();
}

void st_LevelOptions::InitDefaultText(){
	char deffilename[MAX_PATH];
	int handle;
	m_BOPText="";
}

void st_LevelOptions::Reset(){
	m_FNLevelPath	= "level";
	m_LevelName		= "Unnamed place";

    m_Envs.resize	(1);

    m_SkydomeObjName= "";

    InitDefaultText	();

    m_CurEnv		= 0;

	m_DOClusterSize = 8.f;

    m_BuildParams.Init();
}
//------------------------------------------------------------------------------

#define MAX_VISUALS 16384

EScene::EScene()
{
	m_Valid = false;
	m_Locked = 0;
    for (int i=0; i<OBJCLASS_COUNT; i++){
        ObjectList lst;
        EObjClass cls = (EObjClass)i;
        m_Objects.insert(mk_pair(cls,lst));
        m_SceneTools.insert(mk_pair(cls,(ESceneCustomMTools*)NULL));
    }
    // first init scene graph for objects
    mapRenderObjects.init(MAX_VISUALS);
// Build options
    m_SkyDome 		= 0;
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
    m_SceneTools[OBJCLASS_DO] 	= xr_new<EDetailManager>();
    m_SceneTools[OBJCLASS_AIMAP]= xr_new<ESceneAIMapTools>();
	m_LastAvailObject 		= 0;
    m_LevelOp.Reset			();
	ELog.Msg				( mtInformation, "Scene: initialized" );
	m_Valid 				= true;
    m_Modified 				= false;
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
    xr_delete				(m_SkyDome);

    for (int i=0; i<OBJCLASS_COUNT; i++)
    	xr_delete			(m_SceneTools[(EObjClass)i]);
    m_SceneTools.clear		();
}

void EScene::AddObject( CCustomObject* object, bool bUndo ){
	VERIFY( object );
	VERIFY( m_Valid );
    ObjectList& lst = ListObj(object->ClassID);
    lst.push_back( object );
    UI.UpdateScene();
	if (bUndo){
	    object->Select(true);
    	UndoSave();
    }
}

bool EScene::RemoveObject( CCustomObject* object, bool bUndo )
{
	VERIFY( object );
	VERIFY( m_Valid );
    ObjectList& lst = ListObj(object->ClassID);
    // remove object from Snap List if exists
    if (object->ClassID==OBJCLASS_SCENEOBJECT){
    	// remove froom snap list 
    	m_ESO_SnapObjects.remove			(object);
        for (int i=0; i<OBJCLASS_COUNT; i++){
            ESceneCustomMTools* mt = m_SceneTools[(EObjClass)i];
            if (mt) mt->OnObjectRemove(object);
        }
        
		UpdateSnapList						();
        
        // signal everyone "I'm deleting"
	    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	    ObjectList& lst = (*it).second;
			for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            	(*_F)->OnObjectRemove(object);
        }
    }
    // remove from scene list
    lst.remove		(object);
    UI.UpdateScene	();
	if (bUndo) 		UndoSave();
}

bool EScene::ContainsObject( CCustomObject* object, EObjClass classfilter ){
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
	return cnt;
}

int EScene::FrustumSelect( int flag, EObjClass classfilter ){
	CFrustum frustum;
	int count = 0;
    if (!UI.SelectionFrustum(frustum)) return 0;
                                                                              
	if (classfilter==OBJCLASS_DO) 		return GetMTools(OBJCLASS_DO)->FrustumSelect(flag);
	if (classfilter==OBJCLASS_AIMAP)	return GetMTools(OBJCLASS_AIMAP)->FrustumSelect(flag);

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if((*_F)->Visible()&&(*_F)->FrustumSelect(flag,frustum))
                	count++;
    }
    UI.RedrawScene();
	return count;
}
int EScene::FrustumPick( const CFrustum& frustum, EObjClass classfilter, ObjectList& ol ){
	int count = 0;
    ObjectList& lst = ListObj(classfilter);
	ObjectIt _F = lst.begin();
	for(;_F!=lst.end();_F++)
		if((*_F)->Visible()){
			if( (*_F)->FrustumPick(frustum) ){
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
			if( (*_F)->SpherePick(center, radius) ){
            	ol.push_back(*_F);
				count++;
			}
		}
	return count;
}

int EScene::SelectObjects( bool flag, EObjClass classfilter ){
	int count = 0;
	if (classfilter==OBJCLASS_DO) 		return GetMTools(OBJCLASS_DO)->SelectObjects(flag);
	if (classfilter==OBJCLASS_AIMAP)	return GetMTools(OBJCLASS_AIMAP)->SelectObjects(flag);

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
	    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
    	        if((*_F)->Visible()){
        	        (*_F)->Select( flag );
            	    count++;
	            }
    }
    UI.RedrawScene();
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
    UI.RedrawScene();
	return count;
}

int EScene::ShowObjects( bool flag, EObjClass classfilter, bool bAllowSelectionFlag, bool bSelFlag )
{
	int count = 0;
	if (classfilter==OBJCLASS_AIMAP)	return GetMTools(OBJCLASS_AIMAP)->ShowObjects(flag,bAllowSelectionFlag,bSelFlag);
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
    UI.RedrawScene();
	return count;
}

int EScene::InvertSelection( EObjClass classfilter ){
	int count = 0;
    if (classfilter==OBJCLASS_DO)		return GetMTools(OBJCLASS_DO)->InvertSelection	();
	if (classfilter==OBJCLASS_AIMAP)	return GetMTools(OBJCLASS_AIMAP)->InvertSelection();

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
			for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if((*_F)->Visible()){
                    (*_F)->Select(-1);
                    count++;
                }
    }

    UI.RedrawScene();
	return count;
}

int EScene::RemoveSelection( EObjClass classfilter )
{
	int count = 0;
    ESceneCustomMTools* mt = GetMTools(classfilter);
	if (mt)	return mt->RemoveSelection();
    
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first)){
            ObjectIt _F = lst.begin();
            while(_F!=lst.end()){
            	if((*_F)->Selected()){
                    count ++;
                    if ((*_F)->OnSelectionRemove()){
	                    ObjectIt _D = _F; _F++;
    	             	RemoveObject(*_D,false);
        	            xr_delete((*_D));
                    }else{
                    	_F++;
                    }
            	}else{
                	_F++;
                }
            }
        }
    }

    UI.UpdateScene(true);
	return count;
}

int EScene::RayQuery(SPickQuery& PQ, const Fvector& start, const Fvector& dir, float dist, u32 flags, ObjectList* snap_list)
{
    VERIFY			(snap_list);
    PQ.prepare_rq	(start,dir,dist,flags);
	XRC.ray_options	(flags);
    for(ObjectIt _F=snap_list->begin();_F!=snap_list->end();_F++)
        ((CSceneObject*)(*_F))->RayQuery(PQ);
	return PQ.r_count();
}

int EScene::BoxQuery(SPickQuery& PQ, const Fbox& bb, u32 flags, ObjectList* snap_list)
{
    VERIFY			(snap_list);
    PQ.prepare_bq	(bb,flags);
	XRC.box_options	(flags);
    for(ObjectIt _F=snap_list->begin();_F!=snap_list->end();_F++)
        ((CSceneObject*)(*_F))->BoxQuery(PQ);
	return PQ.r_count();
}

int EScene::RayQuery(SPickQuery& PQ, const Fvector& start, const Fvector& dir, float dist, u32 flags, CDB::MODEL* model)
{
    PQ.prepare_rq	(start,dir,dist,flags);
	XRC.ray_options	(flags);
    XRC.ray_query	(model,start,dir,dist);
    for (int r=0; r<XRC.r_count(); r++)
        PQ.append	(model,XRC.r_begin()+r);
	return PQ.r_count();
}

int EScene::BoxQuery(SPickQuery& PQ, const Fbox& bb, u32 flags, CDB::MODEL* model)
{
    PQ.prepare_bq	(bb,flags);
	XRC.box_options	(flags);
    Fvector c,d;
    bb.getcenter	(c);
    bb.getradius	(d);
    XRC.box_query	(model,c,d);
    for (int r=0; r<XRC.r_count(); r++)
        PQ.append	(model,XRC.r_begin()+r);
	return PQ.r_count();
}

CCustomObject *EScene::RayPick(const Fvector& start, const Fvector& direction, EObjClass classfilter, SRayPickInfo* pinf, bool bDynamicTest, ObjectList* snap_list)
{
	if( !valid() )
		return 0;

	float nearest_dist = pinf?pinf->inf.range+EPS_L:flt_max;
	CCustomObject *nearest_object = 0;

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		ObjectList* lst=0;
    	if (it->first==OBJCLASS_SCENEOBJECT)
        	 lst=(snap_list)?snap_list:&(it->second);
        else lst=&(it->second);
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first)){
            if (classfilter==OBJCLASS_DO){
                GetMTools(OBJCLASS_DO)->RaySelect(true,nearest_dist,start,direction);
            }else if (classfilter==OBJCLASS_AIMAP){
				GetMTools(OBJCLASS_AIMAP)->RaySelect(true,nearest_dist,start,direction);
            }else{
                for(ObjectIt _F=lst->begin();_F!=lst->end();_F++){
                    if((*_F)->Visible()){
                        if((classfilter==OBJCLASS_SCENEOBJECT)&&!bDynamicTest&&((CSceneObject*)(*_F))->IsDynamic()) continue;
                        if((*_F)->RayPick(nearest_dist,start,direction,pinf))
                            nearest_object = (*_F);
                    }
                }
            }
        }
    }
	return nearest_object;
}

int EScene::RaySelect(int flag, EObjClass classfilter, bool bOnlyNearest){
	if( !valid() ) return 0;

	float nearest_dist = UI.ZFar();
	CCustomObject *nearest_object = 0;
    int count=0;

	Fvector& start		= UI.m_CurrentRStart;
    Fvector& direction	= UI.m_CurrentRNorm;

	if (classfilter==OBJCLASS_DO) 		return GetMTools(OBJCLASS_DO)->RaySelect(flag,nearest_dist,start,direction);
	if (classfilter==OBJCLASS_AIMAP)	return GetMTools(OBJCLASS_AIMAP)->RaySelect(flag,nearest_dist,start,direction);

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if((*_F)->Visible()&&(*_F)->RayPick(nearest_dist,start,direction)){
                	nearest_object=*_F;
                    count++;
            	}
    }
    if (nearest_object) nearest_object->RaySelect(flag,start,direction);
    UI.RedrawScene();
	return count;
}

int EScene::BoxPick(const Fbox& box, SBoxPickInfoVec& pinf, ObjectList* snap_list){
	ObjectList& lst = (snap_list)?*snap_list:ListObj(OBJCLASS_SCENEOBJECT);
	for(ObjectIt _F=lst.begin();_F!=lst.end();_F++)
        ((CSceneObject*)*_F)->BoxPick(box,pinf);
    return pinf.size();
}

int EScene::SelectionCount(bool testflag, EObjClass classfilter){
	int count = 0;
    if (classfilter==OBJCLASS_DO)		return GetMTools(OBJCLASS_DO)->SelectionCount(testflag);
	if (classfilter==OBJCLASS_AIMAP)	return GetMTools(OBJCLASS_AIMAP)->SelectionCount(testflag);

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==(*it).first))
			for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
    		    if((*_F)->Visible()	&& ((*_F)->Selected() == testflag)) count++;
    }
	return count;
}

//----------------------------------------------------
void __fastcall object_Normal_0(EScene::mapObject_Node *N){ ((CSceneObject*)N->val)->Render( 0, false ); }
void __fastcall object_Normal_1(EScene::mapObject_Node *N){ ((CSceneObject*)N->val)->Render( 1, false ); }
void __fastcall object_Normal_2(EScene::mapObject_Node *N){ ((CSceneObject*)N->val)->Render( 2, false ); }
void __fastcall object_Normal_3(EScene::mapObject_Node *N){ ((CSceneObject*)N->val)->Render( 3, false ); }
//----------------------------------------------------
void __fastcall object_StrictB2F_0(EScene::mapObject_Node *N){((CSceneObject*)N->val)->Render( 0, true );}
void __fastcall object_StrictB2F_1(EScene::mapObject_Node *N){((CSceneObject*)N->val)->Render( 1, true );}
void __fastcall object_StrictB2F_2(EScene::mapObject_Node *N){((CSceneObject*)N->val)->Render( 2, true );}
void __fastcall object_StrictB2F_3(EScene::mapObject_Node *N){((CSceneObject*)N->val)->Render( 3, true );}
//----------------------------------------------------

#define RENDER_CLASS(P,C,B2F)\
    _E=LastObj(C); _F=FirstObj(C);\
    for(;_F!=_E;_F++) if((*_F)->Visible()) (*_F)->Render(P,B2F);

#define RENDER_CLASS_NORMAL(P,C)\
 	Device.SetShader(Device.m_WireShader);\
 	RCache.set_xform_world(Fidentity);\
    _E=LastObj(C); _F=FirstObj(C);\
    for(;_F!=_E;_F++) if((*_F)->Visible()) (*_F)->Render(P,false);

#define RENDER_CLASS_ALPHA(P,C)\
 	Device.SetShader(Device.m_SelectionShader);\
 	RCache.set_xform_world(Fidentity);\
    _E=LastObj(C); _F=FirstObj(C);\
    for(;_F!=_E;_F++) if((*_F)->Visible()) (*_F)->Render(P,true);

void EScene::RenderSky(const Fmatrix& camera)
{
	if( !valid() )	return;
//	draw sky
	if (m_SkyDome&&fraBottomBar->miDrawSky->Checked){
        st_Environment& E = m_LevelOp.m_Envs[m_LevelOp.m_CurEnv];
        m_SkyDome->PPosition = camera.c;
        m_SkyDome->UpdateTransform(true);
		Device.SetRS(D3DRS_TEXTUREFACTOR, E.m_SkyColor.get());
    	m_SkyDome->RenderSingle();
	    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    }
}

void EScene::Render( const Fmatrix& camera )
{
	if( !valid() )	return;
//	if( locked() )	return;

	SetLights();

    ObjectIt _F,_E;

	// sort objects
    const Fvector& cam_pos=Device.m_Camera.GetPosition();
    ObjectList& lst = ListObj(OBJCLASS_SCENEOBJECT);
    _E=lst.end(); _F=lst.begin();
    for(;_F!=_E;_F++){
        if( (*_F)->Visible()&& (*_F)->IsRender() ){
            CSceneObject* _pT = (CSceneObject*)(*_F);
            Fmatrix m; _pT->GetFullTransformToWorld(m);
            float distSQ = cam_pos.distance_to_sqr(m.c);
            mapRenderObjects.insertInAnyWay(distSQ,_pT);
        }
    }
// priority #0
    // render normal objects
    mapRenderObjects.traverseLR		(object_Normal_0);
    mapRenderObjects.traverseRL		(object_StrictB2F_0);
	RENDER_CLASS			(0,OBJCLASS_GROUP,		false);

    // draw detail objects (normal)
    GetMTools(OBJCLASS_DO)->OnRender	(0,false);
	RENDER_CLASS			(0,OBJCLASS_GROUP,		true);

// priority #1
    mapRenderObjects.traverseLR		(object_Normal_1);
	// draw lights, sounds, respawn points, pclipper, sector, event
    RENDER_CLASS_NORMAL		(1,OBJCLASS_LIGHT);
    RENDER_CLASS			(1,OBJCLASS_SOUND_SRC,	false);
    RENDER_CLASS			(1,OBJCLASS_SOUND_ENV,	false);
    RENDER_CLASS_NORMAL		(1,OBJCLASS_SPAWNPOINT);
    RENDER_CLASS_NORMAL		(1,OBJCLASS_WAY);
    RENDER_CLASS			(1,OBJCLASS_SHAPE,		false);
    RENDER_CLASS_NORMAL		(1,OBJCLASS_PS);
	RENDER_CLASS_NORMAL		(1,OBJCLASS_PORTAL);
	RENDER_CLASS			(1,OBJCLASS_GROUP,		false);
    GetMTools(OBJCLASS_DO)->OnRender		(1,false);
    GetMTools(OBJCLASS_AIMAP)->OnRender		(1,false);

    mapRenderObjects.traverseRL(object_StrictB2F_1);
    GetMTools(OBJCLASS_DO)->OnRender		(1,true);
    GetMTools(OBJCLASS_AIMAP)->OnRender		(1,true);
	// draw clip planes, glows, event, sectors, portals
	RENDER_CLASS_ALPHA		(1,OBJCLASS_LIGHT);
    RENDER_CLASS			(1,OBJCLASS_SOUND_SRC,	true);
    RENDER_CLASS			(1,OBJCLASS_SOUND_ENV,	true);
	RENDER_CLASS_ALPHA		(1,OBJCLASS_GLOW);
    RENDER_CLASS			(1,OBJCLASS_SHAPE,		true);
	RENDER_CLASS			(1,OBJCLASS_GROUP,		true);
    RENDER_CLASS			(1,OBJCLASS_SPAWNPOINT,	true);

// priority #2
    mapRenderObjects.traverseLR(object_Normal_2);
    GetMTools(OBJCLASS_DO)->OnRender(2,				false);
    RENDER_CLASS_NORMAL		(2,OBJCLASS_SECTOR);
	RENDER_CLASS			(2,OBJCLASS_GROUP,		false);
    mapRenderObjects.traverseRL(object_StrictB2F_2);
    GetMTools(OBJCLASS_DO)->OnRender(2,				true);
	RENDER_CLASS_ALPHA		(2,OBJCLASS_SECTOR);
	RENDER_CLASS			(2,OBJCLASS_GROUP,		true);

// priority #3
    mapRenderObjects.traverseLR(object_Normal_3);
    GetMTools(OBJCLASS_DO)->OnRender(3,				false);
	RENDER_CLASS			(3,OBJCLASS_GROUP,		false);
    mapRenderObjects.traverseRL(object_StrictB2F_3);
    GetMTools(OBJCLASS_DO)->OnRender(3,				true);
	RENDER_CLASS			(3,OBJCLASS_GROUP,		true);

	// draw lights (flares)
    RENDER_CLASS			(3,OBJCLASS_LIGHT,		true);

    // render snap
    RenderSnapList			();

	// draw PS
    RCache.set_xform_world(Fidentity);
    _F = FirstObj(OBJCLASS_PS);
    _E = LastObj(OBJCLASS_PS);
   	for(;_F!=_E;_F++)
    	if((*_F)->Visible()){
        	(*_F)->Render(1,false);
			(*_F)->Render(1,true);
        }

    // draw compiler errors
	if (1){
	 	Device.SetShader		(Device.m_SelectionShader);
 		RCache.set_xform_world	(Fidentity);
        Device.RenderNearer		(0.0003f);
		Device.SetRS			(D3DRS_CULLMODE,D3DCULL_NONE);
        AnsiString temp;
        int cnt=0;
        for (ERR::VertIt vit=m_CompilerErrors.m_TJVerts.begin(); vit!=m_CompilerErrors.m_TJVerts.end(); vit++){
        	temp.sprintf		("TJ: %d",cnt++);
        	DU.dbgDrawVert(vit->p[0],						0xff0000ff,	temp.c_str());
        }
        cnt=0;
        for (ERR::EdgeIt eit=m_CompilerErrors.m_MultiEdges.begin(); eit!=m_CompilerErrors.m_MultiEdges.end(); eit++){
        	temp.sprintf		("ME: %d",cnt++);
        	DU.dbgDrawEdge(eit->p[0],eit->p[1],			0xff00ff00,	temp.c_str());
        }
        cnt=0;
        for (ERR::FaceIt fit=m_CompilerErrors.m_InvalidFaces.begin(); fit!=m_CompilerErrors.m_InvalidFaces.end(); fit++){
        	temp.sprintf		("IF: %d",cnt++);
        	DU.dbgDrawFace(fit->p[0],fit->p[1],fit->p[2],	0xffff0000,	temp.c_str());
        }
	    Device.SetRS			(D3DRS_CULLMODE,D3DCULL_CCW);
        Device.ResetNearer		();
	}

    mapRenderObjects.clear			();

	ClearLights();
}

void EScene::UpdateSkydome()
{
	xr_delete(m_SkyDome);
    if (!m_LevelOp.m_SkydomeObjName.IsEmpty()){
        m_SkyDome = xr_new<CSceneObject>((LPVOID)0,"$sky");
        CEditableObject* EO = m_SkyDome->SetReference(m_LevelOp.m_SkydomeObjName.c_str());
        if (!EO){
        	ELog.DlgMsg(mtError,"Object %s can't find in library.",m_LevelOp.m_SkydomeObjName.c_str());
            xr_delete(m_SkyDome);
        }
    }
}

void EScene::OnFrame( float dT )
{
	if( !valid() ) return;
	if( locked() ) return;

    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
    		(*_F)->OnFrame();
    }
    GetMTools(OBJCLASS_AIMAP)->OnFrame();
}

void EScene::ClearObjects(bool bDestroy){
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
        if (bDestroy)
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++) delete (*_F);
        lst.clear();
    }
    GetMTools(OBJCLASS_DO)->Clear	();
    GetMTools(OBJCLASS_AIMAP)->Clear();

    xr_delete				(m_SkyDome);
    ClearSnapList			(false);
    m_CompilerErrors.Clear	();
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
    UI.Command(COMMAND_UPDATE_CAPTION);
}

bool EScene::IsModified()
{
    return (m_Modified && (ObjCount()||UI.GetEditFileName()[0]));
}

bool EScene::IfModified()
{
	if (locked()){ 
        ELog.DlgMsg( mtError, "Scene sharing violation" );
        return false;
    }
    if (m_Modified && (ObjCount()||UI.GetEditFileName()[0])){
        int mr = ELog.DlgMsg(mtConfirmation, "The scene has been modified. Do you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else m_Modified = false; break;
        case mrNo: m_Modified = false; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void EScene::Unload()
{
	m_LastAvailObject = 0;
	ClearObjects(true);
	if (m_SummaryInfo) m_SummaryInfo->HideProperties();
}


bool EScene::Validate(bool bNeedOkMsg, bool bTestPortal, bool bTestHOM, bool bTestGlow, bool bTestShaderCompatible)
{
	bool bRes = true;
	if (bTestPortal){
		if (!PortalUtils.Validate(false)){
			ELog.Msg(mtError,"*ERROR: Scene has non associated face!");
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
            if (!S->GetReference()){
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

void EScene::OnObjectsUpdate(){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	(*_F)->OnSceneUpdate();
	}
}

void EScene::OnDeviceCreate()
{
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	(*_F)->OnDeviceCreate();
	}
    GetMTools(OBJCLASS_AIMAP)->OnDeviceCreate();
    GetMTools(OBJCLASS_DO)->OnDeviceCreate();
}

void EScene::OnDeviceDestroy()
{
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	(*_F)->OnDeviceDestroy();
	}
    GetMTools(OBJCLASS_AIMAP)->OnDeviceDestroy();
    GetMTools(OBJCLASS_DO)->OnDeviceDestroy();
}

int EScene::GetQueryObjects(ObjectList& objset, EObjClass classfilter, int iSel, int iVis, int iLock){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = it->second;
        if ((classfilter==OBJCLASS_DUMMY)||(classfilter==it->first)){
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
                if(	((iSel==-1)||((*_F)->Selected()==iSel))&&
                    ((iVis==-1)||((*_F)->Visible()==iVis))&&
                    ((iLock==-1)||((*_F)->Locked()==iLock))){
                        objset.push_back(*_F);
                }
            }
        }
    }
    return objset.size();
}

void EScene::ZoomExtents( BOOL bSel )
{
	EObjClass cls = Tools.CurrentClassID();
	Fbox BB;	BB.invalidate();
	Fbox bb;
	if (cls==OBJCLASS_DUMMY){
        for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
            ObjectList& lst = (*it).second;
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if ((*_F)->Visible()&&((bSel&&(*_F)->Selected())||(!bSel)))
                    if ((*_F)->GetBox(bb)) BB.merge(bb);
        }
    }else{
    	ObjectIt _F = FirstObj(cls);
	    ObjectIt _E = LastObj(cls);
    	for(;_F!=_E;_F++)
            if ((*_F)->Visible()&&((bSel&&(*_F)->Selected())||(!bSel)))
				if ((*_F)->GetBox(bb)) BB.merge(bb);
    }
    if (BB.is_valid()) Device.m_Camera.ZoomExtents(BB);
    else ELog.Msg(mtError,"Can't calculate bounding box. Nothing selected or some object unsupported this function.");
}
//--------------------------------------------------------------------------------------------------

void EScene::ResetAnimation(){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = it->second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
        	(*_F)->ResetAnimation();
	}
}
//--------------------------------------------------------------------------------------------------

void EScene::SynchronizeObjects()
{
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            (*_F)->OnSynchronize();
	}
    GetMTools(OBJCLASS_AIMAP)->OnSynchronize();
}
//--------------------------------------------------------------------------------------------------

void EScene::OnShowHint(AStringVec& dest)
{
    CCustomObject* obj = RayPick(UI.m_CurrentRStart,UI.m_CurrentRNorm,Tools.CurrentClassID(),0,true,0);
    if (obj) obj->OnShowHint(dest);
}
//--------------------------------------------------------------------------------------------------

static SSceneSummary s_summary;
void EScene::ShowSummaryInfo()
{
	s_summary.Clear	();
	bool bRes=false;
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            if ((*_F)->GetSummaryInfo(&s_summary)) bRes=true;
        }
	}
    // append sky dome
	if (m_SkyDome) m_SkyDome->GetSummaryInfo(&s_summary);

	PropItemVec items;
	if (bRes){
        // fill items
        s_summary.FillProp(items);
        m_SummaryInfo->ShowProperties();
    }else{
    	ELog.DlgMsg(mtInformation,"Summary info empty.");
    }
	m_SummaryInfo->AssignItems(items,false,"Level Summary Info");
}
//--------------------------------------------------------------------------------------------------



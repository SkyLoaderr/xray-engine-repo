//----------------------------------------------------
// file: rpoint.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "spawnpoint.h"
#include "xr_ini.h"
#include "clsid_game.h"
#include "d3dutils.h"
#include "render.h"
#include "gamefont.h"
#include "bottombar.h"
#include "scene.h"
#include "xrServer_Objects_ALife_All.h"
#include "SkeletonCustom.h"
#include "ESceneSpawnTools.h"
#include "ObjectAnimator.h"

#include "eshape.h"
#include "sceneobject.h"

#define SPAWNPOINT_VERSION   			0x0014
//----------------------------------------------------
#define SPAWNPOINT_CHUNK_VERSION		0xE411
#define SPAWNPOINT_CHUNK_POSITION		0xE412
#define SPAWNPOINT_CHUNK_TEAMID			0xE413
#define SPAWNPOINT_CHUNK_DIRECTION		0xE414
#define SPAWNPOINT_CHUNK_SQUADID		0xE415
#define SPAWNPOINT_CHUNK_GROUPID		0xE416
#define SPAWNPOINT_CHUNK_TYPE			0xE417
#define SPAWNPOINT_CHUNK_FLAGS			0xE418

#define SPAWNPOINT_CHUNK_ENTITYREF		0xE419
#define SPAWNPOINT_CHUNK_SPAWNDATA		0xE420

#define SPAWNPOINT_CHUNK_ATTACHED_OBJ	0xE421

//----------------------------------------------------
#define MAX_TEAM 6
const u32 RP_COLORS[MAX_TEAM]={0xff0000,0x00ff00,0x0000ff,0xffff00,0x00ffff,0xff00ff};

ShaderMap CSpawnPoint::m_Icons;
//----------------------------------------------------

void CSpawnPoint::SSpawnData::Create(LPCSTR entity_ref)
{
    m_Data 	= F_entity_Create(entity_ref);

    CSE_Shape* cform = dynamic_cast<CSE_Shape*>(m_Data);
//    if (cform) cform->shapes.push_back(xrSE_CFormed::shape_def());
    if (cform){
    	cform->shapes.reserve		(128);
//	   	cform->shapes.push_back		(xrSE_CFormed::shape_def());
//	   	cform->shapes.push_back		(xrSE_CFormed::shape_def());
//	   	cform->shapes.push_back		(xrSE_CFormed::shape_def());
    }

    if (m_Data){
        if (pSettings->line_exist(entity_ref,"$player")){
            if (pSettings->r_bool(entity_ref,"$player"))
                m_Data->s_flags.set(M_SPAWN_OBJECT_ASPLAYER,TRUE);
        }
        m_ClassID = pSettings->r_clsid(entity_ref,"class");
        strcpy(m_Data->s_name,entity_ref);
    }
}
void CSpawnPoint::SSpawnData::Destroy()
{
    F_entity_Destroy(m_Data);
}
void CSpawnPoint::SSpawnData::Save(IWriter& F)
{
    F.open_chunk		(SPAWNPOINT_CHUNK_ENTITYREF);
    F.w_stringZ			(m_Data->s_name);
    F.close_chunk		();

    F.open_chunk		(SPAWNPOINT_CHUNK_SPAWNDATA);
    NET_Packet 			Packet;
    m_Data->Spawn_Write	(Packet,TRUE);
    F.w_u32				(Packet.B.count);
    F.w					(Packet.B.data,Packet.B.count);
    F.close_chunk		();
}
bool CSpawnPoint::SSpawnData::Load(IReader& F)
{
    string64 			temp;
    F.r_stringZ			(temp);

    NET_Packet 			Packet;
    R_ASSERT(F.find_chunk(SPAWNPOINT_CHUNK_SPAWNDATA));
    Packet.B.count 		= F.r_u32();
    F.r					(Packet.B.data,Packet.B.count);
    Create				(temp);
    if (Valid())
    	if (!m_Data->Spawn_Read(Packet))
        	Destroy		();

    return Valid();
}
bool CSpawnPoint::SSpawnData::ExportGame(SExportStreams& F, CSpawnPoint* owner)
{
	// set params
    strcpy	  					(m_Data->s_name_replace,owner->Name);
    m_Data->o_Position.set		(owner->PPosition);
    m_Data->o_Angle.set			(owner->PRotation);

    // export cform (if needed)
    CSE_Shape* cform 			= dynamic_cast<CSE_Shape*>(m_Data);
// SHAPE
    if (cform&&!(owner->m_AttachedObject&&(owner->m_AttachedObject->ClassID==OBJCLASS_SHAPE))){
		ELog.DlgMsg				(mtError,"Spawn Point: '%s' must contain attached shape.",owner->Name);
    	return false;
    }
    if (cform){
	    CEditShape* shape		= dynamic_cast<CEditShape*>(owner->m_AttachedObject); R_ASSERT(shape);
		shape->ApplyScale		();
    	cform->shapes 			= shape->GetShapes();
    }
    // end

    NET_Packet					Packet;
    m_Data->Spawn_Write			(Packet,TRUE);

    F.spawn.stream.open_chunk	(F.spawn.chunk++);
    F.spawn.stream.w			(Packet.B.data,Packet.B.count);
    F.spawn.stream.close_chunk	();

    return true;
}
void CSpawnPoint::SSpawnData::FillProp(LPCSTR pref, PropItemVec& items)
{
	m_Data->FillProp	(pref,items);
}
void CSpawnPoint::SSpawnData::Render(bool bSelected, const Fmatrix& parent,int priority, bool strictB2F)
{
    CSE_Visual* V				= dynamic_cast<CSE_Visual*>(m_Data);
	if (V&&V->visual)			::Render->model_Render(V->visual,parent,priority,strictB2F,1.f);
    if (bSelected&&(1==priority)&&(false==strictB2F)){
        CSE_Motion* M			= dynamic_cast<CSE_Motion*>(m_Data);
        if (M&&M->animator)		M->animator->DrawPath();
    }
}
void CSpawnPoint::SSpawnData::OnFrame()
{
    CSE_Visual* V				= dynamic_cast<CSE_Visual*>(m_Data);
	if (V&&V->visual&&PKinematics(V->visual)) PKinematics(V->visual)->Calculate();
    CSE_Motion* M				= dynamic_cast<CSE_Motion*>(m_Data);
	if (M&&M->animator)			M->animator->OnFrame();
}
void CSpawnPoint::SSpawnData::OnDeviceCreate()
{
    // if visualed
    CSE_Visual* V				= dynamic_cast<CSE_Visual*>(m_Data);
	if (V)						V->OnChangeVisual(0);
}
//----------------------------------------------------

void CSpawnPoint::SSpawnData::OnDeviceDestroy()
{
    // if visualed
    CSE_Visual* V				= dynamic_cast<CSE_Visual*>(m_Data);
	if (V&&V->visual)			::Render->model_Delete(V->visual);
}
//----------------------------------------------------
//------------------------------------------------------------------------------
CSpawnPoint::CSpawnPoint(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct(data);
}

void CSpawnPoint::Construct(LPVOID data){
	ClassID			= OBJCLASS_SPAWNPOINT;
    m_dwTeamID		= 0;
    m_AttachedObject= 0;
    m_Type			= ptRPoint;
    if (data){
	    CreateSpawnData(LPCSTR(data));
    	if (!m_SpawnData.Valid()){
    		if (strcmp(LPSTR(data),RPOINT_CHOOSE_NAME)==0){
	        	m_Type = ptRPoint;
    	    }else{
        		SetValid(false);
	        }
        }
    }else{
		SetValid(false);
    }
}

CSpawnPoint::~CSpawnPoint()
{
	xr_delete(m_AttachedObject);
    OnDeviceDestroy();
}

void CSpawnPoint::Select(int  flag)
{
	inherited::Select(flag);
    if (m_AttachedObject) m_AttachedObject->Select(flag);
}

void CSpawnPoint::SetPosition(const Fvector& pos)
{
	inherited::SetPosition	(pos);
    if (m_AttachedObject) m_AttachedObject->PPosition = pos;
}
void CSpawnPoint::SetRotation(const Fvector& rot)
{
	inherited::SetRotation	(rot);
    if (m_AttachedObject) m_AttachedObject->PRotation = rot;
}
void CSpawnPoint::SetScale(const Fvector& scale)
{
	inherited::SetScale		(scale);
    if (m_AttachedObject) m_AttachedObject->PScale = scale;
}

bool CSpawnPoint::AttachObject(CCustomObject* obj)
{
	bool bAllowed = false;
    // ������� ����������
    if (m_SpawnData.Valid()){
    	switch(obj->ClassID){    
        case OBJCLASS_SHAPE:
	    	bAllowed = !!dynamic_cast<CSE_Shape*>(m_SpawnData.m_Data);
        break;
//        case OBJCLASS_SCENEOBJECT:
//	    	bAllowed = !!dynamic_cast<xrSE_Visualed*>(m_SpawnData.m_Data);
//        break;
        }
    }
    // �������� ����
	if (bAllowed){
        DetachObject();
        m_AttachedObject = obj;
        m_AttachedObject->OnAttach(this);
        PPosition 	= m_AttachedObject->PPosition;
        PRotation 	= m_AttachedObject->PRotation;
        PScale 		= m_AttachedObject->PScale;
    }
    return bAllowed;
}

void CSpawnPoint::DetachObject()
{
	if (m_AttachedObject){
		m_AttachedObject->OnDetach();
    	m_AttachedObject = 0;
    }
}

bool CSpawnPoint::CreateSpawnData(LPCSTR entity_ref)
{
	R_ASSERT(entity_ref&&entity_ref[0]);
    m_SpawnData.Destroy	();
    m_SpawnData.Create	(entity_ref);
    if (m_SpawnData.Valid()) m_Type = ptSpawnPoint;
    return m_SpawnData.Valid();
}
//----------------------------------------------------

bool CSpawnPoint::GetBox( Fbox& box ){
	box.set( PPosition, PPosition );
	box.min.x -= RPOINT_SIZE;
	box.min.y -= 0;
	box.min.z -= RPOINT_SIZE;
	box.max.x += RPOINT_SIZE;
	box.max.y += RPOINT_SIZE*2.f;
	box.max.z += RPOINT_SIZE;
//	box.min.sub(RPOINT_SIZE);
//	box.max.add(RPOINT_SIZE);
	return true;
}

void CSpawnPoint::OnFrame()
{
	inherited::OnFrame();
    if (m_AttachedObject) 		m_AttachedObject->OnFrame	();
	if (m_SpawnData.Valid())    m_SpawnData.OnFrame			();
}

void CSpawnPoint::Render( int priority, bool strictB2F )
{
	inherited::Render(priority, strictB2F);
    // render attached object
    if (m_AttachedObject) 		m_AttachedObject->Render(priority, strictB2F);
	if (m_SpawnData.Valid())    m_SpawnData.Render(Selected(),_Transform(),priority, strictB2F);
    // render spawn point
    if (1==priority){
        if (strictB2F){
            RCache.set_xform_world(FTransformRP);
            if (m_SpawnData.Valid()){
                // render icon
                ref_shader s 	   	= GetIcon(m_SpawnData.m_Data->s_name);
                DU.DrawEntity		(0xffffffff,s);
            }else{
                float k = 1.f/(float(m_dwTeamID+1)/float(MAX_TEAM));
                int r = m_dwTeamID%MAX_TEAM;
                Fcolor c;
                c.set(RP_COLORS[r]);
                c.mul_rgb(k*0.9f+0.1f);
                DU.DrawEntity(c.get(),Device.m_WireShader);
            }
        }else{
            ESceneSpawnTools* st = dynamic_cast<ESceneSpawnTools*>(ParentTools); VERIFY(st);
            if (st->m_Flags.is(ESceneSpawnTools::flShowSpawnType)){ 
                AnsiString s_name;
                if (m_SpawnData.Valid()){
                    s_name	= m_SpawnData.m_Data->s_name;
                }else{
                    switch (m_Type){
                    case ptRPoint: 	s_name.sprintf("RPoint T:%d",m_dwTeamID); break;
                    default: THROW2("CSpawnPoint:: Unknown Type");
                    }
                }
                
                Fvector D;	D.sub(Device.vCameraPosition,PPosition);
                float dist 	= D.normalize_magn();
                if (!st->m_Flags.is(ESceneSpawnTools::flPickSpawnType)||
                    !Scene.RayPickObject(dist,PPosition,D,OBJCLASS_SCENEOBJECT,0,0))
                        DU.DrawText	(PPosition,s_name.c_str(),0xffffffff,0xff000000);
            }
            if(Selected()){
                RCache.set_xform_world(Fidentity);
                Fbox bb; GetBox(bb);
                u32 clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                Device.SetShader(Device.m_WireShader);
                DU.DrawSelectionBox(bb,&clr);
            }
        }
    }
}

bool CSpawnPoint::FrustumPick(const CFrustum& frustum)
{
    if (m_AttachedObject&&m_AttachedObject->FrustumPick(frustum)) return true;
    Fbox bb; GetBox(bb);
    u32 mask=0xff;
    return (frustum.testAABB(bb.data(),mask));
}

bool CSpawnPoint::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf)
{
	bool bPick = false;
    if (m_AttachedObject) bPick = m_AttachedObject->RayPick(distance, start, direction, pinf);

	Fvector pos,ray2;
    pos.set(PPosition.x,PPosition.y+RPOINT_SIZE,PPosition.z);
	ray2.sub( pos, start );

    float d = ray2.dotproduct(direction);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (RPOINT_SIZE*RPOINT_SIZE)) && (d>RPOINT_SIZE) ){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
        }
    }

	return bPick;
}
//----------------------------------------------------
bool CSpawnPoint::OnAppendObject(CCustomObject* object)
{
	R_ASSERT(!m_AttachedObject);
    if (object->ClassID!=OBJCLASS_SHAPE) return false;
    // all right
    m_AttachedObject = object;
    return true;
}

bool CSpawnPoint::Load(IReader& F){
	u32 version = 0;

    R_ASSERT(F.r_chunk(SPAWNPOINT_CHUNK_VERSION,&version));
    if(version!=SPAWNPOINT_VERSION){
        ELog.Msg( mtError, "SPAWNPOINT: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    // new generation
    if (F.find_chunk(SPAWNPOINT_CHUNK_ENTITYREF)){
        if (!m_SpawnData.Load(F)){
            ELog.Msg( mtError, "SPAWNPOINT: Can't load Spawn Data.");
            return false;
        }
        SetValid	(true);
    }else{
	    if (F.find_chunk(SPAWNPOINT_CHUNK_TYPE))     m_Type 		= (EPointType)F.r_u32();
        if (m_Type==ptReserved){
            ELog.Msg( mtError, "SPAWNPOINT: Unsupported spawn element.");
            return false;
        }
        if (m_Type>=ptMaxType){
            ELog.Msg( mtError, "SPAWNPOINT: Unsupported spawn version.");
            return false;
        }
    	switch (m_Type){
        case ptRPoint:
		    if (F.find_chunk(SPAWNPOINT_CHUNK_TEAMID)) 	m_dwTeamID	= F.r_u32();
        break;
        default: THROW;
        }
    }

	// objects
    Scene.ReadObjects(F,SPAWNPOINT_CHUNK_ATTACHED_OBJ,OnAppendObject);

	UpdateTransform	();

    return true;
}

void CSpawnPoint::Save(IWriter& F){
	CCustomObject::Save(F);

	F.open_chunk		(SPAWNPOINT_CHUNK_VERSION);
	F.w_u16				(SPAWNPOINT_VERSION);
	F.close_chunk		();

    // save attachment
    if (m_AttachedObject){
	    ObjectList lst; lst.push_back(m_AttachedObject);
		Scene.SaveObjects(lst,SPAWNPOINT_CHUNK_ATTACHED_OBJ,F);
    }

	if (m_SpawnData.Valid()){
    	m_SpawnData.Save(F);
    }else{
		F.w_chunk	(SPAWNPOINT_CHUNK_TYPE,		&m_Type,	sizeof(u32));
    	switch (m_Type){
        case ptRPoint:
	        F.w_chunk	(SPAWNPOINT_CHUNK_TEAMID,	&m_dwTeamID,sizeof(u32));
        break;
        default: THROW;
        }
    }
}
//----------------------------------------------------

bool CSpawnPoint::ExportGame(SExportStreams& F)
{
	// spawn
	if (m_SpawnData.Valid()){
    	m_SpawnData.ExportGame	(F,this);
    }else{
        // game
        switch (m_Type){
        case ptRPoint:
	        F.rpoint.stream.open_chunk	(F.rpoint.chunk++);
            F.rpoint.stream.w_fvector3		(PPosition);
            F.rpoint.stream.w_fvector3		(PRotation);
            F.rpoint.stream.w_u32		(m_dwTeamID);
			F.rpoint.stream.close_chunk	();
        break;
        default: THROW;
        }
    }
    return true;
}
//----------------------------------------------------

void CSpawnPoint::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref,items);

    if (m_SpawnData.Valid()){
    	m_SpawnData.FillProp(pref,items);
    }else{
    	switch (m_Type){
        case ptRPoint:{
			PHelper.CreateU32(items, FHelper.PrepareKey(pref,"Respawn Point","Team"), &m_dwTeamID, 0,64,1);
        }break;
        default: THROW;
        }
    }
}
//----------------------------------------------------

void CSpawnPoint::OnDeviceCreate()
{
	m_SpawnData.OnDeviceCreate	();
}
//----------------------------------------------------

void CSpawnPoint::OnDeviceDestroy()
{
	m_SpawnData.OnDeviceDestroy	();
	if (m_Icons.empty()) return;
	for (ShaderPairIt it=m_Icons.begin(); it!=m_Icons.end(); it++)
    	it->second.destroy();
    m_Icons.clear();
}
//----------------------------------------------------

ref_shader CSpawnPoint::CreateIcon(LPCSTR name)
{
    ref_shader S;
    if (pSettings->line_exist(name,"$ed_icon")){
	    LPCSTR tex_name = pSettings->r_string(name,"$ed_icon");
    	S.create("editor\\spawn_icon",tex_name);
        m_Icons[name] = S;
    }else{
        S = 0;
    }
    return S;
}

ref_shader CSpawnPoint::GetIcon(LPCSTR name)
{
	ShaderPairIt it = m_Icons.find(name);
	if (it==m_Icons.end())	return CreateIcon(name);
	else					return it->second;
}
//----------------------------------------------------

bool CSpawnPoint::OnChooseQuery(LPCSTR specific)
{
	return (m_SpawnData.Valid()&&(0==strcmp(m_SpawnData.m_Data->s_name,specific)));
}


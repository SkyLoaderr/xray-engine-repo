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
#define SPAWNPOINT_CHUNK_SPAWNDATA_EXT	0xE421
//----------------------------------------------------
#define MAX_TEAM 6
const DWORD RP_COLORS[MAX_TEAM]={0xff0000,0x00ff00,0x0000ff,0xffff00,0x00ffff,0xff00ff};

ShaderMap CSpawnPoint::m_Icons;
//----------------------------------------------------

void CSpawnPoint::SSpawnData::Create(LPCSTR entity_ref)
{
    m_Data 	= F_entity_Create(entity_ref);
    if (m_Data){ 
        if (pSettings->LineExists(entity_ref,"$player")){
            if (pSettings->ReadBOOL(entity_ref,"$player"))
                m_Data->s_flags.set(M_SPAWN_OBJECT_ASPLAYER,TRUE);
        }
        m_ClassID = pSettings->ReadCLSID(entity_ref,"class");
        strcpy(m_Data->s_name,entity_ref);
    }
}
void CSpawnPoint::SSpawnData::Destroy()
{
    F_entity_Destroy(m_Data);
}
void CSpawnPoint::SSpawnData::Render(const Fvector& pos)
{
    if (fraBottomBar->miSpawnPointDrawText->Checked&&m_Data->s_name){
        FVF::TL	P;
        float 			cx,cy;
        P.transform		( pos, Device.mFullTransform );
        cx				= iFloor(Device._x2real(P.p.x));
        cy				= iFloor(Device._y2real(P.p.y));
        Device.pSystemFont->SetColor(0xffffffff);
        Device.pSystemFont->Out		(cx,cy,m_Data->s_name);
    }
    Shader* s = GetIcon	(m_Data->s_name);
    DU::DrawEntity		(0xffffffff,s);
}
void CSpawnPoint::SSpawnData::Save(CFS_Base& F)
{
    F.open_chunk		(SPAWNPOINT_CHUNK_ENTITYREF);
    F.WstringZ			(m_Data->s_name);
    F.close_chunk		();
    
    F.open_chunk		(SPAWNPOINT_CHUNK_SPAWNDATA);
    NET_Packet 			Packet;
    m_Data->Spawn_Write	(Packet,TRUE);
    F.Wdword			(Packet.B.count);
    F.write				(Packet.B.data,Packet.B.count);
    F.close_chunk		();

    F.open_chunk		(SPAWNPOINT_CHUNK_SPAWNDATA_EXT);
    F.WstringZ			(m_Refs?m_Refs->Name:0);
    F.close_chunk		();
}
bool CSpawnPoint::SSpawnData::Load(CStream& F)
{
    string64 			temp;
    F.RstringZ			(temp);

    NET_Packet 			Packet;
    R_ASSERT(F.FindChunk(SPAWNPOINT_CHUNK_SPAWNDATA));
    Packet.B.count 		= F.Rdword();
    F.Read				(Packet.B.data,Packet.B.count);
    Create				(temp);
    if (Valid())		m_Data->Spawn_Read	(Packet);

    if (F.FindChunk(SPAWNPOINT_CHUNK_SPAWNDATA_EXT)){
    	F.RstringZ		(temp);
        
	    F.close_chunk		();
    }

    return Valid();
}
bool CSpawnPoint::SSpawnData::ExportGame(SExportStreams& F, LPCSTR name)
{
    strcpy	  			(m_Data->s_name_replace,name);
    NET_Packet			Packet;
    m_Data->Spawn_Write	(Packet,TRUE);
        
    F.spawn.stream.open_chunk	(F.spawn.chunk++);
    F.spawn.stream.write		(Packet.B.data,Packet.B.count);
    F.spawn.stream.close_chunk	();
    return true;
}
void CSpawnPoint::SSpawnData::FillProp(LPCSTR pref, PropItemVec& items)
{
	m_Data->FillProp	(pref,items);
}
void CSpawnPoint::SSpawnData::OnObjectRemove(const CCustomObject* object)
{
	if (m_Refs==object) m_ Refs = 0;
}
//------------------------------------------------------------------------------

CSpawnPoint::CSpawnPoint(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct(data);
}

void CSpawnPoint::Construct(LPVOID data){
	ClassID		= OBJCLASS_SPAWNPOINT;
    m_dwTeamID	= 0;
    m_Type		= ptRPoint;
    if (data){
	    CreateSpawnData(LPCSTR(data));
    	if (!m_SpawnData.Valid()){
    		if (strcmp(LPSTR(data),RPOINT_CHOOSE_NAME)==0){
	        	m_Type = ptRPoint;
    	    }else if (strcmp(LPSTR(data),AIPOINT_CHOOSE_NAME)==0){
            	m_Type = ptAIPoint;
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
	F_entity_Destroy(m_SpawnData.m_Data);
    OnDeviceDestroy();
}

bool CSpawnPoint::CreateSpawnData(LPCSTR entity_ref)
{
	R_ASSERT(entity_ref&&entity_ref[0]);
    m_SpawnData.Destroy	();
    m_SpawnData.Create	(entity_ref);
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

void CSpawnPoint::Render( int priority, bool strictB2F )
{
	inherited::Render(priority, strictB2F);
    if (1==priority){
		Fbox bb; GetBox(bb);
		if (::Render->occ_visible(bb)){
            if (strictB2F){
                Device.SetTransform(D3DTS_WORLD,_Transform());
                if (m_SpawnData.Valid()){
                	m_SpawnData.Render(PPosition);
                }else{
                	float k = 1.f/(float(m_dwTeamID+1)/float(MAX_TEAM));
                    int r = m_dwTeamID%MAX_TEAM;
                    Fcolor c;
                    c.set(RP_COLORS[r]);
                    c.mul_rgb(k*0.9f+0.1f);
                    DU::DrawEntity(c.get(),Device.m_WireShader);
                    if (fraBottomBar->miSpawnPointDrawText->Checked)
                    {
                    	AnsiString s_name;
                        switch (m_Type){
                        case ptRPoint: 	s_name.sprintf("RPoint T:%d",m_dwTeamID); break;
                        case ptAIPoint: s_name.sprintf("AIPoint"); break;
                        default: THROW;
                        }
                    	
                        FVF::TL	P;
                        float 			cx,cy;
                        P.transform		( PPosition, Device.mFullTransform );
                        cx				= iFloor(Device._x2real(P.p.x));
                        cy				= iFloor(Device._y2real(P.p.y));
                        Device.pSystemFont->SetColor(0xffffffff);
                        Device.pSystemFont->Out		(cx,cy,s_name.c_str());
                    }
                }
            }else{
                if( Selected() ){
                    Fbox bb; GetBox(bb);
                    DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                    Device.SetShader(Device.m_WireShader);
                    DU::DrawSelectionBox(bb,&clr);
                }
            }
        }
    }
}

bool CSpawnPoint::FrustumPick(const CFrustum& frustum)
{
    Fbox bb; GetBox(bb);
    DWORD mask=0xff;
    return (frustum.testAABB(bb.min,bb.max,mask));
}

bool CSpawnPoint::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
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
	return false;
}
//----------------------------------------------------

bool CSpawnPoint::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(SPAWNPOINT_CHUNK_VERSION,&version));
    if(version!=SPAWNPOINT_VERSION){
        ELog.Msg( mtError, "SPAWNPOINT: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    // new generation
    if (F.FindChunk(SPAWNPOINT_CHUNK_ENTITYREF)){
        if (!m_SpawnData.Valid()){
            ELog.Msg( mtError, "SPAWNPOINT: Can't find CLASS_ID.");
            return false;
        }
        SetValid	(true);
    }else{
	    if (F.FindChunk(SPAWNPOINT_CHUNK_TYPE))     m_Type 		= (EPointType)F.Rdword();
        if (m_Type>=ptMaxType){
            ELog.Msg( mtError, "SPAWNPOINT: Unsupported spawn version.");
            return false;
        }
    	switch (m_Type){
        case ptRPoint: 
		    if (F.FindChunk(SPAWNPOINT_CHUNK_TEAMID)) 	m_dwTeamID	= F.Rdword();
        break;
        case ptAIPoint:
        break;
        default: THROW;
        }
    }

	UpdateTransform	();

    return true;
}

void CSpawnPoint::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(SPAWNPOINT_CHUNK_VERSION);
	F.Wword			(SPAWNPOINT_VERSION);
	F.close_chunk	();

	if (m_SpawnData.Valid()){
    	m_SpawnData.Save(F);
    }else{
		F.write_chunk	(SPAWNPOINT_CHUNK_TYPE,		&m_Type,	sizeof(DWORD));
    	switch (m_Type){
        case ptRPoint: 
	        F.write_chunk	(SPAWNPOINT_CHUNK_TEAMID,	&m_dwTeamID,sizeof(DWORD));
        break;
        case ptAIPoint:
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
    	m_SpawnData.ExportGame	(F,Name);
    }else{
        // game
        switch (m_Type){
        case ptRPoint: 
	        F.rpoint.stream.open_chunk	(F.rpoint.chunk++);
            F.rpoint.stream.Wvector		(PPosition);
            F.rpoint.stream.Wvector		(PRotation);
            F.rpoint.stream.Wdword		(m_dwTeamID);
			F.rpoint.stream.close_chunk	();
        case ptAIPoint:
        break;
        default: THROW;
        }
    }
    F.aipoint.stream.open_chunk		(F.aipoint.chunk++);
    F.aipoint.stream.Wvector		(PPosition);
    F.aipoint.stream.close_chunk	();
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
			PHelper.CreateU32(items, PHelper.PrepareKey(pref,"Respawn Point","Team"), &m_dwTeamID, 0,64,1);
        }break;
        case ptAIPoint: 
        	PHelper.CreateCaption(items,PHelper.PrepareKey(pref,"AI Point","Reserved"),"-");
        break;
        default: THROW;
        }
    }
}
//----------------------------------------------------

void CSpawnPoint::OnDeviceCreate()
{
}
//----------------------------------------------------

void CSpawnPoint::OnDeviceDestroy()
{
	if (m_Icons.empty()) return;
	for (ShaderPairIt it=m_Icons.begin(); it!=m_Icons.end(); it++)
    	Device.Shader.Delete(it->second);
    m_Icons.clear();
}
//----------------------------------------------------

Shader* CSpawnPoint::CreateIcon(LPCSTR name)
{
    Shader* S = 0;
    if (pSettings->LineExists(name,"$ed_icon")){
	    LPCSTR tex_name = pSettings->ReadSTRING(name,"$ed_icon");
    	S = Device.Shader.Create("editor\\spawn_icon",tex_name);
        m_Icons[name] = S;
    }else{
        S = 0;
    }
    return S;
}

Shader* CSpawnPoint::GetIcon(LPCSTR name)
{
	ShaderPairIt it = m_Icons.find(name);
	if (it==m_Icons.end())	return CreateIcon(name);
	else					return it->second;
}
//----------------------------------------------------

void CSpawnPoint::OnObjectRemove(CCustomObject* object)
{
	m_SpawnData.OnObjectRemove(object);
}
//----------------------------------------------------


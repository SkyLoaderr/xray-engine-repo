//----------------------------------------------------
// file: rpoint.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "spawnpoint.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "Texture.h"
#include "Frustum.h"
#include "D3DUtils.h"

#define SPAWNPOINT_VERSION   			0x0012
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
//----------------------------------------------------
#define MAX_TEAM 8
const DWORD RP_COLORS[MAX_TEAM]={0xff0000,0x00ff00,0x0000ff,0xffff00,0x00ffff,0x7f0000,0x007f00,0x00007f};

CSpawnPoint::CSpawnPoint(LPVOID data):CCustomObject(data){
	Construct(data);
}

void CSpawnPoint::Construct(LPVOID data){
	ClassID		= OBJCLASS_SPAWNPOINT;
    m_SpawnData	= 0;
    if (data){
    	if (!CreateSpawnData(LPCSTR(data))){
        	SetValid(false);
        }
    }
}

CSpawnPoint::~CSpawnPoint(){
	F_entity_Destroy(m_SpawnData);
}

bool CSpawnPoint::CreateSpawnData(LPCSTR entity_ref){
	R_ASSERT(entity_ref&&entity_ref[0]);
	F_entity_Destroy(m_SpawnData);
	m_SpawnData = F_entity_Create(entity_ref);
    if (m_SpawnData) strcpy(m_SpawnData->s_name,entity_ref);
    return !!m_SpawnData;
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

void CSpawnPoint::Render( int priority, bool strictB2F ){
    if ((1==priority)&&(false==strictB2F)){
        if (Device.m_Frustum.testSphere(PPosition,RPOINT_SIZE)){
		    DU::DrawFlag(PPosition,PRotation.y,RPOINT_SIZE*2,RPOINT_SIZE,.3f,RP_COLORS[0],true);
            if( Selected() ){
                Fbox bb; GetBox(bb);
	            DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
    	        DU::DrawSelectionBox(bb,&clr);
            }
        }
    }
}

bool CSpawnPoint::FrustumPick(const CFrustum& frustum){
    return (frustum.testSphere(PPosition,RPOINT_SIZE))?true:false;
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
    R_ASSERT(F.FindChunk(SPAWNPOINT_CHUNK_ENTITYREF));
    string64 entity_ref;
    F.RstringZ	(entity_ref);

    NET_Packet 	Packet;
    R_ASSERT(F.FindChunk(SPAWNPOINT_CHUNK_SPAWNDATA));
    Packet.B.count = F.Rdword();
    F.Read		(Packet.B.data,Packet.B.count);
    m_SpawnData	= F_entity_Create(entity_ref);
    if (!m_SpawnData){
        ELog.Msg( mtError, "SPAWNPOINT: Can't find CLASS_ID.");
        return false;
    }
    m_SpawnData->Spawn_Read(Packet);

	UpdateTransform	();

    return true;
}

void CSpawnPoint::Save(CFS_Base& F){
	R_ASSERT(m_SpawnData);
	CCustomObject::Save(F);

	F.open_chunk	(SPAWNPOINT_CHUNK_VERSION);
	F.Wword			(SPAWNPOINT_VERSION);
	F.close_chunk	();

    F.open_chunk	(SPAWNPOINT_CHUNK_ENTITYREF);
    F.WstringZ		(m_SpawnData->s_name);
	F.close_chunk	();
    
    F.open_chunk	(SPAWNPOINT_CHUNK_SPAWNDATA);
    NET_Packet 		Packet;
    m_SpawnData->Spawn_Write(Packet,FALSE);
    F.Wdword		(Packet.B.count);
    F.write			(Packet.B.data,Packet.B.count);
    F.close_chunk	();
}
//----------------------------------------------------

bool CSpawnPoint::ExportSpawn( CFS_Base& F, int chunk_id )
{
	R_ASSERT			(m_SpawnData);
    strcpy				(m_SpawnData->s_name_replace,Name);
    NET_Packet 			Packet;
    m_SpawnData->Spawn_Write(Packet,FALSE);
    F.open_chunk		(chunk_id);
    F.write				(Packet.B.data,Packet.B.count);
    F.close_chunk		();
    return true;
}
//----------------------------------------------------




















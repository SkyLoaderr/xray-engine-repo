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

CSpawnPoint::CSpawnPoint(LPVOID data, LPCSTR name):CCustomObject(data,name){
	Construct(data);
}

void CSpawnPoint::Construct(LPVOID data){
	ClassID		= OBJCLASS_SPAWNPOINT;
    m_SpawnData	= 0;
    m_SpawnClassID = 0;
    m_dwTeamID	= 0;
    m_Type		= ptRPoint;
    if (data){
	    CreateSpawnData(LPCSTR(data));
    	if (!m_SpawnData){
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

CSpawnPoint::~CSpawnPoint(){
	F_entity_Destroy(m_SpawnData);
}

bool CSpawnPoint::CreateSpawnData(LPCSTR entity_ref){
	R_ASSERT(entity_ref&&entity_ref[0]);
	F_entity_Destroy(m_SpawnData);
	m_SpawnData = F_entity_Create(entity_ref);
    if (m_SpawnData){ 
		m_SpawnClassID = pSettings->ReadCLSID(entity_ref,"class");
    	strcpy(m_SpawnData->s_name,entity_ref);
    }
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

void CSpawnPoint::Render( int priority, bool strictB2F )
{
	inherited::Render(priority, strictB2F);
    if ((1==priority)&&(false==strictB2F)){
    	Fbox bb; GetBox(bb);
	    if (::Render->occ_visible(bb)){
        	if (m_SpawnData){
                switch (m_SpawnClassID){
                case CLSID_OBJECT_ACTOR:		break;
                case CLSID_OBJECT_DUMMY:		break;
                case CLSID_AI_HEN:				break;
                case CLSID_AI_RAT:				break;
                case CLSID_AI_SOLDIER:			break;
                case CLSID_AI_ZOMBIE:			break;
                case CLSID_AI_CROW:				break;
                case CLSID_EVENT:				break;
                case CLSID_CAR_NIVA:			break;
                case CLSID_OBJECT_W_M134:		break;
                case CLSID_OBJECT_W_FN2000:		break;
                case CLSID_OBJECT_W_AK74:		break;
                case CLSID_OBJECT_W_LR300:		break;
                case CLSID_OBJECT_W_HPSA:		break;
                case CLSID_OBJECT_W_PM:			break;
                case CLSID_OBJECT_W_FORT:		break;
                case CLSID_OBJECT_W_BINOCULAR:	break;
                default:
	            	DU::DrawFlag(PPosition,PRotation.y,RPOINT_SIZE*2,RPOINT_SIZE,.3f,0xffffff,true);
                }
				DU::DrawFlag(PPosition,PRotation.y,RPOINT_SIZE*2,RPOINT_SIZE,.3f,0xffffff,true);
            }else				
            	DU::DrawFlag(PPosition,PRotation.y,RPOINT_SIZE*2,RPOINT_SIZE,.3f,RP_COLORS[m_dwTeamID],false);
            if( Selected() ){
                Fbox bb; GetBox(bb);
	            DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
    	        DU::DrawSelectionBox(bb,&clr);
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
        string64 entity_ref;
        F.RstringZ	(entity_ref);

        NET_Packet 	Packet;
        R_ASSERT(F.FindChunk(SPAWNPOINT_CHUNK_SPAWNDATA));
        Packet.B.count = F.Rdword();
        F.Read		(Packet.B.data,Packet.B.count);
		CreateSpawnData(entity_ref);
        if (!m_SpawnData){
            ELog.Msg( mtError, "SPAWNPOINT: Can't find CLASS_ID.");
            return false;
        }
        m_SpawnData->Spawn_Read(Packet);
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

	if (m_SpawnData){
        F.open_chunk	(SPAWNPOINT_CHUNK_ENTITYREF);
        F.WstringZ		(m_SpawnData->s_name);
        F.close_chunk	();
    
        F.open_chunk	(SPAWNPOINT_CHUNK_SPAWNDATA);
        NET_Packet 		Packet;
        m_SpawnData->Spawn_Write(Packet,FALSE);
        F.Wdword		(Packet.B.count);
        F.write			(Packet.B.data,Packet.B.count);
        F.close_chunk	();
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
	if (m_SpawnData){
        strcpy	  					(m_SpawnData->s_name_replace,Name);
        NET_Packet					Packet;
        m_SpawnData->Spawn_Write	(Packet,FALSE);
        
        F.spawn.stream.open_chunk	(F.spawn.chunk++);
        F.spawn.stream.write		(Packet.B.data,Packet.B.count);
        F.spawn.stream.close_chunk	();
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

bool CSpawnPoint::FillProp(PropValueVec& values)
{
	inherited::FillProp(values);
    
    if (m_SpawnData){
    	m_SpawnData->FillProp(values);
    }else{
    	switch (m_Type){
        case ptRPoint:{
            FILL_PROP_EX(values, "Respawn Point", "Team",	&m_dwTeamID, PROP::CreateDWORD(64,1));
        }break;
        case ptAIPoint: 
            FILL_PROP_EX(values, "AI Point", "Reserved", "-", PROP::CreateMarker());
        break;
        default: THROW;
        }
    }
	
    return true;
}
//----------------------------------------------------



//----------------------------------------------------
// file: rpoint.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "rpoint.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "Texture.h"
#include "Frustum.h"
#include "D3DUtils.h"

#define RPOINT_VERSION   				0x0011
//----------------------------------------------------
#define RPOINT_CHUNK_VERSION			0xE411
#define RPOINT_CHUNK_POSITION			0xE412
#define RPOINT_CHUNK_TEAMID				0xE413
#define RPOINT_CHUNK_DIRECTION			0xE414
#define RPOINT_CHUNK_SQUADID			0xE415
#define RPOINT_CHUNK_GROUPID			0xE416
#define RPOINT_CHUNK_TYPE				0xE417
#define RPOINT_CHUNK_FLAGS				0xE418
#define RPOINT_CHUNK_ENTITYREFS			0xE419
//----------------------------------------------------
#define MAX_TEAM 8
const DWORD RP_COLORS[MAX_TEAM]={0xff0000,0x00ff00,0x0000ff,0xffff00,0x00ffff,0x7f0000,0x007f00,0x00007f};

CRPoint::CRPoint( char *name ):CCustomObject(){
	Construct();
    Name		= name;
}

CRPoint::CRPoint():CCustomObject(){
	Construct();
}

void CRPoint::Construct(){
	ClassID		= OBJCLASS_RPOINT;
    m_dwTeamID	= 0;
    m_dwSquadID	= 0;
    m_dwGroupID	= 0;
    m_Type		= etPlayer;
    ZeroMemory(&m_Flags,sizeof(Flags));
    m_EntityRefs[0]=0;
}

CRPoint::~CRPoint(){
}

//----------------------------------------------------
bool CRPoint::GetBox( Fbox& box ){
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

void CRPoint::Render( int priority, bool strictB2F ){
    if ((1==priority)&&(false==strictB2F)){
        if (Device.m_Frustum.testSphere(PPosition,RPOINT_SIZE)){
		    DU::DrawFlag(PPosition,PRotate.y,RPOINT_SIZE*2,RPOINT_SIZE,.3f,RP_COLORS[m_dwTeamID],etEntity==m_Type);
            if( Selected() ){
                Fbox bb; GetBox(bb);
	            DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
    	        DU::DrawSelectionBox(bb,&clr);
            }
        }
    }
}

bool CRPoint::FrustumPick(const CFrustum& frustum){
    return (frustum.testSphere(PPosition,RPOINT_SIZE))?true:false;
}

bool CRPoint::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
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

bool CRPoint::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(RPOINT_CHUNK_VERSION,&version));
    if( (version!=0x0010)&&(version!=RPOINT_VERSION) ){
        ELog.DlgMsg( mtError, "RPOINT: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    if (version==0x0010){
	    R_ASSERT(F.ReadChunk(RPOINT_CHUNK_POSITION,&FPosition));
	    if (F.FindChunk(RPOINT_CHUNK_DIRECTION))FRotate.x = F.Rfloat();
        UpdateTransform();
    }

    if (F.FindChunk(RPOINT_CHUNK_TEAMID)) 	m_dwTeamID = F.Rdword();

    // new generation
    if (F.FindChunk(RPOINT_CHUNK_SQUADID))  	m_dwSquadID = F.Rdword();
    if (F.FindChunk(RPOINT_CHUNK_GROUPID))  	m_dwGroupID = F.Rdword();
    if (F.FindChunk(RPOINT_CHUNK_TYPE))     	m_Type 		= F.Rdword();
    if (F.FindChunk(RPOINT_CHUNK_FLAGS))    	F.Read		(&m_Flags,sizeof(DWORD));
    if (F.FindChunk(RPOINT_CHUNK_ENTITYREFS))	F.RstringZ	(m_EntityRefs);
    return true;
}

void CRPoint::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(RPOINT_CHUNK_VERSION);
	F.Wword			(RPOINT_VERSION);
	F.close_chunk	();

    F.write_chunk	(RPOINT_CHUNK_TEAMID,&m_dwTeamID,sizeof(DWORD));
    F.write_chunk	(RPOINT_CHUNK_SQUADID,&m_dwSquadID,sizeof(DWORD));
    F.write_chunk	(RPOINT_CHUNK_GROUPID,&m_dwGroupID,sizeof(DWORD));
    F.write_chunk	(RPOINT_CHUNK_TYPE,&m_Type,sizeof(DWORD));
    F.write_chunk	(RPOINT_CHUNK_FLAGS,&m_Flags,sizeof(DWORD));
    F.write_chunk	(RPOINT_CHUNK_ENTITYREFS,&m_EntityRefs,strlen(m_EntityRefs));
}

//----------------------------------------------------


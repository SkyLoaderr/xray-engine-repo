#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomMTools.h"
 
static const u16 TOOLS_VERSION  		= 0x0000;
//----------------------------------------------------
static const u32 CHUNK_VERSION			= 0xFFFF0000;
static const u32 CHUNK_FLAGS			= 0xFFFF0001;
//----------------------------------------------------

ESceneCustomMTools::ESceneCustomMTools(EObjClass cls)
{
    ClassID				= cls;
    m_Flags.set			(flVisible);
    // controls
    sub_target			= 0;
    pCurControl 		= 0;
    pFrame				= 0;
    action				= -1;
}

ESceneCustomMTools::~ESceneCustomMTools()
{
	OnDestroy			();
}
 
void ESceneCustomMTools::OnCreate()
{
    CreateControls		();
}
void ESceneCustomMTools::OnDestroy()
{
    RemoveControls		();
}

bool ESceneCustomMTools::Load(IReader& F)
{
    if (F.find_chunk(CHUNK_VERSION)){
		u16 version = F.r_u16();
        if( version!=TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }
    }

    if (F.find_chunk(CHUNK_FLAGS))
    	m_Flags.set	(F.r_u32());
    return true;
}

void ESceneCustomMTools::Save(IWriter& F)
{
	F.open_chunk	(CHUNK_VERSION);
    F.w_u16			(TOOLS_VERSION);
	F.close_chunk	();

	F.open_chunk	(CHUNK_FLAGS);
    F.w_u32			(m_Flags.get());
	F.close_chunk	();
}

bool ESceneCustomMTools::LoadSelection(IReader& F)
{
	u16 version 	= 0;

    if (F.find_chunk(CHUNK_VERSION)){
        R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
        if( version!=TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }
    }
    return true;
}

void ESceneCustomMTools::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&TOOLS_VERSION,sizeof(TOOLS_VERSION));
}

#include "stdafx.h"
#pragma hdrstop

#include "ESceneGlowTools.h"

// chunks
static const u16 GLOW_TOOLS_VERSION  	= 0x0000;
//----------------------------------------------------
enum{
    CHUNK_VERSION			= 0x1001ul,
    CHUNK_FLAGS				= 0x1002ul,
};
//----------------------------------------------------

bool ESceneGlowTools::Load(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=GLOW_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }

	if (!inherited::Load(F)) return false;

    if (F.find_chunk(CHUNK_FLAGS))
    	m_Flags.set	(F.r_u32());

    return true;
}
//----------------------------------------------------

void ESceneGlowTools::Save(IWriter& F)
{
	inherited::Save	(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&GLOW_TOOLS_VERSION,sizeof(GLOW_TOOLS_VERSION));

	F.open_chunk	(CHUNK_FLAGS);
    F.w_u32			(m_Flags.get());
	F.close_chunk	();
}
//----------------------------------------------------
 
bool ESceneGlowTools::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=GLOW_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}
//----------------------------------------------------

void ESceneGlowTools::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&GLOW_TOOLS_VERSION,sizeof(GLOW_TOOLS_VERSION));
    
	inherited::SaveSelection(F);
}
//----------------------------------------------------

 
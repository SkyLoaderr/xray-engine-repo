#include "stdafx.h"
#pragma hdrstop

#include "ESceneLightTools.h"
#include "elight.h"
#include "scene.h"
#include "ui_main.h"

// chunks
static const u16 LIGHT_TOOLS_VERSION  	= 0x0000;
//----------------------------------------------------
enum{
    CHUNK_VERSION			= 0x1001ul,
    CHUNK_LCONTROLS			= 0x1002ul,
    CHUNK_LCONTROLS_LAST	= 0x1003ul,
    CHUNK_FLAGS				= 0x1004ul,
    CHUNK_HEMI				= 0x1005ul,
    CHUNK_SUN_SHADOW		= 0x1006ul,
};
//----------------------------------------------------

bool ESceneLightTools::Load(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version))
        if( version!=LIGHT_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }

	if (!inherited::Load(F)) return false;

    if (F.find_chunk(CHUNK_FLAGS))
    	m_Flags.set	(F.r_u32());

    if (F.find_chunk(CHUNK_HEMI)){
     	m_HemiQuality		= F.r_u8();
    }

    if (F.find_chunk(CHUNK_SUN_SHADOW)){
     	m_SunShadowQuality	= F.r_u8();
        F.r_fvector2		(m_SunShadowDir);
    }
    
    if (F.find_chunk(CHUNK_LCONTROLS_LAST))
		lcontrol_last_idx	= F.r_u32();
    
	IReader* R 		= F.open_chunk(CHUNK_LCONTROLS); 
    if (R){
        while (!R->eof()){
            AnsiString	l_name;
            R->r_stringZ(l_name);
            u32 l_idx	= R->r_u32();
            AppendLightControl(l_name.c_str(),&l_idx);
        }
        R->close		();
    }

    return true;
}
//----------------------------------------------------

void ESceneLightTools::Save(IWriter& F)
{
	inherited::Save	(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&LIGHT_TOOLS_VERSION,sizeof(LIGHT_TOOLS_VERSION));

	F.open_chunk	(CHUNK_FLAGS);
    F.w_u32			(m_Flags.get());
	F.close_chunk	();

	F.open_chunk	(CHUNK_HEMI);
    F.w_u8			(m_HemiQuality);
    F.close_chunk	();

	F.open_chunk	(CHUNK_SUN_SHADOW);
    F.w_u8			(m_SunShadowQuality);
    F.w_fvector2	(m_SunShadowDir);
    F.close_chunk	();

	F.open_chunk	(CHUNK_LCONTROLS_LAST);
	F.w_u32			(lcontrol_last_idx);
    F.close_chunk	();

	F.open_chunk	(CHUNK_LCONTROLS);
	ATokenIt		_I 	= lcontrols.begin();
    ATokenIt		_E 	= lcontrols.end();
    for (;_I!=_E; _I++){
        F.w_stringZ	(_I->name.c_str());
        F.w_u32		(_I->id);
    }
    F.close_chunk	();
}
//----------------------------------------------------
 
bool ESceneLightTools::LoadSelection(IReader& F)
{
	u16 version 	= 0;
    R_ASSERT(F.r_chunk(CHUNK_VERSION,&version));
    if( version!=LIGHT_TOOLS_VERSION ){
        ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
        return false;
    }

	return inherited::LoadSelection(F);
}
//----------------------------------------------------

void ESceneLightTools::SaveSelection(IWriter& F)
{
	F.w_chunk		(CHUNK_VERSION,(u16*)&LIGHT_TOOLS_VERSION,sizeof(LIGHT_TOOLS_VERSION));
    
	inherited::SaveSelection(F);
}
//----------------------------------------------------

//----------------------------------------------------
// Export level lights
//----------------------------------------------------
#include "LightAnimLibrary.h"
bool ESceneLightTools::ExportLevelLights()
{
	if (!m_Objects.empty()){
    	AnsiString fname= Scene.LevelPath()+LIGHT_EXPORT_FILE_NAME;
        IWriter* F		= FS.w_open(fname.c_str());
        F->open_chunk	(ELight::LIGHT_EXPORT_CHUNK_VERSION);
        F->w_u16		(LIGHT_EXPORT_VERSION);
        F->close_chunk	();
        
        F->open_chunk	(ELight::LIGHT_EXPORT_CHUNK_LIGHTS);
        for (int type=0; type<ELight::ltMaxCount; type++){
            F->open_chunk	(type);
            int k		= 0;
            for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            	CLight* L	= dynamic_cast<CLight*>(*it); VERIFY(L);
            	if (L->m_Type==ELight::EType(type)){
                    F->open_chunk	(k++);
                        F->w_stringZ	(L->Name);
                        F->w_fvector3	(L->FTransform.c);
                        F->w_fvector3	(L->FTransform.k);
                        F->w_fvector3	(L->FTransform.i);
                        F->w_float		(L->m_Cone);
                        F->w_float		(L->m_Range);
                        F->w_float		(L->m_VirtualSize);
                        F->w_fcolor		(L->m_Color);
                        F->w_float		(L->m_Brightness);
                        F->w_stringZ	(L->m_pAnimRef?L->m_pAnimRef->cName:"");
                        F->w_stringZ	(L->m_FalloffTex.c_str());
                        F->w_stringZ	(FindLightControl(L->m_LControl)->name.c_str());
                        F->w_u32		(L->m_Flags.get());
                    F->close_chunk	();
                }
            }
            F->close_chunk	();
        }
        F->close_chunk	();

        FS.w_close		(F);
        return true;
    }
    return false;
}
//----------------------------------------------------


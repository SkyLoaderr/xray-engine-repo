//----------------------------------------------------
// file: BuilderLTX.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "ESound.h"
#include "ELight.h"
#include "SpawnPoint.h"
#include "Event.h"
#include "WayPoint.h"
#include "xr_ini.h"
#include "xr_efflensflare.h"
#include "GroupObject.h"
//----------------------------------------------------

//----------------------------------------------------
BOOL SceneBuilder::ParseLTX(CInifile* pIni, ObjectList& lst, LPCSTR prefix)
{
	BOOL bResult=TRUE;
	AnsiString suns;
	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
		switch ((*it)->ClassID){
        case OBJCLASS_SOUND:{
            ESound *s = (ESound *)(*it);
            if (s->m_WAVName.IsEmpty()){
            	ELog.DlgMsg(mtError,"*ERROR: Sound '%s'. Not assigned wave file!", s->Name);
                bResult=FALSE;
                break;
            }
            AnsiString ln; ln.sprintf("%s,%f,%f,%f",s->m_WAVName.c_str(),s->PPosition.x,s->PPosition.y,s->PPosition.z);
            pIni->w_string("static_sounds",s->Name,ln.c_str());
        }break;
        case OBJCLASS_LIGHT:{
            CLight *l = (CLight *)(*it);
            if (l->m_D3D.type==D3DLIGHT_DIRECTIONAL){
	            if (suns.Length()) suns += ", ";
    	        suns += l->Name;
           	    pIni->w_fcolor	(l->Name, "sun_color", 			l->m_D3D.diffuse);
               	pIni->w_fvector3(l->Name, "sun_dir", 			l->m_D3D.direction);
               	pIni->w_string	(l->Name, "gradient", 			l->m_LensFlare.m_Flags.is(CLensFlare::flGradient)?"on":"off");
               	pIni->w_string	(l->Name, "source", 			l->m_LensFlare.m_Flags.is(CLensFlare::flSource)?"on":"off");
               	pIni->w_string	(l->Name, "flares", 			l->m_LensFlare.m_Flags.is(CLensFlare::flFlare)?"on":"off");
                pIni->w_float	(l->Name, "gradient_opacity", 	l->m_LensFlare.m_Gradient.fOpacity);
                pIni->w_string	(l->Name, "gradient_texture", 	l->m_LensFlare.m_Gradient.texture);
                pIni->w_float	(l->Name, "gradient_radius", 	l->m_LensFlare.m_Gradient.fRadius);
                pIni->w_string	(l->Name, "source_texture", 	l->m_LensFlare.m_Source.texture);
                pIni->w_float	(l->Name, "source_radius", 	l->m_LensFlare.m_Source.fRadius);
                AnsiString FT=""; AnsiString FR=""; AnsiString FO=""; AnsiString FP="";
                AnsiString T="";
                int i=l->m_LensFlare.m_Flares.size();
                for (CEditFlare::FlareIt it = l->m_LensFlare.m_Flares.begin(); it!=l->m_LensFlare.m_Flares.end(); it++,i--){
                	FT += it->texture;
                	T.sprintf("%.3f",it->fRadius); 	FR += T;
                	T.sprintf("%.3f",it->fOpacity);	FO += T;
                	T.sprintf("%.3f",it->fPosition);FP += T;
                    if (i>1){FT+=","; FR+=","; FO+=","; FP+=",";}
                }
               	pIni->w_string	(l->Name, "flare_textures",	FT.c_str());
               	pIni->w_string	(l->Name, "flare_radius",	FR.c_str());
               	pIni->w_string	(l->Name, "flare_opacity",	FO.c_str());
               	pIni->w_string	(l->Name, "flare_position",	FP.c_str());
            }
        }break;
        case OBJCLASS_GROUP:{
            CGroupObject* group = (CGroupObject*)(*it);
            bResult = ParseLTX(pIni,group->GetObjects(),group->Name);
        }break;
        }
        if (!bResult) return FALSE;
        if (suns.Length()) pIni->w_string("environment", "suns", suns.c_str());
    }
    return TRUE;
}
//----------------------------------------------------

BOOL SceneBuilder::BuildLTX()
{
	bool bResult=true;
	int objcount = Scene.ObjCount();
	if( objcount <= 0 ) return true;

	AnsiString ltx_filename	= m_LevelPath+"level.ltx";

    if (FS.exist(ltx_filename.c_str()))
    	EFS.MarkFile(ltx_filename,true);

	// -- defaults --
    CMemoryWriter F;
	if( !Scene.m_LevelOp.m_BOPText.IsEmpty() )
		F.w_stringZ( Scene.m_LevelOp.m_BOPText.c_str() );
	F.save_to(ltx_filename.c_str());

    CInifile* pIni = xr_new<CInifile>(ltx_filename.c_str(),false);

	Scene.WriteToLTX(pIni);

    // required strings
    pIni->w_string("static_sounds",		"; sounds","");
    pIni->w_string("sound_environment",	"; sound environment","");

    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++)
        if (!ParseLTX(pIni,(*it).second)){bResult = FALSE; break;}

    delete pIni;

	return bResult;
}


//----------------------------------------------------
// file: BuilderLTX.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "Sound.h"
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
            CSound *s = (CSound *)(*it);
            if (strlen(s->m_fName)==0){
            	ELog.DlgMsg(mtError,"*ERROR: Sound '%s'. Not assigned wave file!", s->Name);
                bResult=FALSE;
                break;
            }
            AnsiString ln; ln.sprintf("%s,%f,%f,%f",s->m_fName,s->PPosition.x,s->PPosition.y,s->PPosition.z);
            pIni->WriteString("static_sounds",s->Name,ln.c_str());
        }break;
        case OBJCLASS_LIGHT:{
            CLight *l = (CLight *)(*it);
            if (l->m_D3D.type==D3DLIGHT_DIRECTIONAL){
	            if (suns.Length()) suns += ", ";
    	        suns += l->Name;
           	    pIni->WriteColor	(l->Name, "sun_color", 			l->m_D3D.diffuse.get());
               	pIni->WriteVector	(l->Name, "sun_dir", 			l->m_D3D.direction);
               	pIni->WriteString	(l->Name, "gradient", 			l->m_LensFlare.m_dwFlags&CLensFlare::flGradient?"on":"off");
               	pIni->WriteString	(l->Name, "source", 			l->m_LensFlare.m_dwFlags&CLensFlare::flSource?"on":"off");
               	pIni->WriteString	(l->Name, "flares", 			l->m_LensFlare.m_dwFlags&CLensFlare::flFlare?"on":"off");
                pIni->WriteFloat	(l->Name, "gradient_opacity", 	l->m_LensFlare.m_Gradient.fOpacity);
                pIni->WriteString	(l->Name, "gradient_texture", 	l->m_LensFlare.m_Gradient.texture);
                pIni->WriteFloat	(l->Name, "gradient_radius", 	l->m_LensFlare.m_Gradient.fRadius);
                pIni->WriteString	(l->Name, "source_texture", 	l->m_LensFlare.m_Source.texture);
                pIni->WriteFloat	(l->Name, "source_radius", 	l->m_LensFlare.m_Source.fRadius);
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
               	pIni->WriteString	(l->Name, "flare_textures",	FT.c_str());
               	pIni->WriteString	(l->Name, "flare_radius",	FR.c_str());
               	pIni->WriteString	(l->Name, "flare_opacity",	FO.c_str());
               	pIni->WriteString	(l->Name, "flare_position",	FP.c_str());
            }
        }break;
        case OBJCLASS_GROUP:{ 
            CGroupObject* group = (CGroupObject*)(*it);
            bResult = ParseLTX(pIni,group->GetObjects(),group->Name);
        }break;
        }
        if (!bResult) return FALSE;
        if (suns.Length()) pIni->WriteString("environment", "suns", suns.c_str());
    }
    return TRUE;
}
//----------------------------------------------------

BOOL SceneBuilder::BuildLTX(){
	bool bResult=true;
	int objcount = Scene.ObjCount();
	if( objcount <= 0 ) return true;

	char ltx_filename[MAX_PATH];
	strcpy( ltx_filename, "level.ltx" );
	m_LevelPath.Update( ltx_filename );

    if (Engine.FS.Exist(ltx_filename, false))
    	Engine.FS.MarkFile(ltx_filename,true);

	// -- defaults --
    CFS_Memory F;
	if( !Scene.m_LevelOp.m_BOPText.IsEmpty() )
		F.WstringZ( Scene.m_LevelOp.m_BOPText.c_str() );
	F.SaveTo(ltx_filename,0);

    CInifile* pIni = new CInifile(ltx_filename,false);

	Scene.WriteToLTX(pIni);

    // required strings
    pIni->WriteString("static_sounds",		"; sounds","");
    pIni->WriteString("sound_environment",	"; sound environment","");

    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++)
        if (!ParseLTX(pIni,(*it).second)){bResult = FALSE; break;}
    
    delete pIni;

	return bResult;
}


#include "stdafx.h"
#pragma hdrstop

#include "SceneSummaryInfo.h"
#include "ImageThumbnail.h"
#include "SceneSummaryInfo.h"
#include "Scene.h"
#include "ui_main.h"

void SSceneSummary::FillProp(PropItemVec& items)
{
    // fill items
    PHelper.CreateCaption(items,"Level Name",					Scene.m_LevelOp.m_FNLevelPath);
    PHelper.CreateCaption(items,"Geometry\\Total Faces",   		face_cnt);
    PHelper.CreateCaption(items,"Geometry\\Total Vertices",		vert_cnt);
    PHelper.CreateCaption(items,"Geometry\\MU Objects",			mu_objects.size());
    PHelper.CreateCaption(items,"Geometry\\MU References", 		object_mu_ref_cnt);
    PHelper.CreateCaption(items,"Geometry\\LOD Objects",		lod_objects.size());
    PHelper.CreateCaption(items,"Geometry\\LOD References",		object_lod_ref_cnt);
    PHelper.CreateCaption(items,"Visibility\\HOM Faces",		hom_face_cnt);
    PHelper.CreateCaption(items,"Visibility\\HOM Vertices",		hom_vert_cnt);
    PHelper.CreateCaption(items,"Visibility\\Sectors",			sector_cnt);
    PHelper.CreateCaption(items,"Visibility\\Portals",			portal_cnt);
    PHelper.CreateCaption(items,"Lights\\Count",				light_sun_cnt+light_point_cnt+light_spot_cnt);
    PHelper.CreateCaption(items,"Lights\\By Type\\Sun",			light_sun_cnt);
    PHelper.CreateCaption(items,"Lights\\By Type\\Point",		light_point_cnt);
    PHelper.CreateCaption(items,"Lights\\By Type\\Spot",		light_spot_cnt);
    PHelper.CreateCaption(items,"Lights\\By Usage\\Dynamic",	light_dynamic_cnt);
    PHelper.CreateCaption(items,"Lights\\By Usage\\Static",		light_static_cnt);
    PHelper.CreateCaption(items,"Lights\\By Usage\\Breakable",	light_breakable_cnt);
    PHelper.CreateCaption(items,"Lights\\By Usage\\Procedural",	light_procedural_cnt);
    PHelper.CreateCaption(items,"Glows\\Count",					glow_cnt);
    // textures
    AnsiString temp;
    int base_mem_usage		= 0; 
    int det_mem_usage		= 0;
    PropValue* total_count	= PHelper.CreateCaption	(items,"Textures\\Count","");
    PropValue* total_mem	= PHelper.CreateCaption(items,"Textures\\Memory Usage","");
    PHelper.CreateCaption	(items,"Textures\\Base\\Count",		textures.size());
    PropValue* base_mem		= PHelper.CreateCaption(items,"Textures\\Base\\Memory Usage",	"");
    UI.ProgressStart(textures.size(),"Collect base textures info: ");
    for (AStringSetIt t_it=textures.begin(); t_it!=textures.end(); t_it++){
        UI.ProgressInc	(t_it->c_str());
        EImageThumbnail* T 	= xr_new<EImageThumbnail>(t_it->c_str(),EImageThumbnail::EITTexture,true);
        if (!T->Valid()){
        	ELog.Msg(mtError,"Can't get info from texture: '%s'",t_it->c_str());
        }else{
            int tex_mem		= T->MemoryUsage();
            base_mem_usage	+= tex_mem;
            AnsiString pref	= AnsiString("Textures\\Base\\")+*t_it;
            PropValue* V=0;
            V=PHelper.CreateATexture(items,FHelper.PrepareKey(pref.c_str(),"Texture"), 		(AnsiString*)&*t_it); V->Owner()->Enable(FALSE);
            PHelper.CreateCaption(items,FHelper.PrepareKey(pref.c_str(),"Format"),			T->FormatString());
            PHelper.CreateCaption(items,FHelper.PrepareKey(pref.c_str(),"Size"), 			AnsiString().sprintf("%d x %d x %s",T->_Width(),T->_Height(),T->_Format().HasAlpha()?"32b":"24b"));
            PHelper.CreateCaption(items,FHelper.PrepareKey(pref.c_str(),"Memory Usage"),	AnsiString().sprintf("%d Kb",iFloor(tex_mem/1024)));
        	if (T->_Format().flags.is(STextureParams::flHasDetailTexture)){
            	std::pair<AStringSetIt, bool> I=det_textures.insert(T->_Format().detail_name);
	            V=PHelper.CreateATexture(items,FHelper.PrepareKey(pref.c_str(),"Detail\\Texture"),	(AnsiString*)&*I.first); 	V->Owner()->Enable(FALSE);
	            PHelper.CreateCaption(items,FHelper.PrepareKey(pref.c_str(),"Detail\\Scale"),		AnsiString().sprintf("%3.2f",T->_Format().detail_scale));
            }
//            PHelper.CreateCaption(items,FHelper.PrepareKey(pref.c_str(),"Used In"),			objects);
        }
        xr_delete			(T);
    }
    UI.ProgressEnd			();
    temp.sprintf			("%d Kb",iFloor(base_mem_usage/1024));
    base_mem->ApplyValue	(&temp);

    PHelper.CreateCaption	(items,"Textures\\Detail\\Count",			det_textures.size());
    PropValue* det_mem		= PHelper.CreateCaption(items,"Textures\\Detail\\Memory Usage",	"");
    UI.ProgressStart(textures.size(),"Collect detail textures info: ");
    for (t_it=det_textures.begin(); t_it!=det_textures.end(); t_it++){
        UI.ProgressInc	(t_it->c_str());
        EImageThumbnail* T = xr_new<EImageThumbnail>(t_it->c_str(),EImageThumbnail::EITTexture,true);
        if (!T->Valid()){
        	ELog.Msg(mtError,"Can't get info from texture: '%s'",t_it->c_str());
        }else{
            int tex_mem		= T->MemoryUsage();
            det_mem_usage	+= tex_mem;
            AnsiString pref	= AnsiString("Textures\\Detail\\")+*t_it;
            PropValue*	V	= 0;
            V=PHelper.CreateATexture(items,FHelper.PrepareKey(pref.c_str(),"Texture"), 		(AnsiString*)&*t_it); 	V->Owner()->Enable(FALSE);
            PHelper.CreateCaption(items,FHelper.PrepareKey(pref.c_str(),"Format"),			T->FormatString());
            PHelper.CreateCaption(items,FHelper.PrepareKey(pref.c_str(),"Size"), 			AnsiString().sprintf("%d x %d x %s",T->_Width(),T->_Height(),T->_Format().HasAlpha()?"32b":"24b"));
            PHelper.CreateCaption(items,FHelper.PrepareKey(pref.c_str(),"Memory Usage"),	AnsiString().sprintf("%d Kb",iFloor(tex_mem/1024)));
        }
        xr_delete			(T);
    }
    UI.ProgressEnd			();
    temp.sprintf			("%d Kb",iFloor(det_mem_usage/1024));
    det_mem->ApplyValue		(&temp);

    temp.sprintf			("%d",det_textures.size()+textures.size());
    total_count->ApplyValue	(&temp);
    temp.sprintf			("%d Kb",iFloor((det_mem_usage+base_mem_usage)/1024));
    total_mem->ApplyValue	(&temp);
    
	// sound
    PHelper.CreateCaption(items,"Sounds\\Sources",				sound_source_cnt);
    PHelper.CreateCaption(items,"Sounds\\Waves\\Count",			waves.size());
    for (AStringSetIt w_it=waves.begin(); w_it!=waves.end(); w_it++)
        PHelper.CreateCaption(items,FHelper.PrepareKey("Sounds\\Waves",w_it->c_str()),"-");

    // particles
    PHelper.CreateCaption(items,"Particle System\\Sources",			pe_static_cnt);
    PHelper.CreateCaption(items,"Particle System\\Refs\\Count",		pe_static.size());
    for (AStringSetIt pe_it=pe_static.begin(); pe_it!=pe_static.end(); pe_it++)
        PHelper.CreateCaption(items,FHelper.PrepareKey("Particle System\\Refs",pe_it->c_str()),"-");
}
 

#include "stdafx.h"
#pragma hdrstop

#include "SceneSummaryInfo.h"
#include "ImageManager.h"
#include "EThumbnail.h"
#include "SceneSummaryInfo.h"
#include "Scene.h"
#include "SceneObject.h"
#include "ui_main.h"
#include "PropertiesList.h"

void SSceneSummary::FillProp(PropItemVec& items)
{
    // fill items
    PHelper().CreateCaption(items,"Level Name",					Scene->m_LevelOp.m_FNLevelPath.c_str());
    PHelper().CreateCaption(items,"Geometry\\Bounding\\Min", 	ref_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(bbox.min)));
    PHelper().CreateCaption(items,"Geometry\\Bounding\\Max", 	ref_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(bbox.max)));
    PHelper().CreateCaption(items,"Geometry\\Total Faces",     	ref_str().sprintf("%d",face_cnt));
    PHelper().CreateCaption(items,"Geometry\\Total Vertices",  	ref_str().sprintf("%d",vert_cnt));
    PHelper().CreateCaption(items,"Geometry\\MU Objects",	   	ref_str().sprintf("%d",mu_objects.size()));
    PHelper().CreateCaption(items,"Geometry\\MU References",   	ref_str().sprintf("%d",object_mu_ref_cnt));
    PHelper().CreateCaption(items,"Geometry\\LOD Objects",		ref_str().sprintf("%d",lod_objects.size()));
    PHelper().CreateCaption(items,"Geometry\\LOD References",	ref_str().sprintf("%d",object_lod_ref_cnt));
    PHelper().CreateCaption(items,"Visibility\\HOM Faces",		ref_str().sprintf("%d",hom_face_cnt));
    PHelper().CreateCaption(items,"Visibility\\HOM Vertices",	ref_str().sprintf("%d",hom_vert_cnt));
    PHelper().CreateCaption(items,"Visibility\\Sectors",		ref_str().sprintf("%d",sector_cnt));
    PHelper().CreateCaption(items,"Visibility\\Portals",		ref_str().sprintf("%d",portal_cnt));
    PHelper().CreateCaption(items,"Lights\\Count",				ref_str().sprintf("%d",light_point_cnt+light_spot_cnt));
    PHelper().CreateCaption(items,"Lights\\By Type\\Point",		ref_str().sprintf("%d",light_point_cnt));
    PHelper().CreateCaption(items,"Lights\\By Type\\Spot",		ref_str().sprintf("%d",light_spot_cnt));
    PHelper().CreateCaption(items,"Lights\\By Usage\\Dynamic",	ref_str().sprintf("%d",light_dynamic_cnt));
    PHelper().CreateCaption(items,"Lights\\By Usage\\Static",	ref_str().sprintf("%d",light_static_cnt));
    PHelper().CreateCaption(items,"Lights\\By Usage\\Breakable",ref_str().sprintf("%d",light_breakable_cnt));
    PHelper().CreateCaption(items,"Lights\\By Usage\\Procedural",ref_str().sprintf("%d",light_procedural_cnt));
    PHelper().CreateCaption(items,"Glows\\Count",				ref_str().sprintf("%d",glow_cnt));
    // textures
    int base_mem_usage		= 0; 
    int det_mem_usage		= 0;
    CaptionValue* total_count=PHelper().CreateCaption	(items,"Textures\\Count","");
    CaptionValue* total_mem	= PHelper().CreateCaption(items,"Textures\\Memory Usage","");
    PHelper().CreateCaption	(items,"Textures\\Base\\Count",		ref_str().sprintf("%d",textures.size()));
    CaptionValue* base_mem 	= PHelper().CreateCaption(items,"Textures\\Base\\Memory Usage",	"");
    SPBItem* pb = UI->PBStart(textures.size(),"Collect base textures info: ");
    for (RStringSetIt t_it=textures.begin(); t_it!=textures.end(); t_it++){
        UI->PBInc(pb,t_it->c_str());
        ETextureThumbnail* T 	= (ETextureThumbnail*)ImageLib.CreateThumbnail(t_it->c_str(),ECustomThumbnail::ETTexture,true);
        if (!T->Valid()){
        	ELog.Msg(mtError,"Can't get info from texture: '%s'",t_it->c_str());
        }else{
            int tex_mem		= T->MemoryUsage();
            base_mem_usage	+= tex_mem;
            AnsiString pref	= AnsiString("Textures\\Base\\")+**t_it;
            PropValue* V=0;
            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Texture"), 		(ref_str*)&*t_it, smTexture); V->Owner()->Enable(FALSE);
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Format"),			T->FormatString());
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Size"), 			ref_str().sprintf("%d x %d x %s",T->_Width(),T->_Height(),T->_Format().HasAlpha()?"32b":"24b"));
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Memory Usage"),	ref_str().sprintf("%d Kb",iFloor(tex_mem/1024)));
            if (T->_Format().flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail)){
            	if (*T->_Format().detail_name){
	            	std::pair<RStringSetIt, bool> I=det_textures.insert(*T->_Format().detail_name);
		            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Detail\\Texture"),	(ref_str*)&*I.first,smTexture); 	V->Owner()->Enable(FALSE);
	    	        PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Detail\\Scale"),		ref_str().sprintf("%3.2f",T->_Format().detail_scale));
                }else{
		        	ELog.Msg(mtError,"Empty details on texture: '%s'",t_it->c_str());
                }
            }
//            PHelper().CreateCaption(items,PHelper().PrepareKey(pref.c_str(),"Used In"),			objects);
        }
        xr_delete			(T);
    }
    UI->PBEnd(pb);
    base_mem->ApplyValue	(ref_str().sprintf("%d Kb",iFloor(base_mem_usage/1024)));

    PHelper().CreateCaption	(items,"Textures\\Detail\\Count",			ref_str().sprintf("%d",det_textures.size()));
    CaptionValue* det_mem	= PHelper().CreateCaption(items,"Textures\\Detail\\Memory Usage",	"");
    pb = UI->PBStart(det_textures.size(),"Collect detail textures info: ");
    for (t_it=det_textures.begin(); t_it!=det_textures.end(); t_it++){
        UI->PBInc(pb,t_it->c_str());
        ETextureThumbnail* T = (ETextureThumbnail*)ImageLib.CreateThumbnail(t_it->c_str(),ECustomThumbnail::ETTexture,true);
        if (!T->Valid()){
        	ELog.Msg(mtError,"Can't get info from texture: '%s'",t_it->c_str());
        }else{
            int tex_mem		= T->MemoryUsage();
            det_mem_usage	+= tex_mem;
            AnsiString pref	= AnsiString("Textures\\Detail\\")+**t_it;
            PropValue*	V	= 0;
            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Texture"), 		(ref_str*)&*t_it, smTexture); 	V->Owner()->Enable(FALSE);
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Format"),			T->FormatString());
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Size"), 			ref_str().sprintf("%d x %d x %s",T->_Width(),T->_Height(),T->_Format().HasAlpha()?"32b":"24b"));
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Memory Usage"),	ref_str().sprintf("%d Kb",iFloor(tex_mem/1024)));
        }
        xr_delete			(T);
    }
    UI->PBEnd(pb);
    det_mem->ApplyValue		(ref_str().sprintf("%d Kb",iFloor(det_mem_usage/1024)));
    total_count->ApplyValue	(ref_str().sprintf("%d",det_textures.size()+textures.size()));
    total_mem->ApplyValue	(ref_str().sprintf("%d Kb",iFloor((det_mem_usage+base_mem_usage)/1024)));

    // detail object textures
    int do_mem_usage		= 0;
    PHelper().CreateCaption	(items,"Detail Objects\\Textures\\Count",			ref_str().sprintf("%d",do_textures.size()));
    CaptionValue* do_mem	= PHelper().CreateCaption(items,"Detail Objects\\Textures\\Memory Usage",	"");
    pb = UI->PBStart(do_textures.size(),"Collect detail object textures info: ");
    for (t_it=do_textures.begin(); t_it!=do_textures.end(); t_it++){
        UI->PBInc(pb,t_it->c_str());
        ETextureThumbnail* T = (ETextureThumbnail*)ImageLib.CreateThumbnail(t_it->c_str(),ECustomThumbnail::ETTexture,true);
        if (!T->Valid()){
        	ELog.Msg(mtError,"Can't get info from texture: '%s'",t_it->c_str());
        }else{
            int tex_mem		= T->MemoryUsage();
            do_mem_usage	+= tex_mem;
            AnsiString pref	= AnsiString("Detail Objects\\Textures\\")+**t_it;
            PropValue*	V	= 0;
            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Texture"), 		(ref_str*)&*t_it, smTexture); 	V->Owner()->Enable(FALSE);
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Format"),			T->FormatString());
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Size"), 			ref_str().sprintf("%d x %d x %s",T->_Width(),T->_Height(),T->_Format().HasAlpha()?"32b":"24b"));
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Memory Usage"),	ref_str().sprintf("%d Kb",iFloor(tex_mem/1024)));
        }
        xr_delete			(T);
    }
    UI->PBEnd(pb);
    do_mem->ApplyValue		(ref_str().sprintf("%d Kb",iFloor(do_mem_usage/1024)));

	// sound
    PHelper().CreateCaption(items,"Sounds\\Sources",				ref_str().sprintf("%d",sound_source_cnt));
    PHelper().CreateCaption(items,"Sounds\\Waves\\Count",			ref_str().sprintf("%d",waves.size()));
    for (RStringSetIt w_it=waves.begin(); w_it!=waves.end(); w_it++)
        PHelper().CreateCaption(items,PrepareKey("Sounds\\Waves",w_it->c_str()),"-");

    // particles
    PHelper().CreateCaption(items,"Particle System\\Sources",		ref_str().sprintf("%d",pe_static_cnt));
    PHelper().CreateCaption(items,"Particle System\\Refs\\Count",	ref_str().sprintf("%d",pe_static.size()));
    for (RStringSetIt pe_it=pe_static.begin(); pe_it!=pe_static.end(); pe_it++)
        PHelper().CreateCaption(items,PrepareKey("Particle System\\Refs",pe_it->c_str()),"-");
}

static SSceneSummary s_summary;
void EScene::ShowSummaryInfo()
{
	s_summary.Clear	();
	bool bRes=true;

    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    for (; _I!=_E; _I++)
        if (_I->second)	_I->second->GetSummaryInfo(&s_summary);
    
	PropItemVec items;
	if (bRes){
        // fill items
        s_summary.FillProp(items);
        m_SummaryInfo->ShowProperties();
    }else{
    	ELog.DlgMsg(mtInformation,"Summary info empty.");
    }
	m_SummaryInfo->AssignItems(items);
}
//--------------------------------------------------------------------------------------------------

 

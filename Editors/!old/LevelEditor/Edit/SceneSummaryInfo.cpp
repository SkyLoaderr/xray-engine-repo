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
    PHelper().CreateCaption(items,"Geometry\\Bounding\\Min", 	shared_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(bbox.min)));
    PHelper().CreateCaption(items,"Geometry\\Bounding\\Max", 	shared_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(bbox.max)));
    PHelper().CreateCaption(items,"Geometry\\Total Faces",     	shared_str().sprintf("%d",face_cnt));
    PHelper().CreateCaption(items,"Geometry\\Total Vertices",  	shared_str().sprintf("%d",vert_cnt));
    PHelper().CreateCaption(items,"Geometry\\MU Objects",	   	shared_str().sprintf("%d",mu_objects.size()));
    PHelper().CreateCaption(items,"Geometry\\MU References",   	shared_str().sprintf("%d",object_mu_ref_cnt));
    PHelper().CreateCaption(items,"Geometry\\LOD Objects",		shared_str().sprintf("%d",lod_objects.size()));
    PHelper().CreateCaption(items,"Geometry\\LOD References",	shared_str().sprintf("%d",object_lod_ref_cnt));
    PHelper().CreateCaption(items,"Visibility\\HOM Faces",		shared_str().sprintf("%d",hom_face_cnt));
    PHelper().CreateCaption(items,"Visibility\\HOM Vertices",	shared_str().sprintf("%d",hom_vert_cnt));
    PHelper().CreateCaption(items,"Visibility\\Sectors",		shared_str().sprintf("%d",sector_cnt));
    PHelper().CreateCaption(items,"Visibility\\Portals",		shared_str().sprintf("%d",portal_cnt));
    PHelper().CreateCaption(items,"Lights\\Count",				shared_str().sprintf("%d",light_point_cnt+light_spot_cnt));
    PHelper().CreateCaption(items,"Lights\\By Type\\Point",		shared_str().sprintf("%d",light_point_cnt));
    PHelper().CreateCaption(items,"Lights\\By Type\\Spot",		shared_str().sprintf("%d",light_spot_cnt));
    PHelper().CreateCaption(items,"Lights\\By Usage\\Dynamic",	shared_str().sprintf("%d",light_dynamic_cnt));
    PHelper().CreateCaption(items,"Lights\\By Usage\\Static",	shared_str().sprintf("%d",light_static_cnt));
    PHelper().CreateCaption(items,"Lights\\By Usage\\Breakable",shared_str().sprintf("%d",light_breakable_cnt));
    PHelper().CreateCaption(items,"Lights\\By Usage\\Procedural",shared_str().sprintf("%d",light_procedural_cnt));
    PHelper().CreateCaption(items,"Glows\\Count",				shared_str().sprintf("%d",glow_cnt));
    // textures
    int base_mem_usage		= 0; 
    int det_mem_usage		= 0;
    CaptionValue* total_count=PHelper().CreateCaption	(items,"Textures\\Count","");
    CaptionValue* total_mem	= PHelper().CreateCaption(items,"Textures\\Memory Usage","");
    PHelper().CreateCaption	(items,"Textures\\Base\\Count",		shared_str().sprintf("%d",textures.size()));
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
            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Texture"), 		(shared_str*)&*t_it, smTexture); V->Owner()->Enable(FALSE);
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Format"),			T->FormatString());
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Size"), 			shared_str().sprintf("%d x %d x %s",T->_Width(),T->_Height(),T->_Format().HasAlpha()?"32b":"24b"));
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Memory Usage"),	shared_str().sprintf("%d Kb",iFloor(tex_mem/1024)));
            if (T->_Format().flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail)){
            	if (0!=T->_Format().detail_name.size()){
	            	std::pair<RStringSetIt, bool> I=det_textures.insert(*T->_Format().detail_name);
		            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Detail\\Texture"),	(shared_str*)&*I.first,smTexture); 	V->Owner()->Enable(FALSE);
	    	        PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Detail\\Scale"),		shared_str().sprintf("%3.2f",T->_Format().detail_scale));
                }else{
		        	ELog.Msg(mtError,"Empty details on texture: '%s'",t_it->c_str());
                }
            }
//            PHelper().CreateCaption(items,PHelper().PrepareKey(pref.c_str(),"Used In"),			objects);
        }
        xr_delete			(T);
    }
    UI->PBEnd(pb);
    base_mem->ApplyValue	(shared_str().sprintf("%d Kb",iFloor(base_mem_usage/1024)));

    PHelper().CreateCaption	(items,"Textures\\Detail\\Count",			shared_str().sprintf("%d",det_textures.size()));
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
            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Texture"), 		(shared_str*)&*t_it, smTexture); 	V->Owner()->Enable(FALSE);
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Format"),			T->FormatString());
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Size"), 			shared_str().sprintf("%d x %d x %s",T->_Width(),T->_Height(),T->_Format().HasAlpha()?"32b":"24b"));
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Memory Usage"),	shared_str().sprintf("%d Kb",iFloor(tex_mem/1024)));
        }
        xr_delete			(T);
    }
    UI->PBEnd(pb);
    det_mem->ApplyValue		(shared_str().sprintf("%d Kb",iFloor(det_mem_usage/1024)));
    total_count->ApplyValue	(shared_str().sprintf("%d",det_textures.size()+textures.size()));
    total_mem->ApplyValue	(shared_str().sprintf("%d Kb",iFloor((det_mem_usage+base_mem_usage)/1024)));

    // detail object textures
    int do_mem_usage		= 0;
    PHelper().CreateCaption	(items,"Detail Objects\\Textures\\Count",			shared_str().sprintf("%d",do_textures.size()));
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
            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Texture"), 		(shared_str*)&*t_it, smTexture); 	V->Owner()->Enable(FALSE);
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Format"),			T->FormatString());
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Size"), 			shared_str().sprintf("%d x %d x %s",T->_Width(),T->_Height(),T->_Format().HasAlpha()?"32b":"24b"));
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Memory Usage"),	shared_str().sprintf("%d Kb",iFloor(tex_mem/1024)));
        }
        xr_delete			(T);
    }
    UI->PBEnd(pb);
    do_mem->ApplyValue		(shared_str().sprintf("%d Kb",iFloor(do_mem_usage/1024)));

	// sound
    PHelper().CreateCaption(items,"Sounds\\Sources",				shared_str().sprintf("%d",sound_source_cnt));
    PHelper().CreateCaption(items,"Sounds\\Waves\\Count",			shared_str().sprintf("%d",waves.size()));
    for (RStringSetIt w_it=waves.begin(); w_it!=waves.end(); w_it++)
        PHelper().CreateCaption(items,PrepareKey("Sounds\\Waves",w_it->c_str()),"-");

    // particles
    PHelper().CreateCaption(items,"Particle System\\Sources",		shared_str().sprintf("%d",pe_static_cnt));
    PHelper().CreateCaption(items,"Particle System\\Refs\\Count",	shared_str().sprintf("%d",pe_static.size()));
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

 

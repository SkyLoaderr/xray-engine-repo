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
#include "Library.h"

xr_token summary_texture_type_tokens[]={
	{ "Base",			SSceneSummary::sttBase		},
	{ "Detail",			SSceneSummary::sttDetail	},
	{ "DO",				SSceneSummary::sttDO		},
	{ "Glow",			SSceneSummary::sttGlow		},
	{ "LOD",			SSceneSummary::sttLOD		},
	{ 0,				0							}
};

void SSceneSummary::Prepare()
{
	for (TISetIt t_it=textures.begin(); t_it!=textures.end(); t_it++){
        STextureInfo* info	= (STextureInfo*)(&(*t_it));
    	info->Prepare		();
    }
	for (OISetIt o_it=objects.begin(); o_it!=objects.end(); o_it++){
        SObjectInfo* info	= (SObjectInfo*)(&(*o_it));
    	info->Prepare		();
    }
}

xr_string _itoa(int val)
{
	string64 	tmp; 
    return 		itoa(val,tmp,10);
}
void SSceneSummary::SObjectInfo::Prepare()
{
	if (object_name.size()){
	    CEditableObject* O	= Lib.CreateEditObject(object_name.c_str());
        xr_string pref		= object_name.c_str();
        if (O){
        	SPairInfo 		tmp;
            tmp.first		= pref+"\\References"; 			
            tmp.second 		= _itoa(ref_count);					
            info.push_back	(tmp);
            tmp.first		= pref+"\\Geometry\\Faces"; 	tmp.second = _itoa(O->GetFaceCount());			info.push_back	(tmp);
            tmp.first		= pref+"\\Geometry\\Vertices"; 	tmp.second = _itoa(O->GetVertexCount());		info.push_back	(tmp);
            SurfaceVec& surfaces = O->Surfaces();
            for (SurfaceIt it=surfaces.begin(); it!=surfaces.end(); it++){
            	xr_string pr= pref+xr_string("\\Materials\\")+(*it)->_Name();
	            tmp.first	= pr+"\\Texture"; 		tmp.second = (*it)->_Texture();							info.push_back	(tmp);
	            tmp.first	= pr+"\\Faces"; 		tmp.second = _itoa(O->GetSurfFaceCount((*it)->_Name()));info.push_back	(tmp);
            }
	    	Lib.RemoveEditObject(O);
        }
    }else{
        Msg("!Empty object name found.");
    }
}
void SSceneSummary::SObjectInfo::FillProp(PropItemVec& items, LPCSTR main_pref)
{
	if (object_name.size()){
        for (PIVecIt it=info.begin(); it!=info.end(); it++)
	        PHelper().CreateCaption(items,PrepareKey(main_pref,it->first.c_str()),	it->second.c_str());
    }
}
void SSceneSummary::SObjectInfo::Export	(IWriter* F)
{
	string1024		tmp;
    for (PIVecIt it=info.begin(); it!=info.end(); it++){
        sprintf		(tmp,"%s=%s",it->first.c_str(),it->second.c_str());
		F->w_string	(tmp);
    }
}

void SSceneSummary::STextureInfo::Prepare	()
{
	if (file_name.size()){
        ETextureThumbnail* T 	= (ETextureThumbnail*)ImageLib.CreateThumbnail(file_name.c_str(),ECustomThumbnail::ETTexture,true);
        if (!T->Valid()){
            Msg("!Can't get info from texture: '%s'",file_name.c_str());
        }else{
            info			= T->_Format();
        }
        xr_delete			(T);
    }else{
        Msg("!Empty texture name found.");
    }
}

void SSceneSummary::STextureInfo::FillProp	(PropItemVec& items, LPCSTR main_pref, u32& mem_use)
{
	if (file_name.size()){
        int tex_mem			= info.MemoryUsage(*file_name);
        mem_use				+= tex_mem;
        AnsiString pref		= PrepareKey(AnsiString(main_pref).c_str(),*file_name).c_str();
        PropValue* V=0;
        V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Texture"), 		&file_name, smTexture); V->Owner()->Enable(FALSE);
        PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Format"),		info.FormatString());
        PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Size"), 			shared_str().sprintf("%d x %d x %s",info.width,info.height,info.HasAlpha()?"32b":"24b"));
        PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Memory Usage"),	shared_str().sprintf("%d Kb",iFloor(tex_mem/1024)));
        PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Effective Area"),shared_str().sprintf("%3.2f m^2",effective_area));
        if (info.flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail)){
            if (0!=info.detail_name.size()){
                V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Detail Texture"),	&info.detail_name,smTexture); 	V->Owner()->Enable(FALSE);
                PHelper().CreateCaption(items,PrepareKey(pref.c_str(), "Detail Scale"),		shared_str().sprintf("%3.2f",info.detail_scale));
            }else{
                ELog.Msg(mtError,"Empty details on texture: '%s'",*file_name);
            }
        }
    }
}
void SSceneSummary::STextureInfo::Export	(IWriter* F, u32& mem_use)
{
	string128		mask;
	string1024		tmp;
    strcpy			(mask,"%s=%s,%d,%d,%s,%d,%3.2f");
    if (info.flags.is_any(STextureParams::flDiffuseDetail|STextureParams::flBumpDetail)){
        if (0!=info.detail_name.size()){
        	strcat	(mask,",%s,%3.2f");
        }
    }
    int tex_mem		= info.MemoryUsage(*file_name);
    mem_use			+=tex_mem;
    sprintf			(tmp,mask,*file_name,info.FormatString(),
    				info.width,info.height,info.HasAlpha()?"present":"absent",
                    iFloor(tex_mem/1024),
                    effective_area,
                    *info.detail_name, info.detail_scale);
	F->w_string		(tmp);
}

void SSceneSummary::OnFileClick(PropValue* sender, bool& bModif, bool& bSafe)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0:{
    	xr_string fn = Scene->m_LevelOp.m_FNLevelPath.c_str();
    	if (EFS.GetSaveName(_import_,fn,0,2))
	    	ExportSummaryInfo(fn.c_str());
        ELog.DlgMsg(mtInformation,"Export completed.");
    }break;
	}
    bModif = false;
}
void SSceneSummary::ExportSummaryInfo(LPCSTR fn)
{
	IWriter* F 				= FS.w_open(fn);
    string256				tmp;
    // textures
    u32 total_mem_usage		= 0; 
    F->w_string				("[TEXTURES]");
    F->w_string				("texture name=format,width,height,alpha,mem usage (Kb),area,detail name,detail scale");
    for (u32 stt=sttFirst; stt<sttLast; stt++){
        u32 cur_mem_usage	= 0; 
        float cur_area		= 0; 
    	xr_string pref	= "[";
        pref				+= get_token_name(summary_texture_type_tokens,stt);
        pref				+= "]";
        F->w_string			(pref.c_str());
		for (TISetIt it=textures.begin(); it!=textures.end(); it++){
	        STextureInfo* info= (STextureInfo*)(&(*it));
	    	if (info->type==stt){ 
            	cur_area		+=info->effective_area;
		    	info->Export	(F,cur_mem_usage);
            }
        }
        total_mem_usage		+= cur_mem_usage;
        sprintf				(tmp,"%s mem usage - %d Kb",pref.c_str(),cur_mem_usage);
	    F->w_string			(tmp);
        sprintf				(tmp,"%s effective area - %3.2f m^2",pref.c_str(),cur_area);
	    F->w_string			(tmp);
    }
    sprintf					(tmp,"Total mem usage - %d Kb",total_mem_usage);
    F->w_string				(tmp);
    // objects
    F->w_string				("");
    sprintf					(tmp,"[OBJECTS]");	F->w_string(tmp);
    for (OISetIt o_it=objects.begin(); o_it!=objects.end(); o_it++){
    	SObjectInfo* info= (SObjectInfo*)(&(*o_it));
        info->Export		(F);
    }
    FS.w_close				(F);
}
void SSceneSummary::FillProp(PropItemVec& items)
{
    Prepare	();
    // fill items
    ButtonValue* B =PHelper().CreateButton (items,"Common\\File","Export...",0);
    B->OnBtnClickEvent.bind(this,&SSceneSummary::OnFileClick);
    // fill items
    PHelper().CreateCaption(items,"Common\\Level Name",			Scene->m_LevelOp.m_FNLevelPath.c_str());
    PHelper().CreateCaption(items,"Geometry\\Bounding\\Min", 	shared_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(bbox.min)));
    PHelper().CreateCaption(items,"Geometry\\Bounding\\Max", 	shared_str().sprintf("{%3.2f, %3.2f, %3.2f}",VPUSH(bbox.max)));
    PHelper().CreateCaption(items,"Geometry\\Mesh\\Total Faces",   	shared_str().sprintf("%d",face_cnt));
    PHelper().CreateCaption(items,"Geometry\\Mesh\\Total Vertices",	shared_str().sprintf("%d",vert_cnt));
    PHelper().CreateCaption(items,"Geometry\\MU\\Objects",	   	shared_str().sprintf("%d",mu_objects.size()));
    PHelper().CreateCaption(items,"Geometry\\MU\\References",   shared_str().sprintf("%d",object_mu_ref_cnt));
    PHelper().CreateCaption(items,"Geometry\\LOD\\Objects",		shared_str().sprintf("%d",lod_objects.size()));
    PHelper().CreateCaption(items,"Geometry\\LOD\\References",	shared_str().sprintf("%d",object_lod_ref_cnt));
    PHelper().CreateCaption(items,"Visibility\\HOM\\Faces",		shared_str().sprintf("%d",hom_face_cnt));
    PHelper().CreateCaption(items,"Visibility\\HOM\\Vertices",	shared_str().sprintf("%d",hom_vert_cnt));
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
    // objects
    for (OISetIt o_it=objects.begin(); o_it!=objects.end(); o_it++){
    	SObjectInfo* info= (SObjectInfo*)(&(*o_it));
        info->FillProp		(items,"Objects");
    }
    // textures
    CaptionValue* total_count=PHelper().CreateCaption	(items,"Textures\\Count","");
    CaptionValue* total_mem	= PHelper().CreateCaption	(items,"Textures\\Memory Usage","");
    u32 total_mem_usage		= 0; 
    for (u32 stt=sttFirst; stt<sttLast; stt++){
        u32 cur_mem_usage	= 0; 
        float cur_area		= 0; 
    	shared_str pref		= PrepareKey("Textures",get_token_name(summary_texture_type_tokens,stt));
	    CaptionValue* mem 	= PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Memory Usage").c_str(), "");
	    CaptionValue* area 	= PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Effective Area").c_str(), "");
		for (TISetIt it=textures.begin(); it!=textures.end(); it++){
	        STextureInfo* info= (STextureInfo*)(&(*it));
	    	if (info->type==stt){ 
            	cur_area	+=info->effective_area;
            	info->FillProp(items,pref.c_str(),cur_mem_usage);
            }
        }
	    mem->ApplyValue		(shared_str().sprintf("%d Kb",iFloor(cur_mem_usage/1024)));
	    area->ApplyValue 	(shared_str().sprintf("%3.2f m^2",cur_area));
        total_mem_usage		+= cur_mem_usage;
    }
    total_count->ApplyValue	(shared_str().sprintf("%d",		textures.size()));
    total_mem->ApplyValue	(shared_str().sprintf("%d Kb",	iFloor(total_mem_usage/1024)));
	// sound
    PHelper().CreateCaption	(items,"Sounds\\Occluder\\Faces",		shared_str().sprintf("%d",snd_occ_face_cnt));
    PHelper().CreateCaption	(items,"Sounds\\Occluder\\Vertices",		shared_str().sprintf("%d",snd_occ_vert_cnt));
    PHelper().CreateCaption	(items,"Sounds\\Sources",				shared_str().sprintf("%d",sound_source_cnt));
    PHelper().CreateCaption	(items,"Sounds\\Waves\\Count",			shared_str().sprintf("%d",waves.size()));
    for (RStringSetIt w_it=waves.begin(); w_it!=waves.end(); w_it++)
        PHelper().CreateCaption(items,PrepareKey("Sounds\\Waves",w_it->c_str()),"-");
    // particles
    PHelper().CreateCaption	(items,"Particle System\\Sources",		shared_str().sprintf("%d",pe_static_cnt));
    PHelper().CreateCaption	(items,"Particle System\\Refs\\Count",	shared_str().sprintf("%d",pe_static.size()));
    for (RStringSetIt pe_it=pe_static.begin(); pe_it!=pe_static.end(); pe_it++)
        PHelper().CreateCaption(items,PrepareKey("Particle System\\Refs",pe_it->c_str()),"-");
/*
    int det_mem_usage		= 0;
    CaptionValue* total_count=PHelper().CreateCaption	(items,"Textures\\Count","");
    CaptionValue* total_mem	= PHelper().CreateCaption(items,"Textures\\Memory Usage","");
    PHelper().CreateCaption	(items,"Textures\\Base\\Count",		shared_str().sprintf("%d",textures.size()));
    CaptionValue* base_mem 	= PHelper().CreateCaption(items,"Textures\\Base\\Memory Usage",	"");
    SPBItem* pb = UI->ProgressStart(textures.size(),"Collect base textures info: ");
    for (RStringSetIt t_it=textures.begin(); t_it!=textures.end(); t_it++){
        pb->Inc(t_it->c_str());
        ETextureThumbnail* T 	= (ETextureThumbnail*)ImageLib.CreateThumbnail(t_it->c_str(),ECustomThumbnail::ETTexture,true);
        if (!T->Valid()){
        	ELog.Msg(mtError,"Can't get info from texture: '%s'",t_it->c_str());
        }else{
            int tex_mem		= T->MemoryUsage();
            base_mem_usage	+= tex_mem;
            AnsiString pref	= AnsiString("Textures\\Base\\")+**t_it;
            PropValue* V=0;
            V=PHelper().CreateChoose(items,PrepareKey(pref.c_str(),"Texture"), 		(shared_str*)&*t_it, smTexture); V->Owner()->Enable(FALSE);
            PHelper().CreateCaption(items,PrepareKey(pref.c_str(),"Format"),		T->FormatString());
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
    UI->ProgressEnd(pb);
    base_mem->ApplyValue	(shared_str().sprintf("%d Kb",iFloor(base_mem_usage/1024)));

    PHelper().CreateCaption	(items,"Textures\\Detail\\Count",			shared_str().sprintf("%d",det_textures.size()));
    CaptionValue* det_mem	= PHelper().CreateCaption(items,"Textures\\Detail\\Memory Usage",	"");
    pb = UI->ProgressStart(det_textures.size(),"Collect detail textures info: ");
    for (t_it=det_textures.begin(); t_it!=det_textures.end(); t_it++){
        pb->Inc(t_it->c_str());
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
    UI->ProgressEnd(pb);
    det_mem->ApplyValue		(shared_str().sprintf("%d Kb",iFloor(det_mem_usage/1024)));
    total_count->ApplyValue	(shared_str().sprintf("%d",det_textures.size()+textures.size()));
    total_mem->ApplyValue	(shared_str().sprintf("%d Kb",iFloor((det_mem_usage+base_mem_usage)/1024)));

    // detail object textures
    int do_mem_usage		= 0;
    PHelper().CreateCaption	(items,"Detail Objects\\Textures\\Count",			shared_str().sprintf("%d",do_textures.size()));
    CaptionValue* do_mem	= PHelper().CreateCaption(items,"Detail Objects\\Textures\\Memory Usage",	"");
    pb = UI->ProgressStart(do_textures.size(),"Collect detail object textures info: ");
    for (t_it=do_textures.begin(); t_it!=do_textures.end(); t_it++){
        pb->Inc(t_it->c_str());
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
    UI->ProgressEnd(pb);
    do_mem->ApplyValue		(shared_str().sprintf("%d Kb",iFloor(do_mem_usage/1024)));
*/
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

 

#ifndef EditorChooseEventsH
#define EditorChooseEventsH

#include "ChooseTypes.h"
#include "SkeletonAnimated.h"
//#include "LevelGameDef.h"

void __stdcall  FillEntity(ChooseItemVec& items, void* param)
{
//.    AppendItem						(RPOINT_CHOOSE_NAME);
    CInifile::Root& data 			= pSettings->sections();
    for (CInifile::RootIt it=data.begin(); it!=data.end(); it++){
    	LPCSTR val;
    	if (it->line_exist("$spawn",&val))
            items.push_back(SChooseItem(*it->Name,""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  SelectSoundSource(SChooseItem* item, ECustomThumbnail*& thm, ref_sound& snd, PropItemVec& info_items)
{
	snd.create	(true,item->name.c_str(),0);
    thm 		= xr_new<ESoundThumbnail>(item->name.c_str());
}
//---------------------------------------------------------------------------
void __stdcall  FillSoundSource(ChooseItemVec& items, void* param)
{
    FS_QueryMap lst;
    if (SndLib->GetSounds(lst)){
	    FS_QueryPairIt  it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  FillSoundEnv(ChooseItemVec& items, void* param)
{
    AStringVec lst;
    if (SndLib->GetSoundEnvs(lst)){
	    AStringIt  it				= lst.begin();
    	AStringIt	_E				= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  FillObject(ChooseItemVec& items, void* param)
{
    FS_QueryMap lst;
    if (Lib.GetObjects(lst)){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  FillVisual(ChooseItemVec& items, void* param)
{
    FS_QueryMap lst;
    if (FS.file_list(lst,_game_meshes_,FS_ListFiles|FS_ClampExt,".ogf")){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
void __stdcall  SelectVisual(SChooseItem* item, ECustomThumbnail*& thm, ref_sound& snd, PropItemVec& info_items)
{
    AnsiString fn					= ChangeFileExt(item->name.c_str(),".ogf");
    IRender_Visual* visual			= ::Render->model_Create(fn.c_str());
    if (visual){ 
        PHelper().CreateCaption	(info_items,	"Source",	*visual->desc.source_file?*visual->desc.source_file:"unknown");
        PHelper().CreateCaption	(info_items, 	"Creator N",*visual->desc.create_name?*visual->desc.create_name:"unknown");
        PHelper().CreateCaption	(info_items,	"Creator T",Trim(AnsiString(ctime(&visual->desc.create_time))).c_str());
        PHelper().CreateCaption	(info_items,	"Modif N",	*visual->desc.modif_name ?*visual->desc.modif_name :"unknown");
        PHelper().CreateCaption	(info_items,	"Modif T",	Trim(AnsiString(ctime(&visual->desc.modif_time))).c_str());
        PHelper().CreateCaption	(info_items,	"Build N",	*visual->desc.build_name ?*visual->desc.build_name :"unknown");
        PHelper().CreateCaption	(info_items,	"Build T",	Trim(AnsiString(ctime(&visual->desc.build_time))).c_str());
    }
    ::Render->model_Delete(visual);
}
//---------------------------------------------------------------------------
void __stdcall  FillGameObjectMots(ChooseItemVec& items, void* param)
{
    FS_QueryMap lst;
    if (FS.file_list(lst,_game_meshes_,FS_ListFiles|FS_ClampExt,".omf")){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
void __stdcall  SelectGameObjectMots(SChooseItem* item, ECustomThumbnail*& thm, ref_sound& snd, PropItemVec& info_items)
{
}
//---------------------------------------------------------------------------
void __stdcall  FillGameAnim(ChooseItemVec& items, void* param)
{
    FS_QueryMap lst;
    if (FS.file_list(lst,"$game_anims$",FS_ListFiles,".anm,*.anms")){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __stdcall  FillLAnim(ChooseItemVec& items, void* param)
{
    LAItemVec& lst 					= LALib.Objects();
    LAItemIt it						= lst.begin();
    LAItemIt _E						= lst.end();
    for (; it!=_E; it++)			items.push_back(SChooseItem(*(*it)->cName,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillEShader(ChooseItemVec& items, void* param)
{
    CResourceManager::map_Blender& blenders = Device.Resources->_GetBlenders();
	CResourceManager::map_BlenderIt _S = blenders.begin();
	CResourceManager::map_BlenderIt _E = blenders.end();
	for (; _S!=_E; _S++)			items.push_back(SChooseItem(_S->first,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillCShader(ChooseItemVec& items, void* param)
{
    Shader_xrLCVec& shaders 		= Device.ShaderXRLC.Library();
	Shader_xrLCIt _F 				= shaders.begin();
	Shader_xrLCIt _E 				= shaders.end();
	for ( ;_F!=_E;_F++)				items.push_back(SChooseItem(_F->Name,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillPE(ChooseItemVec& items, void* param)
{
    for (PS::PEDIt E=::Render->PSLibrary.FirstPED(); E!=::Render->PSLibrary.LastPED(); E++)items.push_back(SChooseItem(*(*E)->m_Name,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillParticles(ChooseItemVec& items, void* param)
{
    for (PS::PEDIt E=::Render->PSLibrary.FirstPED(); E!=::Render->PSLibrary.LastPED(); E++)items.push_back(SChooseItem(*(*E)->m_Name,""));
    for (PS::PGDIt G=::Render->PSLibrary.FirstPGD(); G!=::Render->PSLibrary.LastPGD(); G++)items.push_back(SChooseItem(*(*G)->m_Name,""));
}
//---------------------------------------------------------------------------
void __stdcall  FillTexture(ChooseItemVec& items, void* param)
{
    FS_QueryMap	lst;
    if (ImageLib.GetTextures(lst)){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
void __stdcall  SelectTexture(SChooseItem* item, ECustomThumbnail*& thm, ref_sound& snd, PropItemVec& info_items)
{
	thm		= xr_new<ETextureThumbnail>(item->name.c_str());
}
//---------------------------------------------------------------------------
void __stdcall  FillGameMaterial(ChooseItemVec& items, void* param)
{
	GameMtlIt _F 					= GMLib.FirstMaterial();
	GameMtlIt _E 					= GMLib.LastMaterial();
	for ( ;_F!=_E;_F++)				items.push_back(SChooseItem(*(*_F)->m_Name,""));
}
//---------------------------------------------------------------------------

void __stdcall  FillSkeletonAnims(ChooseItemVec& items, void* param)
{
	IRender_Visual* V 				= ::Render->model_Create((LPCSTR)param);
    if (PSkeletonAnimated(V)){
        CSkeletonAnimated::accel_map *ll_motions	= PSkeletonAnimated(V)->LL_Motions();
        CSkeletonAnimated::accel_map::iterator _I, _E;
        _I							= ll_motions->begin();
        _E							= ll_motions->end();
        for (; _I!=_E; ++_I) 		items.push_back(SChooseItem(*_I->first,""));
    }
	::Render->model_Delete			(V);
}
void __stdcall  FillSkeletonBones(ChooseItemVec& items, void* param)
{
	IRender_Visual* V 				= ::Render->model_Create((LPCSTR)param);
    if (PKinematics(V)){
        CSkeletonAnimated::accel  	*ll_bones	= PKinematics(V)->LL_Bones();
        CSkeletonAnimated::accel::iterator _I, _E;
        _I							= ll_bones->begin();
        _E							= ll_bones->end();
        for (; _I!=_E; ++_I) 		items.push_back(SChooseItem(*_I->first,""));
    }
	::Render->model_Delete			(V);
}

void FillChooseEvents()
{
	TfrmChoseItem::AppendEvents	(smSoundSource,		"Select Sound Source",		FillSoundSource,	SelectSoundSource,	false);
	TfrmChoseItem::AppendEvents	(smSoundEnv,		"Select Sound Environment",	FillSoundEnv,		0,false);
	TfrmChoseItem::AppendEvents	(smObject,			"Select Library Object",	FillObject,			0,false);
	TfrmChoseItem::AppendEvents	(smEShader,			"Select Engine Shader",		FillEShader,		0,false);
	TfrmChoseItem::AppendEvents	(smCShader,			"Select Compiler Shader",	FillCShader,		0,false);
	TfrmChoseItem::AppendEvents	(smPE,				"Select Particle Effect",	FillPE,				0,false);
	TfrmChoseItem::AppendEvents	(smParticles,		"Select Particle System", 	FillParticles,		0,false);
	TfrmChoseItem::AppendEvents	(smTexture,			"Select Texture",			FillTexture,		SelectTexture,		false);
	TfrmChoseItem::AppendEvents	(smEntity,			"Select Entity",			FillEntity,			0,false);
	TfrmChoseItem::AppendEvents	(smLAnim,			"Select Light Animation",	FillLAnim,			0,false);
	TfrmChoseItem::AppendEvents	(smVisual,			"Select Visual",			FillVisual,			SelectVisual,		false);
	TfrmChoseItem::AppendEvents	(smSkeletonAnims,	"Select Skeleton Animation",FillSkeletonAnims,	0,false);
	TfrmChoseItem::AppendEvents	(smSkeletonBones,	"Select Skeleton Bones",	FillSkeletonBones,	0,false);
	TfrmChoseItem::AppendEvents	(smGameMaterial,	"Select Game Material",		FillGameMaterial,	0,false);
	TfrmChoseItem::AppendEvents	(smGameAnim,		"Select Animation",			FillGameAnim,		0,false);
	TfrmChoseItem::AppendEvents	(smGameSMotions,	"Select Game Object Motions",FillGameObjectMots,SelectGameObjectMots,false);
}
//---------------------------------------------------------------------------
#endif
 
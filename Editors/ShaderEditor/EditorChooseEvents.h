#ifndef EditorChooseEventsH
#define EditorChooseEventsH
void __fastcall FillEntity(ChooseItemVec& items)
{
//.    AppendItem						(RPOINT_CHOOSE_NAME);
    CInifile::Root& data 			= pSettings->sections();
    for (CInifile::RootIt it=data.begin(); it!=data.end(); it++){
    	LPCSTR val;
    	if (it->line_exist("$spawn",&val))
//			if (CInifile::IsBOOL(val))	
            items.push_back(SChooseItem(*it->Name,""));
    }
}
//---------------------------------------------------------------------------
void __fastcall FillSoundSource(ChooseItemVec& items)
{
    FS_QueryMap lst;
    if (SndLib->GetSounds(lst)){
	    FS_QueryPairIt  it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __fastcall FillSoundEnv(ChooseItemVec& items)
{
    AStringVec lst;
    if (SndLib->GetSoundEnvs(lst)){
	    AStringIt  it				= lst.begin();
    	AStringIt	_E				= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __fastcall FillObject(ChooseItemVec& items)
{
    FS_QueryMap lst;
    if (Lib.GetObjects(lst)){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __fastcall FillGameObject(ChooseItemVec& items)
{
    FS_QueryMap lst;
    if (FS.file_list(lst,_game_meshes_,FS_ListFiles|FS_ClampExt,".ogf")){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __fastcall FillGameAnim(ChooseItemVec& items)
{
    FS_QueryMap lst;
    if (FS.file_list(lst,"$game_anims$",FS_ListFiles,".anm,*.anms")){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __fastcall FillLAnim(ChooseItemVec& items)
{
    LAItemVec& lst 					= LALib.Objects();
    LAItemIt it						= lst.begin();
    LAItemIt _E						= lst.end();
    for (; it!=_E; it++)			items.push_back(SChooseItem((*it)->cName,""));
}
//---------------------------------------------------------------------------
void __fastcall FillEShader(ChooseItemVec& items)
{
    CResourceManager::map_Blender& blenders = Device.Resources->_GetBlenders();
	CResourceManager::map_BlenderIt _S = blenders.begin();
	CResourceManager::map_BlenderIt _E = blenders.end();
	for (; _S!=_E; _S++)			items.push_back(SChooseItem(_S->first,""));
}
//---------------------------------------------------------------------------
void __fastcall FillCShader(ChooseItemVec& items)
{
    Shader_xrLCVec& shaders 		= Device.ShaderXRLC.Library();
	Shader_xrLCIt _F 				= shaders.begin();
	Shader_xrLCIt _E 				= shaders.end();
	for ( ;_F!=_E;_F++)				items.push_back(SChooseItem(_F->Name,""));
}
//---------------------------------------------------------------------------
void __fastcall FillPE(ChooseItemVec& items)
{
    for (PS::PEDIt E=::Render->PSLibrary.FirstPED(); E!=::Render->PSLibrary.LastPED(); E++)items.push_back(SChooseItem(*(*E)->m_Name,""));
}
//---------------------------------------------------------------------------
void __fastcall FillParticles(ChooseItemVec& items)
{
    for (PS::PEDIt E=::Render->PSLibrary.FirstPED(); E!=::Render->PSLibrary.LastPED(); E++)items.push_back(SChooseItem(*(*E)->m_Name,""));
    for (PS::PGDIt G=::Render->PSLibrary.FirstPGD(); G!=::Render->PSLibrary.LastPGD(); G++)items.push_back(SChooseItem(*(*G)->m_Name,""));
}
//---------------------------------------------------------------------------
void __fastcall FillTexture(ChooseItemVec& items)
{
    FS_QueryMap	lst;
    if (ImageLib.GetTextures(lst)){
	    FS_QueryPairIt	it			= lst.begin();
    	FS_QueryPairIt	_E			= lst.end();
	    for (; it!=_E; it++)		items.push_back(SChooseItem(it->first.c_str(),""));
    }
}
//---------------------------------------------------------------------------
void __fastcall FillGameMaterial(ChooseItemVec& items)
{
	GameMtlIt _F 					= GMLib.FirstMaterial();
	GameMtlIt _E 					= GMLib.LastMaterial();
	for ( ;_F!=_E;_F++)				items.push_back(SChooseItem(*(*_F)->m_Name,""));
}

void FillChooseEvents()
{
	TfrmChoseItem::AppendEvents	(smSoundSource,	"Select Sound Source",		FillSoundSource,	0,false);
	TfrmChoseItem::AppendEvents	(smSoundEnv,	"Select Sound Environment",	FillSoundEnv,		0,false);
	TfrmChoseItem::AppendEvents	(smObject,		"Select Library Object",	FillObject,			0,false);
	TfrmChoseItem::AppendEvents	(smEShader,		"Select Engine Shader",		FillEShader,		0,false);
	TfrmChoseItem::AppendEvents	(smCShader,		"Select Compiler Shader",	FillCShader,		0,false);
	TfrmChoseItem::AppendEvents	(smPE,			"Select Particle Effect",	FillPE,				0,false);
	TfrmChoseItem::AppendEvents	(smParticles,	"Select Particle System", 	FillParticles,		0,false);
	TfrmChoseItem::AppendEvents	(smTexture,		"Select Texture",			FillTexture,		0,false);
	TfrmChoseItem::AppendEvents	(smEntity,		"Select Entity",			FillEntity,			0,false);
	TfrmChoseItem::AppendEvents	(smLAnim,		"Select Light Animation",	FillLAnim,			0,false);
	TfrmChoseItem::AppendEvents	(smGameObject,	"Select Game Object",		FillGameObject,		0,false);
	TfrmChoseItem::AppendEvents	(smGameMaterial,"Select Game Material",		FillGameMaterial,	0,false);
	TfrmChoseItem::AppendEvents	(smGameAnim,	"Select Animation",			FillGameAnim,		0,false);
}
//---------------------------------------------------------------------------
#endif
 
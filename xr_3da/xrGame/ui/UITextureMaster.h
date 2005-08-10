// file:		UITextureMaster.h
// description:	holds info about shared textures. able to initialize external
//				through IUITextureControl interface
// created:		11.05.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#pragma once

class IUISimpleTextureControl;

class CUITextureMaster{
public:
	static void ParseShTexInfo			(LPCSTR xml_file);
	static void FreeShTexInfo			();

//	static void InitTexture(const char* texture_name,		IUISimpleTextureControl* tc);
//	static void InitTexture(const xr_string& texture_name,	IUISimpleTextureControl* tc);
//	static void InitTexture(const xr_string& texture_name, const char* shader_name, IUISimpleTextureControl* tc);
	static void InitTexture(const char* texture_name, const char* shader_name, IUISimpleTextureControl* tc);
	static float GetTextureHeight(const char* texture_name);
	static float GetTextureWidth(const char* texture_name);

protected:
	IC	static bool IsSh					(const xr_string& texture_name);
	IC	static bool IsSh					(const char* texture_name);


	typedef shared_str region_name;
	typedef shared_str shader_name;
	typedef xr_map<region_name, Frect>				regions;
	typedef xr_map<region_name, Frect>::iterator	regions_it;
	typedef xr_map<shader_name, regions>			shared_textures;
	typedef xr_map<shader_name, regions>::iterator	shared_textures_it;

	static	shared_textures		m_shTex;
};
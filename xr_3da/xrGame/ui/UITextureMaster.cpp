// file:		UITextureMaster.h
// description:	holds info about shared textures. able to initialize external controls
//				through IUITextureControl interface
// created:		11.05.2005
// author:		Serge Vynnychenko
// mail:		narrator@gsc-game.kiev.ua
//
// copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UITextureMaster.h"
#include "uiabstract.h"
#include "xrXmlParser.h"

CUITextureMaster::shared_textures	CUITextureMaster::m_shTex;

void CUITextureMaster::ParseShTexInfo(LPCSTR xml_file){
	if (!xml_file)
		return;

	CUIXml xml;
	xml.Init(CONFIG_PATH, UI_PATH, xml_file);
	xr_string texture = xml.Read("file_name",0); 

	shared_textures_it	sht_it = m_shTex.find(texture);
	if (m_shTex.end() == sht_it)
	{
		int num = xml.GetNodesNum("",0,"texture");
		regions regs;
		for (int i = 0; i<num; i++)
		{
			Frect r;
			r.x1 = xml.ReadAttribFlt("texture",i,"x");
			r.x2 = xml.ReadAttribFlt("texture",i,"width") + r.x1;
			r.y1 = xml.ReadAttribFlt("texture",i,"y");
			r.y2 = xml.ReadAttribFlt("texture",i,"height") + r.y1;
			xr_string id = xml.ReadAttrib("texture",i,"id");

			regs.insert(mk_pair(id,r));
		}
		m_shTex.insert(mk_pair(texture, regs));
	}
}

void CUITextureMaster::FreeShTexInfo(){
	m_shTex.clear();
#if _DEBUG
	Msg(" -- clearing info about textures from XmlInit");
#endif
}

bool CUITextureMaster::IsSh(const xr_string& texture_name){
	return xr_string::npos == texture_name.find("\\");
}

void CUITextureMaster::InitTexture(const xr_string& texture_name,	IUISimpleTextureControl* tc){
	if (IsSh(texture_name))
	{
		shared_textures_it	sht_it;
		for (sht_it = m_shTex.begin(); sht_it != m_shTex.end(); sht_it++)
		{
			regions_it reg_it = (*sht_it).second.find(texture_name);
			if (reg_it != (*sht_it).second.end())
			{				
				tc->CreateShader((*sht_it).first.c_str());	// texture file name
				tc->SetOriginalRectEx((*reg_it).second);    // region on texture
				return;
			}
		}
	}
	tc->CreateShader(texture_name.c_str());
}

void CUITextureMaster::InitTexture(const char* texture_name, IUISimpleTextureControl* tc){
//	CTimer T;
//	T.Start();
//	Msg("----InitTexture-begin");
	xr_string tx = texture_name;
	InitTexture(tx, tc);
//	Msg("----InitTexture[%d]",T.GetElapsed_ms());
}

float CUITextureMaster::GetTextureHeight(const char* texture_name){
	if (IsSh(texture_name))
	{
		shared_textures_it	sht_it;
		for (sht_it = m_shTex.begin(); sht_it != m_shTex.end(); sht_it++)
		{
			regions_it reg_it = (*sht_it).second.find(texture_name);
			if (reg_it != (*sht_it).second.end())
			{				
				return (*reg_it).second.height();
			}
		}
	}
	R_ASSERT3(false,"CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return 0;
}

float CUITextureMaster::GetTextureWidth(const char* texture_name){
	if (IsSh(texture_name))
	{
		shared_textures_it	sht_it;
		for (sht_it = m_shTex.begin(); sht_it != m_shTex.end(); sht_it++)
		{
			regions_it reg_it = (*sht_it).second.find(texture_name);
			if (reg_it != (*sht_it).second.end())
			{				
				return (*reg_it).second.width();
			}
		}
	}
	R_ASSERT3(false,"CUITextureMaster::GetTextureHeight Can't find texture", texture_name);
	return 0;
}
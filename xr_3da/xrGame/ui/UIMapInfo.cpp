#include "StdAfx.h"
#include "UIMapInfo.h"
#include "UIScrollView.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "../string_table.h"

CUIMapInfo::CUIMapInfo(){
	m_view = xr_new<CUIScrollView>();	
	AttachChild(m_view);
}

CUIMapInfo::~CUIMapInfo(){
	xr_delete(m_view);
}

void CUIMapInfo::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
    m_view->Init(0,0,width,height);
	m_view->Init();
	m_view->SetFixedScrollBar(false);
}

#define ADD_TEXT(x,y,z)	text = *str_tbl.translate(x);										\
						text += ": ";														\
						text += txt_color_tag;												\
						if (ltx.line_exist("map_info",y))									\
							text += *str_tbl.translate(ltx.r_string_wb("map_info",y));		\
						else																\
							text += *str_tbl.translate(z);									\
						text += "%c<default>\\n";											\
						st = xr_new<CUIStatic>();											\
						st->SetFont(txt_font);												\
						st->SetTextColor(header_color);										\
						st->SetText(text.c_str());											\
						st->SetWidth(m_view->GetDesiredChildWidth());						\
						st->AdjustHeightToText();											\
						st->SetAutoDelete(true);											\
						m_view->AddWindow(st)												\

void CUIMapInfo::InitMap(const char* map_name){
	m_view->Clear();
	if (NULL == map_name)
		return;

	CStringTable str_tbl;

 	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, "ui_mapinfo.xml");
	R_ASSERT3(xml_result, "xml file not found", "ui_mapinfo.xml");


	CUIStatic* st;
    // try to find file with info
	xr_string info_path = "text\\map_desc\\";
	info_path += map_name;
	info_path += ".txt";

	if (FS.exist("$game_config$", info_path.c_str()))
	{
		string256 ltxPath;
		FS.update_path	(ltxPath, CONFIG_PATH, info_path.c_str());
		CInifile ltx	(ltxPath);
		xr_string text;


		//map name
		st = xr_new<CUIStatic>(); 
		CUIXmlInit::InitStatic(xml_doc,"map_name",0,st); 
		if (ltx.line_exist("map_info","name"))
			st->SetText(*ltx.r_string_wb("map_info", "name"));
		else
			st->SetText(map_name);
		st->SetWidth(m_view->GetDesiredChildWidth());
		st->AdjustHeightToText();
		st->SetAutoDelete(true);
		m_view->AddWindow(st);

		u32 header_color, txt_color;
		CGameFont* txt_font;
		CUIXmlInit::InitFont(xml_doc,"header",0,header_color, txt_font);
		txt_color = CUIXmlInit::GetARGB(xml_doc,"txt:text", 0);
		string64 txt_color_tag;
		sprintf(txt_color_tag, "%s<%u,%u,%u,%u>", "%c", (txt_color & 0xff000000)>>24, (txt_color & 0x00ff0000)>>16, (txt_color & 0x0000ff00) >> 8, txt_color & 0x000000ff);

		ADD_TEXT("Players",		"players",		"Unknown");		
		ADD_TEXT("Modes",		"modes",		"Unknown");		
		ADD_TEXT("Description",	"short_desc",	"There is no description available");

		if (ltx.line_exist("map_info","large_desc"))
			m_large_desc = str_tbl.translate(ltx.r_string_wb("map_info", "large_desc"));		
	}
	else
	{
		st = xr_new<CUIStatic>(); 
		CUIXmlInit::InitStatic(xml_doc,"map_name",0,st); 
		st->SetText(map_name);
		st->SetWidth(m_view->GetDesiredChildWidth());
		st->AdjustHeightToText();
		st->SetAutoDelete(true);
		m_view->AddWindow(st);
	}

	if (!m_large_desc)
        m_large_desc = str_tbl.translate("no_desc_for_this_map");
}

const char*	 CUIMapInfo::GetLargeDesc(){
	return *m_large_desc;
}

// UIXmlInit.cpp: класс инициализации элементов окошек при помощи XML
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIXmlInit.h"
#include "../hudmanager.h"
#include "../UI.h"
#include "../level.h"


#define HEADER_FONT_NAME		"header"
#define ARIAL_FONT_NAME			"arial"

#define BIG_FONT_NAME			"big"
#define NORMAL_FONT_NAME		"normal"
#define MEDIUM_FONT_NAME		"medium"
#define SMALL_FONT_NAME			"small"

#define GRAFFITI19_FONT_NAME	"graffiti19"
#define GRAFFITI22_FONT_NAME	"graffiti22"
#define GRAFFITI32_FONT_NAME	"graffiti32"
#define GRAFFITI50_FONT_NAME	"graffiti50"

#define LETTERICA16_FONT_NAME	"letterica16"
#define LETTERICA18_FONT_NAME	"letterica18"
#define LETTERICA25_FONT_NAME	"letterica25"


CUIXmlInit::CUIXmlInit()
{
}
CUIXmlInit::~CUIXmlInit()
{
}

bool CUIXmlInit::InitWindow(CUIXml& xml_doc, LPCSTR path, 	
							int index, CUIWindow* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");
	pWnd->Init(x, y, width, height);
	return true;
}
bool CUIXmlInit::InitFrameWindow(CUIXml& xml_doc, LPCSTR path, 
									int index, CUIFrameWindow* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	string256 buf;

	int x		= xml_doc.ReadAttribInt(path, index, "x");
	int y		= xml_doc.ReadAttribInt(path, index, "y");
	int width	= xml_doc.ReadAttribInt(path, index, "width");
	int height	= xml_doc.ReadAttribInt(path, index, "height");

	int	r		= xml_doc.ReadAttribInt(path, index, "r", 0xff);
	int	g		= xml_doc.ReadAttribInt(path, index, "g", 0xff);
	int	b		= xml_doc.ReadAttribInt(path, index, "b", 0xff);
	int	a		= xml_doc.ReadAttribInt(path, index, "a", 0xff);

	pWnd->SetColor(color_rgba(r, g, b, a));
	
	ref_str base_name = xml_doc.Read(strconcat(buf,path,":base_texture"), index, NULL);

	if(!base_name) return false;
		
	pWnd->Init(*base_name, x, y, width, height);

	strconcat(buf,path,":left_top_texture");
	ref_str tex_name = xml_doc.Read(buf, index, NULL);

	x = xml_doc.ReadAttribInt(buf, index, "x");
	y = xml_doc.ReadAttribInt(buf, index, "y");

	if(*tex_name) pWnd->InitLeftTop(*tex_name, x,y);


	strconcat(buf,path,":left_bottom_texture");
	tex_name = xml_doc.Read(buf, index, NULL);

	x = xml_doc.ReadAttribInt(buf, index, "x");
	y = xml_doc.ReadAttribInt(buf, index, "y");

	if(*tex_name) pWnd->InitLeftBottom(*tex_name, x,y);

	//инициализировать заголовок окна
	strconcat(buf,path,":title");
	if(xml_doc.NavigateToNode(buf,index)) InitStatic(xml_doc, buf, index, &pWnd->UITitleText);


	return true;
}


bool CUIXmlInit::InitStatic(CUIXml& xml_doc, LPCSTR path, 
									int index, CUIStatic* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	string256 buf;

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");

	ref_str scale_str = xml_doc.ReadAttrib(path, index, "scale", NULL);
	float scale = 1.f;
	if(*scale_str) scale = (float)atof(*scale_str);
	pWnd->SetTextureScale(scale);

	int stretch_flag = xml_doc.ReadAttribInt(path, index, "stretch");
	if(stretch_flag) 
		pWnd->SetStretchTexture(true);
	else
		pWnd->SetStretchTexture(false); 

	ref_str texture = xml_doc.Read(strconcat(buf,path,":texture"), index, NULL);

	if(*texture)
	{
		pWnd->Init(*texture, x, y, width, height);
	}
	else
	{
		pWnd->Init(x, y, width, height);
	}
	
	ref_str text_path = strconcat(buf,path,":text");
	u32 color;
	CGameFont *pTmpFont = NULL;
	InitFont(xml_doc, *text_path, index, color, pTmpFont);
	pWnd->SetTextColor(color);
	pWnd->SetFont(pTmpFont);

	int text_x = RecalcXForResolution(xml_doc.ReadAttribInt(*text_path, index, "x"));
	int text_y = RecalcYForResolution(xml_doc.ReadAttribInt(*text_path, index, "y"));
	ref_str text = xml_doc.Read(*text_path, index, NULL);

	pWnd->SetTextX(text_x);
	pWnd->SetTextY(text_y);
	pWnd->SetText(*text);

	return true;
}

bool CUIXmlInit::InitButton(CUIXml& xml_doc, LPCSTR path, 
						int index, CUIButton* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	string256 buf;

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");
	u32 accel = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "accel", -1));
	
	LPCSTR  texture = xml_doc.Read(strconcat(buf,path,":texture"), index, NULL);

	if(texture)
		pWnd->Init(texture, x, y, width, height);
	else
		pWnd->Init(x, y, width, height);
	
	pWnd->SetAccelerator(accel);

	// Init font from xml config file
	CGameFont *LocalFont = NULL;
	u32 cl;

	ref_str text_path = strconcat(buf,path,":text");
	InitFont(xml_doc, *text_path, index, cl, LocalFont);
	if (LocalFont)
	{
		pWnd->SetFont(LocalFont);
		pWnd->SetTextColor(cl);
	}

	LPCSTR  text = xml_doc.Read(buf, index, NULL);
	pWnd->SetText(text);
	
	return true;
}

bool CUIXmlInit::InitDragDropList(CUIXml& xml_doc, LPCSTR path, 
						int index, CUIDragDropList* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");


	pWnd->Init(x,y, width,height);

	int cell_width = xml_doc.ReadAttribInt(path, index, "cell_width");
	int cell_height = xml_doc.ReadAttribInt(path, index, "cell_height");
	int rows_num = xml_doc.ReadAttribInt(path, index, "rows_num");
	int cols_num = xml_doc.ReadAttribInt(path, index, "cols_num");
	int rows_num_view = xml_doc.ReadAttribInt(path, index, "rows_num_view");
	
	ref_str show_grid_str = xml_doc.ReadAttrib(path, index, "show_grid", NULL);
	int show_grid = 1;
	if(*show_grid_str) show_grid = atoi(*show_grid_str);

	pWnd->SetCellWidth(cell_width);
	pWnd->SetCellHeight(cell_height);
	if(show_grid)
		pWnd->InitGrid(rows_num, cols_num, true, rows_num_view);
	else
		pWnd->InitGrid(rows_num, cols_num, 0, rows_num_view);


	return true;
}

bool CUIXmlInit::InitListWnd(CUIXml& xml_doc, LPCSTR path, 
										   int index, CUIListWnd* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "Node not found");
	
	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");
	int item_height = xml_doc.ReadAttribInt(path, index, "item_height");
	int active_background = xml_doc.ReadAttribInt(path, index, "active_bg");

	// Init font from xml config file
	string256 buf;
	CGameFont *LocalFont = NULL;
	u32 cl;

	ref_str text_path = strconcat(buf,path,":font");
	InitFont(xml_doc, *text_path, index, cl, LocalFont);
	if (LocalFont)
	{
		pWnd->SetFont(LocalFont);
		pWnd->SetTextColor(cl);
	}

	pWnd->Init(x,y, width,height,item_height);
	pWnd->EnableActiveBackground(!!active_background);

	return true;
}

bool CUIXmlInit::InitProgressBar(CUIXml& xml_doc, LPCSTR path, 
						int index, CUIProgressBar* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	string256 buf;
	
	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "length");
	int height = xml_doc.ReadAttribInt(path, index, "broad");
	bool is_horizontal = (xml_doc.ReadAttribInt(path, index, "horz")==1);

	pWnd->Init(x, y, width, height, is_horizontal);

	int min = xml_doc.ReadAttribInt(path, index, "min");
	int max = xml_doc.ReadAttribInt(path, index, "max");
	int pos = xml_doc.ReadAttribInt(path, index, "pos");
	
	pWnd->SetRange((s16)min,(s16)max);
	pWnd->SetProgressPos((s16)pos);
	
	strconcat(buf,path,":texture");

	LPCSTR texture = xml_doc.Read(buf, index, NULL);
	if(!texture) return false;

	int progress_length = xml_doc.ReadAttribInt(buf, index, "length");
	bool tile = (xml_doc.ReadAttribInt(buf, index, "tile")==1);
	int r = xml_doc.ReadAttribInt(buf, index, "r");
	int g = xml_doc.ReadAttribInt(buf, index, "g");
	int b = xml_doc.ReadAttribInt(buf, index, "b");

	u32 color = RGB_ALPHA(0xFF, r,g,b);

	pWnd->SetProgressTexture(texture,progress_length,tile,color);


	texture = xml_doc.Read(strconcat(buf,path,":background"), index, NULL);
	x = xml_doc.ReadAttribInt(buf, index, "x");
	y = xml_doc.ReadAttribInt(buf, index, "y");

	pWnd->SetBackgroundTexture(texture,x,y);
	
	return true;
}

bool CUIXmlInit::InitAutoStatic(CUIXml& xml_doc, LPCSTR tag_name, CUIWindow* pParentWnd)
{
	int items_num = xml_doc.GetNodesNum(xml_doc.GetRoot(), tag_name);

	CUIStatic* pUIStatic = NULL;
	for(int i=0; i<items_num; i++)
	{
		pUIStatic = xr_new<CUIStatic>();
		InitStatic(xml_doc, tag_name, i, pUIStatic);
		pUIStatic->SetAutoDelete(true);
		pParentWnd->AttachChild(pUIStatic);
		pUIStatic = NULL;
	}
	return true;
}

bool CUIXmlInit::InitFont(CUIXml &xml_doc, LPCSTR path, int index, u32 &color, CGameFont *&pFnt)
{
	ref_str font_name = xml_doc.ReadAttrib(path, index, "font", NULL);

	int r = xml_doc.ReadAttribInt(path, index, "r");
	int g = xml_doc.ReadAttribInt(path, index, "g");
	int b = xml_doc.ReadAttribInt(path, index, "b");

	//чтоб не было тупых ошибок когда забыли поставить альфу
	ref_str alpha = xml_doc.ReadAttrib(path, index, "a", NULL);
	int a = 0xFF;
	if(*alpha) a = xml_doc.ReadAttribInt(path, index, "a");

	color = RGB_ALPHA(a,r,g,b);

	if(*font_name)
	{
		if(!xr_strcmp(*font_name, HEADER_FONT_NAME))
		{
			pFnt = HUD().pFontHeaderRussian;
		}
		else if(!xr_strcmp(*font_name, NORMAL_FONT_NAME) || !xr_strcmp(*font_name, GRAFFITI19_FONT_NAME))
		{
			pFnt = HUD().pFontGraffiti19Russian;
		}
		else if(!xr_strcmp(*font_name, GRAFFITI22_FONT_NAME))
		{
			pFnt = HUD().pFontGraffiti22Russian;
		}
		else if(!xr_strcmp(*font_name, GRAFFITI32_FONT_NAME))
		{
			pFnt = HUD().pFontGraffiti32Russian;
		}
		else if(!xr_strcmp(*font_name, GRAFFITI50_FONT_NAME))
		{
			pFnt = HUD().pFontGraffiti50Russian;
		}
		else if(!xr_strcmp(*font_name, ARIAL_FONT_NAME))
		{
			pFnt = HUD().pArialN21Russian;
		}
		else if(!xr_strcmp(*font_name, BIG_FONT_NAME))
		{
			pFnt = HUD().pFontBigDigit;
		}
		else if(!xr_strcmp(*font_name, MEDIUM_FONT_NAME))
		{
			pFnt = HUD().pFontMedium;
		}
		else if(!xr_strcmp(*font_name, SMALL_FONT_NAME))
		{
			pFnt = HUD().pFontSmall;
		}
		else if(!xr_strcmp(*font_name, LETTERICA16_FONT_NAME))
		{
			pFnt = HUD().pFontLetterica16Russian;
		}
		else if(!xr_strcmp(*font_name, LETTERICA18_FONT_NAME))
		{
			pFnt = HUD().pFontLetterica18Russian;
		}
		else if(!xr_strcmp(*font_name, LETTERICA25_FONT_NAME))
		{
			pFnt = HUD().pFontLetterica25;
		}
	}
	return true;
}

bool CUIXmlInit::InitTabControl(CUIXml &xml_doc, LPCSTR path, int index, CUITabControl *pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");
	
	bool status = true;

	status &= InitWindow(xml_doc, path, index, pWnd);
	int tabsCount = xml_doc.GetNodesNum(path, 0, "button");

	XML_NODE* tab_node = xml_doc.NavigateToNode(path,index);
	xml_doc.SetLocalRoot(tab_node);

	for (int i = 0; i < tabsCount; ++i)
	{
		CUIButton *newButton = xr_new<CUIButton>();
		status &= InitButton(xml_doc, "button", i, newButton);
		pWnd->AddItem(newButton);
	}
	
	xml_doc.SetLocalRoot(xml_doc.GetRoot());

	return status;
}

//////////////////////////////////////////////////////////////////////////


bool CUIXmlInit::InitFrameLine(CUIXml& xml_doc, const char* path, int index, CUIFrameLineWnd* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	string256 buf;

	int x			= xml_doc.ReadAttribInt(path, index, "x");
	int y			= xml_doc.ReadAttribInt(path, index, "y");
	int width		= xml_doc.ReadAttribInt(path, index, "width");
	int height		= xml_doc.ReadAttribInt(path, index, "height");
	bool vertical	= !!xml_doc.ReadAttribInt(path, index, "vertical");

	int	r			= xml_doc.ReadAttribInt(path, index, "r", 0xff);
	int	g			= xml_doc.ReadAttribInt(path, index, "g", 0xff);
	int	b			= xml_doc.ReadAttribInt(path, index, "b", 0xff);
	int	a			= xml_doc.ReadAttribInt(path, index, "a", 0xff);

	pWnd->SetColor(color_rgba(r, g, b, a));

	ref_str base_name = xml_doc.Read(strconcat(buf,path,":base_texture"), index, NULL);

	if(!base_name) return false;

	pWnd->Init(*base_name, x, y, width, height, !vertical);

	strconcat(buf,path,":title");
	if(xml_doc.NavigateToNode(buf,index)) InitStatic(xml_doc, buf, index, &pWnd->UITitleText);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitTextBanner(CUIXml &xml_doc, const char *path, int index, CUITextBanner *pBnr)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	std::map<ref_str, CUITextBanner::TextBannerStyles> conformityTable;
	conformityTable["none"]		= CUITextBanner::TextBannerStyles::tbsNone;
	conformityTable["fade"]		= CUITextBanner::TextBannerStyles::tbsFade;
	conformityTable["flicker"]	= CUITextBanner::TextBannerStyles::tbsFlicker;

//	XML_NODE *tab_node = xml_doc.NavigateToNode(path, index);
//	xml_doc.SetLocalRoot(tab_node);

	int animationsCount = xml_doc.GetNodesNum(path, index, "animation");

	XML_NODE *tab_node = xml_doc.NavigateToNode(path, index);
	XML_NODE *old_node = xml_doc.GetLocalRoot();
	xml_doc.SetLocalRoot(tab_node);

	ref_str a;

	for (int i = 0; i < animationsCount; ++i)
	{
		a						= xml_doc.ReadAttrib("animation", i, "anim", "none");
		EffectParams * param	= pBnr->SetStyleParams(conformityTable[a]);

		param->bCyclic			= !!xml_doc.ReadAttribInt("animation", i, "cyclic", 1);
		param->bOn				= !!xml_doc.ReadAttribInt("animation", i, "on", 1);
		param->fPeriod			= static_cast<float>(atof(xml_doc.ReadAttrib("animation", i, "period", "1")));
		param->iEffectStage		= xml_doc.ReadAttribInt("animation", i, "stage", 0);
	}

	xml_doc.SetLocalRoot(old_node);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitMultiTextStatic(CUIXml &xml_doc, const char *path, int index, CUIMultiTextStatic *pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	bool status = true;
	string128	buf;

	status &= InitStatic(xml_doc, path, index, pWnd);
	int phrasesCount = xml_doc.GetNodesNum(path, index, "phrase");

	strconcat(buf, path, ":phrase");
	XML_NODE* tab_node = xml_doc.NavigateToNode(path,index);
	xml_doc.SetLocalRoot(tab_node);

	CUIMultiTextStatic::SinglePhrase * p;
	u32	argb = 0;
	const char * const ph = "phrase";

	for (int i = 0; i < phrasesCount; ++i)
	{
		p = pWnd->AddPhrase();

		status	&= InitTextBanner(xml_doc, ph, i, &p->effect);
		p->outX	= static_cast<float>(xml_doc.ReadAttribInt(ph, i, "x", 0));
		p->outY = static_cast<float>(xml_doc.ReadAttribInt(ph, i, "y", 0));

		CGameFont *pFont;
		InitFont(xml_doc, ph, i, argb, pFont);
        p->effect.SetFont(pFont);
		p->effect.SetTextColor(argb);
	}

	xml_doc.SetLocalRoot(xml_doc.GetRoot());

	return status;
}

//////////////////////////////////////////////////////////////////////////

int CUIXmlInit::RecalcXForResolution(int origninalCoordinate)
{
	return static_cast<int>(origninalCoordinate * (static_cast<float>(Device.dwWidth) / UI_BASE_WIDTH));
}

//////////////////////////////////////////////////////////////////////////

int CUIXmlInit::RecalcYForResolution(int origninalCoordinate)
{
	return static_cast<int>(origninalCoordinate * (static_cast<float>(Device.dwHeight) / UI_BASE_HEIGHT));
}
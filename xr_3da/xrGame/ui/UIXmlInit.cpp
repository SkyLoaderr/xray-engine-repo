// UIXmlInit.cpp: класс инициализации элементов окошек при помощи XML
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIXmlInit.h"
#include "../hudmanager.h"
#include "../UI.h"
#include "../level.h"
#include "../UICustomItem.h"

#include "../string_table.h"
#include "UIFrameWindow.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIListWnd.h"
#include "UITabControl.h"
#include "UIFrameLineWnd.h"
#include "UITextBanner.h"
#include "UIMultiTextStatic.h"
#include "UIAnimatedStatic.h"

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

//////////////////////////////////////////////////////////////////////////

const char * const		COLOR_DEFINITIONS					= "color_defs.xml";

CUIXmlInit::ColorDefs	*CUIXmlInit::m_pColorDefs			= NULL;

//////////////////////////////////////////////////////////////////////////

CUIXmlInit::CUIXmlInit()
{
	// Init colors
	InitColorDefs();
}

//////////////////////////////////////////////////////////////////////////

CUIXmlInit::~CUIXmlInit()
{
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitWindow(CUIXml& xml_doc, LPCSTR path, 	
							int index, CUIWindow* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	InitAlignment(xml_doc, path, index, x, y);
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");
	pWnd->Init(x, y, width, height);
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitFrameWindow(CUIXml& xml_doc, LPCSTR path, 
									int index, CUIFrameWindow* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	string256 buf;

	int x		= xml_doc.ReadAttribInt(path, index, "x");
	int y		= xml_doc.ReadAttribInt(path, index, "y");
	int width	= xml_doc.ReadAttribInt(path, index, "width");
	int height	= xml_doc.ReadAttribInt(path, index, "height");

	InitAlignment(xml_doc, path, index, x, y);

	int	r		= xml_doc.ReadAttribInt(path, index, "r", 0xff);
	int	g		= xml_doc.ReadAttribInt(path, index, "g", 0xff);
	int	b		= xml_doc.ReadAttribInt(path, index, "b", 0xff);
	int	a		= xml_doc.ReadAttribInt(path, index, "a", 0xff);

	pWnd->SetColor(color_rgba(r, g, b, a));
	
	shared_str base_name = xml_doc.Read(strconcat(buf,path,":base_texture"), index, NULL);

	if(!base_name) return false;
		
	pWnd->Init(*base_name, x, y, width, height);

	strconcat(buf,path,":left_top_texture");
	shared_str tex_name = xml_doc.Read(buf, index, NULL);

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

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitStatic(CUIXml& xml_doc, LPCSTR path, 
									int index, CUIStatic* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	string256 buf;

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");

	InitAlignment(xml_doc, path, index, x, y);

	shared_str scale_str = xml_doc.ReadAttrib(path, index, "scale", NULL);
	float scale = 1.f;
	if(*scale_str) scale = (float)atof(*scale_str);
	pWnd->SetTextureScale(scale);

	int stretch_flag = xml_doc.ReadAttribInt(path, index, "stretch");
	if(stretch_flag) 
		pWnd->SetStretchTexture(true);
	else
		pWnd->SetStretchTexture(false); 

	pWnd->Init(x, y, width, height);

	InitTexture(xml_doc, path, index, pWnd);
	
	shared_str text_path = strconcat(buf,path,":text");
	u32 color;
	CGameFont *pTmpFont = NULL;
	InitFont(xml_doc, *text_path, index, color, pTmpFont);
	pWnd->SetTextColor(color);
	pWnd->SetFont(pTmpFont);
	// Load font alignment
	shared_str al = xml_doc.ReadAttrib(*text_path, index, "align", "l");
	if (0 == xr_strcmp(al, "c"))
		pWnd->SetTextAlign(CGameFont::alCenter);
	else if (0 == xr_strcmp(al, "r"))
		pWnd->SetTextAlign(CGameFont::alRight);
	else if (0 == xr_strcmp(al, "l"))
		pWnd->SetTextAlign(CGameFont::alLeft);

	// Text coordinates
	int text_x = xml_doc.ReadAttribInt(*text_path, index, "x", -1);
	int text_y = xml_doc.ReadAttribInt(*text_path, index, "y", -1);
	shared_str text = xml_doc.Read(*text_path, index, NULL);

	pWnd->SetTextX(text_x);
	pWnd->SetTextY(text_y);

	CStringTable st;
	pWnd->SetText(*st(*text));

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitButton(CUIXml& xml_doc, LPCSTR path, 
						int index, CUIButton* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	InitStatic(xml_doc, path, index, pWnd);

	u32 accel = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "accel", -1));
	pWnd->SetAccelerator(accel);

	u32 hA = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hA", 255));
	u32 hR = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hR", 153));
	u32 hG = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hG", 153));
	u32 hB = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hB", 153));

	int shadowOffsetX	= xml_doc.ReadAttribInt(path, index, "shadow_offset_x", 0);
	int shadowOffsetY	= xml_doc.ReadAttribInt(path, index, "shadow_offset_y", 0);

	int pushOffsetX		= xml_doc.ReadAttribInt(path, index, "push_off_x", 2);
	int pushOffsetY		= xml_doc.ReadAttribInt(path, index, "push_off_y", 3);

	pWnd->SetHighlightColor(color_argb(hA, hR, hG, hB));
	pWnd->SetShadowOffset(shadowOffsetX, shadowOffsetY);
	pWnd->SetPushOffsetX(pushOffsetX);
	pWnd->SetPushOffsetY(pushOffsetY);
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitDragDropList(CUIXml& xml_doc, LPCSTR path, 
						int index, CUIDragDropList* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y);

	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");


	pWnd->Init(x,y, width,height);

	int cell_width = xml_doc.ReadAttribInt(path, index, "cell_width");
	int cell_height = xml_doc.ReadAttribInt(path, index, "cell_height");
	int rows_num = xml_doc.ReadAttribInt(path, index, "rows_num");
	int cols_num = xml_doc.ReadAttribInt(path, index, "cols_num");
	int rows_num_view = xml_doc.ReadAttribInt(path, index, "rows_num_view");
	
	shared_str show_grid_str = xml_doc.ReadAttrib(path, index, "show_grid", NULL);
	int show_grid = 1;
	if(*show_grid_str) show_grid = atoi(*show_grid_str);

	int unlimitedCapacity = xml_doc.ReadAttribInt(path, index, "unlimited", 0);
	pWnd->SetUnlimitedCapacity(0 != unlimitedCapacity);

	pWnd->SetCellWidth(cell_width);
	pWnd->SetCellHeight(cell_height);
	if(show_grid)
		pWnd->InitGrid(rows_num, cols_num, true, rows_num_view);
	else
		pWnd->InitGrid(rows_num, cols_num, 0, rows_num_view);


	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitListWnd(CUIXml& xml_doc, LPCSTR path, 
										   int index, CUIListWnd* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "Node not found");
	
	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y);

	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");
	int item_height = xml_doc.ReadAttribInt(path, index, "item_height");
	int active_background = xml_doc.ReadAttribInt(path, index, "active_bg");

	// Init font from xml config file
	string256 buf;
	CGameFont *LocalFont = NULL;
	u32 cl;

	shared_str text_path = strconcat(buf,path,":font");
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

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitProgressBar(CUIXml& xml_doc, LPCSTR path, 
						int index, CUIProgressBar* pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	string256 buf;
	
	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y);

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

//////////////////////////////////////////////////////////////////////////

CUIXmlInit::StaticsVec CUIXmlInit::InitAutoStatic(CUIXml& xml_doc, LPCSTR tag_name, CUIWindow* pParentWnd)
{
	int items_num = xml_doc.GetNodesNum(xml_doc.GetRoot(), tag_name);
	// tmp statics vector
	StaticsVec	tmpVec;

	CUIStatic* pUIStatic = NULL;
	for(int i=0; i<items_num; i++)
	{
		pUIStatic = xr_new<CUIStatic>();
		InitStatic(xml_doc, tag_name, i, pUIStatic);
		pUIStatic->SetAutoDelete(true);
		pParentWnd->AttachChild(pUIStatic);
		tmpVec.push_back(pUIStatic);
		pUIStatic = NULL;
	}

	return tmpVec;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitColor  (CUIXml &xml_doc, XML_NODE* pNode, u32 &color)
{
	VERIFY(pNode);

	int r = xml_doc.ReadAttribInt(pNode, "r", 255);
	int g = xml_doc.ReadAttribInt(pNode, "g", 255);
	int b = xml_doc.ReadAttribInt(pNode, "b", 255);
	int	a = xml_doc.ReadAttribInt(pNode, "a", 255);

	color = RGB_ALPHA(a,r,g,b);

	return true;
}

bool CUIXmlInit::InitFont(CUIXml &xml_doc, LPCSTR path, int index, u32 &color, CGameFont *&pFnt)
{
	shared_str font_name = xml_doc.ReadAttrib(path, index, "font", NULL);
	if(*font_name == NULL || xr_strlen(*font_name)<1)
	{
		pFnt = NULL;
		return false;
	}

	InitColor(xml_doc, xml_doc.NavigateToNode(path, index), color);

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

//////////////////////////////////////////////////////////////////////////

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

	InitAlignment(xml_doc, path, index, x, y);

	int width		= xml_doc.ReadAttribInt(path, index, "width");
	int height		= xml_doc.ReadAttribInt(path, index, "height");
	bool vertical	= !!xml_doc.ReadAttribInt(path, index, "vertical");

	int	r			= xml_doc.ReadAttribInt(path, index, "r", 0xff);
	int	g			= xml_doc.ReadAttribInt(path, index, "g", 0xff);
	int	b			= xml_doc.ReadAttribInt(path, index, "b", 0xff);
	int	a			= xml_doc.ReadAttribInt(path, index, "a", 0xff);

	pWnd->SetColor(color_rgba(r, g, b, a));

	shared_str base_name = xml_doc.Read(strconcat(buf,path,":base_texture"), index, NULL);

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

	std::map<shared_str, CUITextBanner::TextBannerStyles> conformityTable;
	conformityTable["none"]		= CUITextBanner::tbsNone;
	conformityTable["fade"]		= CUITextBanner::tbsFade;
	conformityTable["flicker"]	= CUITextBanner::tbsFlicker;

//	XML_NODE *tab_node = xml_doc.NavigateToNode(path, index);
//	xml_doc.SetLocalRoot(tab_node);

	int animationsCount = xml_doc.GetNodesNum(path, index, "animation");

	XML_NODE *tab_node = xml_doc.NavigateToNode(path, index);
	XML_NODE *old_node = xml_doc.GetLocalRoot();
	xml_doc.SetLocalRoot(tab_node);

	shared_str a;

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

		status			&= InitTextBanner(xml_doc, ph, i, &p->effect);
		p->outX			= static_cast<float>(xml_doc.ReadAttribInt(ph, i, "x", 0));
		p->outY			= static_cast<float>(xml_doc.ReadAttribInt(ph, i, "y", 0));
		p->maxWidth		= xml_doc.ReadAttribInt(ph, i, "width", -1);

		CGameFont *pFont;
		InitFont(xml_doc, ph, i, argb, pFont);
        p->effect.SetFont(pFont);
		p->effect.SetTextColor(argb);

		CStringTable st;

		p->str =  st(xml_doc.Read(ph, i));
	}

	xml_doc.SetLocalRoot(xml_doc.GetRoot());

	return status;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitAnimatedStatic(CUIXml &xml_doc, const char *path, int index, CUIAnimatedStatic *pWnd)
{
	R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

	InitStatic(xml_doc, path, index, pWnd);

	u32 framesCount		= static_cast<u32>(xml_doc.ReadAttribInt(path, index, "frames", 0));
	u32 animDuration	= static_cast<u32>(xml_doc.ReadAttribInt(path, index, "duration", 0));
	u32 animCols		= static_cast<u32>(xml_doc.ReadAttribInt(path, index, "columns", 0));
	u32 frameWidth		= static_cast<u32>(xml_doc.ReadAttribInt(path, index, "frame_width", 0));
	u32 frameHeight		= static_cast<u32>(xml_doc.ReadAttribInt(path, index, "frame_height", 0));
	bool cyclic			= !!xml_doc.ReadAttribInt(path, index, "cyclic", 0);
	bool play			= !!xml_doc.ReadAttribInt(path, index, "autoplay", 0);

	pWnd->SetFrameDimentions(frameWidth, frameHeight);
	pWnd->SetFramesCount(framesCount);
	pWnd->m_bCyclic = cyclic;
	pWnd->SetAnimCols(animCols);
	pWnd->SetAnimationDuration(animDuration);
	if (play) pWnd->Play();

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitTexture(CUIXml &xml_doc, const char *path, int index, CUIStatic *pWnd)
{
	string256 buf;
	if (0 == xr_strcmp(path, ""))
	{
		strcpy(buf, "texture");
	}
	else
	{
		strconcat(buf, path, ":texture");
	}

	shared_str texture = xml_doc.Read(buf, index, NULL);
	
	if (0 == texture.size()) return false;

	pWnd->InitTexture(*texture);

	int x			= xml_doc.ReadAttribInt(buf, index, "x", 0);
	int y			= xml_doc.ReadAttribInt(buf, index, "y", 0);
	int width		= xml_doc.ReadAttribInt(buf, index, "width", 0);
	int height		= xml_doc.ReadAttribInt(buf, index, "height", 0);
	int	a			= xml_doc.ReadAttribInt(buf, index, "a", 255);
	int	r			= xml_doc.ReadAttribInt(buf, index, "r", 255);
	int	g			= xml_doc.ReadAttribInt(buf, index, "g", 255);
	int	b			= xml_doc.ReadAttribInt(buf, index, "b", 255);
	shared_str mirrorM = xml_doc.ReadAttrib(buf, index, "mirror", "none");

	if (0 == xr_strcmp(mirrorM, "v"))
		pWnd->GetUIStaticItem().SetMirrorMode(tmMirrorVertical);
	else if (0 == xr_strcmp(mirrorM, "h"))
		pWnd->GetUIStaticItem().SetMirrorMode(tmMirrorHorisontal);
	else if (0 == xr_strcmp(mirrorM, "both"))
		pWnd->GetUIStaticItem().SetMirrorMode(tmMirrorBoth);


	pWnd->SetColor(color_rgba(r, g, b, a));

	if (width != 0 && height != 0)
	{
		pWnd->GetUIStaticItem().SetOriginalRect(x, y, width, height);
		pWnd->ClipperOn();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

int CUIXmlInit::ApplyAlignX(int coord, u32 align)
{
	// Применить выравнивание, только при увеличенном разрешения
	if (UI_BASE_WIDTH >= Device.dwWidth) return coord;

	int retVal = coord;
	
	if (align & alRight)
	{
		retVal = UI_BASE_WIDTH - coord;
		retVal = Device.dwWidth - retVal;
	}
	else if (align & alCenter)
	{
		retVal += (Device.dwWidth - UI_BASE_WIDTH) / 2;
	}

	return retVal;
}

//////////////////////////////////////////////////////////////////////////

int CUIXmlInit::ApplyAlignY(int coord, u32 align)
{
	// Применить выравнивание, только при увеличенном разрешения
	if (UI_BASE_HEIGHT >= Device.dwHeight) return coord;

	int retVal = coord;

	if (align & alBottom)
	{
		retVal = UI_BASE_HEIGHT - coord;
		retVal = Device.dwHeight - retVal;
	}
	else if (align & alCenter)
	{
		retVal += (Device.dwHeight - UI_BASE_HEIGHT) / 2;
	}

	return retVal;
}

//////////////////////////////////////////////////////////////////////////

void CUIXmlInit::ApplyAlign(int &x, int &y, u32 align)
{
	x = ApplyAlignX(x, align);
	y = ApplyAlignY(y, align);
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitAlignment(CUIXml &xml_doc, const char *path,
							   int index, int &x, int &y)
{
	// Alignment: right: "r", bottom: "b". Top, left - useless
	shared_str	alignStr = xml_doc.ReadAttrib(path, index, "align", "");

    bool result = false;

	if (strchr(*alignStr, 'r'))
	{
		x = ApplyAlignX(x, alRight);
		result = true;
	}
	if (strchr(*alignStr, 'b'))
	{
		y = ApplyAlignY(y, alBottom);
		result = true;
	}
	if (strchr(*alignStr, 'c'))
	{
		ApplyAlign(x, y, alCenter);
		result = true;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////

void CUIXmlInit::InitColorDefs()
{
	if (NULL != m_pColorDefs) return;

	m_pColorDefs = xr_new<ColorDefs>();

	CUIXml uiXml;
	bool flag = uiXml.Init("$game_data$", COLOR_DEFINITIONS);
	R_ASSERT3(flag, "xml file not found", COLOR_DEFINITIONS);

	int num = uiXml.GetNodesNum("colors", 0, "color");

	shared_str name;
	int r, b, g, a;

	for (int i = 0; i < num; ++i)
	{
		name	= uiXml.ReadAttrib("color", i, "name", "");
		r		= uiXml.ReadAttribInt("color", i, "r", 0);
		g		= uiXml.ReadAttribInt("color", i, "g", 0);
		b		= uiXml.ReadAttribInt("color", i, "b", 0);
		a		= uiXml.ReadAttribInt("color", i, "a", 255);
		
		m_pColorDefs->push_back(std::make_pair<shared_str, u32>(name, (a<<24) | (r<<16) | (g<<8) | b));
	}
}
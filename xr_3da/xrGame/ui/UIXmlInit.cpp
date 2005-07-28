// UIXmlInit.cpp: ����� ������������� ��������� ������ ��� ������ XML
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
#include "UICheckButton.h" // #include "UI3tButton.h" // #include "UIButton.h"
#include "UICustomSpin.h"
#include "UIRadioButton.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIListWnd.h"
#include "UITabControl.h"
//#include "UIFrameLineWnd.h"
#include "UILabel.h"
#include "UIEditBox.h"
#include "UITextBanner.h"
#include "UIMultiTextStatic.h"
#include "UIAnimatedStatic.h"
#include "uixmlinit.h"
#include "uiartefactpanel.h"
#include "UIScrollView.h"

#include "UITextureMaster.h"

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

CUIXmlInit::ColorDefs		*CUIXmlInit::m_pColorDefs			= NULL;
//CUIXmlInit::shared_textures	CUIXmlInit::m_shTex;

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
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	float x = xml_doc.ReadAttribFlt(path, index, "x");
	float y = xml_doc.ReadAttribFlt(path, index, "y");
	InitAlignment(xml_doc, path, index, x, y, pWnd);
	float width = xml_doc.ReadAttribFlt(path, index, "width");
	float height = xml_doc.ReadAttribFlt(path, index, "height");
	pWnd->Init(x, y, width, height);

   	string512 buf;
	CGameFont *LocalFont = NULL;
	u32 cl;

	strconcat(buf,path,":font");
	InitFont(xml_doc, buf, index, cl, LocalFont);
	if (LocalFont)
	{
		pWnd->SetFont(LocalFont);
//		pWnd->SetTextColor(cl);
	}
	strconcat(buf,path,":window_name");
	if(xml_doc.NavigateToNode(buf,index)){
		pWnd->SetWindowName( xml_doc.Read(buf, index, NULL) );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitFrameWindow(CUIXml& xml_doc, LPCSTR path, 
									int index, CUIFrameWindow* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);


	InitWindow(xml_doc, path, index, pWnd);
	InitTexture(xml_doc, path, index, pWnd);

	string256 buf;

	strconcat(buf,path,":left_top_texture");
	shared_str tex_name = xml_doc.Read(buf, index, NULL);

	float x = xml_doc.ReadAttribFlt(buf, index, "x");
	float y = xml_doc.ReadAttribFlt(buf, index, "y");

	if(*tex_name) pWnd->InitLeftTop(*tex_name, x,y);


	strconcat(buf,path,":left_bottom_texture");
	tex_name = xml_doc.Read(buf, index, NULL);

	x = xml_doc.ReadAttribFlt(buf, index, "x");
	y = xml_doc.ReadAttribFlt(buf, index, "y");

	if(*tex_name) pWnd->InitLeftBottom(*tex_name, x,y);

	//���������������� ��������� ����
	strconcat(buf,path,":title");
	if(xml_doc.NavigateToNode(buf,index)) InitStatic(xml_doc, buf, index, &pWnd->UITitleText);


	return true;
}


bool CUIXmlInit::InitOptionsItem(CUIXml& xml_doc, const char* paht, int index, CUIOptionsItem* pWnd){
	char buf[256];
	strconcat(buf,paht,":options_item");

	if (xml_doc.NavigateToNode(buf,index))
	{
        shared_str entry = xml_doc.ReadAttrib(buf, index,"entry");
        shared_str group = xml_doc.ReadAttrib(buf, index,"group");

		pWnd->Register(*entry, *group);
		return true;
	}
	else return false;	
}


bool CUIXmlInit::InitStatic(CUIXml& xml_doc, LPCSTR path, 
									int index, CUIStatic* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	InitWindow(xml_doc, path, index, pWnd);
	InitMultiText(xml_doc, path, index, pWnd);
	InitTexture	 (xml_doc, path, index, pWnd);

	int flag = xml_doc.ReadAttribInt(path, 0, "heading");
	pWnd->EnableHeading( (flag)?true:false);

	LPCSTR str_flag = xml_doc.ReadAttrib(path, 0, "light_anim", "");
	if( xr_strlen(str_flag) )
		pWnd->SetLightAnim(str_flag);
	
	return true;
}

bool CUIXmlInit::InitCheck(CUIXml& xml_doc, LPCSTR path, int index, CUICheckButton* pWnd){
	InitStatic(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);

	return true;
}

bool CUIXmlInit::InitSpin(CUIXml& xml_doc, const char* path, int index, CUICustomSpin* pWnd){
	InitWindow(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);

	return true;
}

bool CUIXmlInit::InitText(CUIXml& xml_doc, LPCSTR path, int index, CUIStatic* pWnd){
	string256 buf;
	shared_str text_path = strconcat(buf,path,":text");
	u32 color;
	CGameFont *pTmpFont = NULL;
	InitFont(xml_doc, *text_path, index, color, pTmpFont);
	if (pTmpFont)
	{
        pWnd->SetTextColor(color);		
		pWnd->SetFont(pTmpFont);
	}
	// Load font alignment
	shared_str al = xml_doc.ReadAttrib(*text_path, index, "align", "");
	if (0 == xr_strcmp(al, "c"))
		pWnd->SetTextAlign(CGameFont::alCenter);
	else if (0 == xr_strcmp(al, "r"))
		pWnd->SetTextAlign(CGameFont::alRight);
	else if (0 == xr_strcmp(al, "l"))
		pWnd->SetTextAlign(CGameFont::alLeft);

	// Text coordinates
	float text_x = xml_doc.ReadAttribFlt(*text_path, index, "x", -1);
	float text_y = xml_doc.ReadAttribFlt(*text_path, index, "y", -1);
	shared_str text = xml_doc.Read(*text_path, index, NULL);

	pWnd->SetTextX(text_x);
	pWnd->SetTextY(text_y);

	CStringTable st;
	if (*text)
        pWnd->SetText(*st(*text));

	return true;
}


bool CUIXmlInit::InitText(CUIXml& xml_doc, const char* path, int index, IUITextControl* pWnd){
	string256 buf;
	shared_str text_path = strconcat(buf,path,":text");
	u32 color;
	CGameFont *pTmpFont = NULL;
	InitFont(xml_doc, *text_path, index, color, pTmpFont);
	if (pTmpFont)
	{
        pWnd->SetTextColor(color);		
		pWnd->SetFont(pTmpFont);	
	}
	// Load font alignment
	shared_str al = xml_doc.ReadAttrib(*text_path, index, "align", "l");
	if (0 == xr_strcmp(al, "c"))
		pWnd->SetTextAlignment(CGameFont::alCenter);
	else if (0 == xr_strcmp(al, "r"))
		pWnd->SetTextAlignment(CGameFont::alRight);
	else if (0 == xr_strcmp(al, "l"))
		pWnd->SetTextAlignment(CGameFont::alLeft);

	// Text coordinates
	shared_str text = xml_doc.Read(*text_path, index, NULL);

	CStringTable st;
	pWnd->SetText(*st(*text));
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::Init3tButton(CUIXml& xml_doc, const char* path, int index, CUI3tButton* pWnd){
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	InitWindow			(xml_doc, path, index, pWnd);
	InitMultiText		(xml_doc, path, index, pWnd);
	InitMultiTexture	(xml_doc, path, index, pWnd);
	InitTextureOffset	(xml_doc, path, index, pWnd);
	InitSound			(xml_doc, path, index, pWnd);

	u32 accel = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "accel", -1));
	pWnd->SetAccelerator(accel);

	float shadowOffsetX	= xml_doc.ReadAttribFlt(path, index, "shadow_offset_x", 0);
	float shadowOffsetY	= xml_doc.ReadAttribFlt(path, index, "shadow_offset_y", 0);

	pWnd->SetShadowOffset(shadowOffsetX, shadowOffsetY);

	// init hint static
	string256 hint;
	strconcat(hint, path, ":hint");

	if (xml_doc.NavigateToNode(hint, index))
        InitStatic(xml_doc, hint, index, &pWnd->m_hint);

	int r = xml_doc.ReadAttribInt(path, index, "check_mode", -1);
	if(r!=-1)
	pWnd->SetCheckMode ( (r==1)?true : false);
	
	return true;
}

bool CUIXmlInit::InitSound(CUIXml& xml_doc, const char* path, int index, CUI3tButton* pWnd){
	string256 sound_h;
	string256 sound_t;
	strconcat(sound_h, path, ":sound_h");
	strconcat(sound_t, path, ":sound_t");

	shared_str sound_h_result = xml_doc.Read(sound_h, index);
	shared_str sound_t_result = xml_doc.Read(sound_t, index);

	if (xr_strlen(sound_h_result) != 0)
		pWnd->InitSoundH(*sound_h_result);

	if (xr_strlen(sound_t_result) != 0)
		pWnd->InitSoundT(*sound_t_result);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitButton(CUIXml& xml_doc, LPCSTR path, 
						int index, CUIButton* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	InitStatic(xml_doc, path, index, pWnd);

	u32 accel = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "accel", -1));
	pWnd->SetAccelerator(accel);

	u32 hA = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hA", 255));
	u32 hR = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hR", 153));
	u32 hG = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hG", 153));
	u32 hB = static_cast<u32>(xml_doc.ReadAttribInt(path, index, "hB", 153));

	float shadowOffsetX	= xml_doc.ReadAttribFlt(path, index, "shadow_offset_x", 0);
	float shadowOffsetY	= xml_doc.ReadAttribFlt(path, index, "shadow_offset_y", 0);

	float pushOffsetX		= xml_doc.ReadAttribFlt(path, index, "push_off_x", 2);
	float pushOffsetY		= xml_doc.ReadAttribFlt(path, index, "push_off_y", 3);


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
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	float x = xml_doc.ReadAttribFlt(path, index, "x");
	float y = xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y, pWnd);

	float width = xml_doc.ReadAttribFlt(path, index, "width");
	float height = xml_doc.ReadAttribFlt(path, index, "height");


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
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);
	
	float x = xml_doc.ReadAttribFlt(path, index, "x");
	float y = xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y, pWnd);

	float width = xml_doc.ReadAttribFlt(path, index, "width");
	float height = xml_doc.ReadAttribFlt(path, index, "height");
	float item_height = xml_doc.ReadAttribFlt(path, index, "item_height");
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

	float ri = xml_doc.ReadAttribFlt	(path, index, "right_ident", 0.0f);
	pWnd->SetRightIndention				(ri*UI()->GetScaleX());

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitProgressBar(CUIXml& xml_doc, LPCSTR path, 
						int index, CUIProgressBar* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	string256 buf;
	
	float x = xml_doc.ReadAttribFlt(path, index, "x");
	float y = xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y, pWnd);

	float width = xml_doc.ReadAttribFlt(path, index, "length");
	float height = xml_doc.ReadAttribFlt(path, index, "broad");
	bool is_horizontal = (xml_doc.ReadAttribInt(path, index, "horz")==1);

	pWnd->Init(x, y, width, height, is_horizontal);

	int min = xml_doc.ReadAttribInt(path, index, "min");
	int max = xml_doc.ReadAttribInt(path, index, "max");
	int pos = xml_doc.ReadAttribInt(path, index, "pos");
	
	pWnd->SetRange((s16)min,(s16)max);
	pWnd->SetProgressPos((s16)pos);

	// progress
	strconcat(buf,path,":texture");

	LPCSTR texture = xml_doc.Read(buf, index, NULL);
	if(!texture) return false;

	float progress_length = xml_doc.ReadAttribFlt(buf, index, "length");
	int r = xml_doc.ReadAttribInt(buf, index, "r");
	int g = xml_doc.ReadAttribInt(buf, index, "g");
	int b = xml_doc.ReadAttribInt(buf, index, "b");
	float tex_x = xml_doc.ReadAttribFlt(buf, index, "x");
	float tex_y = xml_doc.ReadAttribFlt(buf, index, "y");
	float tex_w = xml_doc.ReadAttribFlt(buf, index, "width");
	float tex_h = xml_doc.ReadAttribFlt(buf, index, "height");

	u32 color = RGB_ALPHA(0xFF, r, g, b);

	pWnd->SetProgressTexture(texture,progress_length,tex_x, tex_y, tex_w, tex_h, color);

	// background
	strconcat(buf,path,":background");
	texture = xml_doc.Read(buf, index, NULL);
	tex_x = xml_doc.ReadAttribFlt(buf, index, "x");
	tex_y = xml_doc.ReadAttribFlt(buf, index, "y");
	tex_w = xml_doc.ReadAttribFlt(buf, index, "width");
	tex_h = xml_doc.ReadAttribFlt(buf, index, "height");
	x = xml_doc.ReadAttribFlt(buf, index, "offs_x");
	y = xml_doc.ReadAttribFlt(buf, index, "offs_y");

	pWnd->SetBackgroundTexture(texture,tex_x,tex_y,tex_w,tex_h,x,y);
	
	strconcat(buf,path,":min_color");
	
	if( xml_doc.NavigateToNode(buf,index) ){
		pWnd->m_bUseColor			= true;
		int r = xml_doc.ReadAttribInt(buf, index, "r");
		int g = xml_doc.ReadAttribInt(buf, index, "g");
		int b = xml_doc.ReadAttribInt(buf, index, "b");
	
		pWnd->m_minColor.set(color_rgba(r,g,b,0xFF));

		strconcat(buf,path,":max_color");
		r = xml_doc.ReadAttribInt(buf, index, "r");
		g = xml_doc.ReadAttribInt(buf, index, "g");
		b = xml_doc.ReadAttribInt(buf, index, "b");
	
		pWnd->m_maxColor.set(color_rgba(r,g,b,0xFF));
	}

	return true;
}

CUIXmlInit::StaticsVec CUIXmlInit::InitAutoStaticGroup(CUIXml& xml_doc, LPCSTR path, CUIWindow* pParentWnd)
{
	int items_num = xml_doc.GetNodesNum(path, 0, "auto_static");
	// tmp statics vector
	StaticsVec	tmpVec;

	XML_NODE* _node = xml_doc.NavigateToNode(path,0);
	xml_doc.SetLocalRoot(_node);

	CUIStatic* pUIStatic = NULL;
	for(int i=0; i<items_num; i++)
	{
		pUIStatic = xr_new<CUIStatic>();
		InitStatic(xml_doc, "auto_static", i, pUIStatic);
		pUIStatic->SetAutoDelete(true);
		pParentWnd->AttachChild(pUIStatic);
		tmpVec.push_back(pUIStatic);
		pUIStatic = NULL;
	}

	xml_doc.SetLocalRoot(xml_doc.GetRoot());
	return tmpVec;
}

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
			pFnt = UI()->Font()->pFontHeaderRussian;
		}
		else if(!xr_strcmp(*font_name, NORMAL_FONT_NAME) || !xr_strcmp(*font_name, GRAFFITI19_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontGraffiti19Russian;
		}
		else if(!xr_strcmp(*font_name, GRAFFITI22_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontGraffiti22Russian;
		}
		else if(!xr_strcmp(*font_name, GRAFFITI32_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontGraffiti32Russian;
		}
		else if(!xr_strcmp(*font_name, GRAFFITI50_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontGraffiti50Russian;
		}
		else if(!xr_strcmp(*font_name, ARIAL_FONT_NAME))
		{
			pFnt = UI()->Font()->pArialN21Russian;
		}
		else if(!xr_strcmp(*font_name, "arial_14"))
		{
			pFnt = UI()->Font()->pFontArial14;
		}
		else if(!xr_strcmp(*font_name, BIG_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontBigDigit;
		}
		else if(!xr_strcmp(*font_name, MEDIUM_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontMedium;
		}
		else if(!xr_strcmp(*font_name, SMALL_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontSmall;
		}
		else if(!xr_strcmp(*font_name, LETTERICA16_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontLetterica16Russian;
		}
		else if(!xr_strcmp(*font_name, LETTERICA18_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontLetterica18Russian;
		}
		else if(!xr_strcmp(*font_name, LETTERICA25_FONT_NAME))
		{
			pFnt = UI()->Font()->pFontLetterica25;
		}
	}
	return true;
}

bool CUIXmlInit::InitTabControl(CUIXml &xml_doc, LPCSTR path, int index, CUITabControl *pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);
	
	bool status = true;

	status &= InitWindow(xml_doc, path, index, pWnd);
	int tabsCount	= xml_doc.GetNodesNum(path, 0, "button");
	int radio		= xml_doc.ReadAttribInt(path, 0, "radio");

	XML_NODE* tab_node = xml_doc.NavigateToNode(path,index);
	xml_doc.SetLocalRoot(tab_node);

	CUITabButton* newButton;

	for (int i = 0; i < tabsCount; ++i)
	{
		newButton = radio ? xr_new<CUIRadioButton>() : xr_new<CUITabButton>();
		status &= Init3tButton(xml_doc, "button", i, newButton);
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

	float x			= xml_doc.ReadAttribFlt(path, index, "x");
	float y			= xml_doc.ReadAttribFlt(path, index, "y");

	InitAlignment(xml_doc, path, index, x, y, pWnd);

	float width		= xml_doc.ReadAttribFlt(path, index, "width");
	float height	= xml_doc.ReadAttribFlt(path, index, "height");
	bool vertical	= !!xml_doc.ReadAttribInt(path, index, "vertical");

	int	r			= xml_doc.ReadAttribInt(path, index, "r", 0xff);
	int	g			= xml_doc.ReadAttribInt(path, index, "g", 0xff);
	int	b			= xml_doc.ReadAttribInt(path, index, "b", 0xff);
	int	a			= xml_doc.ReadAttribInt(path, index, "a", 0xff);

	pWnd->SetColor(color_rgba(r, g, b, a));

	shared_str base_name = xml_doc.Read(strconcat(buf,path,":texture"), index, NULL);

	if(!base_name) 
	{
		pWnd->Init(x, y, width, height);
		return true;
	}

	pWnd->Init(*base_name, x, y, width, height, !vertical);

	strconcat(buf,path,":title");
	if(xml_doc.NavigateToNode(buf,index)) InitStatic(xml_doc, buf, index, &pWnd->UITitleText);

	return true;
}

bool CUIXmlInit::InitLabel(CUIXml& xml_doc, const char* path, int index, CUILabel* pWnd){
	InitFrameLine(xml_doc, path, index, pWnd);
	InitText(xml_doc, path, index, (IUITextControl*)pWnd);

	string256 buf;
	shared_str text_path = strconcat(buf,path,":text");
	float text_x = xml_doc.ReadAttribFlt(*text_path, index, "x", 0);
	float text_y = xml_doc.ReadAttribFlt(*text_path, index, "y", 0);

	if (text_x)
		pWnd->SetTextPosX(text_x);
	if (text_y)
        pWnd->SetTextPosY(text_y);

	return true;
}

bool CUIXmlInit::InitEditBox(CUIXml& xml_doc, const char* path, int index, CUIEditBox* pWnd){
	InitLabel(xml_doc, path, index, pWnd);
	InitOptionsItem(xml_doc, path, index, pWnd);

	if (xml_doc.ReadAttribInt(path,index,"numonly",0))
	{
        pWnd->SetNumbersOnly(true);
		if (xml_doc.ReadAttribInt(path,index,"float",0))
			pWnd->SetFloatNumbers(true);
	}
	if (xml_doc.ReadAttribInt(path, index, "password",0))
		pWnd->SetPasswordMode();

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitTextBanner(CUIXml &xml_doc, const char *path, int index, CUITextBanner *pBnr)
{
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	xr_map<shared_str, CUITextBanner::TextBannerStyles> conformityTable;
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
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

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
		p->outX			= (xml_doc.ReadAttribFlt(ph, i, "x", 0));
		p->outY			= (xml_doc.ReadAttribFlt(ph, i, "y", 0));
		p->maxWidth		= xml_doc.ReadAttribFlt(ph, i, "width", -1);

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
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

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

bool CUIXmlInit::InitTexture(CUIXml& xml_doc, const char* path, int index, IUIMultiTextureOwner* pWnd){
	string256 buf;
	strconcat(buf, path, ":texture");
	shared_str texture;

	if (xml_doc.NavigateToNode(buf))
        texture = xml_doc.Read(buf, index, NULL);

	if (texture.size())
	{
        pWnd->InitTexture(*texture);
		return true;
	}
	else
	{

		strconcat(buf, path, ":base_texture");

		if (xml_doc.NavigateToNode(buf))
			texture = xml_doc.Read(buf, index, NULL);

		if (texture.size())
		{
			pWnd->InitTexture(*texture);
			return true;
		}
	}
	return false;
}

bool CUIXmlInit::InitTexture(CUIXml& xml_doc, const char* path, int index, IUISingleTextureOwner* pWnd){
	InitTexture(xml_doc, path, index, (IUIMultiTextureOwner*)pWnd);

	bool stretch_flag = xml_doc.ReadAttribInt(path, index, "stretch") ? true : false;
	pWnd->SetStretchTexture(stretch_flag);

	string256 buf;
	strconcat(buf, path, ":texture");

	Frect rect;

	rect.x1			= xml_doc.ReadAttribFlt(buf, index, "x", 0);
	rect.y1			= xml_doc.ReadAttribFlt(buf, index, "y", 0);
	rect.x2			= rect.x1 + xml_doc.ReadAttribFlt(buf, index, "width", 0);	
	rect.y2			= rect.y1 + xml_doc.ReadAttribFlt(buf, index, "height", 0);
	int	a			= xml_doc.ReadAttribInt(buf, index, "a", 255);
	int	r			= xml_doc.ReadAttribInt(buf, index, "r", 255);
	int	g			= xml_doc.ReadAttribInt(buf, index, "g", 255);
	int	b			= xml_doc.ReadAttribInt(buf, index, "b", 255);
//	shared_str mirrorM = xml_doc.ReadAttrib(buf, index, "mirror", "none");

	//if (0 == xr_strcmp(mirrorM, "v"))
	//	pWnd->GetUIStaticItem().SetMirrorMode(tmMirrorVertical);
	//else if (0 == xr_strcmp(mirrorM, "h"))
	//	pWnd->GetUIStaticItem().SetMirrorMode(tmMirrorHorisontal);
	//else if (0 == xr_strcmp(mirrorM, "both"))
	//	pWnd->GetUIStaticItem().SetMirrorMode(tmMirrorBoth);


	pWnd->SetTextureColor(color_rgba(r, g, b, a));

	if (rect.width() != 0 && rect.height() != 0)
		pWnd->SetOriginalRect(rect);

	return true;
}

bool CUIXmlInit::InitSharedTexture(CUIXml& xml_doc, const char* t_id, CUIStatic* pWnd){
	R_ASSERT2(false,"don't call this function");
	xr_string fname = xml_doc.Read("file_name", 0);

	if (fname.empty())
		return false;

	int nodes = xml_doc.GetNodesNum("",0,"texture");

	xr_string id;

	for (int i = 0; i<nodes; i++)
	{
        id = xml_doc.ReadAttrib("texture", i, "id");
		if (0 == xr_strcmp(id.c_str(), t_id))
		{
			float x		= xml_doc.ReadAttribFlt("texture", i, "x");
			float y		= xml_doc.ReadAttribFlt("texture", i, "y");
			float width	= xml_doc.ReadAttribFlt("texture", i, "width", -1);
			float height	= xml_doc.ReadAttribFlt("texture", i, "height", -1);

			if (-1 == width || -1 == height)
                return false;

			pWnd->InitTexture(fname.c_str());
			pWnd->SetOriginalRect(x, y, width, height);

			return true;
		}
	}
	return false;
}

bool CUIXmlInit::InitTextureOffset(CUIXml &xml_doc, LPCSTR path, int index, CUIStatic* pWnd){
    string256 textureOffset;
	if (0 == xr_strcmp(path, ""))
		strcpy(textureOffset, "texture_offset");
	else
		strconcat(textureOffset, path, ":texture_offset");

	float x = xml_doc.ReadAttribFlt(textureOffset, index, "x");
	float y = xml_doc.ReadAttribFlt(textureOffset, index, "y");

	pWnd->SetTextureOffset(x, y);

	return true;
}

bool CUIXmlInit::InitMultiTexture(CUIXml &xml_doc, LPCSTR path, int index, CUI3tButton* pWnd){
	string256 buff;
	bool success = false;

	strconcat(buff, path, ":shared_texture");
	shared_str sh_tex_xml = xml_doc.Read(buff, index, NULL);
	CUITextureMaster::ParseShTexInfo(*sh_tex_xml);

	strconcat(buff, path, ":texture");
	shared_str texture = xml_doc.Read(buff, index, NULL);

	if (texture.size() > 0)
	{
		pWnd->InitTexture(*texture);
		return true;
	}

	strconcat(buff, path, ":texture_e");
	texture = xml_doc.Read(buff, index, NULL);
	if (texture.size())
	{
			pWnd->m_background.CreateE()->InitTexture(*texture);
			success = true;
	}

	strconcat(buff, path, ":texture_t");
	texture = xml_doc.Read(buff, index, NULL);
	if (texture.size())
	{
			pWnd->m_background.CreateT()->InitTexture(*texture);
			success = true;
	}

	strconcat(buff, path, ":texture_d");
	texture = xml_doc.Read(buff, index, NULL);
	if (texture.size())
	{
			pWnd->m_background.CreateD()->InitTexture(*texture);
			success = true;
	}

	strconcat(buff, path, ":texture_h");
	texture = xml_doc.Read(buff, index, NULL);   
	if (texture.size())
	{
			pWnd->m_background.CreateH()->InitTexture(*texture);
			success = true;
	}

	if (success)
        pWnd->TextureOn();

	return success;
}

bool CUIXmlInit::InitMultiText(CUIXml& xml_doc, LPCSTR path, int index, CUIStatic* pWnd){
	InitText(xml_doc, path, index, pWnd);

	string256 sText;
	// attention!! order of character is very impartant
	char* last_char = "edth";
	bool pathIsVoid = 0 == xr_strcmp(path, "");

	u32 a, r, g, b;

	if (pathIsVoid)
		strcpy(sText, "text_color:e");
	else
		strconcat(sText, path, ":text_color:e");

	int last_symbol = xr_strlen(sText) - 1;

	for (int i = 1; i <= 4; i++)
	{
		if (xml_doc.NavigateToNode(sText, index))
		{
			a = xml_doc.ReadAttribInt(sText, index, "a", 255);
			r = xml_doc.ReadAttribInt(sText, index, "r", 255);
			g = xml_doc.ReadAttribInt(sText, index, "g", 255);
			b = xml_doc.ReadAttribInt(sText, index, "b", 255);

			pWnd->SetTextColor(color_argb(a, r, g, b),(CUIStatic::E4States)(i - 1));
		}

		sText[last_symbol] = last_char[i];
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

float CUIXmlInit::ApplyAlignX(float coord, u32 align)
{
	return coord;
/*
	// ��������� ������������, ������ ��� ����������� ����������
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
*/
}

//////////////////////////////////////////////////////////////////////////

float CUIXmlInit::ApplyAlignY(float coord, u32 align)
{
	return coord;
/*
	// ��������� ������������, ������ ��� ����������� ����������
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
*/
}

//////////////////////////////////////////////////////////////////////////

void CUIXmlInit::ApplyAlign(float &x, float &y, u32 align)
{
	x = ApplyAlignX(x, align);
	y = ApplyAlignY(y, align);
}

//////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitAlignment(CUIXml &xml_doc, const char *path,
							   int index, float &x, float &y,CUIWindow* pWnd)
{
	xr_string wnd_alignment = xml_doc.ReadAttrib(path, index, "alignment", "");
	
	if ( strchr(wnd_alignment.c_str(), 'c') )
		pWnd->SetAlignment(waCenter);


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
	bool flag = uiXml.Init(CONFIG_PATH, UI_PATH, COLOR_DEFINITIONS);
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

//////////////////////////////////////////////////////////////////////////////

bool CUIXmlInit::InitArtefactPanel(CUIXml &xml_doc, const char* path, int index, CUIArtefactPanel* pWnd){
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	float x = xml_doc.ReadAttribFlt(path, index, "x");
	float y = xml_doc.ReadAttribFlt(path, index, "y");
	InitAlignment(xml_doc, path, index, x, y, pWnd);
	float width = xml_doc.ReadAttribFlt(path, index, "width");
	float height = xml_doc.ReadAttribFlt(path, index, "height");
	pWnd->Init(x, y, width, height);

	float fScale = xml_doc.ReadAttribFlt(path, index, "scale");
	pWnd->SetScaleXY(fScale,fScale);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

u32 CUIXmlInit::GetARGB(CUIXml& xml_doc, const char* path, int index){
	u32 a = xml_doc.ReadAttribInt(path, index, "a");
	u32 r = xml_doc.ReadAttribInt(path, index, "r");
	u32 g = xml_doc.ReadAttribInt(path, index, "g");
	u32 b = xml_doc.ReadAttribInt(path, index, "b");

	return color_argb(a, r, g, b);
}

bool CUIXmlInit::InitScrollView	(CUIXml& xml_doc, const char* path, int index, CUIScrollView* pWnd)
{
	R_ASSERT3(xml_doc.NavigateToNode(path,index), "XML node not found", path);

	InitWindow							(xml_doc, path, index, pWnd);
	float ri							= xml_doc.ReadAttribFlt	(path, index, "right_ident", 0.0f);
	pWnd->SetRightIndention				(ri*UI()->GetScaleX());
	pWnd->Init							();

	bool bVertFlip						= (1==xml_doc.ReadAttribInt	(path, index, "flip_vert", 0));
	pWnd->SetVertFlip					(bVertFlip);
	return								true;
}

// file:		UISkinWindow
// description:	SkinWindow for SkinSelector
// created:		03.02.2005
// author:		Serge Vynnychenko

// copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UISkinWindow.h"
#include "../UI.h"
#include "UISkinSelector.h"
//#include "../game_cl_deathmatch.h"
//#include"../hudmanager.h"
//#include"../level.h"

const int BORDER = 30;


CUISkinWindow::CUISkinWindow(){

}

CUISkinWindow::~CUISkinWindow(){

}

void CUISkinWindow::Init(CUIXml& xmlDoc, int index, shared_str section){
	CUIXmlInit xml_init;
	shared_str texture;
	string256 buf;
	int width	( UI_BASE_WIDTH / SKINS_COUNT );
	int height( xmlDoc.ReadAttribInt(PATH_XML, 0, "height") );
	int x		( index*width );
	int y		(UI_BASE_HEIGHT / 2 - height / 2 );

	CUIWindow::Init(x, y, width, height);

/////// UIBackground
	AttachChild(&UIBackground);

	texture = xmlDoc.Read(strconcat(buf,PATH_XML,":base_texture"), 0, NULL);
	UIBackground.Init(*texture, 0, 0, width, height);

	CUIFrameWindow* pWnd = &UIBackground;
// init left top
	strconcat(buf, PATH_XML, ":left_top_texture");
	texture = xmlDoc.Read(buf, index, NULL);
	x = xmlDoc.ReadAttribInt(buf, index, "x");
	y = xmlDoc.ReadAttribInt(buf, index, "y");
	if(*texture)
		pWnd->InitLeftTop(*texture, x, y);

// init left bottom	
	strconcat(buf,PATH_XML,":left_bottom_texture");
	texture = xmlDoc.Read(buf, index, NULL);
	x = xmlDoc.ReadAttribInt(buf, index, "x");
	y = xmlDoc.ReadAttribInt(buf, index, "y");
	if(*texture) 
		pWnd->InitLeftBottom(*texture, x, y);

// init title
	strconcat(buf,PATH_XML,":title");
	if(xmlDoc.NavigateToNode(buf,index)) 
		xml_init.InitStatic(xmlDoc, buf, index, &pWnd->UITitleText);

/////// UIHighlighted
	AttachChild(&UIHighlight);
	UIHighlight.Init(0, 0, width, height);

	RECT r	= UIBackground.GetAbsoluteRect();

	// Координаты бекграунда
	r.bottom	-= BORDER;
	r.left		+= BORDER;
	r.top		+= BORDER;
	r.right		-= BORDER;

	int bkWidth				= r.right	- r.left, 
		bkHeight			= r.bottom	- r.top,
		// Координаты текстуры
		texWidth			= SKIN_TEX_WIDTH,
		texHeight			= SKIN_TEX_HEIGHT;

	// Коеффициент сжатия
	float stretchKoeff = 1.0f;

	// Проверяем какое из отношений меньше
	float dW = static_cast<float>(bkWidth)	/ texWidth;
	float dH = static_cast<float>(bkHeight)	/ texHeight;

	(dW < dH) ? stretchKoeff = dW : stretchKoeff = dH;

	texWidth	= static_cast<int>(texWidth		* stretchKoeff);
	texHeight	= static_cast<int>(texHeight	* stretchKoeff);

	if (dW < dH)
	{
		float f		 = (bkHeight - texHeight) / 2.0f;
		r.top		+= static_cast<long>(f);
		r.bottom	-= static_cast<long>(f);
	}
	else
	{
		float f		 = (bkWidth - texWidth) / 2.0f;
		r.left		+= static_cast<long>(f);
		r.right		-= static_cast<long>(f);
	}

//	UIHighlight.Init(r.left,
//					r.top,
//					r.right - r.left,
//					r.bottom - r.top);
	UIHighlight.Init(BORDER, BORDER, r.right - r.left, r.bottom - r.top);

	UIHighlight.SetShader(GetMPCharIconsShader());
	UIHighlight.GetUIStaticItem().SetColor(clInactive);

	int m_iSkinX = 0, m_iSkinY = 0;
	sscanf(pSettings->r_string("multiplayer_skins", *GetSkinName(section, index)), "%i,%i", &m_iSkinX, &m_iSkinY);

	// Размеры иконки персонажа : 128х341
	UIHighlight.GetUIStaticItem().SetOriginalRect(
		m_iSkinX,
		m_iSkinY,
		CHAR_ICON_FULL_WIDTH*ICON_GRID_HEIGHT,
		static_cast<int>(5.07f*ICON_GRID_WIDTH));

	UIHighlight.ClipperOn();
	UIHighlight.SetTextureScaleXY(stretchKoeff,stretchKoeff);

	if (0 == index)
        UIHighlight.GetUIStaticItem().SetColor(clActive);
}

void CUISkinWindow::OnDbClick(){
	GetMessageTarget()->SendMessage(this, WINDOW_LBUTTON_DB_CLICK, NULL);
}

shared_str CUISkinWindow::GetSkinName(shared_str section, int index){
	string256 skinName;
	shared_str	cfgRecord;
	shared_str ret;
	

	cfgRecord = pSettings->r_string(section, "skins");
	_GetItem(*cfgRecord, index, skinName);

	return ret = skinName;
}


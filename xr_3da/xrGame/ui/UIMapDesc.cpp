#include "stdafx.h"

#include "UIMapDesc.h"
#include "UIStatic.h"
#include "UIScrollView.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "../hudmanager.h"
#include "../level.h"
#include "../game_cl_teamdeathmatch.h"
#include "UIMapInfo.h"
#include <dinput.h>

CUIMapDesc::CUIMapDesc(){
	m_pBackground	= xr_new<CUIStatic>();		AttachChild(m_pBackground);	
	m_pCaption		= xr_new<CUIStatic>();		AttachChild(m_pCaption);

	m_pImage		= xr_new<CUIStatic>();		AttachChild(m_pImage);
	m_pTextDesc		= xr_new<CUIScrollView>();	AttachChild(m_pTextDesc);

	m_pBtnSpectator	= xr_new<CUI3tButton>();	AttachChild(m_pBtnSpectator);
	m_pBtnNext		= xr_new<CUI3tButton>();	AttachChild(m_pBtnNext);

	m_pMapInfo		= xr_new<CUIMapInfo>();		AttachChild(m_pMapInfo);
	

	for (int i = 0; i <3; i++)
	{
		m_pFrame[i] = xr_new<CUIStatic>();
		AttachChild(m_pFrame[i]);
	}

    Init();
}

CUIMapDesc::~CUIMapDesc(){
	xr_delete(m_pCaption);
	xr_delete(m_pBackground);

	xr_delete(m_pImage);
	xr_delete(m_pTextDesc);

	xr_delete(m_pBtnSpectator);
	xr_delete(m_pBtnNext);

	xr_delete(m_pMapInfo);

	for (int i = 0; i <3; i++)
		xr_delete(m_pFrame[i]);		
}

void CUIMapDesc::Init(){
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, "map_desc.xml");
	R_ASSERT3(xml_result, "xml file not found", "map_desc.xml");

	CUIXmlInit::InitWindow		(xml_doc,"map_desc",				0,	this);
	CUIXmlInit::InitStatic		(xml_doc,"map_desc:caption",		0,	m_pCaption);
	CUIXmlInit::InitStatic		(xml_doc,"map_desc:background",		0,	m_pBackground);
	CUIXmlInit::InitScrollView	(xml_doc,"map_desc:text_desc",		0,	m_pTextDesc);

	// init map picture
	CUIXmlInit::InitStatic		(xml_doc,"map_desc:image",			0,	m_pImage);

	string256								gameLtxPath;
	FS.update_path							(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile gameLtx						(gameLtxPath);

	if (gameLtx.section_exist(Level().name()),*Level().name()
		&& gameLtx.line_exist(Level().name(),"texture"))
	{
		m_pImage->InitTexture(gameLtx.r_string(Level().name(),"texture"));
	}
	else
		m_pImage->InitTexture("ui\\ui_nomap");


//	xr_string map_name = "ui\\ui_map_";
//	map_name +=	;
//	map_name += ".dds";
	


	CUIXmlInit::InitStatic		(xml_doc,"map_desc:image_frames_1",	0,	m_pFrame[0]);
	CUIXmlInit::InitStatic		(xml_doc,"map_desc:image_frames_2",	0,	m_pFrame[1]);
	CUIXmlInit::InitStatic		(xml_doc,"map_desc:image_frames_3",	0,	m_pFrame[2]);

	CUIXmlInit::InitStatic		(xml_doc,"map_desc:btn_next",		0,	m_pBtnNext);
	CUIXmlInit::InitStatic		(xml_doc,"map_desc:btn_spectator",	0,	m_pBtnSpectator);
	m_pImage->SetStretchTexture(true);	
}

void CUIMapDesc::SendMessage(CUIWindow* pWnd,s16 msg, void* pData){
	if (BUTTON_CLICKED == msg)
	{
		game_cl_mp * dm = smart_cast<game_cl_mp *>(&(Game()));
		dm->StartStopMenu(this,true);
		if (pWnd == m_pBtnSpectator)
			dm->OnSpectatorSelect();
		else if (pWnd == m_pBtnNext)
			dm->OnMapInfoAccept();
	}

}

bool CUIMapDesc::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (WINDOW_KEY_RELEASED == keyboard_action) 
		return true;

	game_cl_mp * dm = smart_cast<game_cl_mp *>(&(Game()));

	switch (dik){
		case DIK_ESCAPE:
#pragma todo("SATAN -> CRAZY_MAX : please free me from this intolerable emptiness :) ")
			return true;
			break;
		case DIK_RETURN:
			dm->StartStopMenu(this,true);
			dm->OnMapInfoAccept();
			return true;
			break;
	}

    return false;
}






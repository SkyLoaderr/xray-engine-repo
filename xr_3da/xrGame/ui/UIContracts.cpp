//=============================================================================
//  Filename:   UIContracts.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Contracts UI Window
//=============================================================================

#include "StdAfx.h"
#include "UIContracts.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "../UI.h"
#include "../Level.h"

//////////////////////////////////////////////////////////////////////////

const char * const	CONTRACTS_XML			= "contracts.xml";
const char * const	CONTRACTS_CHAR_XML		= "contracts_character.xml";


//////////////////////////////////////////////////////////////////////////

CUIContractsWnd::CUIContractsWnd()
{
	SetWindowName("Contracts");
}

//////////////////////////////////////////////////////////////////////////

void CUIContractsWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", CONTRACTS_XML);
	R_ASSERT3(xml_result, "xml file not found", CONTRACTS_XML);

	CUIXmlInit xml_init;

	AttachChild(&UIListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIListWnd);
	UIListWnd.EnableScrollBar(true);

	AttachChild(&UICharInfo);
	UICharInfo.Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT, CONTRACTS_CHAR_XML);
}

//////////////////////////////////////////////////////////////////////////

void CUIContractsWnd::Show(bool status)
{
	inherited::Show(status);

	if (status)
	{
		CInventoryOwner * pInvOwner  = dynamic_cast<CInventoryOwner*>(Level().CurrentEntity());;

		//инициализировать окошко с информацие о собеседнике
		UICharInfo.InitCharacter(pInvOwner);
	}
}
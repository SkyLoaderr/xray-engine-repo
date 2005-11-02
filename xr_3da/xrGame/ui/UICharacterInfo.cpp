#include "stdafx.h"

#include "UIInventoryUtilities.h"

#include "uicharacterinfo.h"
#include "../actor.h"
#include "../level.h"
#include "../character_info.h"
#include "../string_table.h"
#include "../relation_registry.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "uistatic.h"
#include "UIScrollView.h"

using namespace InventoryUtilities;

CUICharacterInfo::CUICharacterInfo()
{
	ZeroMemory			(m_icons,eMaxCaption*sizeof(CUIStatic*));
	pUIBio				= NULL;
	m_ownerID			= u32(-1);
}

CUICharacterInfo::~CUICharacterInfo()
{
	for(int i=0; i<eMaxCaption;++i)
		if(m_icons[i])xr_delete(m_icons[i]);
}

void CUICharacterInfo::Init(float x, float y, float width, float height, CUIXml* xml_doc)
{
	inherited::Init(x, y, width, height);

	CUIXmlInit xml_init;
	CUIStatic*	pItem = NULL;

	if(xml_doc->NavigateToNode("icon_static",0))	
	{
		pItem = m_icons[eUIIcon] = xr_new<CUIStatic>();
		xml_init.InitStatic	(*xml_doc, "icon_static", 0, pItem);
		pItem->ClipperOn	();
		pItem->Show			(true);
		pItem->Enable		(true);
		AttachChild(pItem);
	}

	if(xml_doc->NavigateToNode("name_static", 0)){
		pItem = m_icons[eUIName] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "name_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 0);
		AttachChild(pItem);
	}

	// rank
	if(xml_doc->NavigateToNode("rank_static", 0))
	{
		pItem = m_icons[eUIRank] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "rank_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 1);
		AttachChild(pItem);
	}

	if(xml_doc->NavigateToNode("rank_caption", 0))
	{
		pItem = m_icons[eUIRankCaption] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "rank_caption", 0, pItem);
		AttachChild(pItem);
	}

	//community
	if(xml_doc->NavigateToNode("community_static", 0))
	{
		pItem = m_icons[eUICommunity] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "community_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 1);
		AttachChild(pItem);
	}

	if(xml_doc->NavigateToNode("community_caption", 0))
	{
		pItem = m_icons[eUICommunityCaption] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "community_caption", 0, pItem);
		AttachChild(pItem);
	}

	//reputation
	if(xml_doc->NavigateToNode("reputation_static", 0))
	{
		pItem = m_icons[eUIReputation] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "reputation_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 1);
		AttachChild(pItem);
	}

	if(xml_doc->NavigateToNode("reputation_caption", 0))
	{
		pItem = m_icons[eUIReputationCaption] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "reputation_caption", 0, pItem);
		AttachChild(pItem);
	}

	// relation
	if(xml_doc->NavigateToNode("relation_static", 0))
	{
		pItem = m_icons[eUIRelation] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "relation_static", 0, pItem);
		pItem->SetElipsis(CUIStatic::eepEnd, 1);
		AttachChild(pItem);
	}

	if(xml_doc->NavigateToNode("relation_caption", 0))
	{
		pItem = m_icons[eUIRelationCaption] = xr_new<CUIStatic>();
		xml_init.InitStatic(*xml_doc, "relation_caption", 0, pItem);
		AttachChild(pItem);
	}

	if (xml_doc->NavigateToNode("biography_list", 0))
	{
		pUIBio = xr_new<CUIScrollView>();pUIBio->SetAutoDelete(true);
		xml_init.InitScrollView(*xml_doc, "biography_list", 0, pUIBio);
		AttachChild(pUIBio);
	}
}

void CUICharacterInfo::Init(float x, float y, float width, float height, const char* xml_name)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3(xml_result, "xml file not found", xml_name);
	Init(x,y,width,height,&uiXml);
}

void  CUICharacterInfo::InitCharacter(CCharacterInfo* pCharInfo)
{
	VERIFY(pCharInfo);

	CStringTable	stbl;
	string256		str;
	if(m_icons[eUIName]){
		CInventoryOwner* IO = smart_cast<CInventoryOwner*>(Level().Objects.net_Find(m_ownerID));
		m_icons[eUIName]->SetText	(IO->Name());
	}

	if(m_icons[eUIRank]){
	#ifdef _DEBUG
		sprintf(str, "%s,%d", *stbl.translate(GetRankAsText(pCharInfo->Rank().value())), pCharInfo->Rank().value());
	#else
		sprintf(str, "%s", *stbl.translate(GetRankAsText(pCharInfo->Rank().value())));
	#endif

	m_icons[eUIRank]->SetText(str);
	}


	if(m_icons[eUIReputation]){
	#ifdef _DEBUG
		sprintf(str, "%s,%d", *stbl.translate(GetReputationAsText(pCharInfo->Reputation().value())), pCharInfo->Reputation().value());
	#else
		sprintf(str, "%s", *stbl.translate(GetReputationAsText(pCharInfo->Reputation().value())));
	#endif
		m_icons[eUIReputation]->SetText(str);
	}

	if(m_icons[eUICommunity]){
		sprintf(str, "%s", *CStringTable().translate(pCharInfo->Community().id()));
		m_icons[eUICommunity]->SetText(str);
	}

	m_icons[eUIIcon]->SetShader(GetCharIconsShader());
	m_icons[eUIIcon]->GetUIStaticItem().SetOriginalRect(	float(pCharInfo->TradeIconX()*ICON_GRID_WIDTH),
															float(pCharInfo->TradeIconY()*ICON_GRID_HEIGHT),
															float(CHAR_ICON_WIDTH*ICON_GRID_WIDTH),
															float(CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT));
	m_icons[eUIIcon]->SetStretchTexture						(true);

	// Bio
	if (pUIBio && pUIBio->IsEnabled())
	{
		pUIBio->Clear();
		if (pCharInfo->Bio())
		{
			CUIStatic* pItem				= xr_new<CUIStatic>();
			pItem->SetWidth					(pUIBio->GetDesiredChildWidth());
			pItem->SetText					(pCharInfo->Bio());
			pItem->AdjustHeightToText		();
			pUIBio->AddWindow				(pItem, true);
		}
	}
}

void CUICharacterInfo::InitCharacter(CInventoryOwner* pOwner)
{
	m_ownerID = (smart_cast<CObject*>(pOwner))->ID();
	InitCharacter(&pOwner->CharacterInfo());

	UpdateRelation();
}

void  CUICharacterInfo::SetRelation(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill)
{
	shared_str relation_str;

	CStringTable stbl;

	m_icons[eUIRelation]->SetTextColor(GetRelationColor(relation));
	string256		str;
#ifdef _DEBUG
	sprintf(str, "%s,%d", *stbl.translate(GetGoodwillAsText(goodwill)), goodwill);
#else
	sprintf(str, "%s", *stbl.translate(GetGoodwillAsText(goodwill)));
#endif

	m_icons[eUIRelation]->SetText(str);
}


//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::ResetAllStrings()
{
	if(m_icons[eUIName])		m_icons[eUIName]->SetText		("");
	if(m_icons[eUIRank])		m_icons[eUIRank]->SetText		("");
	if(m_icons[eUICommunity])	m_icons[eUICommunity]->SetText	("");
	if(m_icons[eUIRelation])	m_icons[eUIRelation]->SetText	("");
	if(m_icons[eUIReputation])	m_icons[eUIReputation]->SetText	("");
}

void CUICharacterInfo::UpdateRelation()
{
	if(!m_icons[eUIRelation] ||!m_icons[eUIRelationCaption]) return;
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());

		if (pActor->ID()==m_ownerID || !hasOwner())
		{
			if(m_icons[eUIRelationCaption])	m_icons[eUIRelationCaption]->Show	(false);
			if(m_icons[eUIRelation])		m_icons[eUIRelation]->Show			(false);
		}
		else
		{
			CInventoryOwner* IO = smart_cast<CInventoryOwner*>(Level().Objects.net_Find(m_ownerID));
			if(pActor)
				SetRelation(RELATION_REGISTRY().GetRelationType(IO,		static_cast<CInventoryOwner*>(pActor)),
							RELATION_REGISTRY().GetAttitude(IO,			static_cast<CInventoryOwner*>(pActor)));
		}
}

void CUICharacterInfo::Update()
{
	inherited::Update();


	if(hasOwner() && (Device.dwFrame%100==0)  ){
		CObject* O = Level().Objects.net_Find(m_ownerID);
		if (NULL==O)
			m_ownerID = u32(-1);
		else
			UpdateRelation();

		if(O && m_icons[eUIIcon]){
			CEntityAlive* ea = smart_cast<CEntityAlive*>(O);
			if(!ea->g_Alive())
				m_icons[eUIIcon]->SetColor(color_argb(255,255,160,160));
		}
	}
}

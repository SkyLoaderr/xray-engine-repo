// UICharacterInfo.cpp:  окошко, с информацией о персонаже
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;

#include "uicharacterinfo.h"
#include "../actor.h"
#include "../level.h"
#include "../character_info.h"
#include "../string_table.h"
#include "../relation_registry.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "uistatic.h"
//#include "UIFrameWindow.h"
#include "UIListWnd.h"

//////////////////////////////////////////////////////////////////////////

CUICharacterInfo::CUICharacterInfo()
{
	ZeroMemory			(m_icons,eMaxCaption*sizeof(CUIStatic*));
	pUIBio				= NULL;
	pInvOwner			= NULL;
	m_bInfoAutoAdjust	= true;
	m_ownerID			= u32(-1);
}

//////////////////////////////////////////////////////////////////////////

CUICharacterInfo::~CUICharacterInfo()
{
	for(int i=0; i<eMaxCaption;++i)
		if(m_icons[i])xr_delete(m_icons[i]);
}

//////////////////////////////////////////////////////////////////////////

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


	///////////////
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
	/////////////////////
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

	/////////////////////
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

	///////////////////
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
		pUIBio = xr_new<CUIListWnd>();
		xml_init.InitListWnd(*xml_doc, "biography_list", 0, pUIBio);
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

//////////////////////////////////////////////////////////////////////////

void  CUICharacterInfo::InitCharacter(CCharacterInfo* pCharInfo)
{
	VERIFY(pCharInfo);

	string256		str;
	if(m_icons[eUIName]){
//		strcpy						(str,pCharInfo->Name());
		m_icons[eUIName]->SetText	(pCharInfo->Name());
	}

	CStringTable	stbl;

	float offset;

#ifdef _DEBUG
	sprintf(str, "%s,%d", *stbl(GetRankAsText(pCharInfo->Rank().value())), pCharInfo->Rank().value());
#else
	sprintf(str, "%s", *stbl(GetRankAsText(pCharInfo->Rank().value())));
#endif
	if (m_bInfoAutoAdjust)
	{
		if (	m_icons[eUIRankCaption] && 
				m_icons[eUIRankCaption]->IsEnabled() && 
				m_icons[eUIRankCaption]->GetFont())
		{
			offset = (m_icons[eUIRankCaption]->GetFont()->SizeOf(m_icons[eUIRankCaption]->GetText()) + m_icons[eUIRankCaption]->GetWndRect().left + 5.0f);
			m_icons[eUIRank]->SetWndRect(offset, m_icons[eUIRank]->GetWndRect().top, GetWndRect().right - offset - 10.0f, m_icons[eUIRank]->GetWndRect().bottom);
		}
	}
	if(m_icons[eUIRank])
		m_icons[eUIRank]->SetText(str);


#ifdef _DEBUG
	sprintf(str, "%s,%d", *stbl(GetReputationAsText(pCharInfo->Reputation().value())), pCharInfo->Reputation().value());
#else
	sprintf(str, "%s", *stbl(GetReputationAsText(pCharInfo->Reputation().value())));
#endif
	if (m_bInfoAutoAdjust)
	{
		if (m_icons[eUIReputationCaption] &&
			m_icons[eUIReputationCaption]->IsEnabled() && 
			m_icons[eUIReputationCaption]->GetFont())
		{
			offset = (m_icons[eUIReputationCaption]->GetFont()->SizeOf(m_icons[eUIReputationCaption]->GetText()) + m_icons[eUIReputationCaption]->GetWndRect().left + 5.0f);
			m_icons[eUIReputation]->SetWndRect(offset, m_icons[eUIReputation]->GetWndRect().top, GetWndRect().right - offset - 10.0f, m_icons[eUIReputation]->GetWndRect().bottom);
		}
	}
	if(m_icons[eUIReputation])
		m_icons[eUIReputation]->SetText(str);

	sprintf(str, "%s", *CStringTable()(pCharInfo->Community().id()));
	if (m_bInfoAutoAdjust)
	{
		if (m_icons[eUICommunityCaption] && 
			m_icons[eUICommunityCaption]->IsEnabled() && 
			m_icons[eUICommunityCaption]->GetFont())
		{
			offset = (m_icons[eUICommunityCaption]->GetFont()->SizeOf(m_icons[eUICommunityCaption]->GetText()) + m_icons[eUICommunityCaption]->GetWndRect().left + 5.0f);
			m_icons[eUICommunity]->SetWndRect(offset, m_icons[eUICommunity]->GetWndRect().top, GetWndRect().right - offset - 10.0f, m_icons[eUICommunity]->GetWndRect().bottom - m_icons[eUICommunity]->GetWndRect().top);
		}
	}
	if(m_icons[eUICommunity])
		m_icons[eUICommunity]->SetText(str);

	m_icons[eUIIcon]->SetShader(GetCharIconsShader());
	m_icons[eUIIcon]->GetUIStaticItem().SetOriginalRect(	float(pCharInfo->TradeIconX()*ICON_GRID_WIDTH),
															float(pCharInfo->TradeIconY()*ICON_GRID_HEIGHT),
															float(CHAR_ICON_WIDTH*ICON_GRID_WIDTH),
															float(CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT));
	m_icons[eUIIcon]->SetStretchTexture						(true);

	// Bio
	if (pUIBio && pUIBio->IsEnabled())
	{
		pUIBio->RemoveAll();
		static CUIString str;
		if (pCharInfo->Bio())
		{
			str.SetText(pCharInfo->Bio());
			pUIBio->AddParsedItem<CUIListItem>(str, 0.0f, pUIBio->GetTextColor(), pUIBio->GetFont());
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::InitCharacter(CInventoryOwner* pOwner)
{
	VERIFY(pOwner);
	pInvOwner = pOwner;
	m_ownerID = (smart_cast<CObject*>(pInvOwner))->ID();
	InitCharacter(&pInvOwner->CharacterInfo());

	UpdateRelation();
}

//////////////////////////////////////////////////////////////////////////


void  CUICharacterInfo::SetRelation(ALife::ERelationType relation, CHARACTER_GOODWILL goodwill)
{
	shared_str relation_str;

	CStringTable stbl;

	switch(relation) {
	case ALife::eRelationTypeFriend:
		m_icons[eUIRelation]->SetTextColor(0xff00ff00);
		break;
	case ALife::eRelationTypeNeutral:
		m_icons[eUIRelation]->SetTextColor(0xffc0c0c0);
		break;
	case ALife::eRelationTypeEnemy:
		m_icons[eUIRelation]->SetTextColor(0xffff0000);
		break;
	default:
		NODEFAULT;
	}

	string256		str;
#ifdef _DEBUG
	sprintf(str, "%s,%d", *stbl(GetGoodwillAsText(goodwill)), goodwill);
#else
	sprintf(str, "%s", *stbl(GetGoodwillAsText(goodwill)));
#endif

	m_icons[eUIRelation]->SetText(str);
	if (m_bInfoAutoAdjust)
	{
		float offset = (m_icons[eUIRelationCaption]->GetFont()->SizeOf(m_icons[eUIRelationCaption]->GetText()) + m_icons[eUIRelationCaption]->GetWndRect().left + 5.0f);
		m_icons[eUIRelation]->SetWndRect(offset, m_icons[eUIRelation]->GetWndRect().top, GetWndRect().right - offset - 10.0f, m_icons[eUICommunity]->GetWndRect().bottom - m_icons[eUIRelation]->GetWndRect().top);
	}
}


//////////////////////////////////////////////////////////////////////////

void CUICharacterInfo::ResetAllStrings()
{
	if(m_icons[eUIName])		m_icons[eUIName]->SetText("");
	if(m_icons[eUIRank])		m_icons[eUIRank]->SetText("");
	if(m_icons[eUICommunity])	m_icons[eUICommunity]->SetText("");
	if(m_icons[eUIRelation])	m_icons[eUIRelation]->SetText("");
	if(m_icons[eUIReputation])	m_icons[eUIReputation]->SetText("");
}

void CUICharacterInfo::UpdateRelation()
{
	if(!m_icons[eUIRelation] ||!m_icons[eUIRelationCaption]) return;
		CActor *m_pActor = smart_cast<CActor *>(pInvOwner);
		if (m_pActor)
		{
			if(m_icons[eUIRelationCaption])	m_icons[eUIRelationCaption]->Show(false);
			if(m_icons[eUIRelation])		m_icons[eUIRelation]->Show(false);
		}
		else
		{
			CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
			if(pActor)
				SetRelation(RELATION_REGISTRY().GetRelationType(pInvOwner, static_cast<CInventoryOwner*>(pActor)),
							RELATION_REGISTRY().GetAttitude(pInvOwner, static_cast<CInventoryOwner*>(pActor)));
		}
}

void CUICharacterInfo::Update()
{
	inherited::Update();
	if( m_ownerID!=u32(-1) ){
		if (NULL==Level().Objects.net_Find(m_ownerID) )
			pInvOwner = NULL;
	}
	if( pInvOwner&&(smart_cast<CObject*>(pInvOwner))->getDestroy() )
		pInvOwner = NULL;

	if(pInvOwner && (Device.dwFrame%30==0 ) )
		UpdateRelation();
}

#include "stdafx.h"
#include "uigroup.h"
#include "hudmanager.h"
#include "entity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define GROUP_OFFSET		55.f
#define GROUP_FIRST_ITEM	59.f
#define GROUP_ITEM_SIZE		10.f
#define ITEM_COLOR_GOOD		0xFF00A651
#define ITEM_COLOR_MED		0xFFFFF200
#define ITEM_COLOR_BAD		0xFFEF0023

CUIGroup::CUIGroup()
{
	back.Init		("ui\\hud_group_back","font",						0,		8,	64,	64,	alRight|alTop);
	back_sel.Init	("ui\\hud_group_back_sel","font",					0,		8,	64,	64,	alRight|alTop);
	list_top.Init	("ui\\hud_group_list_top","font",					0+3,	42,	64,	16,	alRight|alTop);
	list_item.Init	("ui\\hud_group_list_item","font",					0+3,	0,	64,	16,	alRight|alTop);
	list_bottom.Init("ui\\hud_group_list_bottom","font",				0+3,	0,	64,	4,	alRight|alTop);
	list_item_health.Init	("ui\\hud_group_list_item_health","font",	0+3,	0,	4,	8,	alRight|alTop);
	list_item_ammo.Init		("ui\\hud_group_list_item_health","font",	0+7,	0,	4,	8,	alRight|alTop);
	back.SetRect	(0,0,50,43);
}
//--------------------------------------------------------------------

const DWORD DET_COLOR[3]={ITEM_COLOR_BAD,ITEM_COLOR_MED,ITEM_COLOR_GOOD};

void CUIGroup::Render(CGroup& G, int idx, int grp_index, bool bSelected)
{
	// convert
	float offset		= Level().HUD()->ClientToScreenX(UI_BASE_WIDTH-(idx+1)*GROUP_OFFSET, alRight|alTop);

	// out labels
	CHUDManager* HUD	= Level().HUD();
	float sc			= HUD->GetScale();
	HUD->pHUDFont->Color(0xffffffff);
	HUD->pHUDFont->Out	(offset+5,	10,	"#%02d",grp_index+1);
	HUD->pHUDFont->Out	(offset+30,	40,	"%02d",G.Size());
	float sc_offset		= offset*sc;

	if (bSelected){
		back_sel.SetPosX(sc_offset);
		back_sel.Render	();
	}else{
		back.SetPosX	(sc_offset);
		back.Render		();
	}
	list_top.SetPosX	(sc_offset+3*sc);
	list_top.Render		();
	float Y = GROUP_FIRST_ITEM;
	for (int i=0; i<G.Size(); i++,Y+=GROUP_ITEM_SIZE){
		CEntity* E = G.Members[i];
		// update item back
		list_item.SetPos			(sc_offset+3*sc,Y*sc);
		// update item health
		int val = iFloor			(3*float(E->g_Health())/100.f); clamp(val,0,2);
		list_item_health.SetColor	(DET_COLOR[val]);	
		list_item_health.SetPos		(sc_offset+3*sc,Y*sc);	
		// update item ammo 
		list_item_ammo.SetColor		(DET_COLOR[2]);	
		list_item_ammo.SetPos		(sc_offset+7*sc,Y*sc);	
		// render all
		list_item.Render			();
		list_item_health.Render		();
		list_item_ammo.Render		();
		// out name
		HUD->pHUDFont->Out			(offset+11,Y,"%6.6s",E->cName());
	}
	list_bottom.SetPos(sc_offset+3*sc,Y*sc);
	list_bottom.Render();
}
//--------------------------------------------------------------------

CUISquad::CUISquad(){
}

void CUISquad::Init(){
}

CUISquad::~CUISquad(){
}

void CUISquad::Render(CSquad& S, bool* bSel, bool bActive){
	int idx=0;
	for (int i=0; i<MAX_GROUPS; i++,idx){
		CGroup& G		= S.Groups[i];
		if (!G.Empty())	group.Render(G,idx++,i,bSel[i]);
/*		if (!G.Empty()){
			if (bSelGroups[i]) m_Parent->pSmallFont->Color(0xffffff00);
			m_Parent->pSmallFont->OutSet(x,y);
			m_Parent->pSmallFont->OutNext("%d:G#%d",i+1,i+1);
			m_Parent->pSmallFont->Color(0xffffffff);
			CGroup& G					= S.Groups[i];
			m_Parent->pSmallFont->OutNext("+-----");
			m_Parent->pSmallFont->OutNext("|sz:%d",G.Size());
			m_Parent->pSmallFont->OutNext("+-----");
			m_Parent->pSmallFont->OutNext("|CM:%d",G.State);
			m_Parent->pSmallFont->OutNext("+-----");
			m_Parent->pSmallFont->OutNext("|AG:%s",(G.Flags&gtAgressive)?"+":"-");
			m_Parent->pSmallFont->OutNext("|QT:%s",(G.Flags&gtQuiet)?"+":"-");
			m_Parent->pSmallFont->OutNext("+-----");
		}
*/	}
}


#include "StdAfx.h"
#include "UIStatsPlayerList.h"
#include "../game_cl_base.h"
#include "UIStatsPlayerInfo.h"
#include "../string_table.h"
#include "../level.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "xrXmlParser.h"

IC bool	DM_Compare_Players		(LPVOID v1, LPVOID v2);

CUIStatsPlayerList::CUIStatsPlayerList(){
	m_cmp_func		= DM_Compare_Players;
	m_CurTeam		= 0;
	m_bSpectator	= false;

	m_header = xr_new<CUIStatic>();
	m_header_team = NULL;
	m_header_text = NULL;
	m_i.c = 0xff000000;	m_i.f = NULL;	m_i.h = 16;
	m_h = m_i;
	m_t = m_i;
	m_prev_upd_time = 0; items.reserve(32);
}


void CUIStatsPlayerList::AddField(const char* name, float width){
	PI_FIELD_INFO fi;
	fi.name = name;
	fi.width = width;
	m_field_info.push_back(fi);
}

void CUIStatsPlayerList::Init(CUIXml& xml_doc, LPCSTR path){
	// init window
	CUIXmlInit::InitScrollView(xml_doc, path, 0, this);
	SetFixedScrollBar(false);

	m_bSpectator = xml_doc.ReadAttribInt(path,0,"spectator",0)? true: false;
	SetSpectator(m_bSpectator);

	// init item structure
	int tabsCount	= xml_doc.GetNodesNum(path, 0, "field");
	XML_NODE* tab_node = xml_doc.NavigateToNode(path,0);
	xml_doc.SetLocalRoot(tab_node);

	for (int i = 0; i < tabsCount; ++i)
	{
		LPCSTR name = xml_doc.ReadAttrib("field",i,"name");
		float width = xml_doc.ReadAttribFlt("field",i,"width");
		if (0 == xr_strcmp(name, "artefacts") && GameID() != GAME_ARTEFACTHUNT)
			continue;
		AddField(name,width);
	}
	xml_doc.SetLocalRoot(xml_doc.GetRoot());
	string256 _path;
	// init item text params
	CUIXmlInit::InitFont(xml_doc, strconcat(_path, path, ":text_format"), 0, m_i.c, m_i.f);
	m_i.h = xml_doc.ReadAttribFlt(strconcat(_path, path, ":text_format"), 0, "height", 25);

	// init list header
	switch (GameID())
	{
	case GAME_ARTEFACTHUNT:
	case GAME_TEAMDEATHMATCH:
		if (!m_bSpectator)
            InitTeamHeader(xml_doc, path);
	case GAME_DEATHMATCH:
		InitHeader(xml_doc, path);
	default:
		break;
	}
}

void CUIStatsPlayerList::InitHeader(CUIXml& xml_doc, LPCSTR path){
	string256 _path;
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path, path, ":list_header"), 0, m_header);
	m_header->SetWidth(this->GetDesiredChildWidth());
	m_h.h = m_header->GetHeight();

	CUIXmlInit::InitFont(xml_doc, strconcat(_path, path, ":list_header:text_format"), 0, m_h.c, m_h.f);
	float indent = 10;
	if (!m_bSpectator)
	{
		for (u32 i = 0; i<m_field_info.size(); i++)
		{
			CUIStatic* st = xr_new<CUIStatic>();
			st->SetAutoDelete(true);
			//#pragma todo("Satan->Satan: remove stub for height")
			st->Init(indent,10,m_field_info[i].width, 15);
			indent += m_field_info[i].width;

			if (0 == xr_strcmp(m_field_info[i].name, "rank"))
				st->SetText("");
			else if (0 == xr_strcmp(m_field_info[i].name, "death_atf"))
				st->SetText("");
			else
				st->SetTextST(*m_field_info[i].name);
			
			if (m_h.f)
				st->SetFont(m_h.f);
			st->SetTextColor(m_h.c);
			st->SetTextComplexMode(false);
			m_header->AttachChild(st);
		}
	}
	else
	{
		CUIStatic* st = xr_new<CUIStatic>();
		st->SetAutoDelete(true);
		st->Init(10,0,this->GetDesiredChildWidth(),m_h.h);		
		if (m_h.f)
			m_header->SetFont(m_h.f);
		st->SetTextColor(m_h.c);
		st->SetVTextAlignment(valCenter);
		st->SetTextComplexMode(false);
		st->SetText("SPECTATORS");
		m_header->AttachChild(st);
	}
}

void CUIStatsPlayerList::InitTeamHeader(CUIXml& xml_doc, LPCSTR path){
	string256 _path;
	m_header_team = xr_new<CUIWindow>();
	m_header_team->SetAutoDelete(true);
	CUIXmlInit::InitWindow(xml_doc, strconcat(_path, path, ":team_header"), 0, m_header_team);
	m_header_team->SetWidth(this->GetDesiredChildWidth());

	CUIStatic* logo = xr_new<CUIStatic>();
	logo->SetAutoDelete(true);
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path, path, ":team_header:logo"), 0, logo);
	m_header_team->AttachChild(logo);

	if (1 == m_CurTeam)
        logo->InitTexture(pSettings->r_string("team_logo_small", "team1"));
	else if (2 == m_CurTeam)
		logo->InitTexture(pSettings->r_string("team_logo_small", "team2"));
	else
		R_ASSERT2(false, "invalid team");
	
	S_ELEMENT t;
	CUIXmlInit::InitFont(xml_doc, strconcat(_path, path, ":team_header:text_format"), 0, t.c, t.f);
	t.h = m_header_team->GetHeight();

	m_header_text = xr_new<CUIStatic>();
	m_header_text->SetAutoDelete(true);
	CUIXmlInit::InitStatic(xml_doc, strconcat(_path, path, ":team_header:header"), 0, m_header_text);
	m_header_text->SetWidth(GetDesiredChildWidth());
	m_header_text->SetVTextAlignment(valCenter);
	m_header_team->AttachChild(m_header_text);
	if (t.f)
		m_header_text->SetFont(t.f);
	m_header_text->SetTextColor(t.c);
}

void CUIStatsPlayerList::Update(){
	static string512 teaminfo;
	if (m_prev_upd_time > Device.TimerAsyncMM() - 100)
		return;
	m_prev_upd_time = Device.TimerAsyncMM();
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	items.clear			();
	u32 pl_count = Game().players.size();
	int pl_frags = 0;
	u32	pl_artefacts = 0;
	for (;I!=E;++I)		
	{
		game_PlayerState* p = (game_PlayerState*) I->second;
		if (!p || p->team != m_CurTeam) 
			continue;
		if (m_bSpectator && p->testFlag(GAME_PLAYER_FLAG_SPECTATOR) ||
			!m_bSpectator && !p->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) 
		{
			items.push_back(I->second);

			// add to team info
			pl_frags+=p->kills;
            pl_artefacts += p->af_count;
		}
	};

    if (GameID() == GAME_ARTEFACTHUNT && !m_bSpectator)
	{
        sprintf(teaminfo, "Players: %u,  Frags: %d,  Artefacts: %u", pl_count, pl_frags, pl_artefacts);
		m_header_text->SetText(teaminfo);
	}
	else if (GameID() == GAME_TEAMDEATHMATCH && !m_bSpectator)
	{
		sprintf(teaminfo, "Players: %u,  Frags: %d", pl_count, pl_frags);
		m_header_text->SetText(teaminfo);
	}
	

	if (m_bSpectator)
	{
		if(items.empty())
		{
			Clear();
			ShowHeader(false);
			return;
		}
		else
			ShowHeader(true);
	}

	if (m_cmp_func)
        std::sort(items.begin(), items.end(), m_cmp_func);

	int n = (int)items.size();
	n -= m_pad->GetChildWndList().size();

	if (n<0)	
	{
		n = abs(n);
		for (int i = 0; i<n; i++)
            m_pad->DetachChild(*(m_pad->GetChildWndList().begin()));
		m_flags.set			(eNeedRecalc,TRUE);
	}
	else
	{		
		for (int i = 0; i<n; i++)
		{
            CUIStatsPlayerInfo* pi = xr_new<CUIStatsPlayerInfo>(&m_field_info, m_i.f, m_i.c);
			pi->Init(0,0,this->GetDesiredChildWidth(),m_i.h);
			CUIScrollView::AddWindow(pi, true);
			m_flags.set			(eNeedRecalc,TRUE);
		}
	}

	R_ASSERT(items.size() == m_pad->GetChildWndList().size());

	WINDOW_LIST_it	it		= m_pad->GetChildWndList().begin();
	ItemIt			itit	= items.begin();

	for (; it != m_pad->GetChildWndList().end(); it++, itit++)
	{
		CUIStatsPlayerInfo* pi = smart_cast<CUIStatsPlayerInfo*>(*it);
		R_ASSERT(pi);
		game_PlayerState* ps = static_cast<game_PlayerState*>(*itit);
		pi->SetInfo(ps);
	}

	// update player info

	CUIScrollView::Update();	
}

void CUIStatsPlayerList::SetCmpFunc(player_cmp_func pf){
	m_cmp_func = pf;
}

void CUIStatsPlayerList::SetSpectator(bool f){
    m_bSpectator = f;
}

void CUIStatsPlayerList::SetTeam(int team){
	m_CurTeam = team;
}

void CUIStatsPlayerList::AddWindow(CUIWindow* pWnd, bool auto_delete){
	R_ASSERT2(false, "fucking shit!");
}

CUIStatic* CUIStatsPlayerList::GetHeader(){
	return m_header;
}

CUIWindow* CUIStatsPlayerList::GetTeamHeader(){
	return m_header_team;
}

void CUIStatsPlayerList::RecalcSize(){
	CUIScrollView::RecalcSize();
    if(GetHeight()<m_pad->GetHeight())
	{
		SetHeight(m_pad->GetHeight());
		GetMessageTarget()->SendMessage(this, CHILD_CHANGED_SIZE, NULL);
	}
}

void CUIStatsPlayerList::ShowHeader(bool bShow){
	if (m_header)
	{
		m_header->Show(bShow);
		m_header->SetHeight(bShow? m_h.h : 0);
	}
	m_flags.set			(eNeedRecalc,TRUE);
}

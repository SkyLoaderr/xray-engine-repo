#include "StdAfx.h"
#include "UIStatsPlayerList.h"
#include "../game_cl_base.h"
#include "UIStatsPlayerInfo.h"
#include "../string_table.h"
#include "../level.h"
#include "UIStatic.h"

CUIStatsPlayerList::CUIStatsPlayerList(){
	m_cmp_func		= NULL;
	m_CurTeam		= 0;
	m_bSpectator	= false;

	m_header.wnd = NULL;
	m_header.height = 0;
}

CUIStatsPlayerList::~CUIStatsPlayerList(){

}

void CUIStatsPlayerList::AddField(const char* name, float width){
	PI_FIELD_INFO fi;
	fi.name = name;
	fi.width = width;
	m_field_info.push_back(fi);
}

void CUIStatsPlayerList::Update(){
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	items.clear			();
	for (;I!=E;++I)		
	{
		game_PlayerState* p = (game_PlayerState*) I->second;
		if (!p || p->team != m_CurTeam) 
			continue;
		if (m_bSpectator && p->testFlag(GAME_PLAYER_FLAG_SPECTATOR) ||
			!m_bSpectator && !p->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) 
		{
			items.push_back(I->second);
		}
	};

	if(items.empty())
	{
		Clear();
		ShowHeader(false);
		return;
	}
	else
		ShowHeader(true);

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
            CUIStatsPlayerInfo* pi = xr_new<CUIStatsPlayerInfo>(&m_field_info);
#pragma todo("SATAN->SATAN: init height")
			pi->Init(0,0,this->GetDesiredChildWidth(),15);
			CUIScrollView::AddWindow(pi, true);	

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

CUIWindow* CUIStatsPlayerList::GetHeader(CGameFont* pF, const u32 col){
	if (m_header.wnd)
		return m_header.wnd;

	CUIWindow* pWnd = xr_new<CUIWindow>();
	//#pragma todo("Satan->Satan: remove stub for height")
	pWnd->Init(0,0,this->GetDesiredChildWidth(),25);

	float indent = 10;
	CStringTable strtbl;

	if (!m_bSpectator)
	{
		for (u32 i = 0; i<m_field_info.size(); i++)
		{
			CUIStatic* st = xr_new<CUIStatic>();
			//#pragma todo("Satan->Satan: remove stub for height")
			st->Init(indent,10,m_field_info[i].width, 15);
			indent += m_field_info[i].width;
			st->SetText(*strtbl.translate(m_field_info[i].name));
			st->SetAutoDelete(true);
			if (pF)
				st->SetFont(pF);
			st->SetTextColor(col);
			st->SetTextComplexMode(false);
			pWnd->AttachChild(st);
		}
	}
	else
	{
		CUIStatic* st = xr_new<CUIStatic>();
		st->Init(10,10,this->GetDesiredChildWidth(),15);
		st->SetAutoDelete(true);
		if (pF)
			st->SetFont(pF);
		st->SetTextColor(col);
		st->SetTextComplexMode(false);
		st->SetText("-- Spectators --");
		pWnd->AttachChild(st);
	}

	m_header.wnd = pWnd;
	m_header.height = pWnd->GetHeight();

    return pWnd;
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
	if (m_header.wnd)
	{
		m_header.wnd->Show(bShow);
		m_header.wnd->SetHeight(bShow? m_header.height : 0);
	}
}
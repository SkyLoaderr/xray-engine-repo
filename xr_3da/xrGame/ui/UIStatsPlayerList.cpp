#include "StdAfx.h"
#include "UIStatsPlayerList.h"
#include "../game_cl_base.h"
#include "UIStatsPlayerInfo.h"
#include "../string_table.h"
#include "../level.h"
#include "UIStatic.h"

IC bool	DM_Compare_Players		(LPVOID v1, LPVOID v2);

CUIStatsPlayerList::CUIStatsPlayerList(){
	m_cmp_func		= DM_Compare_Players;
	m_CurTeam		= 0;
	m_bSpectator	= false;

	m_header.wnd = NULL;
	m_header.height = 25;

	m_pTextFont	= NULL;
	m_text_col	= 0xff000000;
	m_prev_upd_time = 0; items.reserve(32);
//	m_header_height = 25;
}

CUIStatsPlayerList::~CUIStatsPlayerList(){
//	xr_delete(m_header.wnd);

}

void CUIStatsPlayerList::AddField(const char* name, float width){
	PI_FIELD_INFO fi;
	fi.name = name;
	fi.width = width;
	m_field_info.push_back(fi);
}

void CUIStatsPlayerList::Update(){
	if (m_prev_upd_time > Device.TimerAsyncMM() - 100)
		return;
	m_prev_upd_time = Device.TimerAsyncMM();
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
            CUIStatsPlayerInfo* pi = xr_new<CUIStatsPlayerInfo>(&m_field_info, m_pTextFont, m_text_col);
#pragma todo("SATAN->SATAN: init height")
			pi->Init(0,0,this->GetDesiredChildWidth(),15);
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

void CUIStatsPlayerList::SetHeaderHeight(float h){
	m_header.height = h;
}

CUIStatic* CUIStatsPlayerList::GetHeader(CGameFont* pF, const u32 col, LPCSTR texture){
	if (m_header.wnd)
		return m_header.wnd;

	m_header.wnd = xr_new<CUIStatic>();
	//#pragma todo("Satan->Satan: remove stub for height")
	m_header.wnd->Init(0,0,this->GetDesiredChildWidth(),m_header.height);

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

			if (0 == xr_strcmp(m_field_info[i].name, "rank"))
				st->SetText("");
			else if (0 == xr_strcmp(m_field_info[i].name, "death_atf"))
				st->SetText("");
			else
                st->SetText(*strtbl.translate(m_field_info[i].name));


			st->SetAutoDelete(true);
			if (pF)
				st->SetFont(pF);
			st->SetTextColor(col);
			st->SetTextComplexMode(false);
			m_header.wnd->AttachChild(st);
		}
	}
	else
	{
		CUIStatic* st = xr_new<CUIStatic>();
		st->Init(10,0,this->GetDesiredChildWidth(),m_header.height);
		st->SetAutoDelete(true);
		if (pF)
			m_header.wnd->SetFont(pF);
		st->SetTextColor(col);
		st->SetVTextAlignment(valCenter);
		st->SetTextComplexMode(false);
		st->SetText("SPECTATORS");
		m_header.wnd->InitTexture(texture);
		m_header.wnd->AttachChild(st);
	}

//	m_header.wnd = pWnd;
//	m_header.height = pWnd->GetHeight();

//	if (texture && xr_strlen(texture))
//        m_header.wnd->InitTexture(texture);

    return m_header.wnd;
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

void CUIStatsPlayerList::SetTextParams(CGameFont* pF, u32 col){
	m_pTextFont = pF;
	m_text_col = col;
}
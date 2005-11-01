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

	PI_FIELD_INFO f;

	f.name = "name";
	f.width = 100;
	m_field_info.push_back(f);
	f.name = "frags";
	f.width = 40;
	m_field_info.push_back(f);
	f.name = "deaths";
	f.width = 40;
	m_field_info.push_back(f);
	f.name = "ping";
	f.width = 40;
	m_field_info.push_back(f);
	f.name = "rank";
	f.width = 40;
	m_field_info.push_back(f);
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
		if (!p || p->team != m_CurTeam) continue;
		if (m_bSpectator)
		{
			if (p->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) 
				items.push_back(I->second);
		}
		else
            items.push_back(I->second);
	};

	if(items.empty())
	{
		Clear();
		return;
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

CUIWindow* CUIStatsPlayerList::GetHeader(){
	CUIWindow* pWnd = xr_new<CUIWindow>();
	pWnd->Init(0,0,this->GetDesiredChildWidth(),15);

//	xr_vector<PI_FIELD_INFO>&	field_info = *m_field_info;
	float indent = 0;
	CStringTable strtbl;	
	
	for (u32 i = 0; i<m_field_info.size(); i++)
	{
		CUIStatic* st = xr_new<CUIStatic>();
#pragma todo("Satan->Satan: remove stub for height")
		st->Init(indent,0,m_field_info[i].width, pWnd->GetHeight());
		indent += m_field_info[i].width;
		st->SetText(*strtbl.translate(m_field_info[i].name));
		st->SetAutoDelete(true);
		pWnd->AttachChild(st);
	}

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
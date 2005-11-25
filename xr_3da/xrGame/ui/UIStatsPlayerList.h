#pragma once

#include "UIScrollView.h"
#include "UIStatsPlayerInfo.h"

typedef bool(*player_cmp_func)(LPVOID v1, LPVOID v2);

class CUIStatsPlayerList : public CUIScrollView{
public:
	CUIStatsPlayerList();
	virtual ~CUIStatsPlayerList();

			void SetCmpFunc(player_cmp_func pf);
			void SetSpectator(bool f);
			void SetTeam(int team);
			void AddField(const char* name, float width);
	CUIStatic*	 GetHeader(CGameFont* pF, u32 col, LPCSTR texture);
			void SetTextParams(CGameFont* pF, u32 col);
			void SetHeaderHeight(float h);
	virtual void AddWindow(CUIWindow* pWnd, bool auto_delete = true);
	virtual void Update();

protected:
	virtual void		RecalcSize			();
			void		ShowHeader(bool bShow);

	player_cmp_func		m_cmp_func;
	int					m_CurTeam;
	bool				m_bSpectator;
    
	DEFINE_VECTOR	(LPVOID,ItemVec,ItemIt);
	ItemVec			items;

	xr_vector<PI_FIELD_INFO>	m_field_info;

	typedef struct{
		CUIStatic*	wnd;
		float		height;
	} S_HEADER_DATA;

    S_HEADER_DATA	m_header;
	CGameFont*	m_pTextFont;
	u32			m_text_col;
	u32			m_prev_upd_time;
//	float		m_header_height;
};
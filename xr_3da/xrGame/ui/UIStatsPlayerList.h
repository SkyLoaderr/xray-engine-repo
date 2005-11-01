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
	CUIWindow*	 GetHeader();
	virtual void AddWindow(CUIWindow* pWnd, bool auto_delete = true);
	virtual void Update();

protected:
	virtual void		RecalcSize			();


	player_cmp_func		m_cmp_func;
	int					m_CurTeam;
	bool				m_bSpectator;

	DEFINE_VECTOR	(LPVOID,ItemVec,ItemIt);
	ItemVec			items;

	xr_vector<PI_FIELD_INFO>	m_field_info;
};
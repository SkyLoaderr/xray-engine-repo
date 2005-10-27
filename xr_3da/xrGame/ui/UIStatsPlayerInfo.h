#pragma once

#include "UIWindow.h"

struct game_PlayerState;
class CUIStatic;

class CUIStatsPlayerInfo : public CUIWindow {
public:
	CUIStatsPlayerInfo();
	~CUIStatsPlayerInfo();
			void SetInfo(game_PlayerState* pInfo);
			void AddField(shared_str& name, float len);
	virtual void Update();

protected:
//			int GetInfoByID(const char* id);
	const char* GetInfoByID(const char* id);
	game_PlayerState*	m_pPlayerInfo;

	typedef struct{
		shared_str name;
		CUIStatic* wnd;
	} FIELD;

	xr_vector<FIELD>	m_fields;
};
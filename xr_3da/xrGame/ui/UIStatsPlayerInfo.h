#pragma once

#include "UIWindow.h"

struct game_PlayerState;
class CUIStatic;

typedef struct {
	shared_str	name;
	float		width;
} PI_FIELD_INFO;

class CUIStatsPlayerInfo : public CUIWindow {
public:
	CUIStatsPlayerInfo(xr_vector<PI_FIELD_INFO>* info, CGameFont* pF, u32 text_col);
	~CUIStatsPlayerInfo();

			void SetInfo(game_PlayerState* pInfo);
	virtual void Init(float x, float y, float width, float height);
	virtual void Update();

protected:
			void AddField(float len, CGameFont* pF, u32 text_col, bool icon);
//			int GetInfoByID(const char* id);
	const char* GetInfoByID(const char* id);
	game_PlayerState*	m_pPlayerInfo;

	xr_vector<CUIStatic*>	m_fields;
	xr_vector<PI_FIELD_INFO>*	m_field_info;
	CGameFont*	m_pF;
	u32			m_text_col;
	CUIStatic*	m_pBackground;
};
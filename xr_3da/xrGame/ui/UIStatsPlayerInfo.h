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
	CUIStatsPlayerInfo(xr_vector<PI_FIELD_INFO>* info);
	~CUIStatsPlayerInfo();

			void SetInfo(game_PlayerState* pInfo);			
	virtual void Update();

protected:
			void AddField(float len);
//			int GetInfoByID(const char* id);
	const char* GetInfoByID(const char* id);
	game_PlayerState*	m_pPlayerInfo;

	xr_vector<CUIStatic*>	m_fields;
	xr_vector<PI_FIELD_INFO>*	m_field_info;
};
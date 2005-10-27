#pragma once

#include "UIScrollView.h"

typedef bool(*player_cmp_func)(LPVOID v1, LPVOID v2);

class CUIStatsPlayerList : protected CUIScrollView{
public:
	CUIStatsPlayerList();
	virtual ~CUIStatsPlayerList();

	virtual void Update();

protected:


};
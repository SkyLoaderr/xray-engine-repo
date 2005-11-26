#pragma once

#include "UIStatic.h"

class CUIStatsIcon : public CUIStatic {
public:
	CUIStatsIcon();
	virtual void SetText(LPCSTR str);


protected:
	enum DEF_TEX{
		RANK_0 = 0,
		RANK_1,
		RANK_2,
		RANK_3,
		RANK_4,
		RANK_5,
		ARTEFACT,
		DEATH,

		MAX_DEF_TEX
	};
	typedef struct {
		ref_shader	sh;
		Frect		rect;
	} TEX_INFO;


	static void	InitTexInfo();
	static TEX_INFO		m_tex_info[MAX_DEF_TEX];
};
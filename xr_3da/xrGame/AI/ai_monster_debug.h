	#pragma once

class CEntityAlive;

class CMonsterDebug {
	CEntityAlive *pMonster;

	struct _elem {
		string128	text;
		u32			col;
	};
	
	struct _elem_line {
		Fvector pos;
		u32		col;
	};

	struct _elem_const {
		string128	text;
		float		x;
		float		y;
	};

	xr_vector<_elem>		_data;
	xr_vector<_elem_line>	_lines;
	xr_vector<_elem_const>	_text;

	float	floor_height;
	Fvector shift;

	bool	active;

public:
			CMonsterDebug		(CEntityAlive *pM, const Fvector &shift_vec, float height);
			~CMonsterDebug();

	void	SetActive			(bool bActivity = true) {active = bActivity;}	
	
	// -------------------------------------------------------------------
	// Monster caption
	void	M_Add				(u32 index, LPCSTR str, u32 col);
	void	M_Update			();
	void	M_Clear				();	
	
	// -------------------------------------------------------------------
	// Level lines
	void	L_Add				(const Fvector &pos, u32 col);
	void	L_Update			();
	void	L_Clear				();

	// -------------------------------------------------------------------
	// Hud text
	void	HT_Add				(float x, float y, LPCSTR str);
	void	HT_Update			();
	void	HT_Clear			();

};



#pragma once

class CEntityAlive;

class CMonsterDebug {
	CEntityAlive *pMonster;

	struct _elem {
		string128	text;
		u32			col;
	};
	
	xr_vector<_elem> _data;

	float	floor_height;
	Fvector shift;

	bool	active;

public:
			CMonsterDebug(CEntityAlive *pM, const Fvector &shift_vec, float height);
			~CMonsterDebug();

	void	Clear();
	void	Add(u32 index, LPCSTR str, u32 col);

	void	Update();
	void	SetActive(bool bActivity = true) {active = bActivity;}
};
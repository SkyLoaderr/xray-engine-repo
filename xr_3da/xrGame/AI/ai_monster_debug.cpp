#include "stdafx.h"
#include "ai_monster_debug.h"
#include "..\\entity.h"
#include "..\\hudmanager.h"

CMonsterDebug::CMonsterDebug(CEntityAlive *pM, const Fvector &shift_vec, float height)
{
	pMonster		= pM;
	floor_height	= height;
	shift			= shift_vec;
	active			= false;
}

CMonsterDebug::~CMonsterDebug()
{
	
}

void CMonsterDebug::Clear()
{
	_data.clear();
}
void CMonsterDebug::Add(u32 index, LPCSTR str, u32 col)
{
	if (_data.size() < index) index = _data.size();

	_elem new_elem;
	strcpy(new_elem.text, str);
	new_elem.col = col;

	_data.push_back(new_elem);
}

void CMonsterDebug::Update()
{ 
	if (!active) return;

	Fmatrix res;
	res.mul(Device.mFullTransform,pMonster->XFORM());

	Fvector4 v_res;

	res.transform(v_res,shift);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	for (u32 i=0; i<_data.size(); i++) {
		HUD().pFontMedium->SetColor(_data[i].col);
		HUD().pFontMedium->OutSet(x,y-=floor_height);
		HUD().pFontMedium->OutNext(_data[i].text);
	}
}




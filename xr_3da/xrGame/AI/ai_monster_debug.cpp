#include "stdafx.h"
#include "ai_monster_debug.h"
#include "..\\entity.h"
#include "..\\hudmanager.h"

#define DISABLE_MONSTER_DEBUG

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

void CMonsterDebug::M_Clear()
{
	_data.clear();
}
void CMonsterDebug::M_Add(u32 index, LPCSTR str, u32 col)
{
	if (_data.size() < index) index = _data.size();

	_elem new_elem;
	strcpy(new_elem.text, str);
	new_elem.col = col;

	if (index < _data.size()) _data[index] = new_elem;
	else _data.push_back(new_elem);
}

void CMonsterDebug::M_Update()
{ 
	if (!active) return;

#ifdef DISABLE_MONSTER_DEBUG
	if (active) return;
#endif

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

void CMonsterDebug::L_Add(const Fvector &pos, u32 col)
{
	for	(u32 i=0; i<_lines.size(); i++) {
		if (_lines[i].pos.similar(pos)) return;
	}
	
	_elem_line new_line;

	new_line.pos = pos;
	new_line.col = col;
	
	_lines.push_back(new_line);
}

void CMonsterDebug::L_Clear()
{
	_lines.clear();
}

void CMonsterDebug::L_Update()
{
	if (!active) return;

#ifdef DISABLE_MONSTER_DEBUG
	if (active) return;
#endif


	for (u32 i=0; i<_lines.size(); i++) {
		RCache.dbg_DrawAABB(_lines[i].pos,0.35f,0.35f,0.35f,_lines[i].col);

		Fvector upV;
		upV = _lines[i].pos;
		upV.y += 5.0f;

		RCache.dbg_DrawLINE(Fidentity,_lines[i].pos,upV,_lines[i].col);
	}
}


void CMonsterDebug::HT_Add(float x, float y, LPCSTR str)
{
	for	(u32 i=0; i<_text.size(); i++) {
		if (strcmp(_text[i].text, str) == 0) return;
	}

	_elem_const new_text;
		
	strcpy(new_text.text, str);
	
	new_text.x	=	x;
	new_text.y	=	y;
	
	_text.push_back(new_text);
}

void CMonsterDebug::HT_Clear()
{
	_text.clear();
}

void CMonsterDebug::HT_Update()
{
	if (!active) return;
#ifdef DISABLE_MONSTER_DEBUG
	if (active) return;
#endif


	for (u32 i=0; i<_text.size(); i++) {
		HUD().pFontSmall->OutSet(_text[i].x,_text[i].y);
		HUD().pFontSmall->OutNext(_text[i].text);
	}
}



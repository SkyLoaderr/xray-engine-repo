// LevelFogOfWar.h:  для карты уровня в одиночном режиме игры
// 
//////////////////////////////////////////////////////////////////////

#pragma once

/*
//клеточка, которыми покрывается уровень;
//для "тумана войны" над картой
struct SFogOfWarCell 
{
	bool IsOpened() {return 0xFF == shape;}
	void Open()	{shape = 0xFF;}

	//вид клеточки - зависит от количества открытых
	//соседей и их расположения
	//0		- полностью закрыта
	//0xFF	- полность открыта
	unsigned char shape;
};*/

//флаги, опряделяющие форму клеточки тумна
const static u8 FOG_OPEN_LT		= 0x01;
const static u8 FOG_OPEN_RT		= 0x02;
const static u8 FOG_OPEN_LB		= 0x04;
const static u8 FOG_OPEN_RB		= 0x08;
const static u8 FOG_OPEN_T		= FOG_OPEN_LT|FOG_OPEN_RT;
const static u8 FOG_OPEN_B		= FOG_OPEN_LB|FOG_OPEN_RB;
const static u8 FOG_OPEN_L		= FOG_OPEN_LT|FOG_OPEN_LB;
const static u8 FOG_OPEN_R		= FOG_OPEN_RT|FOG_OPEN_RB;
const static u8 FOG_OPEN_TOTAL	= FOG_OPEN_LT|FOG_OPEN_RT|FOG_OPEN_LB|FOG_OPEN_RB;
const static u8 FOG_CLOSED		= 0x00;



#include "alife_abstract_registry.h"

class CUICustomMap;
class CFogOfWarWrapper;

class CLevelFogOfWar: public IPureSerializeObject<IReader,IWriter> 
{
public:
	ref_shader			hShader;
	ref_geom			hGeom;	

	CLevelFogOfWar		();
	shared_str			m_level_name;
	Frect				m_levelRect;
	u32					m_rowNum, m_colNum;
	xr_vector<bool>		m_cells;

	void				Open					(Fvector2 pos);
	void				Open					(u32 row, u32 col, bool mask);
	void				Init					(const shared_str& level);
	void				Draw					(CUICustomMap* m);
	void				GetTexUVLT				(Fvector2& frame, u32 col, u32 row);
	Ivector2			ConvertRealToLocal		(const Fvector2& src);
	Irect				ConvertRealToLocal		(const Frect& src);
	Fvector2			ConvertLocalToReal		(const Ivector2& src);

	virtual void		save					(IWriter &stream);
	virtual void		load					(IReader &stream);

};


DEFINE_VECTOR(CLevelFogOfWar,FOG_STORAGE_T,FOG_STORAGE_IT);
//DEFINE_VECTOR(int,FOG_STORAGE_T,FOG_STORAGE_IT);

typedef CALifeAbstractRegistry<u16, FOG_STORAGE_T> CFogOfWarRegistry;


class CFogOfWarMngr
{
	CFogOfWarWrapper*				m_fogOfWarRegistry;
	FOG_STORAGE_T&					GetFogStorage();
public:
	CFogOfWarMngr					();
	CLevelFogOfWar*			GetFogOfWar	(const shared_str& level_name);
};

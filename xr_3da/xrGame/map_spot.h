#pragma once
#include "ui/UIStatic.h"

class CMapLocation;
class CUIXml;


class CMapSpot :public CUIStatic
{
	typedef CUIStatic inherited;
	CMapLocation*			m_map_location;
public:
							CMapSpot						(CMapLocation*);
	virtual					~CMapSpot						();
	virtual		void		Load							(CUIXml* xml, LPCSTR path);
	CMapLocation*			MapLocation						()							{return m_map_location;}
	virtual LPCSTR			GetHint							();
	virtual		void		Update							();
	virtual		void		Draw							();
};


class CMapSpotPointer :public CMapSpot
{
	typedef CMapSpot inherited;
	xr_string				m_pointer_hint;
public:
							CMapSpotPointer					(CMapLocation*);
	virtual					~CMapSpotPointer				();
	virtual		void		Draw							();
	virtual		void		Update							();
	virtual LPCSTR			GetHint							();
};
/*
class CZoneMapSpot :public CMapSpot
{
	typedef CMapSpot inherited;

public:
							CZoneMapSpot					(CMapLocation*);
	virtual					~CZoneMapSpot					();
	virtual		void		Draw							();
	virtual		void		Update							();
};
*/
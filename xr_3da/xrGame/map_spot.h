#pragma once
#include "ui/UIStatic.h"
#include "ui/UIColorAnimatorWrapper.h"

class CMapLocation;
class CUIXml;

class CMapSpot :public CUIStatic
{
	typedef CUIStatic inherited;
	CMapLocation*			m_map_location;

protected:
	enum ESpotFlags{eHeading = u32(1 << 0)};
	Flags32					m_flags;

public:
							CMapSpot						(CMapLocation*);
	virtual					~CMapSpot						();
	virtual		void		Load							(CUIXml* xml, LPCSTR path);
	CMapLocation*			MapLocation						()							{return m_map_location;}
	virtual LPCSTR			GetHint							();
	virtual		void		Update							();
};

class CLevelMapSpot :public CMapSpot
{
	typedef CMapSpot inherited;
public:
							CLevelMapSpot					(CMapLocation*);
	virtual					~CLevelMapSpot					();
	virtual		void		Draw							();
};

class CZoneMapSpot :public CMapSpot
{
	typedef CMapSpot inherited;

public:
							CZoneMapSpot					(CMapLocation*);
	virtual					~CZoneMapSpot					();
	virtual		void		Draw							();
	virtual		void		Update							();
};

#pragma once
#pragma once

class CHairsZone :
	public CVisualZone
{
typedef				CVisualZone		inherited;		
public:
	virtual			void		Affect				(SZoneObjectInfo* O)		;

protected:
	virtual			bool		BlowoutState		()					;


};
#pragma once

class CAmebaZone :
public CVisualZone
{
typedef				CVisualZone		inherited;	
float m_fFlotationFactor;
public:
									CAmebaZone		()					;
									~CAmebaZone		()					;
	virtual				void		Affect			(CObject* O)		;

protected:
	virtual				bool		BlowoutState	()					;
	virtual				void		Load			(LPCSTR section)	;
};
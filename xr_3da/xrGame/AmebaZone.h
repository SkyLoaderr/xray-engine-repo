#pragma once

class CAmebaZone :
public CVisualZone,
public CPHUpdateObject
{
typedef				CVisualZone		inherited;	
float m_fVelocityLimit;
public:
									CAmebaZone		()						;
									~CAmebaZone		()						;
	virtual				void		Affect			(CObject* O)			;
	
protected:
	virtual				void		PhTune			(dReal step)			;
	virtual				void		PhDataUpdate	(dReal step)			{;}
	virtual				bool		BlowoutState	()						;
	virtual				void		SwitchZoneState	(EZoneState new_state)	;
	virtual				void		Load			(LPCSTR section)		;
};
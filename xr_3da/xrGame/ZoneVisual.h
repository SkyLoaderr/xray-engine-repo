#pragma once

class CVisualZone :
	public CCustomZone
{
typedef				CCustomZone		inherited;
public:
					CVisualZone						()				;
	virtual			~CVisualZone					()				;
	virtual BOOL	net_Spawn						(LPVOID DC)		;
protected:
private:
};

//////////////////////////////////////////////////////////////////////////
// GraviZone.h:		гравитационная аномалия
//					все неживые объекты (предметы и трупы)
//					поднимает в воздух и качает там какое-то
//					время
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "customzone.h"
#include "ai/telekinesis.h"

class CGraviZone : public CCustomZone,
				   public CTelekinesis
{
private:
	typedef		CCustomZone					inherited;
	typedef		CTelekinesis				TTelekinesis;
public:
	CGraviZone(void);
	virtual ~CGraviZone(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	//воздействие зоной на объект
	virtual void	Affect(CObject* O);

	virtual void	shedule_Update		(u32 dt);
};

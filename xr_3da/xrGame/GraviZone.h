//////////////////////////////////////////////////////////////////////////
// GraviZone.h:		гравитационна€ аномали€
//////////////////////////////////////////////////////////////////////////
//					состоит как бы из 2х зон
//					одна зат€гивает объект, друга€ взрывает и 
//					все неживые объекты (предметы и трупы)
//					поднимает в воздух и качает там какое-то
//					врем€
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
	virtual			~CGraviZone(void);

	virtual void	Load (LPCSTR section);

	virtual BOOL	net_Spawn(LPVOID DC);
	virtual void	net_Destroy();

	//воздействие зоной на объект
	virtual void	Affect(CObject* O);

	virtual void	shedule_Update		(u32 dt);

protected:
	//врем€ вт€гивани€ объектов в зону
	u32				m_dwThrowInTime;
	//сила импульса вт€гивани€ в зону
	float			m_fThrowInImpulse;
	
	//радиус
	float			m_fBlowtRadius;
};

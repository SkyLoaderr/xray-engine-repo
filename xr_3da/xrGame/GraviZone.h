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
	virtual bool	BlowoutState();
	virtual bool	IdleState();

	virtual float	RelativePower(float dist);

protected:
	//сила импульса вт€гивани€ в зону (дл€ веса 100 кг)
	float			m_fThrowInImpulse;
	//сила импульса вт€гивани€ в зону дл€ живых существ
	float			m_fThrowInImpulseAlive;
	//коэфф. зат€гивани€ (чем меньше, тем плавнее зат€гивает)
	float			m_fThrowInAtten;
	//радиус действи€ выброса (в процентах от всего)
	float			m_fBlowoutRadiusPercent;

	//параметры телекинеза	
	float			m_fTeleHeight;
	u32				m_dwTimeToTele;
	u32				m_dwTelePause;
	u32				m_dwTeleTime;
	
	//им€ партиклов телекинеза
	void			PlayTeleParticles(CGameObject* pObject);
	void			StopTeleParticles(CGameObject* pObject);

	ref_str			m_sTeleParticlesBig;
	ref_str			m_sTeleParticlesSmall;
};
#pragma once

class CVisualZone :
	public CCustomZone
{
typedef				CCustomZone		inherited	;
shared_str			m_idle_animation			;
shared_str			m_attack_animation			;
u32					m_dwAttackAnimaionStart		;
u32					m_dwAttackAnimaionEnd		;
public:
					CVisualZone						()						;
	virtual			~CVisualZone					()						;
	virtual BOOL	net_Spawn						(CSE_Abstract* DC)				;
	virtual void	net_Destroy						()						;
	virtual void    AffectObjects					()						;
	virtual void	SwitchZoneState					(EZoneState new_state)	;
	virtual void	Load							(LPCSTR section)		;
	virtual void	UpdateBlowout					()						;
protected:
private:
};

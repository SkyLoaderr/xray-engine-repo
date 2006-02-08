#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CWeaponKnife: public CWeapon {
private:
	typedef CWeapon inherited;
protected:
	// HUD :: Animations
	MotionSVec		mhud_idle;
	MotionSVec		mhud_hide;
	MotionSVec		mhud_show;
	MotionSVec		mhud_attack;
	MotionSVec		mhud_attack2;
	MotionSVec		mhud_attack_e;
	MotionSVec		mhud_attack2_e;

	bool m_attackStart;

protected:

	virtual void	switch2_Idle	();
	virtual void	switch2_Hiding	();
	virtual void	switch2_Hidden	();
	virtual void	switch2_Showing	();
	virtual void	switch2_Attacking	();
	virtual void	switch2_Attacking2	();

	virtual void	OnAnimationEnd	();
	virtual void	OnStateSwitch	(u32 S);

	void			state_Attacking	(float dt);

	virtual void	KnifeStrike		(const Fvector& pos, const Fvector& dir);

	float			fWallmarkSize;
	u16				knife_material_idx;

protected:	
	ALife::EHitType			m_eHitType;

	ALife::EHitType			m_eHitType_1;
	int						iHitPower_1;
	float					fHitImpulse_1;

	ALife::EHitType			m_eHitType_2;
	int						iHitPower_2;
	float					fHitImpulse_2;
protected:
	virtual void			LoadFireParams		(LPCSTR section, LPCSTR prefix);
public:
	CWeaponKnife(); 
	virtual			~CWeaponKnife();

	void			Load				(LPCSTR section);

	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual	void	UpdateCL			();

	virtual void	Fire2Start			();
	virtual void	Fire2End			();
	virtual void	FireStart			();
	virtual void	FireEnd				();


	virtual const char* Name();
	virtual bool Action	(s32 cmd, u32 flags);

	virtual void	StartIdleAnim		();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponKnife)
#undef script_type_list
#define script_type_list save_type_list(CWeaponKnife)

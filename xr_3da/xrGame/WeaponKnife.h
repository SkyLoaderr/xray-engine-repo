#pragma once
#include "WeaponCustomPistol.h"



class CWeaponKnife: public CWeapon
{
private:
	typedef CWeapon inherited;
protected:

	// Media :: sounds
	ref_sound			sndZoomIn;
	ref_sound			sndZoomOut;
	ref_sound			sndGyro;
	ref_sound			sndShow;
	ref_sound			sndHide;

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
public:
	CWeaponKnife(); 
	virtual			~CWeaponKnife();

	void			Load				(LPCSTR section);
	virtual	void	UpdateCL			();
	virtual	void	renderable_Render	();

	virtual void	Hide				();
	virtual void	Show				();

	virtual void	Fire2Start			();
	virtual void	Fire2End			();
	virtual void	FireStart			();
	virtual void	FireEnd				();


	virtual const char* Name();
	virtual bool Action	(s32 cmd, u32 flags);

	virtual void	StartIdleAnim		();
};

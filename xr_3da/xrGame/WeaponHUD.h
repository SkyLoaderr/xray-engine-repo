#ifndef __XR_WEAPON_HUD_H__
#define __XR_WEAPON_HUD_H__

// refs
class ENGINE_API FBasicVisual;
class ENGINE_API CInifile;
class CEntity;

class CWeaponHUD : 
	public pureDeviceCreate, 
	public pureDeviceDestroy
{
	Fmatrix			m_Offset;

	Fmatrix			mTransform;
	LPSTR			pVisualName;
	FBasicVisual*	pVisual;

	enum EHUDState{
		hsIdle = 0,
		hsFireCycle,
		hsFireSpinup,
		hsShoot
	};

	EHUDState		new_mstate, cur_mstate;

	CMotionDef*		mIdle;
	CMotionDef*		mFireCycled;
	CMotionDef*		mFireSpinup;
	vector<CMotionDef*>mShoots;
public:	
	int				iFireBone;
	Fvector			vFirePoint;
public:
					CWeaponHUD		();
	virtual			~CWeaponHUD		();

	// misc
	virtual void	Load			(CInifile* ini, const char* section);

	IC FBasicVisual*Visual			()	{ return pVisual; }
	IC Fmatrix&		Transform		()	{ return mTransform; }

	// logic & effects
	void			Shoot			();
	void			FireSpinup		()	{ new_mstate = hsFireSpinup;}
	void			FireCycleStart	()	{ new_mstate = hsFireCycle;	}
	void			FireEnd			()	{ new_mstate = hsIdle;	}

	void			UpdatePosition	(const Fmatrix& transform);
	void			UpdateAnimation	();

	// device
	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};
 
#endif // __XR_WEAPON_HUD_H__

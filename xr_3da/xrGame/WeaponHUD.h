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
		hsFire
	};

	EHUDState		new_mstate, cur_mstate;
public:
					CWeaponHUD		();
	virtual			~CWeaponHUD		();

	// misc
	virtual void	Load			(CInifile* ini, const char* section);

	IC FBasicVisual*Visual			()	{ return pVisual; }
	IC Fmatrix&		Transform		()	{ return mTransform; }

	// logic & effects
	void			FireStart		()	{ new_mstate = hsFire;	}
	void			FireEnd			()	{ new_mstate = hsIdle;	}

	void			UpdatePosition	(const Fmatrix& transform);
	void			UpdateAnimation	();

	// device
	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};
 
#endif // __XR_WEAPON_HUD_H__

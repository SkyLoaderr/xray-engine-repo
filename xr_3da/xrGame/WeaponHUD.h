#ifndef __XR_WEAPON_HUD_H__
#define __XR_WEAPON_HUD_H__

// refs
class ENGINE_API FBasicVisual;
class ENGINE_API CInifile;
class ENGINE_API CMotionDef;
class CEntity;
class CWeapon;

class CWeaponHUD : 
	public pureDeviceCreate, 
	public pureDeviceDestroy
{
	Fmatrix				m_Offset;
	Fmatrix				mTransform;
	LPSTR				pVisualName;
	FBasicVisual*		pVisual;
public:	
	int					iFireBone;
	Fvector				vFirePoint;
public:
						CWeaponHUD		();
	virtual				~CWeaponHUD		();

	// misc
	virtual void		Load			(CInifile* ini, const char* section);

	IC FBasicVisual*	Visual			()	{ return pVisual; }
	IC Fmatrix&			Transform		()	{ return mTransform; }

	// Animations
	void				animPlay		(CMotionDef* M, BOOL bMixIn=TRUE, CWeapon* W=0);
	CMotionDef*			animGet			(LPCSTR name);
	
	void				UpdatePosition	(const Fmatrix& transform);

	// device
	virtual void		OnDeviceCreate	();
	virtual void		OnDeviceDestroy	();
};
 
#endif // __XR_WEAPON_HUD_H__

#ifndef __XR_WEAPON_HUD_H__
#define __XR_WEAPON_HUD_H__

// refs
class ENGINE_API IVisual;
class ENGINE_API CInifile;
class ENGINE_API CMotionDef;
class CEntity;
class CWeapon;
class CInventoryItem;

class CWeaponHUD
{
	Fmatrix				m_Offset;
	Fmatrix				m_CamAssist;
	Fmatrix				mTransform;
	LPSTR				pVisualName;
	IVisual*			pVisual;
public:	
	int					iFireBone;
	Fvector				vFirePoint;
	Fvector				vFirePoint2;
	Fvector				vShellPoint;
public:
						CWeaponHUD		();
	virtual				~CWeaponHUD		();

	// misc
	virtual void		Load			(LPCSTR section);

	IC IVisual*			Visual			()	{ return pVisual; }
	IC Fmatrix&			Transform		()	{ return mTransform; }

	// Animations
	void				animPlay		(CMotionDef* M, BOOL bMixIn=TRUE, CInventoryItem*  W=0);
	CMotionDef*			animGet			(LPCSTR name);
	
	void				UpdatePosition	(const Fmatrix& transform);
};
 
#endif // __XR_WEAPON_HUD_H__

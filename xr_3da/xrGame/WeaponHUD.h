#ifndef __XR_WEAPON_HUD_H__
#define __XR_WEAPON_HUD_H__

// refs
class ENGINE_API IRender_Visual;
class ENGINE_API CInifile;
class ENGINE_API CMotionDef;
class CEntity;
class CHudItem;
class CInventoryItem;

class CWeaponHUD
{
	Fmatrix				m_Offset;
	Fmatrix				m_CamAssist;
	Fmatrix				mTransform;
	ref_str				pVisualName;
	IRender_Visual*		pVisual;

	//������������ ������ ������
	CHudItem*			m_pParentWeapon;		


	//����, ���� hud ������� �� ������������
	bool				m_bHidden;
public:	
	int					iFireBone;
	Fvector				vFirePoint;
	Fvector				vFirePoint2;

	//������
	Fvector				vShellDir;
	Fvector				vShellPoint;
public:
						CWeaponHUD		();
						CWeaponHUD		(CHudItem* pHudItem);
	virtual				~CWeaponHUD		();

	// misc
	virtual void		Load			(LPCSTR section);

	IC IRender_Visual*	Visual			()	{ return pVisual; }
	IC Fmatrix&			Transform		()	{ return mTransform; }

	// Animations
	void				animPlay		(CMotionDef* M, BOOL bMixIn=TRUE, CInventoryItem*  W=0);
	CMotionDef*			animGet			(LPCSTR name);
	
	void				UpdatePosition	(const Fmatrix& transform);

	bool				IsHidden		() {return m_bHidden;}
	void				Hide			() {m_bHidden = true;}
	void				Show			() {m_bHidden = false;}
};
 
#endif // __XR_WEAPON_HUD_H__

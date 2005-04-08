// WeaponHUD.h: HUD ��� ������ � ������ ���������, �������
//				����� ������� � ����� ���������, ����� ������������
//				��� ������������� �������� � ����� �� 3-�� ����
//////////////////////////////////////////////////////////////////////



#ifndef __XR_WEAPON_HUD_H__
#define __XR_WEAPON_HUD_H__

#include "../SkeletonAnimated.h"

// refs
class ENGINE_API IRender_Visual;
class ENGINE_API CInifile;
class ENGINE_API CMotionDef;
class CEntity;
class CHudItem;
class CInventoryItem;


struct SHARED_HUD_INFO
{
private:
	//������� ������ �� ������
	u16					ref_count;
public:
	SHARED_HUD_INFO		(LPCSTR section, CHudItem* pHudItem);
	~SHARED_HUD_INFO	();

	IRender_Visual*		pVisual;

	Fmatrix				m_Offset;
	Fmatrix				m_CamAssist;
	Fmatrix				mTransform;

	int					iFireBone;
	Fvector				vFirePoint;
	Fvector				vFirePoint2;

	Fvector				vShellPoint;
public:
	u16					GetRefCount()   {return ref_count;}
	void				AddRef()		{ref_count++;}
	void				ReleaseRef()	{if(ref_count>0) ref_count--;}
};

DEFINE_MAP (shared_str, SHARED_HUD_INFO*, SHARED_HUD_INFO_MAP, SHARED_HUD_INFO_MAP_IT);


//---------------------------------------------------------------------------

class CWeaponHUD
{
	//��� ������ HUD
	shared_str			m_sHudSectionName;
	//������������ ������ HUD
	CHudItem*			m_pParentWeapon;		
	//����, ���� hud ������� �� ������������
	bool				m_bHidden;
	//��� ���� ����� ����� ����� ���������� ��������
	bool				m_bCurrentEntityIsParent;

	//��������� �� ������ ��� HUD ������ �� ������������ �������
	SHARED_HUD_INFO*	m_pSharedHudInfo;

	//������� ��� ������������ ��������
	u32					m_dwAnimTime;
	u32					m_dwAnimEndTime;
	bool				m_bStopAtEndAnimIsRunning;
	CInventoryItem*		m_pCallbackItem;

	//������������ ������� ������� ��������, � ����� callback
	void				StopCurrentAnim	();

	//������� � �������� ��� ������ �����������
	float				m_fZoomRotateX;
	float				m_fZoomRotateY;
	Fvector				m_fZoomOffset;
public:
						CWeaponHUD		();
						CWeaponHUD		(CHudItem* pHudItem);
						~CWeaponHUD		();

	// misc
	void				Load			(LPCSTR section);
	void				net_DestroyHud	();
	void				Init			();

	IC IRender_Visual*	Visual			()	{ return m_pSharedHudInfo->pVisual; }
	IC Fmatrix&			Transform		()	{ return m_pSharedHudInfo->mTransform; }

	int					FireBone		()	{return m_pSharedHudInfo->iFireBone;}
	Fvector&			FirePoint		()	{return m_pSharedHudInfo->vFirePoint;}
	Fvector&			FirePoint2		()	{return m_pSharedHudInfo->vFirePoint2;}
	Fvector&			ShellPoint		()	{return m_pSharedHudInfo->vShellPoint;}
	void				SetFirePoint	(const Fvector &fp)			{m_pSharedHudInfo->vFirePoint.set(fp);}
	void				SetShellPoint	(const Fvector &sp)			{m_pSharedHudInfo->vShellPoint.set(sp);}

	const Fvector&		ZoomOffset		()	const {return m_fZoomOffset;}
	float				ZoomRotateX		()	const {return m_fZoomRotateX;}
	float				ZoomRotateY		()	const {return m_fZoomRotateY;}
	void				SetZoomOffset	(const Fvector& zoom_offset)  { m_fZoomOffset = zoom_offset;}
	void				SetZoomRotateX	(float zoom_rotate_x)		  { m_fZoomRotateX = zoom_rotate_x;}
	void				SetZoomRotateY	(float zoom_rotate_y)		  { m_fZoomRotateY = zoom_rotate_y;}
	

	// Animations
	void				animPlay		(MotionID M, BOOL bMixIn=TRUE, CInventoryItem*  W=0);
	void				animDisplay		(MotionID M,	BOOL bMixIn);
	MotionID			animGet			(LPCSTR name);
	
	void				UpdatePosition	(const Fmatrix& transform);

	bool				IsHidden		() {return m_bHidden;}
	void				Hide			() {m_bHidden = true;}
	void				Show			() {m_bHidden = false;}

	void				SetCurrentEntityHud	(bool current_entity) {m_bCurrentEntityIsParent = current_entity;}
	
	//���������� HUD ������ ���������� �� ������ �����
	void				UpdateHud		();

	void				StopCurrentAnimWithoutCallback	();


protected:
	static SHARED_HUD_INFO*				GetHudInfo			(shared_str section, CHudItem* pHudItem);
	static void							ReleaseHudInfo		(shared_str section);
	static SHARED_HUD_INFO_MAP			m_SharedHudInfoMap;
};
 
#endif // __XR_WEAPON_HUD_H__
